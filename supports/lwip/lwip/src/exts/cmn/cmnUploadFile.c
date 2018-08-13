
#include "lwipExt.h"


#include "compact.h"
//#include "lwipExt.h"
#include "http.h"

//#include "eos.h"

#include "lwip/apps/tftp_server.h"

#include "bspHwSpiFlash.h"

static char _firmwareUpdateInit(EXT_RUNTIME_CFG *runCfg, EXT_FM_T fmT, const char	*fname, char isWrite)
{
	memset(&runCfg->firmUpdateInfo, 0, sizeof(EXT_FM_UPDATE) );
	runCfg->firmUpdateInfo.type = fmT;

#ifdef	ARM
	unsigned	int		startSector;
	if( fmT== EXT_FM_TYPE_RTOS) 
	{
		startSector = SFLASH_START_SECTOR_TEMP_RTOS;
	}
	else
	{
		startSector = SFLASH_START_SECTOR_TEMP_FPGA;
	}

	gmacBMCastEnable(EXT_FALSE);

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

#if 0// LWIP_MDNS_RESPONDER
	extLwipMdsnDestroy(&guNetIf);
#endif


	return EXIT_SUCCESS;
}

static void _firmwareUpdateEnd(EXT_RUNTIME_CFG	*runCfg)
{
#ifdef	ARM
	/* write the left data in last page into flash*/
	bspSpiFlashFlush();

	gmacBMCastEnable(EXT_TRUE);

	/* only RTOS updating need saving configuration*/
	if(runCfg->firmUpdateInfo.type == EXT_FM_TYPE_RTOS || runCfg->firmUpdateInfo.type == EXT_FM_TYPE_FPGA)
	{
		bspCfgSave(runCfg, EXT_CFG_MAIN);
	}

	/* reboot */
#else	
	fclose(runCfg->firmUpdateInfo.fp);
#endif

	EXT_DEBUGF(TFTP_DEBUG|EXT_HTTPD_DATA_DEBUG, ("Received %d bytes %s firmware and save to Flash"EXT_NEW_LINE, runCfg->firmUpdateInfo.size, (runCfg->firmUpdateInfo.type == EXT_FM_TYPE_RTOS)?"RTOS":"FPGA"));
}


static char _extUploadOpen(struct _MuxHttpConn *mhc)
{
//	struct _MuxUploadContext *ctx = mhc->uploadCtx;
	EXT_RUNTIME_CFG	*runCfg = mhc->nodeInfo->runCfg;

	EXT_FM_T	fmT = EXT_FM_TYPE_NONE;
	if( IS_STRING_EQUAL(mhc->uri, EXT_WEBPAGE_UPDATE_MCU) )
	{
		fmT = EXT_FM_TYPE_RTOS;
	}
	else if(IS_STRING_EQUAL(mhc->uri, EXT_WEBPAGE_UPDATE_FPGA) )
	{
		fmT = EXT_FM_TYPE_FPGA;
	}

	return _firmwareUpdateInit(runCfg, fmT, (const char *)mhc->filename,  1);
}

static void  _extUploadClose(struct _MuxHttpConn *mhc)
{
//	struct _MuxUploadContext *ctx = mhc->uploadCtx;
	EXT_RUNTIME_CFG	*runCfg = mhc->nodeInfo->runCfg;
	
	EXT_DEBUGF(EXT_HTTPD_DATA_DEBUG, ("File finished:'%s' %d bytes", mhc->filename, runCfg->firmUpdateInfo.size ));
	_firmwareUpdateEnd(runCfg);

}


static unsigned short _extUploadWrite(struct _MuxHttpConn *mhc, void *data, unsigned short size)
{
	int len;
//	struct _MuxUploadContext *ctx = mhc->uploadCtx;
	EXT_RUNTIME_CFG	*runCfg = mhc->nodeInfo->runCfg;

#ifdef	ARM
	len = bspSpiFlashWrite((unsigned char *)data, (unsigned int)size);
	if( len != (int)size )	
	{
		return -1;
	}
#else
	len = fwrite(data, 1, (unsigned int)size, runCfg->firmUpdateInfo.fp );
#endif
	
	EXT_DEBUGF(EXT_HTTPD_DATA_DEBUG, ("File:'%s' written %d bytes", mhc->filename, len ));

	memset(data, 0, size);
	
	runCfg->firmUpdateInfo.size += (unsigned int)len;
	return len;
}

const struct _MuxUploadContext extUpload =
{
	open	: _extUploadOpen,
	close	: _extUploadClose,
	write	: _extUploadWrite,
};


