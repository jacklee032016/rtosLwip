
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <string.h>

#include "lwipMux.h"

#include "muxHttp.h"

#ifdef __ARM__
#include "bspHwSpiFlash.h"
#endif

#define	UPLOAD_PROGRESS_BAR		0

extern	const struct _MuxUploadContext muxUpload;

#define	_FIND_HEADER_END(data, data_len) \
		lwip_strnstr((char *)(data), MHTTP_CRLF MHTTP_CRLF, (data_len))


#define	CONTENT_MULTIPART_FORMAT			"multipart/form-data"

#define	HTTP_BUNDARY						"boundary="

#define	UPLOAD_FORM_INPUT				"fileToUpload"

#define	UPLOAD_FORM_FILENAME				"filename"

static int __httpWebPagePrintHeader(char *data, unsigned int size, MuxHttpConn *mhc)
{
	int index = 0;
	
	index += snprintf(data+index, size-index, "HTTP/1.0 200 OK"MUX_NEW_LINE );
	index += snprintf(data+index, size-index, "Server: "MHTTPD_SERVER_AGENT"" MUX_NEW_LINE);
	index += snprintf(data+index, size-index, "Content-type: text/html" MUX_NEW_LINE );
#if UPLOAD_PROGRESS_BAR
	index += snprintf(data+index, size-index, MUX_NEW_LINE MUX_NEW_LINE );
#else
	index += snprintf(data+index, size-index, "Content-Length: 955 " MUX_NEW_LINE MUX_NEW_LINE );
#endif
	
	return index;
}

#if UPLOAD_PROGRESS_BAR
static char _updatePageBegin(MuxHttpConn  *mhc, char *title, char *msg)
{
	int index = 0;
	int headerLength = 0;
	int contentLength = 0;

	memset(mhc->updateProgress, 0, sizeof(mhc->updateProgress));
	index += __httpWebPagePrintHeader((char *)mhc->updateProgress+index, sizeof(mhc->updateProgress)-index, mhc);
	headerLength = index;

//	contentLength += snprintf((char *)mhc->updateProgress+index, sizeof(mhc->updateProgress)-index, "<DIV class=\"title\"><H2>%s</H2></DIV><DIV class=\"fields-info\"><DIV class=\"field\"><LABEL >Result:%s</LABEL></DIV></DIV>",
//		title, msg);

	contentLength += snprintf((char *)mhc->updateProgress+index+contentLength, sizeof(mhc->updateProgress)-index-contentLength, "<DIV class=\"title\"><H2>%s</H2></DIV>", title);

	contentLength += snprintf((char *)mhc->updateProgress+index+contentLength, sizeof(mhc->updateProgress)-index-contentLength, " <progress id='progressBar1' value='0'></progress> <br>");

	contentLength += snprintf((char *)mhc->updateProgress+index+contentLength, sizeof(mhc->updateProgress)-index-contentLength, "<script>");
	contentLength += snprintf((char *)mhc->updateProgress+index+contentLength, sizeof(mhc->updateProgress)-index -contentLength, " document.getElementById('progressBar1').max = %hd; ", mhc->contentLength);
	contentLength += snprintf((char *)mhc->updateProgress+index+contentLength, sizeof(mhc->updateProgress)-index -contentLength, "</script>");

	index += snprintf((char *)mhc->updateProgress+headerLength-8, 5, "%d", contentLength);

	mhc->updateLength = (unsigned short)(index+contentLength);
	mhc->updateIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}

static char _updatePageRefresh(MuxHttpConn  *mhc, char *msg)
{
	int index = 0;

//	index += snprintf((char *)mhc->updateProgress+index, sizeof(mhc->updateProgress)-index, "%s",
//		msg);

	index += snprintf((char *)mhc->updateProgress+index, sizeof(mhc->updateProgress)-index, "<script>");
	index += snprintf((char *)mhc->updateProgress+index, sizeof(mhc->updateProgress)-index, " document.getElementById('progressBar1').value = %d; ", mhc->nodeInfo->runCfg->firmUpdateInfo.size );
	index += snprintf((char *)mhc->updateProgress+index, sizeof(mhc->updateProgress)-index, "</script>");

	mhc->updateLength = (unsigned short)(index);
	mhc->updateIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}

