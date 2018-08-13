
/*
* header only for http service
*/

#ifndef	__EXT_HTTP_H__
#define	__EXT_HTTP_H__

/**
 *
 * rudimentary server-side-include facility which will replace tags of the form
 * <!--#tag--> in any file whose extension is .shtml, .shtm or .ssi with
 * strings provided by an include handler whose pointer is provided to the
 * module via function http_set_ssi_handler().
 
 * Additionally, a simple common gateway interface (CGI) handling mechanism has been added to allow clients
 * to hook functions to particular request URIs.
 *
 * To enable SSI support, define label LWIP_HTTPD_SSI in lwipopts.h.
 * To enable CGI support, define label LWIP_HTTPD_CGI in lwipopts.h.
 *
 * By default, the server assumes that HTTP headers are already present in
 * each file stored in the file system.  By defining LWIP_HTTPD_DYNAMIC_HEADERS in
 * lwipopts.h, this behavior can be changed such that the server inserts the
 * headers automatically based on the extension of the file being served.  If
 * this mode is used, be careful to ensure that the file system image used
 * does not already contain the header information.
 *
 * File system images without headers can be created using the makefsfile
 * tool with the -h command line option.
 *
 *
 * Notes about valid SSI tags
 * --------------------------
 *
 * The following assumptions are made about tags used in SSI markers:
 *
 * 1. No tag may contain '-' or whitespace characters within the tag name.
 * 2. Whitespace is allowed between the tag leadin "<!--#" and the start of
 *    the tag name and between the tag name and the leadout string "-->".
 * 3. The maximum tag name length is LWIP_HTTPD_MAX_TAG_NAME_LEN, currently 8 characters.
 *
 * Notes on CGI usage
 * ------------------
 *
 * The simple CGI support offered here works with GET method requests only
 * and can handle up to 16 parameters encoded into the URI. The handler
 * function may not write directly to the HTTP output but must return a
 * filename that the HTTP server will send to the browser as a response to
 * the incoming CGI request.
 *
 *
 *
 * The list of supported file types is quite short, so if makefsdata complains
 * about an unknown extension, make sure to add it (and its doctype) to
 * the 'g_psHTTPHeaders' list.
 */

#include "lwip/opt.h"

#include "lwip/init.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/apps/httpApp.h"
#include "lwip/apps/httpFs.h"
#include "lwip/def.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"

#include <string.h> /* memset */
#include <stdlib.h> /* atoi */
#include <stdio.h>

#include "lwipExt.h"


#define	MHTTP_CRLF					"\r\n"

/** These defines check whether tcp_write has to copy data or not */

/** This was TI's check whether to let TCP copy data or not
 * \#define HTTP_IS_DATA_VOLATILE(mhc) ((mhc->file < (char *)0x20000000) ? 0 : TCP_WRITE_FLAG_COPY)
 */
#ifndef	MHTTP_IS_DATA_VOLATILE
#if	MHTTPD_SSI
/* Copy for SSI files, no copy for non-SSI files */
#define	MHTTP_IS_DATA_VOLATILE(mhc)   ((mhc)->ssi ? TCP_WRITE_FLAG_COPY : 0)
#else
/** Default: don't copy if the data is sent from file-system directly */
#define	MHTTP_IS_DATA_VOLATILE(mhc)	(((mhc->file != NULL) && (mhc->handle != NULL) && (mhc->file == \
                                   (const char*)mhc->handle->data + mhc->handle->len - mhc->left)) \
                                   ? 0 : TCP_WRITE_FLAG_COPY)
#endif
#endif

/** Default: headers are sent from ROM */
#ifndef	MHTTP_IS_HDR_VOLATILE
#define	MHTTP_IS_HDR_VOLATILE(mhc, ptr)			0
#endif

/* Return values for http_send_*() */
#define	MHTTP_DATA_TO_SEND_BREAK			2
#define	MHTTP_DATA_TO_SEND_CONTINUE			1
#define	MHTTP_NO_DATA_TO_SEND				0

/* WS, Web Socket*/
enum
{
	WS_TEXT_MODE	= 0x01,
	WS_BIN_MODE	= 0x02,
}WS_MODE;

