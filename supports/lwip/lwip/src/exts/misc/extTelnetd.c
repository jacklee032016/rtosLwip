/*
 * telnet server based on TCP PCB. May.3,2018, J.L.
 */

#include "lwipExt.h"

#define	TELNET_DEBUG						EXT_DBG_OFF

#define	EXT_TELNET_PORT					23

/* polled once per second (e.g. continue write on memory error) */
#define	EXT_TELNET_POLL_INTERVAL			2

#define	EXT_TELNET_TIMEOUT				30		/* seconds */

#define	TELNET_PROTOCOL_IAC				0xFF	/* Intepret As Command */

#if LWIP_TCP


#define	EXT_CMD_BUFSIZE			1024
static unsigned char		_telnetCmdBuffer[EXT_CMD_BUFSIZE];

static unsigned char		_telnetResponseBuffer[ EXT_COMMAND_BUFFER_SIZE ];

enum	EXT_TELNET_STATE
{
	TELNET_CS_NONE = 0,
	TELNET_CS_RECEIVING,
	TELNET_CS_PROCESSING,
	TELNET_CS_CLOSING /*TELNET_CS_ERROR */
};

typedef	struct _ext_telnet_conn
{
	u8_t				state;
	u8_t				retries;

	unsigned char		*cmd;
	unsigned char		*response;

	unsigned char		isContinue;	/* continue with the same command after last process */
	
	unsigned short	readIndex; /* read from TCP */
	unsigned short	writeIndex;

	unsigned char		timeoutSecond;	/* poll interval is one second */
	
	struct tcp_pcb	*pcb;

}EXT_TELNET_CONN_T;

typedef	struct
{
	struct tcp_pcb			*rawPcb;
	EXT_RUNTIME_CFG		*runCfg;

	EXT_TELNET_CONN_T		*conn;	/* only one connection can support */
}MuxTelnetCtx;	


static MuxTelnetCtx  _extTelnetCtx;

#ifdef	X86

void bspConsoleDumpMemory(uint8_t *buffer, uint32_t size, uint32_t address)
{
	uint32_t i, j;
	uint32_t last_line_start;
	uint8_t *tmp;

	for (i = 0; i < (size / 16); i++)
	{
		printf("0x%08X: ", (unsigned int)(address + (i * 16)));
		tmp = (uint8_t *) & buffer[i * 16];
		for (j = 0; j < 4; j++)
		{
			printf("%02X %02X %02X %02X ", tmp[0], tmp[1], tmp[2], tmp[3]);
			tmp += 4;
		}
#if 0		
		tmp = (uint8_t *) & buffer[i * 16];
		for (j = 0; j < 16; j++)
		{
			bspConsolePutChar(*tmp++);
		}
#endif		
		printf(""EXT_NEW_LINE);
	}
	
	if ((size % 16) != 0)
	{
		last_line_start = size - (size % 16);
		printf("0x%08X: ", (unsigned int)(address + last_line_start));
		for (j = last_line_start; j < last_line_start + 16; j++)
		{
			if ((j != last_line_start) && (j % 4 == 0))
			{
				printf(" ");
			}
			if (j < size)
				printf("%02X ", buffer[j]);
			else
				printf("  ");
		}
#if 0		
		printf(" ");
		for (j = last_line_start; j < size; j++)
		{
			bspConsolePutChar(buffer[j]);
		}
#endif		
		printf(""EXT_NEW_LINE);
	}
}

#else
//extern	_CODE char *versionString;

#endif

static void _telnetConnFree(EXT_TELNET_CONN_T *tConn)
{
	if (tConn != NULL)
	{
		mem_free(tConn);
	}
}

static void _telnetConnClose(struct tcp_pcb *tpcb, EXT_TELNET_CONN_T *tConn)
{
	tcp_arg(tpcb, NULL);
	tcp_sent(tpcb, NULL);
	tcp_recv(tpcb, NULL);
	tcp_err(tpcb, NULL);
	tcp_poll(tpcb, NULL, 0);

	_telnetConnFree(tConn);

	tcp_close(tpcb);
}