static char _updatePageEnd(MuxHttpConn  *mhc)
{
	int index = 0;

//	index += snprintf((char *)mhc->updateProgress+index, sizeof(mhc->updateProgress)-index, "<DIV id=\"nav\"><a data-text=\"Info\" id=\"nav_info\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">Info</a>", 
//		MUX_WEBPAGE_INFO);
	index += snprintf((char *)mhc->updateProgress+index, sizeof(mhc->updateProgress)-index, "<script>");
	index += snprintf((char *)mhc->updateProgress+index, sizeof(mhc->updateProgress)-index, " document.getElementById('progressBar1').value = %d; ", mhc->contentLength);
	index += snprintf((char *)mhc->updateProgress+index, sizeof(mhc->updateProgress)-index, "</script>");

	mhc->updateLength = (unsigned short)(index);
	mhc->updateIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}
#else
static char _updatePageResult(MuxHttpConn  *mhc, char *title, char *msg)
{
	int index = 0;
	int headerLength = 0;
	int contentLength = 0;

	memset(mhc->updateProgress, 0, sizeof(mhc->updateProgress));
	index += __httpWebPagePrintHeader((char *)mhc->updateProgress+index, sizeof(mhc->updateProgress)-index, mhc);
	headerLength = index;

	contentLength += snprintf((char *)mhc->updateProgress+index+contentLength, sizeof(mhc->updateProgress)-index-contentLength, "<DIV class=\"title\"><H2>%s</H2></DIV>Result:%s. Please reboot system</DIV>",
		title, msg);
#if 0
	contentLength += snprintf((char *)mhc->updateProgress+index+contentLength, sizeof(mhc->updateProgress)-index-contentLength, "<a href=\"%s\">Reboot</a><BR>", MUX_WEBPAGE_REBOOT);
	contentLength += snprintf((char *)mhc->updateProgress+index+contentLength, sizeof(mhc->updateProgress)-index-contentLength, "<a data-text=\"Info\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">Reboot</a>", MUX_WEBPAGE_REBOOT);
#endif
	index += snprintf((char *)mhc->updateProgress+headerLength-8, 5, "%d", contentLength);

	mhc->updateLength = (unsigned short)(index+contentLength);
	mhc->updateIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}

#endif


/** Handle a post request. Called from http_parse_request when method 'POST' is found.
 * @return ERR_OK: POST correctly parsed and accepted by the application.
 *         ERR_INPROGRESS: POST not completely parsed (no error yet)
 *         another err_t: Error parsing POST or denied by the application
 */
char muxHttpPostRequest(MuxHttpConn *mhc, unsigned char *data, u16_t data_len)
{
	char	ret;
	int contentLen;
	char *endOfLine;
//	struct pbuf *inp =  mhc->req;
	
	/* search for end-of-header (first double-CRLF) */
	char* crlfcrlf = _FIND_HEADER_END(data, data_len);

	if (crlfcrlf != NULL)
	{

		contentLen = muxHttpParseContentLength(mhc, data, crlfcrlf);
		if(contentLen<0)
		{
			goto badPostRequest;
		}

		char* boundary = lwip_strnstr((char *)data, HTTP_BUNDARY, crlfcrlf - (char *)data );
		if(boundary )
		{
			mhc->reqType = MUX_HTTP_REQ_T_UPLOAD;
			if( !IS_STRING_EQUAL(mhc->uri, MUX_WEBPAGE_UPDATE_MCU) && !IS_STRING_EQUAL(mhc->uri, MUX_WEBPAGE_UPDATE_FPGA)  )
			{
				goto badPostRequest;
			}	

			mhc->uploadCtx = (struct _MuxUploadContext *)&muxUpload;
//			mhc->uploadCtx->priv = mhc;

			boundary += (9);
//			boundary += (sizeof(char) * 9);
			endOfLine = strchr((char *)boundary, ASCII_KEY_LF);//, crlfcrlf - boundary );
//			endOfLine = strstr((char *)boundary, MHTTP_CRLF MHTTP_CRLF);
			memset(mhc->boundary, 0, sizeof(mhc->boundary) );
			snprintf(mhc->boundary, sizeof(mhc->boundary), "%.*s", endOfLine -1 - boundary, boundary);
			MUX_DEBUGF(MUX_HTTPD_DEBUG, ("UPLOAD: bounary(%d):'%.*s'", strlen(mhc->boundary), strlen(mhc->boundary), mhc->boundary));
		}
		else
		{
			mhc->reqType = MUX_HTTP_REQ_T_REST;
			ret = muxNmosPostDataBegin(mhc, data, data_len);
			if (ret != EXIT_SUCCESS)
			{/* This is URI is not for POST */
				return EXIT_SUCCESS;
			}
		}

		/* try to pass in data of the first pbuf(s) */
		struct pbuf *q = mhc->req;
		u16_t dataOffset = (u16_t)LWIP_MIN(data_len, crlfcrlf + 4 - (char *)data);
		
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Receiving %d data, %d content at offset: %d", data_len, contentLen, dataOffset) );
#if MHTTPD_POST_MANUAL_WND
		mhc->no_auto_wnd = !isAutoWnd;
#endif /* LWIP_HTTPD_POST_MANUAL_WND */

		/* set the Content-Length to be received for this POST */
		mhc->postDataLeft = (u32_t)contentLen;

		/* get to the pbuf where the body starts */
		while((q != NULL) && (q->len <= dataOffset))
		{
			dataOffset -= q->len;
			q = q->next;
		}
		
		if (q != NULL)
		{
			/* hide the remaining HTTP header */
			pbuf_header(q, -(s16_t)dataOffset);
#if	MHTTPD_POST_MANUAL_WND
			if (!isAutoWnd)
			{
				/* already tcp_recved() this data... */
				mhc->unrecved_bytes = q->tot_len;
			}
#endif
			pbuf_ref(q);
			return muxHttpPostRxDataPbuf(mhc, q);
		}
		else if (mhc->postDataLeft == 0)
		{/* when the first pbuf only contains headers, no data */
			q = pbuf_alloc(PBUF_RAW, 0, PBUF_REF);
			return muxHttpPostRxDataPbuf(mhc, q);
		}
		else
		{
			return ERR_OK;
		}
TRACE();
	}

TRACE();
	/* if we come here, the POST is incomplete */
	return ERR_INPROGRESS;

badPostRequest:
TRACE();
	mhc->uploadStatus = _UPLOAD_STATUS_ERROR;

	return ERR_ARG;
}


static char  __findEndingBoundary(MuxHttpConn *mhc)
{
#if 0
	char* boundary = lwip_strnstr((char *)mhc->data, mhc->boundary, mhc->dataSendIndex );
#else
	char* boundary = memmem(mhc->data, mhc->dataSendIndex , mhc->boundary, strlen(mhc->boundary) );
#endif
	if(boundary )
	{
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Boundary found: last packet %d, content %d bytes", mhc->dataSendIndex, (boundary- (char *)mhc->data - 4) ) );
		mhc->dataSendIndex = (boundary- (char *)mhc->data - 4);

//		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("last packet %d bytes:'%.*s'", mhc->dataSendIndex, mhc->dataSendIndex, mhc->data) );
		mhc->uploadStatus = _UPLOAD_STATUS_END;
//		snprintf(mhc->uri, sizeof(mhc->uri), "%d bytes uploaded for '%s'", mhc->dataSendIndex, mhc->filename );
		snprintf(mhc->uri, sizeof(mhc->uri), "%d bytes uploaded for '%s'", mhc->nodeInfo->runCfg->firmUpdateInfo.size, mhc->filename );
		return MUX_TRUE;
	}
#if 0	
	else
	{
		printf("Boundary '%s' NOT found in '%.*s'", mhc->boundary, mhc->dataSendIndex, (char *)mhc->data) ;
		MUX_ERRORF( ("\r\nBoundary '%s' NOT found in '%.*s'\r\n", mhc->boundary, mhc->dataSendIndex, (char *)mhc->data) );
	}
#endif

	return MUX_FALSE;
}

/* save data in block and flash/save into spi/file */
static char __muxHttpPostData(MuxHttpConn *mhc, struct pbuf *p)
{
	unsigned short len, copied = 0;//, total_copied = 0;

	while(copied != p->tot_len)
	{
		len = LWIP_MIN(p->tot_len - copied, (unsigned short)sizeof(mhc->data)-mhc->dataSendIndex );
	
		len = pbuf_copy_partial(p, mhc->data+mhc->dataSendIndex, len, copied );
		mhc->dataSendIndex += len;
		copied += len;

#if 0	
		mhc->data[mhc->dataSendIndex] = 0;
#endif

		if(mhc->dataSendIndex == sizeof(mhc->data) )
		{
			MUX_DEBUGF(MUX_HTTPD_DATA_DEBUG, ("write to flash: packet %d bytes, copied %d len %d byte data", p->tot_len, copied, len) );
			__findEndingBoundary(mhc);
			len =mhc->uploadCtx->write(mhc, mhc->data, mhc->dataSendIndex);
			if( len != mhc->dataSendIndex)
			{
				MUX_ERRORF(("Write %d bytes of %d bytes to %s", len, mhc->dataSendIndex, mhc->filename) );
				return EXIT_FAILURE;
			}

#if UPLOAD_PROGRESS_BAR
			_updatePageRefresh(mhc, "..");
#endif
			mhc->dataSendIndex = 0;
		}

		MUX_DEBUGF(MUX_HTTPD_DATA_DEBUG, ("packet %d bytes, copied %d (total %d)byte data", p->tot_len, copied, mhc->dataSendIndex) );
	}
	

	if(copied == p->tot_len)
	{
#if 1
		pbuf_free(p);
#else
		pbuf_free(mhc->req);
#endif
#if 0
		if(copied != len)
		{
			MUX_INFOF(("Only copied %d bytes from %d byte data", copied, len) );
			return EXIT_FAILURE;
		}
#endif

	}
	else
	{
		MUX_ERRORF(("copied %d bytes from %d byte data", copied, len) );
	}


	return EXIT_SUCCESS;
}

static char muxHttpPostDataRecv(MuxHttpConn *mhc, struct pbuf *p)
{
	unsigned short len;//, copied;
	
//	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("'%s'", (char *)p->payload) );
	if(HTTPREQ_IS_REST(mhc) )
	{
		return muxNmosPostDataRecv(mhc, p);
	}

	if(mhc->uploadStatus == _UPLOAD_STATUS_INIT )
	{
		len = LWIP_MIN(p->tot_len, sizeof(mhc->data));
		//copied = 
		pbuf_copy_partial(p, mhc->data, len, 0);

		char *endOfFilename = NULL;
		char	*filename = lwip_strnstr((char *)mhc->data, UPLOAD_FORM_FILENAME, (unsigned int)len );
		if(filename )
		{
//			filename += ( 10);
			filename += (sizeof(char) * 10);

			endOfFilename = strchr((char *)filename, '"');
			snprintf(mhc->filename, sizeof(mhc->filename), "%.*s", (endOfFilename-filename), filename );
			if(strlen(mhc->filename) == 0)
			{
				mhc->uploadStatus = _UPLOAD_STATUS_ERROR;
				snprintf(mhc->uri, sizeof(mhc->uri), "Error: no file is transmitted"  );

				return muxHttpWebPageResult(mhc,(char *)"Upload Firmware", mhc->uri);
			}
			
			mhc->uploadStatus = _UPLOAD_STATUS_COPY;

			mhc->uploadCtx->open(mhc);

#if UPLOAD_PROGRESS_BAR
			_updatePageBegin(mhc, (char *)"Upload Firmware", mhc->uri);
#endif

			char* crlfcrlf = _FIND_HEADER_END(mhc->data, len);

			u16_t dataOffset = crlfcrlf + 4 - (char *)mhc->data;

//			MUX_DEBUGF(MUX_HTTPD_DEBUG, ("offsetData :%d, %d", dataOffset, p->len) );
			/* get to the pbuf where the body starts */
			while((p != NULL) && (p->len <= dataOffset))
			{
				dataOffset -= p->len;
				p = p->next;
			}
			
			MUX_DEBUGF(MUX_HTTPD_DATA_DEBUG, ("UPLOAD copied %d byte filename: '%s', move header :%d", (endOfFilename-filename), mhc->filename, dataOffset) );
			/* hide the remaining HTTP header */
			pbuf_header(p, -(s16_t)dataOffset);
			mhc->dataSendIndex = 0;
		}
		else
		{
			mhc->uploadStatus = _UPLOAD_STATUS_ERROR;
			return EXIT_FAILURE;
		}
	}

	if(__muxHttpPostData(mhc, p) == EXIT_FAILURE )
	{
		return EXIT_FAILURE;
	}
	
	
	return EXIT_SUCCESS;
}

/* begin to execute on the recevied data of POST request or when conn is closed */
void muxHttpPostDataFinished(MuxHttpConn *mhc)
{
//	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("POST request on '%s' ended: '%.*s' bounary:'%s'", mhc->uri, mhc->dataSendIndex, mhc->data, mhc->boundary) );
//	printf("\r\nPOST request on '%s' ended: '%.*s' bounary:'%s'\r\n", mhc->uri, mhc->dataSendIndex, mhc->data, mhc->boundary) ;

	if(HTTPREQ_IS_REST(mhc) )
	{
		return muxNmosPostDataFinished(mhc);
	}

	if(mhc->uploadStatus == _UPLOAD_STATUS_ERROR || mhc->uploadStatus == _UPLOAD_STATUS_END)
	{
		MUX_ERRORF(("POST request has been handled"));
		return;
	}

	MUX_DEBUGF(MUX_HTTPD_DATA_DEBUG, ("UPLOAD last packet") );

	if(__findEndingBoundary(mhc) )
	{
		snprintf(mhc->uri, sizeof(mhc->uri), "%d bytes uploaded for '%s'", mhc->nodeInfo->runCfg->firmUpdateInfo.size+ mhc->dataSendIndex, mhc->filename );
	}
	else
	{
		mhc->uploadStatus = _UPLOAD_STATUS_ERROR;
		snprintf(mhc->uri, sizeof(mhc->uri), "Error %d bytes uploaded for %s, no boundary found", mhc->nodeInfo->runCfg->firmUpdateInfo.size+ mhc->dataSendIndex, mhc->filename );
	}

	mhc->uploadCtx->write(mhc, mhc->data, mhc->dataSendIndex);

	mhc->uploadCtx->close(mhc);

#if UPLOAD_PROGRESS_BAR
	_updatePageEnd(mhc);
#endif
	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("POST upload %d byte into file %s", mhc->nodeInfo->runCfg->firmUpdateInfo.size, mhc->filename ) );
	
//	mhc->uploadCtx->priv = NULL;
	mhc->uploadCtx = NULL;

#if UPLOAD_PROGRESS_BAR
#else
	_updatePageResult(mhc, (char *)"Upload Firmware", mhc->uri);
#endif
//	TRACE();
}


