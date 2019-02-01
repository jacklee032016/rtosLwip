
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
	EXT_HTTP_REQ_T_UNKNOWN = 0,
	EXT_HTTP_REQ_T_REST,
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


struct _ExtHttpConn;
struct _MuxUploadContext;

struct _MuxUploadContext
{
	char (*open)(struct _ExtHttpConn *mhc);

	void (*close)(struct _ExtHttpConn *mhc, char isFinished);

	unsigned short (*write)(struct _ExtHttpConn *mhc, void *data, unsigned short size);

	// void 	*priv;*//
};


struct _MuxUploadContext 	MuxUploadContext;



#if LWIP_EXT_HTTPD_TASK
typedef	enum
{
	H_EVENT_NEW = EXT_EVENT_NONE +1,
	H_EVENT_RECV,
	H_EVENT_POLL,
	H_EVENT_SENT,

	H_EVENT_CLOSE,

	H_EVENT_ERROR,
	
	H_EVENT_MAX,
}H_EVENT_T;


typedef enum
{
	H_STATE_INIT = EXT_STATE_CONTINUE +1,
	H_STATE_REQ,
	H_STATE_DATA, 
	H_STATE_RESP,
	H_STATE_CLOSE,
	H_STATE_ERROR,		/* error event from TCP stack implementation  */
	H_STATE_FREE,		/* wait to free memory */
	H_STATE_MAX
}H_STATE_T;

#endif


typedef struct _ExtHttpConn
{
#if	MHTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
	struct _ExtHttpConn		*next;
#endif /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */

	H_STATE_T				state;
	unsigned char				eventCount;

	char						name[32];

	struct mfs_file			file_handle;
	struct mfs_file			*handle;	/* if associated with a file */
	const char				*file;       /* Pointer to first unsent byte in buf. which is ROM for file */


	struct tcp_pcb			*pcb;
	struct pbuf				*req;


	HTTP_METHOD_T			method;
	unsigned char				isV09;

	char						uri[MHTTPD_URI_BUF_LEN+1];
	char						*headers;	/* pointer to headers, after url */
	unsigned short			headerLength;
	unsigned short			leftData;		/* left data in the first pbuf which contains URL|headers */

	unsigned short			responseHeaderLength;


	/* HTTP upload file */
	char						boundary[MHTTPD_URI_BUF_LEN+1];		/* for HTTP upload, also as content of error message */
	char						filename[MHTTPD_URI_BUF_LEN+1];		/* also as title of error message */
	char						uploadStatus;

	char						countOfParseFileName;
	unsigned short			recvLength;
	struct _MuxUploadContext  *uploadCtx;

	EXT_HTTP_REQ_T			reqType;
	unsigned char				data[MHTTPD_MAX_REQ_LENGTH];		/* used for request and response both */

	/* must be 32-bit integers */
	uint32_t					dataSendIndex;		/* index of data buffer before writing to flash */
	uint32_t					contentLength;

#if 0
	/* move to HttpState to save memory */
	unsigned char				updateProgress[1024];		/* used for request and response both */
#endif
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


	s32_t				postDataLeft;
#if	MHTTPD_POST_MANUAL_WND
	u32_t				unrecved_bytes;
	u8_t					no_auto_wnd;
	u8_t					post_finished;
#endif


	unsigned int			startTimestamp;	/* ms */

	MuxNmosApiReq		apiReq;

//	void					*priv;
	EXT_RUNTIME_CFG		*runCfg;
#if LWIP_EXT_NMOS
	MuxNmosNode			*nodeInfo;
#endif
}ExtHttpConn;


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

/* HTTP Header ends with CR-LF + CR_LF */
#define	_FIND_HEADER_END(data, data_len) \
		lwip_strnstr((char *)(data), MHTTP_CRLF MHTTP_CRLF, (data_len))

#define	HTTP_SET_FREE(ehc) 		do{(ehc)->state = H_STATE_FREE;}while(0)

#define	HTTP_CHECK_FREE(ehc) 		((ehc)->state == H_STATE_FREE)

/* return content length of response */
typedef uint16_t (*ExtHttpCallback)(struct _ExtHttpConn  *mhc, void *data);

typedef enum
{
	WEB_RESP_HTML,
	WEB_RESP_JSON,
	WEB_RESP_SDP,
	WEB_RESP_UNKNOWN
}WEB_RESP_T;

typedef struct
{
	const char				*uri;	/* when command is NULL, this is end of handlers table */

	HTTP_METHOD_T			method;
	
	ExtHttpCallback			handler;	/* when handler is NULL, this command is not handled now */
	char						respType;	/* response type */
}MuxHttpHandle;



