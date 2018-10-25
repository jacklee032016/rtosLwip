
#include "lwipExt.h"

#include "http.h"



/** Minimum length for a valid HTTP/0.9 request: "GET /\r\n" -> 7 bytes */
#define	MHTTP_MIN_REQ_LEN			7

#if	MHTTPD_SUPPORT_11_KEEPALIVE
#define	MHTTP11_CONNECTIONKEEPALIVE			"Connection: keep-alive"
#define	MHTTP11_CONNECTIONKEEPALIVE2		"Connection: Keep-Alive"
#endif


#define	__HTTP_CRLF_SIZE		4

static char _httpParseUrl(ExtHttpConn *mhc, unsigned char *data, u16_t data_len)
{
	char		*sp;
	u16_t uri_len;

	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("parsing URI(%d):'%.*s'", data_len, data_len, data));
	sp = lwip_strnstr((char *)data, " ", data_len);
#if	MHTTPD_SUPPORT_V09
	if (sp == NULL)
	{
		/* HTTP 0.9: respond with correct protocol version */
		sp = lwip_strnstr((char *)data + 1, MHTTP_CRLF, data_len);
		mhc->isV09 = 1;

		if (mhc->method == HTTP_METHOD_POST )
		{
			/* HTTP/0.9 does not support POST */
			return EXIT_FAILURE;
		}
	}
#endif /* MHTTPD_SUPPORT_V09 */

	uri_len = (u16_t)(sp - (char *)(data));
	if ((sp == 0) || uri_len <= 0 ) /* validate URL parsing */
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("invalid URI:'%.*s'", data_len, data));
		return EXIT_SUCCESS;
	}

	/* wait for CRLFCRLF (indicating end of HTTP headers) before parsing anything */
	if (lwip_strnstr((char *)data, MHTTP_CRLF MHTTP_CRLF, data_len) != NULL)
	{
//		char *uri = data;
#if	MHTTPD_SUPPORT_11_KEEPALIVE
		/* This is HTTP/1.0 compatible: for strict 1.1, a connection would always be persistent unless "close" was specified. */
		if (!mhc->isV09 && (lwip_strnstr((char *)data, MHTTP11_CONNECTIONKEEPALIVE, data_len) || lwip_strnstr((char *)data, MHTTP11_CONNECTIONKEEPALIVE2, data_len)))
		{
			mhc->keepalive = 1;
		}
		else
		{
			mhc->keepalive = 0;
		}
#endif

		/* null-terminate the METHOD (pbuf is freed anyway wen returning) */
//		*sp1 = 0;
//		uri[uri_len] = 0;
		memset(mhc->uri, 0, sizeof(mhc->uri));
		uri_len = LWIP_MIN(sizeof(mhc->uri), uri_len);
		memcpy(mhc->uri, data, uri_len );
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Received request URI(%hu): '%s'", uri_len, mhc->uri));
		
		mhc->headers = lwip_strnstr((char *)(data+uri_len),  MHTTP_CRLF, data_len - uri_len);
		if(mhc->headers )
		{
			mhc->headers += 2; /* move CRLF */
			unsigned short left = data_len - ((unsigned char *)mhc->headers - data);
			
			char* crlfcrlf = _FIND_HEADER_END(mhc->headers, left);
			if(crlfcrlf != NULL )
			{
			TRACE();
				mhc->headerLength = crlfcrlf - mhc->headers;
			}
			else
			{
				mhc->headerLength = left -__HTTP_CRLF_SIZE;
			}

			mhc->leftData = left - mhc->headerLength - __HTTP_CRLF_SIZE;
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Headers %d bytes: '%.*s'", mhc->headerLength, mhc->headerLength, mhc->headers) );
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Data %d bytes: '%.*s'", mhc->leftData,   mhc->leftData, mhc->headers+mhc->headerLength+__HTTP_CRLF_SIZE)  );
		}
		return EXIT_SUCCESS;
	}
	else
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("invalid URI:'%.*s'", data_len, data));
	}

	return EXIT_FAILURE;
}