#define	EXT_WS_FRAME_FLAG_FIN				0x80


/* low 4 bits in of firast byte in WS frame */
enum
{
	EXT_WS_CODE_TEXT		= 0x01,
	EXT_WS_CODE_BINARY	= 0x02,
	
	EXT_WS_CODE_CLOSE	= 0x08,
	EXT_WS_CODE_PING		= 0x09,
	EXT_WS_CODE_PONG	= 0x0A,
}EXT_WS_CODE_T;

typedef void (*tWsHandler)(struct tcp_pcb *pcb, uint8_t *data, u16_t data_len, uint8_t mode);
typedef void (*tWsOpenHandler)(struct tcp_pcb *pcb, const char *uri);

/**
 * Write data into a websocket.
 *
 * @param pcb tcp_pcb to send.
 * @param data data to send.
 * @param len data length.
 * @param mode WS_TEXT_MODE or WS_BIN_MODE.
 * @return ERR_OK if write succeeded.
 */
err_t websocket_write(struct tcp_pcb *pcb, const uint8_t *data, uint16_t len, uint8_t mode);

/**
 * Register websocket callback functions. Use NULL if callback is not needed.
 *
 * @param ws_open_cb called when new websocket is opened.
 * @param ws_cb called when data is received from client.
 */
void websocket_register_callbacks(tWsOpenHandler ws_open_cb, tWsHandler ws_cb);



#if	MHTTPD_POST_MAX_RESPONSE_URI_LEN > MHTTPD_MAX_REQUEST_URI_LEN
#define	MHTTPD_URI_BUF_LEN		MHTTPD_POST_MAX_RESPONSE_URI_LEN
#endif

#ifndef	MHTTPD_URI_BUF_LEN
#define	MHTTPD_URI_BUF_LEN		MHTTPD_MAX_REQUEST_URI_LEN
#endif


/* The number of individual strings that comprise the headers sent before each requested file.
 */
#define	MNUM_FILE_HDR_STRINGS						5

#define	MHDR_STRINGS_IDX_HTTP_STATUS					0 /* e.g. "HTTP/1.0 200 OK\r\n" */
#define	MHDR_STRINGS_IDX_SERVER_NAME				1 /* e.g. "Server: "HTTPD_SERVER_AGENT"\r\n" */
#define	MHDR_STRINGS_IDX_CONTENT_LEN_KEPALIVE		2 /* e.g. "Content-Length: xy\r\n" and/or "Connection: keep-alive\r\n" */
#define	MHDR_STRINGS_IDX_CONTENT_LEN_NR				3 /* the byte count, when content-length is used */
#define	MHDR_STRINGS_IDX_CONTENT_TYPE				4 /* the content type (or default answer content type including default document) */

/* The dynamically generated Content-Length buffer needs space for CRLF + NULL */
#define	MHTTPD_MAX_CONTENT_LEN_OFFSET 3
#ifndef	MHTTPD_MAX_CONTENT_LEN_SIZE
/* The dynamically generated Content-Length buffer shall be able to work with ~953 MB (9 digits) */
#define	MHTTPD_MAX_CONTENT_LEN_SIZE   (9 + MHTTPD_MAX_CONTENT_LEN_OFFSET)
#endif

#define	EXT_NMOS_MAX_HTTP_DHRS				4


#if	MHTTPD_SSI

enum mtag_check_state
{
	TAG_NONE,       /* Not processing an SSI tag */
	TAG_LEADIN,     /* Tag lead in "<!--#" being processed */
	TAG_FOUND,      /* Tag name being read, looking for lead-out start */
	TAG_LEADOUT,    /* Tag lead out "-->" being processed */
	TAG_SENDING     /* Sending tag replacement string */
};

struct mhttp_ssi_state
{
	const char				*parsed;     /* Pointer to the first unparsed byte in buf. */
#if !MHTTPD_SSI_INCLUDE_TAG
	const char				*tag_started;/* Pointer to the first opening '<' of the tag. */
#endif

