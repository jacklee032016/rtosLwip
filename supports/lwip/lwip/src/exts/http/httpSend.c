
#include "lwipExt.h"

#include "http.h"

/** Sub-function of http_send(): end-of-file (or block) is reached,
 * either close the file or read the next block (if supported).
 *
 * @returns: 0 if the file is finished or no data has been read
 *           1 if the file is not finished and data has been read
 */
static u8_t _mhttpSendCheckEof(ExtHttpConn *mhc)
{
	int bytes_left;
#if	MHTTPD_DYNAMIC_FILE_READ
	int count;
#ifdef MHTTPD_MAX_WRITE_LEN
	int max_write_len;
#endif
#endif

	/* Do we have a valid file handle? */
	if (mhc->handle == NULL)
	{
		/* No - close the connection. */
//		extHttpConnEof(mhc);
		return MHTTP_NO_DATA_TO_SEND;
	}
	
	bytes_left = mfsBytesLeft(mhc->handle);
	if (bytes_left <= 0)
	{
		/* We reached the end of the file so this request is done. */
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("End of file."));
//		extHttpConnEof( mhc);
		return MHTTP_NO_DATA_TO_SEND;
	}
	
#if	MHTTPD_DYNAMIC_FILE_READ
	/* Do we already have a send buffer allocated? */
	if(mhc->buf)
	{
		/* Yes - get the length of the buffer */
		count = LWIP_MIN(mhc->buf_len, bytes_left);
	}
	else
	{
		/* We don't have a send buffer so allocate one now */
		count = tcp_sndbuf(mhc->pcb);
		if(bytes_left < count)
		{
			count = bytes_left;
		}
#ifdef	MHTTPD_MAX_WRITE_LEN
		/* Additional limitation: e.g. don't enqueue more than 2*mss at once */
		max_write_len = MHTTPD_MAX_WRITE_LEN(mhc->pcb);
		if (count > max_write_len)
		{
			count = max_write_len;
		}
#endif

		do
		{
			mhc->buf = (char*)mem_malloc((mem_size_t)count);
			if (mhc->buf != NULL)
			{
				mhc->buf_len = count;
				break;
			}

			count = count / 2;
		} while (count > 100);

		/* Did we get a send buffer? If not, return immediately. */
		if (mhc->buf == NULL)
		{
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("No buff"));
			return 0;
		}
	}

	/* Read a block of data from the file. */
	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Trying to read %d bytes.", count));

#if	MHTTPD_FS_ASYNC_READ
	count = mfsReadAsync(mhc->handle, mhc->buf, count, mhttpContinue, mhc);
#else
	count = mfsRead(mhc->handle, mhc->buf, count);
#endif

	if (count < 0)
	{
		if (count == FS_READ_DELAYED)
		{/* Delayed read, wait for FS to unblock us */
			return 0;
		}

		/* We reached the end of the file so this request is done.
		* @todo: close here for HTTP/1.1 when reading file fails */
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("End of file."));
//		extHttpConnEof( mhc);
		return MHTTP_NO_DATA_TO_SEND;
	}

	/* Set up to send the block of data we just read */
	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Read %d bytes.", count));
	mhc->left = count;
	mhc->file = mhc->buf;

#if	MHTTPD_SSI
	if (mhc->ssi)
	{
		mhc->ssi->parse_left = count;
		mhc->ssi->parsed = mhc->buf;
	}
#endif
#else
	EXT_ASSERT(("SSI and DYNAMIC_HEADERS turned off but eof not reached"), 0);
#endif

	return 1;
}


/*
 * Try to send more data on this pcb.
 * This function can be called multiple time, and start write data from last time
 */
static u8_t _mHttpSendFile( ExtHttpConn *mhc)
{
	err_t err;
	u16_t len;
	u8_t data_to_send = MHTTP_NO_DATA_TO_SEND;

	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("pcb=%p mhc=%p left=%d", (void*)mhc->pcb, (void*)mhc, mhc != NULL ? (int)mhc->left : 0));
#if 	MHTTPD_POST_MANUAL_WND
	if (mhc->unrecved_bytes != 0)
	{
		return MHTTP_NO_DATA_TO_SEND;
	}
#endif