/* send in any state */
static void _telnetConnSend(struct tcp_pcb *tpcb, EXT_TELNET_CONN_T *tConn, unsigned char *data, unsigned short size)
{
	err_t wr_err = ERR_OK;

#if 0
	if(tConn && tConn->state != TELNET_CS_PROCESSING)
	{
		return;
	}
#endif

//	while ((wr_err == ERR_OK) && (size >0) &&  (size <= tcp_sndbuf(tpcb)))
	if((wr_err == ERR_OK) && (size >0) &&  (size <= tcp_sndbuf(tpcb)))
	{

		EXT_DEBUGF(TELNET_DEBUG,("TELNET output :'%.*s'", size, data ));
		/* enqueue data for transmission */
		wr_err = tcp_write(tpcb, data, size,  1);
		if (wr_err == ERR_OK)
		{
			tcp_recved(tpcb, size );
#if 1		
			if( tConn)
			{
				if( tConn->isContinue == EXT_FALSE)
				{/* we can read more data now */
				}
#endif			
				tConn->writeIndex = 0;
			}
		}
		else if(wr_err == ERR_MEM)
		{/* we are low on memory, try later / harder, defer to poll */
			/* continue in this state to send more */
//			tConn->state = TELNET_CS_RECEIVING; 
			if(tConn )
			{
				tConn->state = TELNET_CS_CLOSING;
			}
		}
		else
		{/* other problem ?? */
			if(tConn )
			{
				tConn->state = TELNET_CS_CLOSING;
			}
		}
	}
}

static char _telnetMsgPrintout(EXT_TELNET_CONN_T *tConn, char *msg)
{
	tConn->writeIndex = 0;
	tConn->writeIndex += snprintf(((char *)tConn->response)+tConn->writeIndex, EXT_COMMAND_BUFFER_SIZE- tConn->writeIndex, "%s", msg);
	tConn->isContinue = EXT_FALSE;

	_telnetConnSend(tConn->pcb, tConn, tConn->response, tConn->writeIndex);
	return EXIT_SUCCESS;

//	return EXIT_FAILURE;
}

static void _telnetMsgPrompt(EXT_TELNET_CONN_T *tConn)
{
//	_telnetPrintout(versionString, conn);
	tConn->timeoutSecond = 0;
	_telnetMsgPrintout(tConn, (char *) LWIP_NEW_LINE	"MuxLab telnet shell."LWIP_NEW_LINE
		"For help, try the \"help\" command; \"quit\" exit telnet."LWIP_NEW_LINE "> " );
}  



static char _telnetCommandProcess(EXT_TELNET_CONN_T *tConn )
{
#if 0//def	X86
	static int count = 0;
	
	int index = 0;
	
//	*outBuffer = 0;

	index += snprintf( (char *)tConn->response+index, (EXT_COMMAND_BUFFER_SIZE-index), "Telnet #%d reply: '%s'"EXT_NEW_LINE, count++, (char *)tConn->cmd);

	if(count == 3)
	{
		count = 0;
		tConn->isContinue = EXT_FALSE;/* no more data */
	}
	else 
	{
		tConn->isContinue = EXT_TRUE;/* more data */
	}
#else
	/* Get the next output string from the command interpreter. */
	tConn->isContinue = cmnCmdLineProcess((const char * const)tConn->cmd, (char *)tConn->response, EXT_COMMAND_BUFFER_SIZE );
#endif

	tConn->writeIndex = strlen((char *)tConn->response);

	_telnetConnSend(tConn->pcb, tConn, tConn->response, tConn->writeIndex);

	return EXT_TRUE;
}