	const char				*tag_end;    /* Pointer to char after the closing '>' of the tag. */
	u32_t					parse_left; /* Number of unparsed bytes in buf. */
	u16_t					tag_index;   /* Counter used by tag parsing state machine */
	u16_t					tag_insert_len; /* Length of insert in string tag_insert */
	
#if	MHTTPD_SSI_MULTIPART
	u16_t					tag_part; /* Counter passed to and changed by tag insertion function to insert multiple times */
#endif

	u8_t						tag_name_len; /* Length of the tag name in string tag_name */
	char						tag_name[MHTTPD_MAX_TAG_NAME_LEN + 1]; /* Last tag name extracted */
	char						tag_insert[MHTTPD_MAX_TAG_INSERT_LEN + 1]; /* Insert string for tag_name */
	enum mtag_check_state	tag_state; /* State of the tag processor */
};
#endif /* LWIP_HTTPD_SSI */


typedef	enum
{
	HTTP_METHOD_UNKNOWN = 0,
	HTTP_METHOD_GET,
	HTTP_METHOD_POST,
	HTTP_METHOD_PUT,
	HTTP_METHOD_DELETE,
	HTTP_METHOD_PATCH
}HTTP_METHOD_T;

typedef	enum
{
	EXT_HTTP_REQ_T_REST = 0,
	EXT_HTTP_REQ_T_FILE,
	EXT_HTTP_REQ_T_WEBSOCKET,
	EXT_HTTP_REQ_T_CGI,		/* web page of info/status, which are created dynamically*/
	EXT_HTTP_REQ_T_UPLOAD,	/* POST data for upload */
}EXT_HTTP_REQ_T;



#define	NMOS_URI_RESOURCE_LENGTH	16
#define	NMOS_URI_RESOURCE_MAX		4

typedef	struct
{
	unsigned char		versionFlags;

	unsigned char		resourceCount;
	char				resources[NMOS_URI_RESOURCE_MAX][NMOS_URI_RESOURCE_LENGTH];
	
	EXT_UUID_T		uuid;

	void				*priv;
}MuxNmosApiReq;


typedef	enum
{
	_UPLOAD_STATUS_INIT = 0,
	_UPLOAD_STATUS_COPY,
	_UPLOAD_STATUS_END,
	_UPLOAD_STATUS_ERROR
}_UPLOAD_STATUS;


struct _MuxHttpConn;
struct _MuxUploadContext;

struct _MuxUploadContext
{
	char (*open)(struct _MuxHttpConn *mhc);

	void (*close)(struct _MuxHttpConn *mhc);

	unsigned short (*write)(struct _MuxHttpConn *mhc, void *data, unsigned short size);

	// void 	*priv;*//
};


struct _MuxUploadContext 	MuxUploadContext;




typedef struct _MuxHttpConn
{
#if	MHTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
	struct _MuxHttpConn		*next;
#endif /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */

	struct mfs_file			file_handle;
	struct mfs_file			*handle;	/* if associated with a file */
	const char				*file;       /* Pointer to first unsent byte in buf. which is ROM for file */


	struct tcp_pcb			*pcb;
	struct pbuf				*req;


	HTTP_METHOD_T			method;
	unsigned char				isV09;

	char						uri[MHTTPD_URI_BUF_LEN+1];

	/* HTTP upload file */
	char						boundary[MHTTPD_URI_BUF_LEN+1];		/* for HTTP upload */
	char						filename[MHTTPD_URI_BUF_LEN+1];
	char						uploadStatus;
	unsigned short			recvLength;
	struct _MuxUploadContext  *uploadCtx;

	EXT_HTTP_REQ_T			reqType;
	unsigned char				data[MHTTPD_MAX_REQ_LENGTH];		/* used for request and response both */

	unsigned short			dataSendIndex;
	unsigned short			contentLength;

	unsigned char				updateProgress[1024];		/* used for request and response both */
	unsigned short			updateIndex;
	unsigned short			updateLength;
	
	unsigned	short			httpStatusCode;	/* 200, 400, etc */


#if	MHTTPD_DYNAMIC_FILE_READ
	char						*buf;        /* File read buffer. */
	int						buf_len;      /* Size of file read buffer, buf. */
#endif

	u32_t					left;       /* Number of unsent bytes in buf. */
	u8_t						retries;
#if	MHTTPD_SUPPORT_11_KEEPALIVE
	u8_t						keepalive;
#endif

#if	MHTTPD_SSI
	struct mhttp_ssi_state		*ssi;
#endif

#if	MHTTPD_CGI
	char					*params[MHTTPD_MAX_CGI_PARAMETERS]; /* Params extracted from the request URI */
	char					*param_vals[MHTTPD_MAX_CGI_PARAMETERS]; /* Values for each extracted param */
#endif

#if 0
	const char			*hdrs[MNUM_FILE_HDR_STRINGS]; /* HTTP headers to be sent. */
	char					hdr_content_len[MHTTPD_MAX_CONTENT_LEN_SIZE];
	u16_t				hdr_pos;     /* The position of the first unsent header byte in the current string */
	u16_t				hdr_index;   /* The index of the hdr string currently being sent. */	
#else
#endif

#if	MHTTPD_TIMING
	u32_t					time_started;
#endif


	u32_t				postDataLeft;
#if	MHTTPD_POST_MANUAL_WND
	u32_t				unrecved_bytes;
	u8_t					no_auto_wnd;
	u8_t					post_finished;
#endif


	unsigned int			startTimestamp;	/* ms */

	MuxNmosApiReq		apiReq;

//	void					*priv;
	MuxNmosNode			*nodeInfo;
}MuxHttpConn;