#if	MHTTPD_FS_ASYNC_READ
	/* Check if we are allowed to read from this file.
	(e.g. SSI might want to delay sending until data is available) */
	if (mfsIsFileReady(mhc->handle, mhttpContinue, mhc) == MHTTP_NO_DATA_TO_SEND)
	{
		return MHTTP_NO_DATA_TO_SEND;
	}
#endif

	/* Have we run out of file data to send? If so, we need to read the next block from the file. */
	if (mhc->left == 0 )
	{
		if (!_mhttpSendCheckEof(mhc))
		{
			return MHTTP_NO_DATA_TO_SEND;
		}
	}

#if	MHTTPD_SSI
	if(mhc->ssi)
	{
		data_to_send = _mhttpSendSsi( mhc);
	}
	else
#endif
	{
		len = (u16_t)LWIP_MIN(mhc->left, 0xffff);

		err = extHttpWrite(mhc, mhc->file, &len, MHTTP_IS_DATA_VOLATILE(mhc));
		if (err == ERR_OK)
		{
			data_to_send = 1;
			mhc->file += len;
			mhc->left -= len;
		}
	}

	if((mhc->left == 0) && (mfsBytesLeft(mhc->handle) <= 0))
	{
		/* We reached the end of the file so this request is done. This adds the FIN flag right into the last data segment. */
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("End of file."));
//		extHttpConnEof( mhc); /* free in state of close */
		return MHTTP_NO_DATA_TO_SEND;
	}
	
	return data_to_send;
}


/** 
 * return length send out
 */
static unsigned short _mhttpSendOneHeader(ExtHttpConn *mhc, const void *hdr, unsigned short len, unsigned short sndBufSize)
{
	err_t err;
	u8_t apiflags = TCP_WRITE_FLAG_COPY|TCP_WRITE_FLAG_MORE;
	unsigned short _len = len;

	if( len > sndBufSize)
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("HDR length %d is more than snd buf length %d", len, sndBufSize) );
		return 0;
	}
	
	err = extHttpWrite(mhc, (const void *)hdr, &_len, apiflags);
	if ((err == ERR_OK) )
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("HDR '%s' , length %d", (char *)hdr, len) );
		return _len;
	}

	return 0;
}


static char _contentLength[64];

static char	_mHttpSendRest(ExtHttpConn *mhc)
{
	err_t err;
	uint32_t len, sndBufSize;

	EXT_ASSERT(("HMC is not REST conn"), HTTPREQ_IS_REST(mhc) );

	sndBufSize = tcp_sndbuf(mhc->pcb);

	if(mhc->contentLength == mhc->dataSendIndex || mhc->contentLength == 0)
	{
		return EXIT_SUCCESS;
	}

	if(mhc->dataSendIndex == 0)
	{/* first packet for this response, so header first */
		const char *hdr = extHttpFindStatusHeader( mhc->httpStatusCode);
			/* How much do we have to send from the current header? */
		len = (u16_t)strlen(hdr);
		sndBufSize -= _mhttpSendOneHeader(mhc, hdr, len, sndBufSize);

		len = (u16_t)strlen(httpCommonHeader);
		sndBufSize -= _mhttpSendOneHeader(mhc, httpCommonHeader, len, sndBufSize);

		len = snprintf(_contentLength, sizeof(_contentLength), "Content-Length: %"U32_F MHTTP_CRLF MHTTP_CRLF, mhc->contentLength);
		sndBufSize -= _mhttpSendOneHeader(mhc, (const void *)_contentLength, len, sndBufSize);
	}
	
	len = mhc->contentLength-mhc->dataSendIndex;
	err = extHttpWrite(mhc, mhc->data+mhc->dataSendIndex, (uint16_t *)&len, MHTTP_IS_DATA_VOLATILE(mhc));
	if (err != ERR_OK)
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG,("Output Failed: Content %"U32_F" bytes '%s':", len, (char *)mhc->data+mhc->dataSendIndex));
		return EXIT_FAILURE;
	}
	
	EXT_DEBUGF(EXT_HTTPD_DEBUG,("Output Content %"U32_F" bytes '%s':", len, (char *)mhc->data+mhc->dataSendIndex));
	CONSOLE_DEBUG_MEM(mhc->data+mhc->dataSendIndex, (uint32_t)len, (uint32_t)mhc->dataSendIndex, "HTTP Output Data");
	mhc->dataSendIndex += len;

	if((mhc->contentLength == mhc->dataSendIndex) )
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("%"U32_F" byte data sent, End of REST HMC", mhc->dataSendIndex));
		extHttpConnEof( mhc);
