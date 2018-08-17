/****************************************************************//**
 *
 * @file tftp_server.c
 *
 * @author   Logan Gunthorpe <logang@deltatee.com>
 *           Dirk Ziegelmeier <dziegel@gmx.de>
 *
 * @brief    Trivial File Transfer Protocol (RFC 1350)
 *
 * Copyright (c) Deltatee Enterprises Ltd. 2013
 * All rights reserved.
 *
 ********************************************************************/

/**
 * @defgroup tftp TFTP server
 * @ingroup apps
 *
 * This is simple TFTP server for the lwIP raw API.
 */

#include "lwipExt.h"

#include "lwip/apps/tftp_server.h"

#if LWIP_EXT_TFTP

#include "lwip/udp.h"
#include "lwip/timeouts.h"
#include "lwip/debug.h"

#define TFTP_MAX_PAYLOAD_SIZE 512
#define TFTP_HEADER_LENGTH    4

/* Op code */
#define TFTP_RRQ		1		/* Read REQ */
#define TFTP_WRQ	2		
#define TFTP_DATA	3
#define TFTP_ACK		4
#define TFTP_ERROR	5

enum tftp_error
{
	TFTP_ERROR_FILE_NOT_FOUND    = 1,
	TFTP_ERROR_ACCESS_VIOLATION  = 2,
	TFTP_ERROR_DISK_FULL         = 3,
	TFTP_ERROR_ILLEGAL_OPERATION = 4,
	TFTP_ERROR_UNKNOWN_TRFR_ID   = 5,
	TFTP_ERROR_FILE_EXISTS       = 6,
	TFTP_ERROR_NO_SUCH_USER      = 7
};

#include <string.h>

struct tftp_state
{
	const struct tftp_context	*ctx;
	void						*handle;
	
	struct pbuf				*last_data;
	struct udp_pcb			*upcb;
	
	ip_addr_t					addr;
	u16_t					port;
	
	int						timer;
	int						last_pkt;
	u16_t					blknum;
	u8_t						retries;
	u8_t						mode_write;
};

static struct tftp_state _tftpState;

static void _tftpTmr(void* arg);

static void _tftpCloseHandle(void)
{
	_tftpState.port = 0;
	ip_addr_set_any(0, &_tftpState.addr);

	if(_tftpState.last_data != NULL)
	{
		pbuf_free(_tftpState.last_data);
		_tftpState.last_data = NULL;
	}

	sys_untimeout(_tftpTmr, NULL);

	if (_tftpState.handle)
	{
		_tftpState.ctx->close(_tftpState.handle);
		_tftpState.handle = NULL;
		LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp: closing"LWIP_NEW_LINE));
	}
}

static void _tftpSendError(const ip_addr_t *addr, u16_t port, enum tftp_error code, const char *str)
{
	int str_length = strlen(str);
	struct pbuf* p;
	u16_t* payload;

//	LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp reply error:%s(%d)"LWIP_NEW_LINE, str, code) );
	EXT_INFOF( ("tftp reply error:%s(%d)"LWIP_NEW_LINE, str, code) );

	p = pbuf_alloc(PBUF_TRANSPORT, (u16_t)(TFTP_HEADER_LENGTH + str_length + 1), PBUF_RAM);
	if(p == NULL)
	{
		return;
	}

	payload = (u16_t*) p->payload;
	payload[0] = PP_HTONS(TFTP_ERROR);
	payload[1] = lwip_htons(code);
	MEMCPY(&payload[2], str, str_length + 1);

	udp_sendto(_tftpState.upcb, p, addr, port);
	pbuf_free(p);
}

static void _tftpSendAck(u16_t blknum)
{
	struct pbuf* p;
	u16_t* payload;

	p = pbuf_alloc(PBUF_TRANSPORT, TFTP_HEADER_LENGTH, PBUF_RAM);
	if(p == NULL)
	{
		return;
	}
	payload = (u16_t*) p->payload;

	payload[0] = PP_HTONS(TFTP_ACK);
	payload[1] = lwip_htons(blknum);

	udp_sendto(_tftpState.upcb, p, &_tftpState.addr, _tftpState.port);
	LWIP_DEBUGF(TFTP_DEBUG, ("tftp: send ACK for block# %d"LWIP_NEW_LINE, blknum));
	pbuf_free(p);
}

static void _tftpResendData(void)
{
		TRACE();
	struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, _tftpState.last_data->len, PBUF_RAM);
	if(p == NULL)
	{
		return;
	}

	if(pbuf_copy(p, _tftpState.last_data) != ERR_OK)
	{
		pbuf_free(p);
		return;
	}

	udp_sendto(_tftpState.upcb, p, &_tftpState.addr, _tftpState.port);
	pbuf_free(p);
}

