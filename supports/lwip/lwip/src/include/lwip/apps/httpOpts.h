/*
*
 */

#ifndef __HTTP_OPTS_H__
#define __HTTP_OPTS_H__

#include "lwip/opt.h"


#if !defined MHTTPD_CGI
#define	MHTTPD_CGI					0
#endif

#if !defined	MHTTPD_CGI_SSI
#define	MHTTPD_CGI_SSI				0
#endif

#if !defined MHTTPD_SSI
#define	MHTTPD_SSI					0
#endif

/** Set this to 1 to implement an SSI tag handler callback that gets a const char*
 * to the tag (instead of an index into a pre-registered array of known tags) */
#if !defined MHTTPD_SSI_RAW
#define	MHTTPD_SSI_RAW			0
#endif


/* The maximum number of parameters that the CGI handler can be sent. */
#if !defined MHTTPD_MAX_CGI_PARAMETERS
#define	MHTTPD_MAX_CGI_PARAMETERS		16
#endif

/** LWIP_HTTPD_SSI_MULTIPART==1: SSI handler function is called with 2 more
 * arguments indicating a counter for insert string that are too long to be
 * inserted at once: the SSI handler function must then set 'next_tag_part'
 * which will be passed back to it in the next call. */
#if !defined MHTTPD_SSI_MULTIPART
#define	MHTTPD_SSI_MULTIPART				0
#endif

/* The maximum length of the string comprising the tag name */
#if !defined MHTTPD_MAX_TAG_NAME_LEN
#define	MHTTPD_MAX_TAG_NAME_LEN			8
#endif

/* The maximum length of string that can be returned to replace any given tag */
#if !defined MHTTPD_MAX_TAG_INSERT_LEN
#define	MHTTPD_MAX_TAG_INSERT_LEN		192
#endif

#if !defined	MHTTPD_POST_MANUAL_WND
#define	MHTTPD_POST_MANUAL_WND			0
#endif

/** This string is passed in the HTTP header as "Server: " */
#if !defined	MHTTPD_SERVER_AGENT
#define	MHTTPD_SERVER_AGENT		"MuxLab/500767"
#endif

/** Set this to 1 if you want to include code that creates HTTP headers
 * at runtime. Default is off: HTTP headers are then created statically
 * by the makefsdata tool. Static headers mean smaller code size, but
 * the (readonly) fsdata will grow a bit as every file includes the HTTP
 * header. */
#if !defined	MHTTPD_DYNAMIC_HEADERS
#define	MHTTPD_DYNAMIC_HEADERS		0
#endif

#if !defined MHTTPD_DEBUG
#define	MHTTPD_DEBUG					LWIP_DBG_ON
#endif

/** Set this to 1 to use a memp pool for allocating 
 * ExtHttpConn instead of the heap.
 */
#if !defined MHTTPD_USE_MEM_POOL
#define	MHTTPD_USE_MEM_POOL			0
#endif

/** The server port for HTTPD to use */
#if !defined MHTTPD_SERVER_PORT
#define	MHTTPD_SERVER_PORT			80
#endif

/** Maximum retries before the connection is aborted/closed.
 * - number of times pcb->poll is called -> default is 4*500ms = 2s;
 * - reset when pcb->sent is called
 */
#if !defined MHTTPD_MAX_RETRIES
#define	MHTTPD_MAX_RETRIES                   4
#endif

/** The poll delay is X*500ms */
#if !defined MHTTPD_POLL_INTERVAL
#define	MHTTPD_POLL_INTERVAL			4
#endif

/** Priority for tcp pcbs created by HTTPD (very low by default).
 *  Lower priorities get killed first when running out of memory.
 */
#if !defined MHTTPD_TCP_PRIO
#define	MHTTPD_TCP_PRIO				TCP_PRIO_MIN
#endif

/** Set this to 1 to enable timing each file sent */
#if !defined MHTTPD_TIMING
#define	MHTTPD_TIMING					0
#endif

/** Set this to 1 to enable timing each file sent */
#if !defined MHTTPD_DEBUG_TIMING
#define	MHTTPD_DEBUG_TIMING				LWIP_DBG_OFF
#endif

/** Set this to one to show error pages when parsing a request fails instead
    of simply closing the connection. */
#if !defined MHTTPD_SUPPORT_EXTSTATUS
#define	MHTTPD_SUPPORT_EXTSTATUS			0
#endif

/** Set this to 0 to drop support for HTTP/0.9 clients (to save some bytes) */
#if !defined	MHTTPD_SUPPORT_V09
#define	MHTTPD_SUPPORT_V09				1
#endif

/** Set this to 1 to enable HTTP/1.1 persistent connections.
 * ATTENTION: If the generated file system includes HTTP headers, these must
 * include the "Connection: keep-alive" header (pass argument "-11" to makefsdata).
 */
#if !defined MHTTPD_SUPPORT_11_KEEPALIVE
#define	MHTTPD_SUPPORT_11_KEEPALIVE		0
#endif


/** Number of rx pbufs to enqueue to parse an incoming request (up to the first newline) */
#if !defined 	MHTTPD_REQ_QUEUELEN
#define	MHTTPD_REQ_QUEUELEN				5
#endif

/** Number of (TCP payload-) bytes (in pbufs) to enqueue to parse and incoming
    request (up to the first double-newline) */
#if !defined MHTTPD_REQ_BUFSIZE
#define	MHTTPD_REQ_BUFSIZE				MHTTPD_MAX_REQ_LENGTH
#endif

