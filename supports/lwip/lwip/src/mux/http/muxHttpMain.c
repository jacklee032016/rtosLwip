
#include "lwipMux.h"

#include "muxHttp.h"

#if	MHTTPD_SSI
/* SSI insert handler function pointer. */
tSSIHandler	g_pfnSSIHandler;
#if !MHTTPD_SSI_RAW
int g_iNumTags;
const char **g_ppcTags;
#endif /* !MHTTPD_SSI_RAW */

#define LEN_TAG_LEAD_IN			5
const char * const g_pcTagLeadIn = "<!--#";

#define LEN_TAG_LEAD_OUT		3
const char * const g_pcTagLeadOut = "-->";
#endif /* MHTTPD_SSI */

#if	MHTTPD_CGI
/* CGI handler information */
const tCGI *g_pCGIs;
int g_iNumCGIs;
int http_cgi_paramcount;
#define http_cgi_params     mhc->params
#define http_cgi_param_vals mhc->param_vals

#elif	MHTTPD_CGI_SSI
char *http_cgi_params[MHTTPD_MAX_CGI_PARAMETERS]; /* Params extracted from the request URI */
char *http_cgi_param_vals[MHTTPD_MAX_CGI_PARAMETERS]; /* Values for each extracted param */
#endif


#if	MHTTPD_URI_BUF_LEN
/* Filename for response file to send when POST is finished or
 * search for default files when a directory is requested. */
char mhttpUriBuf[MHTTPD_URI_BUF_LEN+1];
#endif



#if	MHTTPD_FS_ASYNC_READ
/** Try to send more data if file has been blocked before
 * This is a callback function passed to fs_read_async().
 */
static void mhttpContinue(void *connection)
{
	MuxHttpConn *mhc = (MuxHttpConn*)connection;
	if (mhc && (mhc->pcb) && (mhc->handle))
	{
		LWIP_ASSERT("mhc->pcb != NULL", mhc->pcb != NULL);
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("httpd_continue: try to send more data"LWIP_NEW_LINE));
		if (muxHttpSend(mhc))
		{
			/* If we wrote anything to be sent, go ahead and send it now. */
			MUX_DEBUGF(MUX_HTTPD_DEBUG, ("tcp_output"));
			tcp_output(mhc->pcb);
		}
	}
}
#endif

/**
 * The pcb had an error and is already deallocated.
 * The argument might still be valid (if != NULL).
 */
static void __mhttpErr(void *arg, err_t err)
{
	MuxHttpConn *mhc = (MuxHttpConn *)arg;
	LWIP_UNUSED_ARG(err);

	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("http_err: %s", lwip_strerr(err)));

	if (mhc != NULL)
	{
		mhttpConnFree(mhc);
	}
}

/**
 * Data has been sent and acknowledged by the remote host.
 * This means that more data can be sent.
 */
static err_t __mhttpSent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
	MuxHttpConn *mhc = (MuxHttpConn *)arg;

	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("http_sent %p", (void*)pcb));

	LWIP_UNUSED_ARG(len);

	if (mhc == NULL)
	{
		return ERR_OK;
	}

	mhc->retries = 0;
	muxHttpSend( mhc);

	return ERR_OK;
}

/**
 * The poll function is called every 2nd second.
 * If there has been no data sent (which resets the retries) in 8 seconds, close.
 * If the last portion of a file has not been sent in 2 seconds, close.
 *
 * This could be increased, but we don't want to waste resources for bad connections.
 * poll to control this pcb; the arg is come from 'tcp_arg()'.
 */
err_t mhttpPoll(void *arg, struct tcp_pcb *pcb)
{
	MuxHttpConn *mhc = (MuxHttpConn *)arg;
	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("http_poll: pcb=%p mhc=%p pcb_state=%s", (void*)pcb, (void*)mhc, tcp_debug_state_str(pcb->state)));

	if (mhc == NULL)
	{
		err_t closed;
		/* arg is null, close. */
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("http_poll: arg is NULL, close"));
		closed = mhttpConnClose(NULL, pcb);
		LWIP_UNUSED_ARG(closed);
#if	MHTTPD_ABORT_ON_CLOSE_MEM_ERROR
		if (closed == ERR_MEM)
		{
			tcp_abort(pcb);
			return ERR_ABRT;
		}
#endif
		return ERR_OK;
	}
	else
	{
		mhc->retries++;
		if (mhc->retries == MHTTPD_MAX_RETRIES)
		{
			MUX_DEBUGF(MUX_HTTPD_DEBUG, ("http_poll: too many retries, close"));
			mhttpConnClose(mhc, pcb);
			return ERR_OK;
		}

		/* If this connection has a file open, try to send some more data. If
		* it has not yet received a GET request, don't do this since it will
		* cause the connection to close immediately. */
		if(mhc )//&& (mhc->handle))
		{
			MUX_DEBUGF(MUX_HTTPD_DEBUG, ("http_poll: try to send more data"));
			if(muxHttpSend( mhc))
			{/* If we wrote anything to be sent, go ahead and send it now. */
				MUX_DEBUGF(MUX_HTTPD_DEBUG, ("tcp_output"));
				tcp_output(pcb);
			}
		}
	}

	return ERR_OK;
}