/* return length of method, otherwise return 0 */
static int _httpParseMethod(ExtHttpConn *mhc, unsigned char *data, u16_t data_len)
{
	int ret = 0;
	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("CRLF received, parsing request"));

	/* parse method */
	if (!strncmp((char *)data, "GET ", 4)) 
	{
		/* received GET request */
		mhc->method = HTTP_METHOD_GET;
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Received GET request"));
		ret = 4;
	}
	else if (!strncmp((char *)data, "PUT ", 4)) 
	{
		mhc->method = HTTP_METHOD_PUT;
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Received PUT request"));
		ret = 4;
	}
	else if (!strncmp((char *)data, "POST ", 5))
	{
		/* store request type */
		mhc->method = HTTP_METHOD_POST;
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Received POST request"));
		ret = 5;
	}
	else if (!strncmp((char *)data, "DELETE ", 7))
	{
		/* store request type */
		mhc->method = HTTP_METHOD_DELETE;
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Received DELETE request"));
		ret = 7;
	}
	else if (!strncmp((char *)data, "PATCH ", 6))
	{
		/* store request type */
		mhc->method = HTTP_METHOD_PATCH;
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Received PATCH request"));
		ret = 6;
	}
	else
	{
		/* null-terminate the METHOD (pbuf is freed anyway wen returning) */
		data[4] = 0;
		/* unsupported method! */
		EXT_ERRORF( ("Unsupported request method (not implemented): \"%s\"", data));
		//mhttpFindErrorFile(mhc, WEB_RES_NOT_IMP);
//		return 0;
	}
	
	/* if we come here, method is OK, parse URI */
//	left_len = (u16_t)(data_len - ((sp1 +1) - data));
			
	return ret;
}




/* *data: current pointer of data handled; endHeader: the end of HTTP header */
static err_t	__httpHeaderContentLength(ExtHttpConn *ehc )
{
	char *scontent_len_end, *scontent_len;
	char *content_len_num;
	int contentLen;

		/* search for "Content-Length: " */
#define HTTP_HDR_CONTENT_LEN                "Content-Length: "
#define HTTP_HDR_CONTENT_LEN_LEN            16
#define HTTP_HDR_CONTENT_LEN_DIGIT_MAX_LEN  10

	scontent_len = lwip_strnstr((char *)ehc->headers, HTTP_HDR_CONTENT_LEN, ehc->headerLength);
	if (scontent_len == NULL)
	{
		/* If we come here, headers are fully received (double-crlf), but Content-Length
		was not included. Since this is currently the only supported method, we have to fail in this case! */
//		EXT_ERRORF(("Error when parsing Content-Length"));
//		extHttpRestError(ehc, WEB_RES_BAD_REQUEST, "Content-Length is wrong");
		return ERR_ARG;
	}
	
	scontent_len_end = lwip_strnstr(scontent_len + HTTP_HDR_CONTENT_LEN_LEN, MHTTP_CRLF, HTTP_HDR_CONTENT_LEN_DIGIT_MAX_LEN);
	if (scontent_len_end == NULL)
	{
		EXT_ERRORF( ("Error when parsing number in Content-Length: '%s'",scontent_len+HTTP_HDR_CONTENT_LEN_LEN ));
		extHttpRestError(ehc, WEB_RES_BAD_REQUEST, "Content-Length is wrong");
		return ERR_VAL;
	}

	content_len_num = scontent_len + HTTP_HDR_CONTENT_LEN_LEN;
	contentLen = atoi(content_len_num);
			
	if (contentLen == 0)
	{
		/* if atoi returns 0 on error, fix this */
		if ((content_len_num[0] != '0') || (content_len_num[1] != '\r'))
		{
			contentLen = -1;
		}
	}
			
	if (contentLen < 0)
	{
		EXT_ERRORF( ("POST received invalid Content-Length: %s", content_len_num));
		extHttpRestError(ehc, WEB_RES_BAD_REQUEST, "Content-Length is wrong");
		return ERR_VAL;
	}

	ehc->contentLength = contentLen;
	EXT_DEBUGF(EXT_HTTPD_DATA_DEBUG, ("Parsing content length: %d bytes", ehc->contentLength));

	return ERR_OK;
}