#define	EXT_WEB_CFG_FIELD_MODEL					"model"
#define	EXT_WEB_CFG_FIELD_PRODUCT				"name"
#define	EXT_WEB_CFG_FIELD_VERSION				"version"

#define	EXT_WEB_CFG_FIELD_ADDRESS				"address"
#define	EXT_WEB_CFG_FIELD_NETMASK				"netmask"
#define	EXT_WEB_CFG_FIELD_GATEWAY				"gateway"
#define	EXT_WEB_CFG_FIELD_IS_DHCP				"isDHCP"

#define	EXT_WEB_CFG_FIELD_MAC					"MAC"

#define	EXT_WEB_CFG_FIELD_SDP_VEDIO_IP			"sdpVideoIp"
#define	EXT_WEB_CFG_FIELD_SDP_AUDIO_IP			"sdpAudioIp"

#define	EXT_WEB_CFG_FIELD_SDP_VEDIO_PORT		"sdpVideoPort"
#define	EXT_WEB_CFG_FIELD_SDP_AUDIO_PORT		"sdpAudioPort"

#define	EXT_WEB_CFG_FIELD_SDP_VEDIO_URI			"sdpVideoUri"
#define	EXT_WEB_CFG_FIELD_SDP_AUDIO_URI			"sdpAudioUri"

#define	EXT_WEB_CFG_FIELD_IP_VEDIO				"ipVideo"
#define	EXT_WEB_CFG_FIELD_IP_AUDIO				"ipAudio"
#define	EXT_WEB_CFG_FIELD_IP_ANC					"ipAnc"
#define	EXT_WEB_CFG_FIELD_IP_AUX					"ipAux"

#define	EXT_WEB_CFG_FIELD_PORT_VEDIO			"portVideo"
#define	EXT_WEB_CFG_FIELD_PORT_AUDIO			"portAudio"
#define	EXT_WEB_CFG_FIELD_PORT_DATA				"portData"
#define	EXT_WEB_CFG_FIELD_PORT_STREM			"portStrem"

#define	EXT_WEB_CFG_FIELD_FPGA_AUTO				"mediaSet"

#define	EXT_WEB_CFG_FIELD_FPGA_AUTO_V_AUTO		"Auto"
#define	EXT_WEB_CFG_FIELD_FPGA_AUTO_V_MANUAL		"Manual"


#define	EXT_WEB_CFG_FIELD_VIDEO_PARAMS			"videoParams"		/* replace width/height/framerate/interlaced(segmented). 01.15, 2019 */

#define	EXT_WEB_CFG_FIELD_VIDEO_WIDTH			"videoWidth"
#define	EXT_WEB_CFG_FIELD_VIDEO_HEIGHT			"videoHeight"

#define	EXT_WEB_CFG_FIELD_COLOR_SPACE			"colorSpace"
#define	EXT_WEB_CFG_FIELD_COLOR_DEPTH			"colorDepth"
#define	EXT_WEB_CFG_FIELD_FRAME_RATE			"videoFps"
#define	EXT_WEB_CFG_FIELD_VIDEO_INTERLACE		"videoIsIntlce"
#define	EXT_WEB_CFG_FIELD_VIDEO_SEGMENTED		"videoIsSgmt"

#define	EXT_WEB_CFG_FIELD_AUDIO_CHANNEL		"audioChannels"
#define	EXT_WEB_CFG_FIELD_AUDIO_SAMP_RATE		"audioSampRate"
#define	EXT_WEB_CFG_FIELD_AUDIO_DEPTH			"audioDepth"
#define	EXT_WEB_CFG_FIELD_AUDIO_PKT_SIZE		"audioPktSize"
#define	EXT_WEB_CFG_FIELD_IS_CONNECT			"isConnect"	/* 811 send to TX(stop/start) or RX(connect/disconnect) */

#define	EXT_WEB_CFG_FIELD_RS232_BAUDRATE		"rs232Baudrate"
#define	EXT_WEB_CFG_FIELD_RS232_DATABITS		"rs232DataBit"
#define	EXT_WEB_CFG_FIELD_RS232_PARITY			"rs232Parity"
#define	EXT_WEB_CFG_FIELD_RS232_STOPBITS		"rs232StopBit"


#define	EXT_HTTP_VERSION_11						"HTTP/1.1 "


#define	EXT_HTTP_HDR_S_STATUS_200				EXT_HTTP_VERSION_11"200 OK"MHTTP_CRLF
#define	EXT_HTTP_HDR_S_STATUS_101				EXT_HTTP_VERSION_11"101 Switching Protocols"MHTTP_CRLF"Upgrade: websocket"MHTTP_CRLF"Connection: Upgrade"MHTTP_CRLF