static char _telnetCommandRead(EXT_TELNET_CONN_T *tConn, struct pbuf *p)
{
	u16_t cur_len;
	unsigned char cmd;

//	err_t ret;

	if(p->tot_len >= EXT_CMD_BUFSIZE)
	{
		//ret = 
		_telnetMsgPrintout(tConn, (char *)"Command is longer than maxumum length"LWIP_NEW_LINE );
		tConn->state = TELNET_CS_CLOSING;
		return EXT_TRUE;
	}

	if(p->tot_len == 0)
	{
		return EXT_TRUE;
	}

	cmd = *((char*)p->payload);
	if(( cmd == TELNET_PROTOCOL_IAC) )
	{
		return EXT_TRUE;
	}

//	CONSOLE_DEBUG_MEM(p->payload, (uint32_t)p->len, 0, "RECV Telnet Data");

	cur_len = pbuf_copy_partial(p, tConn->cmd+tConn->readIndex, EXT_CMD_BUFSIZE - tConn->readIndex, 0);
//	ptr->payload, ptr->len
	if(cur_len <= 0)
	{
		EXT_ERRORF(("copy buffer failed: pkt:%d; buf size:%d", p->tot_len, EXT_CMD_BUFSIZE - tConn->readIndex));
		return EXT_TRUE;
	}
	//cur_len = p->tot_len;
	tConn->readIndex += cur_len;

	*(tConn->cmd + tConn->readIndex) = 0;

	
	cmd = *(tConn->cmd + tConn->readIndex-1);
#if TELNET_DEBUG
	CONSOLE_DEBUG_MEM(tConn->cmd, tConn->readIndex, 0, "RECV Telnet Data");
#endif
	EXT_DEBUGF(TELNET_DEBUG,("copied %d bytes:'%.*s', '0x%x'"LWIP_NEW_LINE, cur_len, tConn->readIndex-2, tConn->cmd, cmd ));
	if ( ( cmd == ASCII_KEY_LF) || (cmd == ASCII_KEY_CR) )
	{
		if(tConn->readIndex== 1 || tConn->readIndex == 2)
		{/* command to short, only LF/CR */
#if TELNET_DEBUG
			EXT_DEBUGF(TELNET_DEBUG,("TELNET: only LF/CR received"LWIP_NEW_LINE ));
#endif
			_telnetMsgPrintout(tConn, (char *)"> " );
			tConn->readIndex = 0;
			tConn->timeoutSecond = 0;
			return EXT_FALSE;
		}
		
		*(tConn->cmd +tConn->readIndex-1) = 0;
		tConn->readIndex--;

		tConn->state = TELNET_CS_PROCESSING;
#if 1		
		if((*(tConn->cmd+tConn->readIndex-1) == ASCII_KEY_CR) || (*(tConn->cmd+tConn->readIndex-1) == ASCII_KEY_CR))
		{
			*(tConn->cmd+tConn->readIndex-1) = 0;
			tConn->readIndex--;
		}
#endif

		EXT_DEBUGF(TELNET_DEBUG,("Processing '%s'"LWIP_NEW_LINE, tConn->cmd ));
		tConn->readIndex = 0;

//		if (_cmdBuffer[0] != 0xff && _cmdBuffer[1] != 0xfe)
		{
			if (strncmp((const char *)tConn->cmd, "quit", 4) == 0)
			{
				//ret = 
				_telnetMsgPrintout(tConn, (char *)"Bye, dear!"LWIP_NEW_LINE );
				tConn->isContinue = EXT_FALSE;
				tConn->state = TELNET_CS_CLOSING;
			}
			else
			{
				_telnetCommandProcess(tConn);
			}
		}
#if 0
		else
		{
			_telnetPrintout(NEWLINE NEWLINE	"MuxLab telnet shell."NEWLINE
				"For help, try the \"help\" command; \"quit\" exit telnet."NEWLINE, conn);
		}

		if (ret == ERR_OK)
		{
			_telnetdPrompt(conn);
		}
#endif
		
//		_telnetMsgPrompt(conn);
		return EXT_TRUE;
	}

	//_telnetMsgPrintout(tConn,(char *) tConn->cmd );
	EXT_DEBUGF(TELNET_DEBUG,("recv: '%s'", (char *) tConn->cmd) );
		

	return EXT_FALSE;
}



