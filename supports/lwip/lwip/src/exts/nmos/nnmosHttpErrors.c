
#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"


const char *httpCommonHeader = EXT_HTTP_HDR_S_SERVER EXT_HTTP_HDR_S_CONTENT_TYPE_JSON;

/* HTTP status header */
const	EXT_CONST_STR	_httpHdrs[] =
{
	{
		type	: WEB_RES_REQUEST_OK,
		name	: EXT_HTTP_HDR_S_STATUS_200
	},
	{
		type	: WEB_RES_SW_PROTOCOL,
		name	: EXT_HTTP_HDR_S_STATUS_101
	},
	{
		type	: WEB_RES_TEMP_REDIRECT,
		name	: EXT_HTTP_HDR_S_STATUS_307
	},
	{
		type	: WEB_RES_BAD_REQUEST,
		name	: EXT_HTTP_HDR_S_STATUS_400
	},
	{
		type	: WEB_RES_NOT_FOUND,
		name	: EXT_HTTP_HDR_S_STATUS_404
	},
	{
		type	: WEB_RES_METHOD_NA,
		name	: EXT_HTTP_HDR_S_STATUS_405
	},
	{
		type	: WEB_RES_LOCKED,
		name	: EXT_HTTP_HDR_S_STATUS_423
	},
	{
		type	: WEB_RES_ERROR,
		name	: EXT_HTTP_HDR_S_STATUS_500
	},
	{
		type	: WEB_RES_NOT_IMP,
		name	: EXT_HTTP_HDR_S_STATUS_501
	},
	

	{
		type	: WEB_RES_ERROR_MAX,
		name	: NULL
	}
};


/* look up status header of HTTP response */
const char *extHttpFindStatusHeader(unsigned short httpStatusCode)
{
	const EXT_CONST_STR *hdr = _httpHdrs;

	while(hdr->type != WEB_RES_ERROR_MAX && hdr->name != NULL )
	{
		if(hdr->type == httpStatusCode)
		{
			return hdr->name;
		}

		hdr++;
	}

	EXT_ERRORF(("HTTP Status header can not found for code %d", httpStatusCode));

	return EXT_HTTP_HDR_S_STATUS_404;
}

char	extHttpRestError(ExtHttpConn *mhc, unsigned short httpErrorCode, const char *debug)
{
	int index = 0;
	const char *statusHdr;
	char *position;

	statusHdr = extHttpFindStatusHeader(httpErrorCode);
	mhc->httpStatusCode = httpErrorCode;
	mhc->reqType = EXT_HTTP_REQ_T_REST;

	statusHdr += 9 + 4;/* sizeof 'HTTP/1.1 ''*/
	position = strstr(statusHdr, MHTTP_CRLF);

	EXT_DEBUGF(EXT_NMOS_DEBUG, ("Request on '%s' is wrong '%s', because %s", mhc->uri, statusHdr, debug));
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "{\""EXT_JSON_KEY_STATUS"\":%d,", httpErrorCode );
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""EXT_JSON_KEY_ERROR"\":\"%.*s\",", (position-statusHdr), statusHdr );
	index += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "\""EXT_JSON_KEY_DEBUG"\":\"%s\"}", debug );

	mhc->contentLength = (unsigned short)index;
	mhc->dataSendIndex = 0;

	return EXIT_SUCCESS;
}