#define	EXT_HTTP_HDR_S_STATUS_307				EXT_HTTP_VERSION_11"307 Temporary Redirect"MHTTP_CRLF
#define	EXT_HTTP_HDR_S_STATUS_400				EXT_HTTP_VERSION_11"400 Bad Request"MHTTP_CRLF
#define	EXT_HTTP_HDR_S_STATUS_404				EXT_HTTP_VERSION_11"404 Not Found"MHTTP_CRLF
#define	EXT_HTTP_HDR_S_STATUS_405				EXT_HTTP_VERSION_11"405 Method Not Allowed"MHTTP_CRLF
#define	EXT_HTTP_HDR_S_STATUS_423				EXT_HTTP_VERSION_11"423 Locked"MHTTP_CRLF
#define	EXT_HTTP_HDR_S_STATUS_500				EXT_HTTP_VERSION_11"500 Internal Server Error"MHTTP_CRLF
#define	EXT_HTTP_HDR_S_STATUS_501				EXT_HTTP_VERSION_11"501 Not Implemented"MHTTP_CRLF
#define	EXT_HTTP_HDR_S_SERVER					"Server: "MHTTPD_SERVER_AGENT MHTTP_CRLF
#define	EXT_HTTP_HDR_S_CONTENT_TYPE_JSON		"Content-Type: application/json"MHTTP_CRLF
#define	EXT_HTTP_HDR_S_CONTENT_LENGTH			"Content-Length: "


#if MHTTPD_USE_MEM_POOL

	//LWIP_MEMPOOL_DECLARE(MHTTPD_STATE,     MEMP_NUM_PARALLEL_HTTPD_CONNS,     sizeof(ExtHttpConn),     "MHTTPD_STATE")
	
	#define HTTP_CONN_ALLOC(ehc) \
		do{HTTP_LOCK(); (ehc)= (ExtHttpConn *)memp_malloc(MEMP_EXT_HTTP_CONN);HTTP_UNLOCK();}while(0)

//		do{HTTP_LOCK(); (ehc)= (ExtHttpConn *)LWIP_MEMPOOL_ALLOC(EXT_HTTP_CONN);HTTP_UNLOCK();}while(0)
		
	#define HTTP_CONN_FREE(ehc) \
		do{HTTP_LOCK(); memp_free(MEMP_EXT_HTTP_CONN, (ehc)); HTTP_UNLOCK();}while(0)

//		do{HTTP_LOCK(); LWIP_MEMPOOL_FREE(EXT_HTTP_CONN, (ehc)); HTTP_UNLOCK();}while(0)
			
	#if	MHTTPD_SSI
		//LWIP_MEMPOOL_DECLARE(HTTPD_SSI_STATE, MEMP_NUM_PARALLEL_HTTPD_SSI_CONNS, sizeof(struct mhttp_ssi_state), "MHTTPD_SSI_STATE")
		#define HTTP_FREE_SSI_STATE(x)  LWIP_MEMPOOL_FREE(HTTPD_SSI_STATE, (x))
		#define HTTP_ALLOC_SSI_STATE()  (struct mhttp_ssi_state *)LWIP_MEMPOOL_ALLOC(HTTPD_SSI_STATE)
	#endif
	
#else

	#define HTTP_CONN_ALLOC(ehc) \
		do{ HTTP_LOCK(); (ehc) =(ExtHttpConn *)mem_malloc(sizeof(ExtHttpConn)); HTTP_UNLOCK();}while(0)
			
	#define HTTP_CONN_FREE(ehc) \
		do{ HTTP_LOCK(); mem_free(ehc); HTTP_UNLOCK();}while(0)
	
	#if	MHTTPD_SSI
		#define HTTP_ALLOC_SSI_STATE()  (struct mhttp_ssi_state *)mem_malloc(sizeof(struct mhttp_ssi_state))
		#define HTTP_FREE_SSI_STATE(x)  mem_free(x)
	#endif
#endif


/* event is managed in task tcp */
#define	HTTP_EVENT_ALLOC(he) \
		do{(he) = (HttpEvent *)memp_malloc(MEMP_EXT_HTTP_EVENT);}while(0)

#define	HTTP_EVENT_FREE(he) \
		do{memp_free(MEMP_EXT_HTTP_EVENT, (he)); (he) = NULL;}while(0)



ExtHttpConn *extHttpConnAlloc(EXT_RUNTIME_CFG *runCfg);
void extHttpConnFree(ExtHttpConn *mhc);

ExtHttpConn *extHttpConnFind(struct tcp_pcb *pcb);
char extHttpConnCheck(ExtHttpConn  *_ehc);