static void _tftpSendData(void)
{
	u16_t *payload;
	int ret;

	if(_tftpState.last_data != NULL)
	{
		pbuf_free(_tftpState.last_data);
	}

	_tftpState.last_data = pbuf_alloc(PBUF_TRANSPORT, TFTP_HEADER_LENGTH + TFTP_MAX_PAYLOAD_SIZE, PBUF_RAM);
	if(_tftpState.last_data == NULL)
	{
		return;
	}

	payload = (u16_t *) _tftpState.last_data->payload;
	payload[0] = PP_HTONS(TFTP_DATA);
	payload[1] = lwip_htons(_tftpState.blknum);

	ret = _tftpState.ctx->read(_tftpState.handle, &payload[2], TFTP_MAX_PAYLOAD_SIZE);
	if (ret < 0)
	{
		_tftpSendError(&_tftpState.addr, _tftpState.port, TFTP_ERROR_ACCESS_VIOLATION, "Error occured while reading the file.");
		_tftpCloseHandle();

		TRACE();
		return;
	}

	pbuf_realloc(_tftpState.last_data, (u16_t)(TFTP_HEADER_LENGTH + ret));
	_tftpResendData();
}


static void _tftpTmr(void* arg)
{
	LWIP_UNUSED_ARG(arg);

	_tftpState.timer++;
//		TRACE();

	if (_tftpState.handle == NULL)
	{
		return;
	}

	sys_timeout(TFTP_TIMER_MSECS, _tftpTmr, NULL);

	if ((_tftpState.timer - _tftpState.last_pkt) > (TFTP_TIMEOUT_MSECS / TFTP_TIMER_MSECS))
	{
		if ((_tftpState.last_data != NULL) && (_tftpState.retries < TFTP_MAX_RETRIES))
		{
			LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp: timeout, retrying"EXT_NEW_LINE));
			_tftpResendData();
			_tftpState.retries++;
		}
		else
		{
			LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp: timeout"EXT_NEW_LINE));
			CANCEL_UPDATE(&extRun);
			_tftpCloseHandle();
		TRACE();
		}
	}
}