/* recv data from pbuf; called when request is received or more data received */
/** Pass received POST body data to the application and correctly handle
 * returning a response document or closing the connection.
 * ATTENTION: The application is responsible for the pbuf now, so don't free it!
 *
 * @param mhc http connection state
 * @param p pbuf to pass to the application
 * @return ERR_OK if passed successfully, another err_t if the response file
 *         hasn't been found (after POST finished)
 */
err_t muxHttpPostRxDataPbuf(MuxHttpConn *mhc, struct pbuf *p)
{
	err_t err;
	unsigned char ret;

	MUX_DEBUGF(MUX_HTTPD_DATA_DEBUG, ("p->len:%d; p->tot_len:%d(postDataLeft:%ld);", p->len, p->tot_len, mhc->postDataLeft ) );

	if (p != NULL)
	{
		/* adjust remaining Content-Length */
		if (mhc->postDataLeft < p->tot_len)
		{
			mhc->postDataLeft = 0;
		}
		else
		{
			mhc->postDataLeft -= p->tot_len;
		}
	}
#if	MHTTPD_POST_MANUAL_WND
	/* prevent connection being closed if httpd_post_data_recved() is called nested */
	mhc->unrecved_bytes++;
#endif

	ret = muxHttpPostDataRecv(mhc, p);
#if	MHTTPD_POST_MANUAL_WND
	mhc->unrecved_bytes--;
#endif
	if (ret != EXIT_SUCCESS)
	{
		/* Ignore remaining content in case of application error */
		mhc->postDataLeft = 0;
		err = ERR_ARG;
		return err;
	}

	if (mhc->postDataLeft == 0)
	{
#if	MHTTPD_POST_MANUAL_WND
		if (mhc->unrecved_bytes != 0)
		{
			return ERR_OK;
		}
#endif
		/* application error or POST finished */
#if	MHTTPD_POST_MANUAL_WND
		/* Prevent multiple calls to mhttpdPostFinished, since it might have already
		been called before from httpd_post_data_recved(). */
		if (mhc->post_finished)
		{
			return ERR_OK;
		}
		
		mhc->post_finished = 1;
#endif

		muxHttpPostDataFinished(mhc);
		return ERR_OK;
	}

	return ERR_OK;
}