err_t extHttpConnClose(ExtHttpConn *mhc, struct tcp_pcb *pcb);

err_t extHttpFileFind(ExtHttpConn *mhc);

err_t extHttpPoll(void *arg, struct tcp_pcb *pcb);

#if	MHTTPD_FS_ASYNC_READ
void mhttpContinue(void *connection);
#endif


u8_t extHttpSend(ExtHttpConn *mhc);

err_t extHttpWrite(ExtHttpConn *mhc, const void* ptr, u16_t *length, u8_t apiflags);

void extHttpConnEof(ExtHttpConn *mhc);


#if	MHTTPD_URI_BUF_LEN
extern char mhttpUriBuf[MHTTPD_URI_BUF_LEN+1];
#endif

err_t extHttpRequestParse( ExtHttpConn *mhc, struct pbuf *inp);


err_t mhttpFindErrorFile(ExtHttpConn *mhc, u16_t error_nr);


err_t extHttpPostRxDataPbuf(ExtHttpConn *mhc, struct pbuf *p);

err_t extNmosHandleRequest(ExtHttpConn *mhc);

err_t extHttpWebSocketParseHeader(ExtHttpConn *ehc);


err_t extHttpWebSocketParseFrame(ExtHttpConn *mhc, struct pbuf *p);

err_t extHttpWebSocketSendClose(ExtHttpConn *mhc);
unsigned short  extHttpWebSocketWrite(ExtHttpConn *mhc, const uint8_t *data, uint16_t len, unsigned char opCode);


const char *extHttpFindStatusHeader(unsigned short httpStatusCode);

char	cmnHttpRestError(ExtHttpConn *mhc, unsigned short httpErrorCode, const char *debug);


uint16_t extHttpWebPageRootHander(ExtHttpConn  *mhc, void *data);

char extHttpWebService(ExtHttpConn *mhc, void *data);
char extHttpWebPageResult(ExtHttpConn  *mhc, char *title, char *msg);

err_t extHttpPostCheckUpdate(ExtHttpConn *ehc, struct pbuf *inp);

err_t extHttpPostDataBegin(ExtHttpConn *ehc);
void extHttpPostDataFinished(ExtHttpConn *mhc, char isFinished);


char extNmosRootApHander(ExtHttpConn  *mhc, void *data);
char extNmosNodeDumpHander(ExtHttpConn  *mhc, void *data);

#if LWIP_EXT_HTTPD_TASK
void	httpFsmHandle(HttpEvent *he);
char extHttpPostEvent(ExtHttpConn *mhc, H_EVENT_T eventType, struct pbuf *p, struct tcp_pcb *pcb);


uint16_t extHttpSimpleRestApi(ExtHttpConn  *mhc, void *data);
uint16_t extHttpSdpAudio(ExtHttpConn  *mhc, void *data);
uint16_t extHttpSdpVideo(ExtHttpConn  *mhc, void *data);


uint16_t extHttpWebUpdateAvParams(ExtHttpConn *ehc, void *pageHandle);
uint16_t extHttpWebUpdateNetParams(ExtHttpConn *ehc, void *pageHandle);
uint16_t extHttpWebUpdateRs232Params(ExtHttpConn *ehc, void *pageHandle);


extern	sys_mutex_t			_httpMutex;

#define	HTTP_LOCK()		\
		do{sys_mutex_lock(&_httpMutex);}while(0)


#define	HTTP_UNLOCK()	\
		do{sys_mutex_unlock(&_httpMutex);}while(0)


#endif

extern	const char *httpCommonHeader;


char extHttpParseSdpClientData(ExtHttpConn *ehc, EXT_RUNTIME_CFG *tmpCfg, char *key, char *value);
char extHttpParseData(ExtHttpConn *ehc, EXT_RUNTIME_CFG *tmpCfg, char *key, char *value);
void extSysCfgDebugData(EXT_RUNTIME_CFG *cfg);


#define	DEBUG_CFG_PARAMS() \
{	EXT_DEBUGF(EXT_DBG_ON, (EXT_NEW_LINE"RX Configuration:") );extSysCfgDebugData(&tmpRuntime);\
	EXT_DEBUGF(EXT_DBG_ON, (EXT_NEW_LINE"Runtime Configuration") );extSysCfgDebugData(&extRun);}



int cmnHttpPrintResponseHeader(ExtHttpConn *mhc, const char contentType);
err_t cmnHttpParseRestJson(EXT_RUNTIME_CFG *rxCfg, char *jsonData, uint16_t size);

uint16_t httpWebPageResult(ExtHttpConn  *ehc, const char *title, char *msg, const char *url);

#endif

