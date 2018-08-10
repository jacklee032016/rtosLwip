
#include "lwipMux.h"

#include "muxHttp.h"



/** Minimum length for a valid HTTP/0.9 request: "GET /\r\n" -> 7 bytes */
#define	MHTTP_MIN_REQ_LEN			7

#if	MHTTPD_SUPPORT_11_KEEPALIVE
#define	MHTTP11_CONNECTIONKEEPALIVE			"Connection: keep-alive"
#define	MHTTP11_CONNECTIONKEEPALIVE2		"Connection: Keep-Alive"
#endif


static char _mHttpParseUrl(MuxHttpConn *mhc, unsigned char *data, u16_t data_len)
{
	char		*sp;
	u16_t uri_len;

	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("parsing URI(%d):'%s'", data_len, data));
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
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("invalid URI:'%s'", data));
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
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Received request URI(%hu): '%s'", uri_len, mhc->uri));
//		snprintf(mhc->uri, sizeof(mhc->uri), "%s", uri);
		return EXIT_SUCCESS;
	}
	else
	{
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("invalid URI:'%s'", data));
	}

	return EXIT_FAILURE;
}

/* return length of method, otherwise return 0 */
static int _mHttpParseMethod(MuxHttpConn *mhc, unsigned char *data, u16_t data_len)
{
	int ret = 0;
	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("CRLF received, parsing request"));

	/* parse method */
	if (!strncmp((char *)data, "GET ", 4)) 
	{
		/* received GET request */
		mhc->method = HTTP_METHOD_GET;
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Received GET request"));
		ret = 4;
	}
	else if (!strncmp((char *)data, "PUT ", 4)) 
	{
		mhc->method = HTTP_METHOD_PUT;
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Received PUT request"));
		ret = 4;
	}
	else if (!strncmp((char *)data, "POST ", 5))
	{
		/* store request type */
		mhc->method = HTTP_METHOD_POST;
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Received POST request"));
		ret = 5;
	}
	else if (!strncmp((char *)data, "DELETE ", 7))
	{
		/* store request type */
		mhc->method = HTTP_METHOD_DELETE;
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Received DELETE request"));
		ret = 7;
	}
	else if (!strncmp((char *)data, "PATCH ", 6))
	{
		/* store request type */
		mhc->method = HTTP_METHOD_PATCH;
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Received PATCH request"));
		ret = 6;
	}
	else
	{
		/* null-terminate the METHOD (pbuf is freed anyway wen returning) */
		data[4] = 0;
		/* unsupported method! */
		MUX_ERRORF( ("Unsupported request method (not implemented): \"%s\"", data));
		//mhttpFindErrorFile(mhc, WEB_RES_NOT_IMP);
//		return 0;
	}
	
	/* if we come here, method is OK, parse URI */
//	left_len = (u16_t)(data_len - ((sp1 +1) - data));
			
	return ret;
}

static char _mHttpParseRequest(MuxHttpConn *mhc, unsigned char *data, u16_t data_len)
{
	int ret;
//	err_t err;

	ret = _mHttpParseMethod(mhc, data, data_len);
	if( ret == 0)
	{
		return EXIT_FAILURE;
	}

	if( _mHttpParseUrl(mhc, data+ret, data_len-ret) == EXIT_FAILURE)
	{
		return EXIT_FAILURE;
	}

	if (HTTP_IS_POST(mhc) )
	{
		return muxHttpPostRequest(mhc, data, data_len);
	}

	if(muxHttpHandleRequest(mhc) == EXIT_FAILURE)
	{
//		err = ERR_ARG;
		return EXIT_FAILURE;
	}

	mhc->postDataLeft = 0;
	return EXIT_SUCCESS;
}
/**
 * When data has been received in the correct state, try to parse it as a HTTP request.
 * @return ERR_OK if request was OK and mhc has been initialized correctly, response is sent in tcp_recv
 *         ERR_INPROGRESS if request was OK so far but not fully received
 *         another err_t otherwise
 */