#ifdef	ARM
#else
static char		_debugBuf[2048];
#endif
/**
 * Data has been received on this pcb.
 * For HTTP 1.0, this should normally only happen once (if the request fits in one packet).
 */
static err_t __mhttpRecv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	MuxHttpConn *mhc = (MuxHttpConn *)arg;
	
	MUX_DEBUGF(MUX_HTTPD_DATA_DEBUG, ("http_recv: pcb=%p pbuf=%p err=%s", (void*)pcb, (void*)p, lwip_strerr(err)));

	if ((err != ERR_OK) || (p == NULL) || (mhc == NULL))
	{/* error or closed by other side? */
		if (p != NULL) 
		{/* Inform TCP that we have taken the data. */
			tcp_recved(pcb, p->tot_len);
			pbuf_free(p);
		}
		
		if (mhc == NULL)
		{/* this should not happen, only to be robust */
			MUX_DEBUGF(MUX_HTTPD_DEBUG, ("Error, http_recv: mhc is NULL, close"));
		}
		
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("HTTP Connection is broken by peer"));
		mhttpConnClose(mhc, pcb);
		return ERR_OK;
	}

#if	MHTTPD_POST_MANUAL_WND
	if (mhc->no_auto_wnd)
	{
		mhc->unrecved_bytes += p->tot_len;
	}
	else
#endif
	{/* Inform TCP that we have taken the data. */
		tcp_recved(pcb, p->tot_len);
	}

#ifdef	ARM
#else
	MUX_DEBUGF(MUX_HTTPD_DATA_DEBUG, (MUX_NEW_LINE MUX_NEW_LINE"Received %"U16_F" bytes", p->tot_len));
	memset(_debugBuf,0 , sizeof(_debugBuf));
	
	pbuf_copy_partial(p, _debugBuf, p->tot_len, 0);
//	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("recv:'%.*s'" , p->tot_len,_debugBuf) );
//	CONSOLE_DEBUG_MEM(mhc->data, p->tot_len, 0, "RECV HTTP Data");
#endif

	if ( HTTPREQ_IS_WEBSOCKET(mhc) )
	{
		err_t ret = muxHttpWebSocketParseFrame(mhc, p);
		if (p != NULL)
		{
			/* otherwise tcp buffer hogs */
			MUX_DEBUGF(MUX_HTTPD_DEBUG, ("[wsoc] freeing buffer"));
			pbuf_free(p);
		}
		
		if (ret == ERR_CLSD)
		{
			mhttpConnClose(mhc, pcb);
		}
		
		/* reset timeout */
		mhc->retries = 0;
		return ERR_OK;
	}


	if (mhc->postDataLeft > 0 || HTTP_IS_POST(mhc) )
	{/* reset idle counter when POST data is received */
		mhc->retries = 0;
		/* this is data for a POST, pass the complete pbuf to the application */
		muxHttpPostRxDataPbuf(mhc, p);
		
		/* pbuf is passed to the application, don't free it! */
#if 0
		if (mhc->postDataLeft == 0)
#else			
		if ( 1)
#endif			
		{/* all data received, send response or close connection */
			muxHttpSend(mhc);
		}

		return ERR_OK;
	}
	else
	{
		if (mhc->handle == NULL)
		{
			err_t parsed = muxHttpRequestParse(mhc, p );
			LWIP_ASSERT("parse request: unexpected return value", parsed == ERR_OK|| parsed == ERR_INPROGRESS ||parsed == ERR_ARG || parsed == ERR_USE);

			if (parsed != ERR_INPROGRESS)
			{
				/* request fully parsed or error */
				if (mhc->req != NULL)
				{
					pbuf_free(mhc->req);
					mhc->req = NULL;
				}
			}

			pbuf_free(p);
			if (parsed == ERR_OK)
			{
#if 0
				if (mhc->postDataLeft == 0)
#else			
				if ( 1)
#endif			
				{
					//MUX_DEBUGF(MUX_HTTPD_DEBUG, ("file %p len %"S32_F"", (const void*)mhc->file, mhc->left));
					muxHttpSend(mhc);
				}
			}
			else if (parsed == ERR_ARG)
			{/* @todo: close on ERR_USE? */
				mhttpConnClose(mhc, pcb);
			}
		}
		else
		{
			MUX_DEBUGF(MUX_HTTPD_DEBUG, ("http_recv: already sending data"));
			/* already sending but still receiving data, we might want to RST here? */
			pbuf_free(p);
		}
	}
	return ERR_OK;
}