TRACE();
	}

	return EXIT_SUCCESS;
}


/*
 * Try to send more data on this pcb.
 * This function can be called multiple time, and start write data from last time
 */
u8_t extHttpSend( ExtHttpConn *mhc)
{
	u8_t ret = MHTTP_DATA_TO_SEND_CONTINUE;

	/* If we were passed a NULL state structure pointer, ignore the call. */
	EXT_ASSERT(("MHC is null"), mhc != NULL);

	if(mhc->pcb == NULL)
	{/* ERROR event has been happened, but FSM still handle existed events */
		return MHTTP_DATA_TO_SEND_BREAK;
	}

	if( HTTPREQ_IS_REST(mhc) && HTTP_IS_FINISHED(mhc) )
	{
		if(_mHttpSendRest(mhc) == EXIT_FAILURE)
		{
			ret = MHTTP_NO_DATA_TO_SEND;
		}
	}
	else if( HTTPREQ_IS_FILE(mhc) )
	{
		ret = _mHttpSendFile(mhc);
	}
	else if( HTTPREQ_IS_WEBSOCKET(mhc) )
	{
		if(mhc->httpStatusCode == WEB_RES_REQUEST_OK)
		{
			unsigned short len = mhc->dataSendIndex;
			extHttpWrite(mhc, mhc->data, &len, TCP_WRITE_FLAG_COPY);
			if(len != mhc->dataSendIndex)
			{
				ret = MHTTP_DATA_TO_SEND_CONTINUE;
			}
		}
		else
		{
			ret = MHTTP_NO_DATA_TO_SEND;
		}
	}
	else if( HTTPREQ_IS_CGI(mhc) )
	{
		EXT_DEBUGF(EXT_HTTPD_DATA_DEBUG, ("send CGI web page data (%"U32_F")\"%s\"", mhc->contentLength, mhc->data));
		unsigned short len = mhc->contentLength - mhc->dataSendIndex;
		extHttpWrite(mhc, mhc->data, &len, TCP_WRITE_FLAG_COPY);
		if(len < (mhc->contentLength - mhc->dataSendIndex) )
		{
			ret = MHTTP_DATA_TO_SEND_CONTINUE;
		}
		else
		{
			ret = MHTTP_NO_DATA_TO_SEND;
//			extHttpConnEof(mhc);
		}
		mhc->dataSendIndex += len;
	}
	else if( (HTTPREQ_IS_UPLOAD(mhc) && mhc->uploadStatus >= _UPLOAD_STATUS_COPY) )
	{
#if 1
//		EXT_DEBUGF(EXT_DBG_ON, ("send update firmware data (%"U32_F")\"%s\"", mhc->contentLength, mhc->data));
		unsigned short len = mhc->contentLength - mhc->dataSendIndex;
		extHttpWrite(mhc, mhc->data, &len, TCP_WRITE_FLAG_COPY);
		if(len < (mhc->contentLength - mhc->dataSendIndex) )
		{
			ret = MHTTP_DATA_TO_SEND_CONTINUE;
		}
#else			
		unsigned short len = mhc->updateLength - mhc->updateIndex;

		EXT_DEBUGF(EXT_HTTPD_DATA_DEBUG, ("send Update progress web page, %d bytes %s", len, httpStats.updateProgress));
		if(len > 0)
		{

			extHttpWrite(mhc, httpStats.updateProgress, &len, TCP_WRITE_FLAG_COPY);
			if(len < (mhc->updateLength - mhc->updateIndex) )
			{
				ret = MHTTP_DATA_TO_SEND_CONTINUE;
			}
			else
			{
				ret = MHTTP_NO_DATA_TO_SEND;
//				extHttpConnEof(mhc);
			}
		}
#endif		
		ret = MHTTP_NO_DATA_TO_SEND;
		mhc->uploadStatus = _UPLOAD_STATUS_END;
		mhc->updateLength = 0;
	}
	else
	{
		ret = MHTTP_NO_DATA_TO_SEND;
	}
	
 	EXT_DEBUGF(EXT_HTTPD_DATA_DEBUG, ("%s send_data end: %s", mhc->name,  (ret==MHTTP_NO_DATA_TO_SEND)?"End":(ret==MHTTP_DATA_TO_SEND_CONTINUE)?"More":"Break"));

 	return ret;
}