err_t muxHttpRequestParse( MuxHttpConn *mhc, struct pbuf *inp)
{
	unsigned char *data;
	char *crlf;
	u16_t data_len;
	struct pbuf *p = inp;
	u16_t clen;
//	err_t err;

	MUX_ASSERT(("p != NULL"), p != NULL);
	MUX_ASSERT(("mhc != NULL"), mhc != NULL);

	if ((mhc->handle != NULL) || (mhc->file != NULL))
	{
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Received data while sending a file"));
		/* already sending a file */
		/* @todo: abort? */
		return ERR_USE;
	}

	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Received %"U16_F" bytes", p->tot_len));
#if 0
	pbuf_copy_partial(p, mhc->data, p->tot_len, 0);
	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("recv:'%s'", mhc->data) );
	CONSOLE_DEBUG_MEM(mhc->data, p->tot_len, 0, "Request Data");
#endif

	/* first check allowed characters in this pbuf? */

	/* enqueue the pbuf */
	if (mhc->req == NULL)
	{
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("First pbuf"));
		mhc->req = p;
	}
	else
	{
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("pbuf enqueued"));
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
			MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Warning: incomplete header due to chained pbufs"));
		}
	}

	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("data %d:'%s'", data_len, data ));

	/* received enough data for minimal request? */
	if (data_len >= MHTTP_MIN_REQ_LEN)
	{
		/* wait for CRLF before parsing anything */
		crlf = lwip_strnstr((char *)data, MHTTP_CRLF, data_len);
		if (crlf == NULL)
		{
			goto badrequest;
		}

		if( muxHttpWebSocketParseHeader(mhc, data, data_len) == EXIT_SUCCESS)
		{
			return ERR_OK;
		}

		if(_mHttpParseRequest(mhc, data, data_len) == EXIT_SUCCESS)
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
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("bad request"));
		/* could not parse request */
		return mhttpFindErrorFile(mhc, WEB_RES_BAD_REQUEST);
	}
}




/* *data: current pointer of data handled; endHeader: the end of HTTP header */
int	 muxHttpParseContentLength(MuxHttpConn *mhc, unsigned char *data, char *endHeader)
{
	char *scontent_len_end, *scontent_len;
	char *content_len_num;
	int contentLen;

		/* search for "Content-Length: " */
#define HTTP_HDR_CONTENT_LEN                "Content-Length: "
#define HTTP_HDR_CONTENT_LEN_LEN            16
#define HTTP_HDR_CONTENT_LEN_DIGIT_MAX_LEN  10

	scontent_len = lwip_strnstr((char *)data, HTTP_HDR_CONTENT_LEN, endHeader - (char *)data );
	if (scontent_len == NULL)
	{
		/* If we come here, headers are fully received (double-crlf), but Content-Length
		was not included. Since this is currently the only supported method, we have to fail in this case! */
		MUX_ERRORF(("Error when parsing Content-Length"));
		muxHttpRestError(mhc, WEB_RES_BAD_REQUEST, "Content-Length is wrong");
		return -1;
	}
	
	scontent_len_end = lwip_strnstr(scontent_len + HTTP_HDR_CONTENT_LEN_LEN, MHTTP_CRLF, HTTP_HDR_CONTENT_LEN_DIGIT_MAX_LEN);
	if (scontent_len_end == NULL)
	{
		MUX_ERRORF( ("Error when parsing number in Content-Length: '%s'",scontent_len+HTTP_HDR_CONTENT_LEN_LEN ));
		muxHttpRestError(mhc, WEB_RES_BAD_REQUEST, "Content-Length is wrong");
		return -1;
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
		MUX_ERRORF( ("POST received invalid Content-Length: %s", content_len_num));
		muxHttpRestError(mhc, WEB_RES_BAD_REQUEST, "Content-Length is wrong");
		return -1;
	}

	MUX_DEBUGF(MUX_HTTPD_DATA_DEBUG, ("Parsing content length: %d bytes", contentLen));

	return contentLen;
}