/* A new incoming connection has been accepted */
static err_t _mhttpAccept(void *arg, struct tcp_pcb *pcb, err_t err)
{
	MuxHttpConn *mhc;
	MUX_RUNTIME_CFG *runCfg = (MUX_RUNTIME_CFG *)arg;
	
	LWIP_UNUSED_ARG(err);

//	MUX_DEBUGF(MUX_HTTPD_DEBUG,("_mhttpAccept %p / total Connection %d:%s", (void*)pcb, runCfg->currentHttpConns, runCfg->name ));
//	MUX_ERRORF(("_mhttpAccept %p / total Connection %d", (void*)pcb, runCfg->currentHttpConns ));

	if ((err != ERR_OK) || (pcb == NULL))
	{
		return ERR_VAL;
	}

	/* Set priority */
	tcp_setprio(pcb, MHTTPD_TCP_PRIO);

	/* Allocate memory for the structure that holds the state of the connection - initialized by that function. */
	mhc = mhttpConnAlloc();
	if (mhc == NULL)
	{
		MUX_DEBUGF(MUX_HTTPD_DEBUG, ("http_accept: Out of memory, RST"));
		return ERR_MEM;
	}

	runCfg->currentHttpConns++;
	
	mhc->pcb = pcb;
	/* Tell TCP that this is the structure we wish to be passed for our callbacks. */
	tcp_arg(pcb, mhc);

	/* Set up the various callback functions */
	tcp_recv(pcb, __mhttpRecv);
	tcp_err(pcb,  __mhttpErr);
	tcp_poll(pcb, mhttpPoll,	MHTTPD_POLL_INTERVAL);
	tcp_sent(pcb, __mhttpSent);

	return ERR_OK;
}

/**
 * @ingroup httpd
 * Initialize the httpd: set up a listening PCB and bind it to the defined port
 */
void mHttpSvrMain(void *data)
{
	struct tcp_pcb *pcb;
	err_t err;

	MUX_RUNTIME_CFG *runCfg = (MUX_RUNTIME_CFG *)data;
#if	MHTTPD_USE_MEM_POOL
	LWIP_MEMPOOL_INIT(MHTTPD_STATE);
#if	MHTTPD_SSI
	LWIP_MEMPOOL_INIT(MHTTPD_SSI_STATE);
#endif
#endif
	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("mHttpSvrMain"));

	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	LWIP_ASSERT("mHttpSvrMain: tcp_new failed", pcb != NULL);
	
	tcp_setprio(pcb, MHTTPD_TCP_PRIO);
	/* set SOF_REUSEADDR here to explicitly bind httpd to multiple interfaces */
	err = tcp_bind(pcb, IP_ANY_TYPE, runCfg->httpPort);
	
	LWIP_UNUSED_ARG(err); /* in case of LWIP_NOASSERT */
	LWIP_ASSERT("mHttpSvrMain: tcp_bind failed", err == ERR_OK);
	
	pcb = tcp_listen(pcb);
	LWIP_ASSERT("mHttpSvrMain: tcp_listen failed", pcb != NULL);

	tcp_arg(pcb, runCfg);

	tcp_accept(pcb, _mhttpAccept);
}

#if	MHTTPD_SSI
/**
 * Set the SSI handler function.
 *
 * @param ssi_handler the SSI handler function
 * @param tags an array of SSI tag strings to search for in SSI-enabled files
 * @param num_tags number of tags in the 'tags' array
 */
void mhttpSetSsiHandler(tSSIHandler ssi_handler, const char **tags, int num_tags)
{
	MUX_DEBUGF(MUX_HTTPD_DEBUG, ("http_set_ssi_handler"));

	LWIP_ASSERT("no ssi_handler given", ssi_handler != NULL);
	g_pfnSSIHandler = ssi_handler;

#if	MHTTPD_SSI_RAW
	LWIP_UNUSED_ARG(tags);
	LWIP_UNUSED_ARG(num_tags);
#else
	LWIP_ASSERT("no tags given", tags != NULL);
	LWIP_ASSERT("invalid number of tags", num_tags > 0);

	g_ppcTags = tags;
	g_iNumTags = num_tags;
#endif
}
#endif


#if	MHTTPD_CGI
/**
 * Set an array of CGI filenames/handler functions
 *
 * @param cgis an array of CGI filenames/handler functions
 * @param num_handlers number of elements in the 'cgis' array
 */
void mhttpSetCgiHandlers(const tCGI *cgis, int num_handlers)
{
	LWIP_ASSERT("no cgis given", cgis != NULL);
	LWIP_ASSERT("invalid number of handlers", num_handlers > 0);

	g_pCGIs = cgis;
	g_iNumCGIs = num_handlers;
}
#endif