#define	HTTP_IS_GET(mhc)	\
			( (mhc)->method == HTTP_METHOD_GET)


#define	HTTP_IS_POST(mhc)	\
			( (mhc)->method == HTTP_METHOD_POST)

#define	HTTP_IS_PUT(mhc)	\
			( (mhc)->method == HTTP_METHOD_PUT)

#define	HTTP_IS_DELETE(mhc)	\
			( (mhc)->method == HTTP_METHOD_DELETE)


#define	HTTPREQ_IS_REST(mhc)	\
			( (mhc)->reqType == EXT_HTTP_REQ_T_REST )

#define	HTTPREQ_IS_FILE(mhc)	\
			( (mhc)->reqType == EXT_HTTP_REQ_T_FILE )

#define	HTTPREQ_IS_WEBSOCKET(mhc)	\
			( (mhc)->reqType == EXT_HTTP_REQ_T_WEBSOCKET )


#define	HTTPREQ_IS_CGI(mhc)	\
			( (mhc)->reqType == EXT_HTTP_REQ_T_CGI )

#define	HTTPREQ_IS_UPLOAD(mhc)	\
			( (mhc)->reqType == EXT_HTTP_REQ_T_UPLOAD )



#define	HTTP_IS_ERROR(mhc)	\
			( (mhc)->httpStatusCode >= WEB_RES_BAD_REQUEST)

#define	HTTP_IS_FINISHED(mhc)	\
			( (mhc)->httpStatusCode >= WEB_RES_REQUEST_OK)




typedef struct
{
	const char				*uri;	/* when command is NULL, this is end of handlers table */

	HTTP_METHOD_T			method;
	
	MuxHttpCallback			handler;	/* when handler is NULL, this command is not handled now */
}MuxHttpHandle;



#define	EXT_WEBPAGE_INFO						"/info"
#define	EXT_WEBPAGE_MEDIA					"/media"
#define	EXT_WEBPAGE_UPDATE_MCU				"/mcuUpdate"
#define	EXT_WEBPAGE_UPDATE_FPGA			"/fpgaUpdate"

#define	EXT_WEBPAGE_REBOOT					"/reboot"


#define	EXT_WEBPAGE_UPDATE_MCU_HTML		"/upgradeMcu.html"
#define	EXT_WEBPAGE_UPDATE_FPGA_HTML		"/upgradeFpga.html"


