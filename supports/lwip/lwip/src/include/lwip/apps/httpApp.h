
/*
* header used in both HTTP tool of fs and http service
*/

#ifndef	__HTTP_APP_H__
#define	__HTTP_APP_H__

#include "lwip/apps/httpOpts.h"
#include "lwip/apps/httpFs.h"


#if	MHTTPD_DYNAMIC_HEADERS
/** This struct is used for a list of HTTP header strings for various filename extensions. */
typedef struct
{
	const char	*extension;
	const char	*contentType;
}MHTTP_HEAD;


/* Indexes into the mHTTPHeaderStrings array */
#define MHTTP_HDR_OK						0 /* 200 OK */
#define MHTTP_HDR_NOT_FOUND				1 /* 404 File not found */
#define MHTTP_HDR_BAD_REQUEST				2 /* 400 Bad request */
#define MHTTP_HDR_NOT_IMPL				3 /* 501 Not Implemented */
#define MHTTP_HDR_OK_11					4 /* 200 OK */
#define MHTTP_HDR_NOT_FOUND_11			5 /* 404 File not found */
#define MHTTP_HDR_BAD_REQUEST_11			6 /* 400 Bad request */
#define MHTTP_HDR_NOT_IMPL_11				7 /* 501 Not Implemented */
#define MHTTP_HDR_CONTENT_LENGTH			8 /* Content-Length: (HTTP 1.0)*/
#define MHTTP_HDR_CONN_CLOSE				9 /* Connection: Close (HTTP 1.1) */
#define MHTTP_HDR_CONN_KEEPALIVE			10 /* Connection: keep-alive (HTTP 1.1) */
#define MHTTP_HDR_KEEPALIVE_LEN			11 /* Connection: keep-alive + Content-Length: (HTTP 1.1)*/
#define MHTTP_HDR_SERVER					12 /* Server: HTTPD_SERVER_AGENT */
#define MDEFAULT_404_HTML					13 /* default 404 body */
#if	MHTTPD_SUPPORT_11_KEEPALIVE
#define MDEFAULT_404_HTML_PERSISTENT		14 /* default 404 body, but including Connection: keep-alive */
#endif


#define	MHTTP_HDR_HTML				"Content-type: text/html\r\n\r\n"
#define	MHTTP_HDR_SSI					"Content-type: text/html\r\nExpires: Fri, 10 Apr 2008 14:00:00 GMT\r\nPragma: no-cache\r\n\r\n"
#define	MHTTP_HDR_GIF					"Content-type: image/gif\r\n\r\n"
#define	MHTTP_HDR_PNG					"Content-type: image/png\r\n\r\n"
#define	MHTTP_HDR_JPG					"Content-type: image/jpeg\r\n\r\n"
#define	MHTTP_HDR_BMP					"Content-type: image/bmp\r\n\r\n"
#define	MHTTP_HDR_ICO					"Content-type: image/x-icon\r\n\r\n"
#define	MHTTP_HDR_APP					"Content-type: application/octet-stream\r\n\r\n"
#define	MHTTP_HDR_JS					"Content-type: application/javascript\r\n\r\n"
#define	MHTTP_HDR_RA					"Content-type: application/javascript\r\n\r\n"
#define	MHTTP_HDR_CSS					"Content-type: text/css\r\n\r\n"
#define	MHTTP_HDR_SWF				"Content-type: application/x-shockwave-flash\r\n\r\n"
#define	MHTTP_HDR_XML					"Content-type: text/xml\r\n\r\n"
#define	MHTTP_HDR_PDF					"Content-type: application/pdf\r\n\r\n"
#define	MHTTP_HDR_JSON				"Content-type: application/json\r\n\r\n"

#define	MHTTP_HDR_DEFAULT_TYPE		"Content-type: text/plain\r\n\r\n"


#endif


#define	HTTP_NEWLINE     			"\r\n"
#define	HTTP_NEWLINE_LEN			2


typedef enum WEB_RESPONSE_CODE
{
	WEB_RES_CONTINUE = 100,
	WEB_RES_SW_PROTOCOL,
	WEB_RES_REQUEST_OK = 200,
	WEB_RES_CREATED,
	WEB_RES_ACCEPTED,
	WEB_RES_PROVISIONAL,
	WEB_RES_NO_CONTENT,
	WEB_RES_R_205,
	WEB_RES_PARTIAL_CONTENT,
	WEB_RES_MULTIPLE = 300,
	WEB_RES_MOVED_PERM,
	WEB_RES_MOVED_TEMP,
	WEB_RES_303,
	WEB_RES_NOT_MODIFIED,
	WEB_RES_TEMP_REDIRECT = 307,
	WEB_RES_BAD_REQUEST = 400,
	WEB_RES_UNAUTHORIZED,
	WEB_RES_PAYMENT,
	WEB_RES_FORBIDDEN,
	WEB_RES_NOT_FOUND,
	WEB_RES_METHOD_NA, /* 405, method not allowed */
	WEB_RES_NON_ACC,   /* non acceptable */
	WEB_RES_PROXY,     /* proxy auth required */
	WEB_RES_REQUEST_TO, /* request timeout */
	WEB_RES_CONFLICT,
	WEB_RES_GONE,
	WEB_RES_LENGTH_REQUIRED,
	WEB_RES_PRECONDITION_FAILED,
	WEB_RES_413,
	WEB_RES_REQUEST_URI_TOO_LONG,
	WEB_RES_415,
	WEB_RES_INVALID_RANGE,

	WEB_RES_LOCKED = 423,
	
	WEB_RES_ERROR = 500,
	WEB_RES_NOT_IMP = 501,
	WEB_RES_BAD_GATEWAY = 502,
	WEB_RES_SERVICE_UNAV = 503,
	WEB_RES_GATEWAY_TO = 504, /* gateway timeout */
	WEB_RES_BAD_VERSION = 505,

	WEB_RES_ERROR_MAX = 599

}WEB_RESPONSE_CODE;



#endif