static void _telnetCallbackError(void *arg, err_t err)
{
	MuxTelnetCtx  *telnetCtx = (MuxTelnetCtx  *)(arg);

	LWIP_UNUSED_ARG(err);

	_telnetConnFree(telnetCtx->conn);
	telnetCtx->conn = NULL;
}

static err_t _telnetCallbackPoll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	MuxTelnetCtx  *telnetCtx = (MuxTelnetCtx  *)(arg);

	if (telnetCtx->conn != NULL)
	{
		ret_err = ERR_OK;
		if(telnetCtx->conn->state == TELNET_CS_CLOSING)
		{
			_telnetConnClose(tpcb, telnetCtx->conn);
			telnetCtx->conn = NULL;
		}
		else
		{
			telnetCtx->conn->timeoutSecond++;
			if(telnetCtx->conn->timeoutSecond >= EXT_TELNET_TIMEOUT)
			{
				_telnetMsgPrintout(telnetCtx->conn, (char *)"Timeout for MuxLab telnet connection"LWIP_NEW_LINE);
				telnetCtx->conn->state = TELNET_CS_CLOSING;
			}
		}
	}
	else
	{/* nothing to be done */
		tcp_abort(tpcb);
		ret_err = ERR_ABRT;
	}
	
	return ret_err;
}

static err_t _telnetCallbackSent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	MuxTelnetCtx  *telnetCtx = (MuxTelnetCtx  *)(arg);

	LWIP_UNUSED_ARG(len);

	telnetCtx->conn->retries = 0;

	if(telnetCtx->conn->state == TELNET_CS_PROCESSING )
	{
		/* still got pbufs to send */
		if(telnetCtx->conn->isContinue )
		{
			_telnetCommandProcess(telnetCtx->conn);
			tcp_sent(tpcb, _telnetCallbackSent);
		}
		else
		{
			_telnetMsgPrompt(telnetCtx->conn);
			telnetCtx->conn->state = TELNET_CS_RECEIVING;
		}
	}
	else if(telnetCtx->conn->state == TELNET_CS_CLOSING)
	{/* no more pbufs to send */
		_telnetConnClose(tpcb, telnetCtx->conn);
		telnetCtx->conn = NULL;
	}
	else
	{/* ignore this event when in RECEIVING state */
	}
	
	return ERR_OK;
}



static err_t _telnetCallbackRecv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	MuxTelnetCtx  *telnetCtx;
	err_t ret_err;

	EXT_ASSERT(("arg != NULL"), arg != NULL);
	telnetCtx = (MuxTelnetCtx  *)(arg);
	if (p == NULL)
	{/* remote host closed connection */
		{/* we're not done yet */
//			_telnetConnSend(tpcb, tConn);
		}
		
		telnetCtx->conn->state = TELNET_CS_CLOSING;
		ret_err = ERR_OK;
	}
	else if(err != ERR_OK)
	{/* cleanup, for unknown reason */
		if (p != NULL)
		{
			pbuf_free(p);
		}
		ret_err = err;
		telnetCtx->conn->state = TELNET_CS_CLOSING;
	}
	else if(telnetCtx->conn->state == TELNET_CS_RECEIVING)
	{
		/* first data chunk in p->payload */
//		tConn->state = TELNET_CS_RECEIVING;
		if(p != NULL)
		{
			unsigned short len =  p->tot_len;
			_telnetCommandRead(telnetCtx->conn, p);
//		_telnetConnSend(tpcb, tConn);
			pbuf_free(p);
			tcp_recved(tpcb, len);
		}
		ret_err = ERR_OK;
	}
	else if (telnetCtx->conn->state == TELNET_CS_PROCESSING)
	{/* ignore when command is being procesed */
		tcp_recved(tpcb, p->tot_len);
		pbuf_free(p);
		ret_err = ERR_OK;
	}
	else
	{/* other state, trash data  */
		tcp_recved(tpcb, p->tot_len);
		pbuf_free(p);
		ret_err = ERR_OK;
	}
	
	return ret_err;
}

