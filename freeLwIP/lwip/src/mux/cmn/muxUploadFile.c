
#include "lwipMux.h"


#include "compact.h"
#include "lwipMux.h"
#include "muxHttp.h"

//#include "muxOs.h"

#include "lwip/apps/tftp_server.h"

#ifdef __ARM__
#include "bspHwSpiFlash.h"
#else
#include <errno.h>
#endif

static char _firmwareUpdateInit(MUX_RUNTIME_CFG *runCfg, MUX_FM_T fmT, const char	*fname, char isWrite)
{
	memset(&runCfg->firmUpdateInfo, 0, sizeof(MUX_FM_UPDATE) );
	runCfg->firmUpdateInfo.type = fmT;

#ifdef	ARM
	unsigned	int		startSector;
	if( fmT== MUX_FM_TYPE_RTOS) 
	{
		startSector = SFLASH_START_SECTOR_TEMP_RTOS;
	}
	else
	{
		startSector = SFLASH_START_SECTOR_TEMP_FPGA;
	}

	runCfg->firmUpdateInfo.isWrite = 1;

	bspSpiFlashInit(startSector, FLASH_N250_SECTOR_ADDRESS(startSector), runCfg->firmUpdateInfo.isWrite);
#else
	if (isWrite)
	{
		runCfg->firmUpdateInfo.fp = fopen(fname, "wb");
	}
	else
	{
		runCfg->firmUpdateInfo.fp = fopen(fname, "rb");
	}

	if(runCfg->firmUpdateInfo.fp == NULL)
	{
		printf("open '%s' for %s failed: %s"LWIP_NEW_LINE, fname, (isWrite==0)?"read":"write", strerror(errno));
		return EXIT_FAILURE;
	}
#endif
	return EXIT_SUCCESS;
}

static void _firmwareUpdateEnd(MUX_RUNTIME_CFG	*runCfg)
{
#ifdef	ARM
	/* write the left data in last page into flash*/
	bspSpiFlashFlush();

	/* only RTOS updating need saving configuration*/
	if(runCfg->firmUpdateInfo.type == MUX_FM_TYPE_RTOS || runCfg->firmUpdateInfo.type == MUX_FM_TYPE_FPGA)
	{
		bspCfgSave(runCfg, MUX_CFG_MAIN);
	}

	/* reboot */
#else	
	fclose(runCfg->firmUpdateInfo.fp);
#endif

	MUX_DEBUGF(MUX_DBG_ON, ("Received %d bytes %s firmware and save to Flash"MUX_NEW_LINE, runCfg->firmUpdateInfo.size, (runCfg->firmUpdateInfo.type == MUX_FM_TYPE_RTOS)?"RTOS":"FPGA"));
}


static char _muxUploadOpen(struct _MuxHttpConn *mhc)
{
//	struct _MuxUploadContext *ctx = mhc->uploadCtx;
	MUX_RUNTIME_CFG	*runCfg = mhc->nodeInfo->runCfg;

	MUX_FM_T	fmT = MUX_FM_TYPE_NONE;
	if( IS_STRING_EQUAL(mhc->uri, MUX_WEBPAGE_UPDATE_MCU) )
	{
		fmT = MUX_FM_TYPE_RTOS;
	}
	else if(IS_STRING_EQUAL(mhc->uri, MUX_WEBPAGE_UPDATE_FPGA) )
	{
		fmT = MUX_FM_TYPE_FPGA;
	}

	return _firmwareUpdateInit(runCfg, fmT, (const char *)mhc->filename,  1);
}

static void  _muxUploadClose(struct _MuxHttpConn *mhc)
{
//	struct _MuxUploadContext *ctx = mhc->uploadCtx;
	MUX_RUNTIME_CFG	*runCfg = mhc->nodeInfo->runCfg;
	
	MUX_DEBUGF(MUX_HTTPD_DATA_DEBUG, ("File finished:'%s' %d bytes", mhc->filename, runCfg->firmUpdateInfo.size ));
	_firmwareUpdateEnd(runCfg);

}