static unsigned short __extTftpWriteData(EXT_RUNTIME_CFG	*runCfg, void *data, unsigned short len)
{
#ifdef	ARM	
	if(bspSpiFlashWrite((unsigned char *) data, (unsigned int) len) != (int)len )	
	{
		return -1;
	}
#else		
	if (fwrite(data, 1, len, runCfg->firmUpdateInfo.fp) != (len) )
	{
		return -1;
	}
#endif

	runCfg->firmUpdateInfo.size += len;
//	EXT_DEBUGF(TFTP_DEBUG, ("TFTP Write: %d bytes, total %d bytes", len, runCfg->firmUpdateInfo.size) );

	return len;
}

static void *_extTftpOpen(void *handle, const char* fname, const char* mode, char isWrite)
{
	struct tftp_context	*ctx = (struct tftp_context *)handle;
	EXT_RUNTIME_CFG	*runCfg = ctx->priv;
	EXT_FM_T	fmT = EXT_FM_TYPE_FPGA;

	EXT_ASSERT(("RunCf is null"), (runCfg!= NULL));

	LWIP_UNUSED_ARG(mode);

	if( IS_STRING_EQUAL(fname, EXT_TFTP_IMAGE_OS_NAME) )
	{
		fmT = EXT_FM_TYPE_RTOS;
	}
	else if(IS_STRING_EQUAL(fname, EXT_TFTP_IMAGE_FPGA_NAME) )
	{
		fmT = EXT_FM_TYPE_FPGA;
	}

	if(_firmwareUpdateInit(runCfg, fmT, fname, isWrite) == EXIT_FAILURE)
	{
		return NULL;
	}

	ctx->dataIndex = 0;
	return ctx;
}


/* save the configuration for this */
static void  _extTftpClose(void* handle)
{
	struct tftp_context	*ctx = (struct tftp_context *)handle;
	EXT_RUNTIME_CFG	*runCfg = ctx->priv;

	if(ctx->dataIndex> 0)
	{
		EXT_DEBUGF(TFTP_DEBUG, ("TFTP Write last packet, %d bytes", ctx->dataIndex) );
		__extTftpWriteData(runCfg, runCfg->bufWrite, ctx->dataIndex);
	}
	
	_firmwareUpdateEnd(runCfg);

	EXT_DEBUGF(TFTP_DEBUG, ("Closed, total %d bytes", runCfg->firmUpdateInfo.size) );
}


static int _extTftpRead(void* handle, void* buf, int bytes)
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
	EXT_RUNTIME_CFG	*runCfg = ctx->priv;
	if (fread(buf, 1, bytes, (FILE*)runCfg->firmUpdateInfo.fp) != (size_t)bytes)
	{
		return -1;
	}
	
	return 0;
#endif
}


static int _extTftpWrite(void* handle, struct pbuf* p)
{
	struct tftp_context	*ctx = (struct tftp_context *)handle;
	EXT_RUNTIME_CFG	*runCfg = ctx->priv;
	unsigned short copied = 0;
	unsigned short len;

	while(copied != p->tot_len)
	{
		len = LWIP_MIN(p->tot_len - copied, ((unsigned short)runCfg->bufLength - ctx->dataIndex) );
	
		len = pbuf_copy_partial(p, runCfg->bufWrite+ctx->dataIndex, len, copied );
		ctx->dataIndex += len;
		copied += len;


		if(ctx->dataIndex == runCfg->bufLength )
		{
//			EXT_DEBUGF(TFTP_DEBUG, ("write to flash: packet %d bytes, copied %d len %d byte data", p->tot_len, copied, len) );
			len = __extTftpWriteData(runCfg, runCfg->bufWrite, ctx->dataIndex);
			if( len != ctx->dataIndex)
			{
				EXT_ERRORF(("TFTP Write %d bytes of %d bytes", len, ctx->dataIndex) );
				return EXIT_FAILURE;
			}

			ctx->dataIndex = 0;
		}

//		EXT_DEBUGF(TFTP_DEBUG, ("packet %d bytes, copied %d (total %d)byte data", p->tot_len, copied, ctx->dataIndex) );
	}
	

	if(copied != p->tot_len)
	{
		EXT_ERRORF(("copied %d bytes from %d byte data", copied, p->tot_len) );
	}

	return 0;
}

struct tftp_context extTftp =
{
	_extTftpOpen,
	_extTftpClose,
	_extTftpRead,
	_extTftpWrite,
	
	&extRun,
	0
};