static const char *_errMsg = "Only one connection can be built at any time for MuxLab Telnel connection";


static err_t _telnetEndConnPoll(void *arg, struct tcp_pcb *tpcb)
{
//	MuxTelnetCtx  *telnetCtx = (MuxTelnetCtx  *)(arg);

	tcp_close(tpcb);
	return ERR_OK;
}

static err_t _telnetCallabckAccept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	err_t	ret_err;
	MuxTelnetCtx  *telnetCtx = (MuxTelnetCtx  *)(arg);

	if ((err != ERR_OK) || (newpcb == NULL))
	{
		return ERR_VAL;
	}

	if(telnetCtx->conn )
	{
		_telnetConnSend(newpcb, NULL, (unsigned char *)_errMsg, strlen(_errMsg));
//		EXT_DELAY_MS(50);
		tcp_arg(newpcb, telnetCtx);
		tcp_poll(newpcb,	_telnetEndConnPoll, EXT_TELNET_POLL_INTERVAL*2);

		return ERR_OK;
	}

	/* Unless this pcb should have NORMAL priority, set its priority now.
	When running out of pcbs, low priority pcbs can be aborted to create
	new pcbs of higher priority. */
	tcp_setprio(newpcb, TCP_PRIO_MIN);

	telnetCtx->conn = (EXT_TELNET_CONN_T *)mem_malloc(sizeof(EXT_TELNET_CONN_T));
	if (telnetCtx->conn != NULL)
	{
		telnetCtx->conn->state = TELNET_CS_RECEIVING;
		telnetCtx->conn->pcb = newpcb;
		telnetCtx->conn->retries = 0;
		telnetCtx->conn->cmd = _telnetCmdBuffer;
		telnetCtx->conn->response = _telnetResponseBuffer;
		telnetCtx->conn->readIndex = 0;
		telnetCtx->conn->writeIndex = 0;
		telnetCtx->conn->timeoutSecond = 0;
		telnetCtx->conn->isContinue = EXT_FALSE;
		memset(_telnetCmdBuffer, 0, sizeof(_telnetCmdBuffer));
		memset(_telnetResponseBuffer, 0, sizeof(_telnetResponseBuffer));

		/* pass newly allocated es to our callbacks */
		tcp_arg(newpcb, telnetCtx);
		
		tcp_recv(newpcb,	_telnetCallbackRecv);
		tcp_err(newpcb,		_telnetCallbackError);
		tcp_poll(newpcb,		_telnetCallbackPoll, EXT_TELNET_POLL_INTERVAL);
		tcp_sent(newpcb,	_telnetCallbackSent);

#ifndef	X86
//		_telnetMsgPrintout(telnetCtx->conn, versionString);
#endif
		_telnetMsgPrompt(telnetCtx->conn);
		
		ret_err = ERR_OK;
	}
	else
	{
		ret_err = ERR_MEM;
	}
	
	return ret_err;
}

void extNetRawTelnetInit(EXT_RUNTIME_CFG *runCfg)
{
	_extTelnetCtx.rawPcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	_extTelnetCtx.runCfg = runCfg;

	if (_extTelnetCtx.rawPcb != NULL)
	{
		err_t err;

		err = tcp_bind(_extTelnetCtx.rawPcb, IP_ANY_TYPE, EXT_TELNET_PORT);
		if (err == ERR_OK)
		{
			_extTelnetCtx.rawPcb = tcp_listen(_extTelnetCtx.rawPcb);
			tcp_arg(_extTelnetCtx.rawPcb, &_extTelnetCtx);
			
			tcp_accept(_extTelnetCtx.rawPcb, _telnetCallabckAccept);
		}
		else
		{
		/* abort? output diagnostic? */
			EXT_ERRORF(("telent bind failed"));
		}
	}
	else
	{
		/* abort? output diagnostic? */
		EXT_ERRORF(("No RAW TCP for telent"));
	}
}

#endif /* LWIP_TCP */