/** Defines the maximum length of a HTTP request line (up to the first CRLF,
    copied from pbuf into this a global buffer when pbuf- or packet-queues
    are received - otherwise the input pbuf is used directly) */
#if !defined	MHTTPD_MAX_REQ_LENGTH
#define	MHTTPD_MAX_REQ_LENGTH		(2048+1024)//LWIP_MIN(1023, (MHTTPD_REQ_QUEUELEN * PBUF_POOL_BUFSIZE))
#endif

/** This is the size of a static buffer used when URIs end with '/'.
 * In this buffer, the directory requested is concatenated with all the
 * configured default file names.
 * Set to 0 to disable checking default filenames on non-root directories.
 */
#if !defined	MHTTPD_MAX_REQUEST_URI_LEN
#define	MHTTPD_MAX_REQUEST_URI_LEN				127
#endif

/** Maximum length of the filename to send as response to a POST request,
 * filled in by the application when a POST is finished.
 */
#if !defined 	MHTTPD_POST_MAX_RESPONSE_URI_LEN
#define	MHTTPD_POST_MAX_RESPONSE_URI_LEN		63
#endif

/** Set this to 0 to not send the SSI tag (default is on, so the tag will
 * be sent in the HTML page */
#if !defined MHTTPD_SSI_INCLUDE_TAG
#define	MHTTPD_SSI_INCLUDE_TAG			1
#endif

/** Set this to 1 to call tcp_abort when tcp_close fails with memory error.
 * This can be used to prevent consuming all memory in situations where the
 * HTTP server has low priority compared to other communication. */
#if !defined	MHTTPD_ABORT_ON_CLOSE_MEM_ERROR
#define	MHTTPD_ABORT_ON_CLOSE_MEM_ERROR			0
#endif

/** Set this to 1 to kill the oldest connection when running out of
 * memory for 'ExtHttpConn' or 'struct mhttp_ssi_state'.
 * ATTENTION: This puts all connections on a linked list, so may be kind of slow.
 */
#if !defined	MHTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
#define	MHTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED		1
#endif

/** Set this to 1 to send URIs without extension without headers (who uses this at all??) */
#if !defined	MHTTPD_OMIT_HEADER_FOR_EXTENSIONLESS_URI
#define	MHTTPD_OMIT_HEADER_FOR_EXTENSIONLESS_URI		0
#endif

/** Default: Tags are sent from ExtHttpConn and are therefore volatile */
#if !defined MHTTP_IS_TAG_VOLATILE
#define	MHTTP_IS_TAG_VOLATILE(ptr)		TCP_WRITE_FLAG_COPY
#endif

/* By default, the httpd is limited to send 2*pcb->mss to keep resource usage low
   when http is not an important protocol in the device. */
#if !defined MHTTPD_LIMIT_SENDING_TO_2MSS
#define	MHTTPD_LIMIT_SENDING_TO_2MSS			1
#endif

/* Define this to a function that returns the maximum amount of data to enqueue.
   The function have this signature: u16_t fn(struct tcp_pcb* pcb); */
#if !defined MHTTPD_MAX_WRITE_LEN
#if MHTTPD_LIMIT_SENDING_TO_2MSS
#define MHTTPD_MAX_WRITE_LEN(pcb)    (2 * tcp_mss(pcb))
#endif
#endif

/*------------------- FS OPTIONS -------------------*/

/** Set this to 1 and provide the functions:
 * - "int fs_open_custom(struct fs_file *file, const char *name)"
 *    Called first for every opened file to allow opening files
 *    that are not included in fsdata(_custom).c
 * - "void fs_close_custom(struct fs_file *file)"
 *    Called to free resources allocated by fs_open_custom().
 */
#if !defined	MHTTPD_CUSTOM_FILES
#define	MHTTPD_CUSTOM_FILES       0
#endif

/** Set this to 1 to support fs_read() to dynamically read file data.
 * Without this (default=off), only one-block files are supported,
 * and the contents must be ready after fs_open().
 */
#if !defined MHTTPD_DYNAMIC_FILE_READ
#define	MHTTPD_DYNAMIC_FILE_READ		0
#endif

/** Set this to 1 to include an application state argument per file
 * that is opened. This allows to keep a state per connection/file.
 */
#if !defined MHTTPD_FILE_STATE
#define	MHTTPD_FILE_STATE         0
#endif

/** HTTPD_PRECALCULATED_CHECKSUM==1: include precompiled checksums for
 * predefined (MSS-sized) chunks of the files to prevent having to calculate
 * the checksums at runtime. */
#if !defined	MHTTPD_PRECALCULATED_CHECKSUM
#define	MHTTPD_PRECALCULATED_CHECKSUM		0
#endif

/** LWIP_HTTPD_FS_ASYNC_READ==1: support asynchronous read operations
 * (fs_read_async returns FS_READ_DELAYED and calls a callback when finished).
 */
#if !defined MHTTPD_FS_ASYNC_READ
#define	MHTTPD_FS_ASYNC_READ			0
#endif

/** Set this to 1 to include "fsdata_custom.c" instead of "fsdata.c" for the
 * file system (to prevent changing the file included in CVS) */
#if !defined	MHTTPD_USE_CUSTOM_FSDATA
#define	MHTTPD_USE_CUSTOM_FSDATA 0
#endif

/* Oct.23, 2018 J.L. */
#define		LWIP_EXT_HTTPD_TASK							1

#define		EXT_HTTPD_MBOX_SIZE						4*2

#endif

