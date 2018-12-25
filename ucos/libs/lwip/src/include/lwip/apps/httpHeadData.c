
/*
* lwip/apps/mhttp.c, this file in included both in tool of fs and mhttp service
*/

/** A list of strings used in HTTP headers (see RFC 1945 HTTP/1.0 and
 * RFC 2616 HTTP/1.1 for header field definitions) */
static const char * const	mHTTPHeaderStrings[] =
{
	"HTTP/1.0 200 OK\r\n",
	"HTTP/1.0 404 File not found\r\n",
	"HTTP/1.0 400 Bad Request\r\n",
	"HTTP/1.0 501 Not Implemented\r\n",
	"HTTP/1.1 200 OK\r\n",
	"HTTP/1.1 404 File not found\r\n",
	"HTTP/1.1 400 Bad Request\r\n",
	"HTTP/1.1 501 Not Implemented\r\n",
	"Content-Length: ",
	"Connection: Close\r\n",
	"Connection: keep-alive\r\n",
	"Connection: keep-alive\r\nContent-Length: ",
	"Server: "MHTTPD_SERVER_AGENT"\r\n",
	"\r\n<html><body><h2>404: The requested file cannot be found.</h2></body></html>\r\n"
#if	MHTTPD_SUPPORT_11_KEEPALIVE
	,"Connection: keep-alive\r\nContent-Length: 77\r\n\r\n<html><body><h2>404: The requested file cannot be found.</h2></body></html>\r\n"
#endif
};

/** A list of extension-to-HTTP header strings (see outdated RFC 1700 MEDIA TYPES
 * and http://www.iana.org/assignments/media-types for registered content types
 * and subtypes) */
static const MHTTP_HEAD mHTTPHeaders[] =
{
	{ "html", MHTTP_HDR_HTML},
	{ "htm",  MHTTP_HDR_HTML},
	{ "shtml",MHTTP_HDR_SSI},
	{ "shtm", MHTTP_HDR_SSI},
	{ "ssi",  MHTTP_HDR_SSI},
	{ "gif",  MHTTP_HDR_GIF},
	{ "png",  MHTTP_HDR_PNG},
	{ "jpg",  MHTTP_HDR_JPG},
	{ "bmp",  MHTTP_HDR_BMP},
	{ "ico",  MHTTP_HDR_ICO},
#if 0	
	{ "class", MHTTP_HDR_APP},
	{ "cls",  MHTTP_HDR_APP},
#endif	
	{ "js",   MHTTP_HDR_JS},
	{ "ram",  MHTTP_HDR_RA},
	{ "css",  MHTTP_HDR_CSS},
	{ "swf",  MHTTP_HDR_SWF},
	{ "xml",  MHTTP_HDR_XML},
	{ "xsl",  MHTTP_HDR_XML},
	{ "pdf",  MHTTP_HDR_PDF},
	{ "json", MHTTP_HDR_JSON}
};


#define NUM_MHTTP_HEADERS		(sizeof(mHTTPHeaders)/sizeof(MHTTP_HEAD))


#if	MHTTPD_SSI
static const char * const mSSIExtensions[] =
{
	".shtml", ".shtm", ".ssi", ".xml"
};
#define NUM_MSHTML_EXTENSIONS (sizeof(mSSIExtensions) / sizeof(const char *))

#endif