static void _tftpRecv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	u16_t *sbuf = (u16_t *) p->payload;
	int opcode;

	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(upcb);

	if (((_tftpState.port != 0) && (port != _tftpState.port)) || (!ip_addr_isany_val(_tftpState.addr) && !ip_addr_cmp(&_tftpState.addr, addr)))
	{
		_tftpSendError(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "Only one connection at a time is supported");
		pbuf_free(p);
		return;
	}

	opcode = sbuf[0];

	_tftpState.last_pkt = _tftpState.timer;
	_tftpState.retries = 0;

	switch (opcode)
	{
		case PP_HTONS(TFTP_RRQ): /* fall through */
		case PP_HTONS(TFTP_WRQ):
		{
			const char tftp_null = 0;
			char filename[TFTP_MAX_FILENAME_LEN];
			char mode[TFTP_MAX_MODE_LEN];
			u16_t filename_end_offset;
			u16_t mode_end_offset;

			memset(filename, 0, sizeof(filename));

#if 1
			if(_tftpState.handle != NULL)
			{
				_tftpSendError(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "Only one connection at a time is supported");
				break;
			}
#endif

			sys_timeout(TFTP_TIMER_MSECS, _tftpTmr, NULL);

			/* find \0 in pbuf -> end of filename string */
			filename_end_offset = pbuf_memfind(p, &tftp_null, sizeof(tftp_null), 2);
			if((u16_t)(filename_end_offset-2) > sizeof(filename))
			{
				_tftpSendError(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "Filename too long/not NULL terminated");
				break;
			}
			
			pbuf_copy_partial(p, filename, filename_end_offset-2, 2);

			LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp request filename '%s'"LWIP_NEW_LINE, filename) );

			/* find \0 in pbuf -> end of mode string */
			mode_end_offset = pbuf_memfind(p, &tftp_null, sizeof(tftp_null), filename_end_offset+1);
			if((u16_t)(mode_end_offset-filename_end_offset) > sizeof(mode))
			{
				LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("Mode too long/not NULL terminated:%d"LWIP_NEW_LINE, (mode_end_offset-filename_end_offset)) );
				_tftpSendError(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "Mode too long/not NULL terminated");
				break;
			}
			pbuf_copy_partial(p, mode, mode_end_offset-filename_end_offset, filename_end_offset+1);

#if EXTLAB_BOARD
			if(! IS_STRING_EQUAL(filename, EXT_TFTP_IMAGE_OS_NAME) && !IS_STRING_EQUAL(filename, EXT_TFTP_IMAGE_FPGA_NAME) )
			{
				_tftpSendError(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "Only '"EXT_TFTP_IMAGE_OS_NAME"' or '"EXT_TFTP_IMAGE_FPGA_NAME"' can be put or get");
				break;
			}
#endif


			_tftpState.handle = _tftpState.ctx->open((void *)_tftpState.ctx, filename, mode, opcode == PP_HTONS(TFTP_WRQ));
			_tftpState.blknum = 1;


			if (!_tftpState.handle )
			{
				_tftpSendError(addr, port, TFTP_ERROR_FILE_NOT_FOUND, "Unable to open requested file.");
				break;
			}

			LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp: %s request from ", (opcode == PP_HTONS(TFTP_WRQ)) ? "write" : "read"));
			ip_addr_debug_print(TFTP_DEBUG | LWIP_DBG_STATE, addr);
			LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, (" for '%s' mode '%s'"LWIP_NEW_LINE, filename, mode));

			ip_addr_copy(_tftpState.addr, *addr);
			_tftpState.port = port;

			if (opcode == PP_HTONS(TFTP_WRQ))
			{
				_tftpState.mode_write = 1;
				_tftpState.blknum = 0;
				_tftpSendAck(0);
			}
			else
			{
				_tftpState.mode_write = 0;
				_tftpSendData();
			}

			break;
		}

		case PP_HTONS(TFTP_DATA):
		{
			int ret;
			u16_t blknum;

#if 1
			if (_tftpState.handle == NULL)
			{
				_tftpSendError(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "No connection");
				break;
			}
#endif

			if (_tftpState.mode_write != 1)
			{
				_tftpSendError(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "Not a write connection");
				break;
			}

			blknum = lwip_ntohs(sbuf[1]);
			pbuf_header(p, -TFTP_HEADER_LENGTH);

			/* only used in RCV data, so add to track blknum. J.L. Aug.7th, 2018 */
			if(blknum == _tftpState.blknum)
			{
				EXT_INFOF( ("tftp: recv replicated block#%d, resend ACK"LWIP_NEW_LINE, blknum));
				_tftpSendAck(blknum);
				break;
			}

			if(blknum != _tftpState.blknum+1)
			{
				EXT_ERRORF( ("tftp: recv block#%d is out of order, should be #%d"LWIP_NEW_LINE, blknum, _tftpState.blknum+1) );
				_tftpSendError(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "error writing file");
				_tftpCloseHandle();
				break;
			}
			
			_tftpState.blknum = blknum;
			LWIP_DEBUGF(TFTP_DEBUG, ("tftp: recv block#%d"LWIP_NEW_LINE, blknum));

			ret = _tftpState.ctx->write(_tftpState.handle, p);
			if (ret < 0)
			{
				_tftpSendError(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "error writing file");
				_tftpCloseHandle();
			}
			else
			{
				_tftpSendAck(blknum);
			}

			if (p->tot_len < TFTP_MAX_PAYLOAD_SIZE)
			{/* this is last packet, means end of this session */
				_tftpCloseHandle();
			}
			break;
		}

		case PP_HTONS(TFTP_ACK):
		{
			u16_t blknum;
			int lastpkt;

#if 1
			if (_tftpState.handle == NULL)
			{
				_tftpSendError(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "No connection");
				break;
			}
#endif
			if (_tftpState.mode_write != 0)
			{
				_tftpSendError(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "Not a read connection");
				break;
			}

			blknum = lwip_ntohs(sbuf[1]);
			if (blknum != _tftpState.blknum)
			{
				_tftpSendError(addr, port, TFTP_ERROR_UNKNOWN_TRFR_ID, "Wrong block number");
				break;
			}

			lastpkt = 0;

			if (_tftpState.last_data != NULL)
			{
				lastpkt = _tftpState.last_data->tot_len != (TFTP_MAX_PAYLOAD_SIZE + TFTP_HEADER_LENGTH);
			}

			if (!lastpkt)
			{
				_tftpState.blknum++;
				_tftpSendData();
			}
			else
			{
				_tftpCloseHandle();
			}

			break;
		}

		default:
			_tftpSendError(addr, port, TFTP_ERROR_ILLEGAL_OPERATION, "Unknown operation");
			break;
	}

	pbuf_free(p);
}

/** @ingroup tftp
 * Initialize TFTP server.
 * @param ctx TFTP callback struct
 */
err_t tftp_init(const struct tftp_context *ctx)
{
	err_t ret;

	struct udp_pcb *pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
	if (pcb == NULL)
	{
		return ERR_MEM;
	}

	ret = udp_bind(pcb, IP_ANY_TYPE, TFTP_PORT);
	if (ret != ERR_OK)
	{
		udp_remove(pcb);
		return ret;
	}

	_tftpState.handle    = NULL;
	_tftpState.port      = 0;
	_tftpState.ctx       = ctx;
	_tftpState.timer     = 0;
	_tftpState.last_data = NULL;
	_tftpState.upcb      = pcb;

	udp_recv(pcb, _tftpRecv, NULL);

	return ERR_OK;
}

#endif /* LWIP_UDP */