char _httpParseHeaders(ExtHttpConn *ehc)
{
	if(ehc->headers == NULL || ehc->headerLength == 0)
	{
		EXT_INFOF(("%s header length is 0", ehc->name) );
		return EXIT_FAILURE;
	}

	/* following check headers */
	/* WebSocket */
	if(extHttpWebSocketParseHeader(ehc) != ERR_INPROGRESS)
	{
		return EXIT_FAILURE;
	}

	/* Update Firmware */
	__httpHeaderContentLength(ehc);
	
	if (HTTP_IS_POST(ehc) )
	{
		if(extHttpPostCheckUpdate(ehc) != ERR_INPROGRESS)
		{
			return EXIT_FAILURE;
		}
	}

	/* following check URL */
#if LWIP_EXT_NMOS
	if(extNmosHandleRequest(ehc) != ERR_INPROGRESS)
	{
		return EXIT_FAILURE;
	}
		
#endif

	if(extHttpWebService(ehc, NULL) == EXIT_SUCCESS)
	{
		return EXIT_SUCCESS;
	}
	else if(extHttpFileFind(ehc) == ERR_OK)
	{
		ehc->reqType = EXT_HTTP_REQ_T_FILE;
		return EXIT_SUCCESS;
	}
	else
	{
	}

	ehc->reqType = EXT_HTTP_REQ_T_REST;
	extHttpRestError(ehc, WEB_RES_NOT_FOUND, "URI not exist in server");


	return EXIT_SUCCESS;
}

static char _httpParseRequest(ExtHttpConn *mhc, unsigned char *data, u16_t data_len)
{
	int ret;
//	err_t err;

	ret = _httpParseMethod(mhc, data, data_len);
	if( ret == 0)
	{
		return EXIT_FAILURE;
	}

	if( _httpParseUrl(mhc, data+ret, data_len-ret) == EXIT_FAILURE)
	{
		return EXIT_FAILURE;
	}

	return _httpParseHeaders(mhc);

	mhc->postDataLeft = 0;
	return EXIT_SUCCESS;
}


/*
* return ERR_OK, goto next state based on reqType; ERR_INPROCESS, stay in same state; others, CLOSE state to close connection
 */
err_t extHttpRequestParse( ExtHttpConn *mhc, struct pbuf *inp)
{
	unsigned char *data;
	char *crlf;
	u16_t data_len;
	struct pbuf *p = inp;
	u16_t clen;
//	err_t err;

	EXT_ASSERT(("p != NULL"), p != NULL);
	EXT_ASSERT(("mhc != NULL"), mhc != NULL);

	if ((mhc->handle != NULL) || (mhc->file != NULL))
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Received data while sending a file"));
		/* already sending a file */
		/* @todo: abort? */
		return ERR_USE;
	}

	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Received %"U16_F" bytes", p->tot_len));
#if 0
	pbuf_copy_partial(p, mhc->data, p->tot_len, 0);
	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("recv:'%s'", mhc->data) );
	CONSOLE_DEBUG_MEM(mhc->data, p->tot_len, 0, "Request Data");
#endif

	/* first check allowed characters in this pbuf? */

	/* enqueue the pbuf */
	if (mhc->req == NULL)
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("First pbuf"));
		mhc->req = p;
	}
	else
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("pbuf enqueued"));
		pbuf_cat(mhc->req, p);
	}
	
	/* increase pbuf ref counter as it is freed when we return but we want to keep it on the req list */
	pbuf_ref(p);

	if (mhc->req->next != NULL)
	{
		data_len = LWIP_MIN(mhc->req->tot_len, sizeof(mhc->data));
		pbuf_copy_partial(mhc->req, mhc->data, data_len, 0);
		mhc->contentLength = data_len;
		data = mhc->data;
	}
	else
	{
		data = p->payload;
		data_len = p->len;
		if (p->len != p->tot_len)
		{
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Warning: incomplete header due to chained pbufs"));
		}
	}

	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("data :'%.*s'", data_len, data ));

	/* received enough data for minimal request? */
	if (data_len >= MHTTP_MIN_REQ_LEN)
	{
		/* wait for CRLF before parsing anything */
		crlf = lwip_strnstr((char *)data, MHTTP_CRLF, data_len);
		if (crlf == NULL)
		{
			goto badrequest;
		}

		if(_httpParseRequest(mhc, data, data_len) == EXIT_SUCCESS)
		{
			return ERR_OK;
		}

	}

	clen = pbuf_clen(mhc->req);
	if ((mhc->req->tot_len <= MHTTPD_REQ_BUFSIZE) && (clen <= MHTTPD_REQ_QUEUELEN))
	{/* request not fully received (too short or CRLF is missing) */
		return ERR_INPROGRESS;
	}
	else
	{
badrequest:
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("bad request"));
		/* could not parse request */
		return mhttpFindErrorFile(mhc, WEB_RES_BAD_REQUEST);
	}
}