#if MHTTPD_USE_MEM_POOL
	LWIP_MEMPOOL_DECLARE(MHTTPD_STATE,     MEMP_NUM_PARALLEL_HTTPD_CONNS,     sizeof(MuxHttpConn),     "MHTTPD_STATE")
	#if	MHTTPD_SSI
		LWIP_MEMPOOL_DECLARE(HTTPD_SSI_STATE, MEMP_NUM_PARALLEL_HTTPD_SSI_CONNS, sizeof(struct mhttp_ssi_state), "MHTTPD_SSI_STATE")
		#define HTTP_FREE_SSI_STATE(x)  LWIP_MEMPOOL_FREE(HTTPD_SSI_STATE, (x))
		#define HTTP_ALLOC_SSI_STATE()  (struct mhttp_ssi_state *)LWIP_MEMPOOL_ALLOC(HTTPD_SSI_STATE)
	#endif
	
	#define HTTP_ALLOC_HTTP_STATE() (MuxHttpConn *)LWIP_MEMPOOL_ALLOC(HTTPD_STATE)
	#define HTTP_FREE_HTTP_STATE(x) LWIP_MEMPOOL_FREE(HTTPD_STATE, (x))
#else
	#define HTTP_ALLOC_HTTP_STATE() (MuxHttpConn *)mem_malloc(sizeof(MuxHttpConn))
	#define HTTP_FREE_HTTP_STATE(x) mem_free(x)
	#if	MHTTPD_SSI
		#define HTTP_ALLOC_SSI_STATE()  (struct mhttp_ssi_state *)mem_malloc(sizeof(struct mhttp_ssi_state))
		#define HTTP_FREE_SSI_STATE(x)  mem_free(x)
	#endif
#endif

MuxHttpConn *mhttpConnAlloc(void);
void mhttpConnFree(MuxHttpConn *mhc);



err_t mhttpConnClose(MuxHttpConn *mhc, struct tcp_pcb *pcb);

err_t extHttpFileFind(MuxHttpConn *mhc);

err_t mhttpPoll(void *arg, struct tcp_pcb *pcb);

#if	MHTTPD_FS_ASYNC_READ
void mhttpContinue(void *connection);
#endif


u8_t extHttpSend(MuxHttpConn *mhc);

err_t extHttpWrite(MuxHttpConn *mhc, const void* ptr, u16_t *length, u8_t apiflags);

void mhttpConnEof(MuxHttpConn *mhc);


#if	MHTTPD_URI_BUF_LEN
extern char mhttpUriBuf[MHTTPD_URI_BUF_LEN+1];
#endif

err_t extHttpRequestParse( MuxHttpConn *mhc, struct pbuf *inp);


#if	MHTTPD_SUPPORT_EXTSTATUS
err_t mhttpFindErrorFile(MuxHttpConn *mhc, u16_t error_nr);
#else
#define mhttpFindErrorFile(mhc, error_nr)		ERR_ARG
#endif


err_t extHttpPostRxDataPbuf(MuxHttpConn *mhc, struct pbuf *p);

char extHttpHandleRequest(MuxHttpConn *mhc);

char extHttpWebSocketParseHeader(MuxHttpConn *mhc, unsigned char *data, u16_t data_len);
err_t extHttpWebSocketParseFrame(MuxHttpConn *mhc, struct pbuf *p);

err_t extHttpWebSocketSendClose(MuxHttpConn *mhc);
unsigned short  extHttpWebSocketWrite(MuxHttpConn *mhc, const uint8_t *data, uint16_t len, unsigned char opCode);


const char *extHttpFindStatusHeader(unsigned short httpStatusCode);

char	extHttpRestError(MuxHttpConn *mhc, unsigned short httpErrorCode, const char *debug);


char extHttpWebPageRootHander(MuxHttpConn  *mhc, void *data);

char extHttpWebService(MuxHttpConn *mhc, void *data);
char extHttpWebPageResult(MuxHttpConn  *mhc, char *title, char *msg);


int	 extHttpParseContentLength(MuxHttpConn *mhc, unsigned char *data, char *endHeader);

char extHttpPostDataBegin(MuxHttpConn *mhc, unsigned char *data, unsigned short len, unsigned char *postAutoWnd);
void extHttpPostDataFinished(MuxHttpConn *mhc);
char extHttpPostRequest(MuxHttpConn *mhc, unsigned char *data, u16_t data_len);


char extNmosRootApHander(MuxHttpConn  *mhc, void *data);
char extNmosNodeDumpHander(MuxHttpConn  *mhc, void *data);


extern	const char *httpCommonHeader;


#endif