static unsigned short _muxUploadWrite(struct _MuxHttpConn *mhc, void *data, unsigned short size)
{
	int len;
//	struct _MuxUploadContext *ctx = mhc->uploadCtx;
	MUX_RUNTIME_CFG	*runCfg = mhc->nodeInfo->runCfg;

#ifdef	ARM
	len = bspSpiFlashWrite((unsigned char *)data, (unsigned int)size);
	if( len != (int)size )	
	{
		return -1;
	}
#else
	len = fwrite(data, 1, (unsigned int)size, runCfg->firmUpdateInfo.fp );
#endif
	
	MUX_DEBUGF(MUX_HTTPD_DATA_DEBUG, ("File:'%s' written %d bytes", mhc->filename, len ));

	memset(data, 0, size);
	
	runCfg->firmUpdateInfo.size += (unsigned int)len;
	return len;
}

const struct _MuxUploadContext muxUpload =
{
	open	: _muxUploadOpen,
	close	: _muxUploadClose,
	write	: _muxUploadWrite,
};


static void *_muxTftpOpen(void *handle, const char* fname, const char* mode, char isWrite)
{
	struct tftp_context	*ctx = (struct tftp_context *)handle;
	MUX_RUNTIME_CFG	*runCfg = ctx->priv;
	MUX_FM_T	fmT = MUX_FM_TYPE_FPGA;
	
	LWIP_UNUSED_ARG(mode);

	if( IS_STRING_EQUAL(fname, MUX_TFTP_IMAGE_OS_NAME) )
	{
		fmT = MUX_FM_TYPE_RTOS;
	}
	else if(IS_STRING_EQUAL(fname, MUX_TFTP_IMAGE_FPGA_NAME) )
	{
		fmT = MUX_FM_TYPE_FPGA;
	}

	if(_firmwareUpdateInit(runCfg, fmT, fname, isWrite) == EXIT_FAILURE)
		return NULL;

	return ctx;
}


/* save the configuration for this */
static void  _muxTftpClose(void* handle)
{
	struct tftp_context	*ctx = (struct tftp_context *)handle;
	MUX_RUNTIME_CFG	*runCfg = ctx->priv;
	
	_firmwareUpdateEnd(runCfg);
}


static int _muxTftpRead(void* handle, void* buf, int bytes)
{
#ifdef	ARM
#if 0
//	if (fread(buf, 1, bytes, (FILE*)handle) != (size_t)bytes)
	if(bspSpiFlashRead(buf, bytes) != bytes)	
	{
		return -1;
	}
	
	return 0;
#else
	return -1;
#endif
#else
	struct tftp_context	*ctx = (struct tftp_context *)handle;
	MUX_RUNTIME_CFG	*runCfg = ctx->priv;
	if (fread(buf, 1, bytes, (FILE*)runCfg->firmUpdateInfo.fp) != (size_t)bytes)
	{
		return -1;
	}
	
	return 0;
#endif
}

static int _muxTftpWrite(void* handle, struct pbuf* p)
{
	struct tftp_context	*ctx = (struct tftp_context *)handle;
	MUX_RUNTIME_CFG	*runCfg = ctx->priv;

	while (p != NULL)
	{
#ifdef	ARM	
		//if (fwrite(p->payload, 1, p->len, (FILE*)handle) != (size_t)p->len)
		if(bspSpiFlashWrite((unsigned char *) p->payload, (unsigned int) p->len) != (int)p->len )	
		{
			return -1;
		}
		
#else		
		if (fwrite(p->payload, 1, p->len, runCfg->firmUpdateInfo.fp) != (size_t)p->len)
		{
			return -1;
		}
#endif

		runCfg->firmUpdateInfo.size += p->len;
		MUX_DEBUGF(MUX_DBG_ON, ("RX: %d bytes, total %d bytes", p->len, runCfg->firmUpdateInfo.size) );
		p = p->next;
	}


	return 0;
}

const struct tftp_context muxTftp =
{
	_muxTftpOpen,
	_muxTftpClose,
	_muxTftpRead,
	_muxTftpWrite,
	
	&muxRun
};


