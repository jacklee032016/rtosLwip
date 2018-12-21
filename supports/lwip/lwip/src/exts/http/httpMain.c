
#include "lwipExt.h"

#include "http.h"

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


#if LWIP_EXT_HTTPD_TASK

static sys_mbox_t 		_httpdMailBox;

sys_mutex_t			_httpMutex;

static void _extHttpdTask(void *arg)
{
	HttpEvent	*he;
#if 1	
//	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)arg;
#else
	EXT_JSON_PARSER  *parser = (EXT_JSON_PARSER *)arg;
#endif

	if (sys_mutex_new(&_httpMutex) != ERR_OK)
	{
		EXT_ASSERT(("failed to create HTTP Mutex"), 0);
	}


	while (1)
	{
		ExtHttpConn *ehc = NULL;
		
//TRACE();
		sys_mbox_fetch(&_httpdMailBox, (void **)&he);
//TRACE();
		if (he == NULL)
		{
			EXT_ASSERT((EXT_TASK_HTTP" task: invalid message"), 0);
			continue;
		}
//TRACE();

		{
			httpFsmHandle(he);
			ehc = (ExtHttpConn *)he->mhc;
			
			HTTP_LOCK();
			ehc->eventCount--;
			HTTP_UNLOCK();

			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("%s: state is %s, event count:%d", ehc->name, CMN_FIND_HTTP_STATE(ehc->state), ehc->eventCount));
			if(ehc && ehc->eventCount==0 && (ehc->state == H_STATE_FREE||ehc->state == H_STATE_CLOSE||ehc->state == H_STATE_ERROR))//
			{
				extHttpConnFree(ehc);
			}
			
		}

//TRACE();
		HTTP_LOCK();	
		HTTP_EVENT_FREE(he);
//		memp_free(MEMP_EXT_HTTPD, he);
//		he = NULL;
		HTTP_UNLOCK();	
//TRACE();
	}
	
}


char extHttpPostEvent(ExtHttpConn *ehc, H_EVENT_T eventType, struct pbuf *p, struct tcp_pcb *pcb)
{
	HttpEvent *he;
	ExtHttpConn *_ehc;

	EXT_ASSERT(( "%s: post event failed MHC and PCB all are null", __FUNCTION__), (pcb != NULL || ehc !=NULL) );

	if(eventType != H_EVENT_ERROR)
	{/* TCP error means TCP_PCB has been deallocated */
		_ehc = extHttpConnFind(pcb);
		if(_ehc == NULL)
		{
#if EXT_HTTPD_DEBUG
			EXT_INFOF(("CONN for PCB %p: from %s:%d has been freed, event '%s' is ignored", 
				(void*)pcb, extCmnIp4addr_ntoa((uint32_t *)&pcb->remote_ip), pcb->remote_port, CMN_FIND_HTTP_EVENT(eventType)));
#else
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("CONN for PCB %p: from %s:%d has been freed, event '%d' is ignored", 
				(void*)pcb, extCmnIp4addr_ntoa((uint32_t *)&pcb->remote_ip), pcb->remote_port, eventType));
#endif

			err_t err = tcp_close(pcb);
			if (err != ERR_OK)
			{
				EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Error %d closing %s: %p"EXT_NEW_LINE, err, ehc->name, (void*)pcb));
				EXT_ERRORF( ("Error %d closing %p"EXT_NEW_LINE, err, (void*)pcb));
				/* error closing, try again later in poll */
				tcp_poll(pcb, extHttpPoll,  MHTTPD_POLL_INTERVAL);

	HTTP_UNLOCK();
	//		UNLOCK_TCPIP_CORE();
				return EXIT_FAILURE;
			}

			tcp_arg(pcb, NULL);
			
			tcp_recv(pcb, NULL);
			tcp_err(pcb, NULL);
			tcp_poll(pcb, NULL, 0);
			tcp_sent(pcb, NULL);

			return EXIT_FAILURE;
		}
	}
	else
	{
		if(extHttpConnCheck(ehc)== EXT_FALSE)
		{
			return EXIT_FAILURE;
		}
		_ehc = ehc;
	}

	LWIP_ASSERT(("Invalid mbox"), sys_mbox_valid_val(_httpdMailBox));
	
	HTTP_LOCK();	
	HTTP_EVENT_ALLOC(he);
	//he = (HttpEvent *)memp_malloc(MEMP_EXT_HTTPD);
	if (he == NULL)
	{
		EXT_ERRORF(("No memory available for HTTP Event now"));
		return EXIT_FAILURE;
	}
	memset(he, 0, sizeof(HttpEvent));

	he->mhc = _ehc;
	he->type = eventType;
	he->pBuf = p;
	he->pcb = pcb;

	/* ERROR means TCP_PCB has been deallocated in TCP */	
	if(eventType == H_EVENT_ERROR)
	{
		_ehc->pcb = NULL;
	}

	ehc->eventCount++;
	
	HTTP_UNLOCK();
	
#if EXT_HTTPD_DEBUG
	EXT_INFOF(("%s: Event.Count %d: '%s' in state %s", ehc->name, ehc->eventCount, CMN_FIND_HTTP_EVENT(eventType), CMN_FIND_HTTP_STATE(ehc->state) ));

#else
	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("%p: Event.Count %d: '%d' in state %d", ehc, ehc->eventCount, eventType, ehc->state ));
#endif
	if (sys_mbox_trypost(&_httpdMailBox, he) != ERR_OK)
	{
		EXT_ERRORF(("Post to "EXT_TASK_HTTP" Mailbox failed"));
		HTTP_EVENT_FREE(he);
		//memp_free(MEMP_EXT_HTTPD, he);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


#if 0
/* TCP_PCB can only handled in context of TCP task */
err_t extHttpClosePcb(ExtHttpConn *ehc)
{
	err_t err = ERR_OK;
	
	if(ehc->state !=  H_STATE_ERROR && ehc->pcb != NULL )
	{/* when TCP error happens, TCP_PCB has been deallocated */
		struct tcp_pcb *pcb;

		pcb = ehc->pcb;
//		EXT_ASSERT(("PCB for CONN %s is null", ehc->name), pcb != NULL);
		ehc->pcb = NULL;


		err = tcp_close(pcb);
		if (err != ERR_OK)
		{
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Error %s (%d) closing %s: %p"EXT_NEW_LINE, lwip_strerr(err), err, ehc->name, (void*)pcb));
			EXT_ERRORF( ("Error %s (%d) closing %p"EXT_NEW_LINE, lwip_strerr(err), err, (void*)pcb));
			/* error closing, try again later in poll */
			tcp_poll(pcb, extHttpPoll,  MHTTPD_POLL_INTERVAL);

			return err;
		}


		
		tcp_arg(pcb, NULL);
		
		tcp_recv(pcb, NULL);
		tcp_err(pcb, NULL);
		tcp_poll(pcb, NULL, 0);
		tcp_sent(pcb, NULL);

		EXT_DEBUGF(EXT_HTTPD_DEBUG,("CONN %s (%s:%d) is freed, ", ehc->name, extCmnIp4addr_ntoa((uint32_t *)&pcb->remote_ip), pcb->remote_port) );
	}

	return err;
}
#endif

#endif



#if	MHTTPD_FS_ASYNC_READ
/** Try to send more data if file has been blocked before
 * This is a callback function passed to fs_read_async().
 */
static void mhttpContinue(void *connection)
{
	ExtHttpConn *mhc = (ExtHttpConn*)connection;
	if (mhc && (mhc->pcb) && (mhc->handle))
	{
		EXT_ASSERT(("mhc->pcb != NULL"), mhc->pcb != NULL);
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("httpd_continue: try to send more data"LWIP_NEW_LINE));
		if (extHttpSend(mhc))
		{
			/* If we wrote anything to be sent, go ahead and send it now. */
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("tcp_output"));
			tcp_output(mhc->pcb);
		}
	}
}
#endif

/**
 * The pcb had an error and is already deallocated.
 * The argument might still be valid (if != NULL).
 */
static void __extHttpErr(void *arg, err_t err)
{
	ExtHttpConn *ehc = NULL;
	LWIP_UNUSED_ARG(err);

//	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("http_err: %s", lwip_strerr(err)));
	EXT_ERRORF(("http_err: %s", lwip_strerr(err)));
	ehc = (ExtHttpConn *)arg;

	if (ehc != NULL)
	{
		ehc->pcb = NULL;
#if LWIP_EXT_HTTPD_TASK
		
		extHttpPostEvent(ehc, H_EVENT_ERROR, NULL, NULL);
#else
		extHttpConnFree(ehc);
#endif
	}
	else
	{
//		EXT_ERRORF(("CONN and PCB can't be all null when TCP error happens"));
	}
}

/**
 * Data has been sent and acknowledged by the remote host.
 * This means that more data can be sent.
 */
static err_t __extHttpSent(void *arg, struct tcp_pcb *pcb, u16_t len)
{/* ack packet length, not snd_buf_size */
	ExtHttpConn *mhc = NULL;

	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("http_sent %p: sent len : %d", (void*)pcb, len));
	mhc = (ExtHttpConn *)arg;

#if LWIP_EXT_HTTPD_TASK
	tcp_recved(pcb, len);

	extHttpPostEvent(mhc, H_EVENT_SENT, NULL, pcb);
#else
	LWIP_UNUSED_ARG(len);
	if (mhc == NULL)
	{
		return ERR_OK;
	}

	mhc->retries = 0;
	extHttpSend( mhc);
#endif

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
err_t extHttpPoll(void *arg, struct tcp_pcb *pcb)
{
	ExtHttpConn *mhc = NULL;

	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("http_poll: pcb=%p pcb_state=%s", (void*)pcb, tcp_debug_state_str(pcb->state)));
	mhc = (ExtHttpConn *)arg;

#if LWIP_EXT_HTTPD_TASK
	if (mhc == NULL)
	{
		tcp_close(pcb);
	}
	else
	{
		mhc->retries++;
		if (mhc->retries == MHTTPD_MAX_RETRIES)
		{
			extHttpPostEvent(mhc, H_EVENT_CLOSE, NULL, pcb);
		}
		else
		{
			extHttpPostEvent(mhc, H_EVENT_POLL, NULL, pcb);
		}

#if 0
		/* If this connection has a file open, try to send some more data. If
		* it has not yet received a GET request, don't do this since it will
		* cause the connection to close immediately. */
		if(mhc )//&& (mhc->handle))
		{
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("POLL: try to send more data"));
			if(extHttpSend( mhc))
			{/* If we wrote anything to be sent, go ahead and send it now. */
				EXT_DEBUGF(EXT_HTTPD_DEBUG, ("tcp_output"));
				tcp_output(he->pcb);
			}
		}
#endif		
	}

#else

	if (mhc == NULL)
	{
		err_t closed;
		/* arg is null, close. */
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("http_poll: arg is NULL, close"));
		closed = extHttpConnClose(NULL, pcb);
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
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("http_poll: too many retries, close"));
			extHttpConnClose(mhc, pcb);
			return ERR_OK;
		}

		/* If this connection has a file open, try to send some more data. If
		* it has not yet received a GET request, don't do this since it will
		* cause the connection to close immediately. */
		if(mhc )//&& (mhc->handle))
		{
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("http_poll: try to send more data"));
			if(extHttpSend( mhc))
			{/* If we wrote anything to be sent, go ahead and send it now. */
				EXT_DEBUGF(EXT_HTTPD_DEBUG, ("tcp_output"));
				tcp_output(pcb);
			}
		}
	}
#endif		

	return ERR_OK;
}

#ifdef	ARM
#else
static char		_debugBuf[2048];
#endif

static err_t __recvData(ExtHttpConn *ehc, struct tcp_pcb *pcb, struct pbuf *p)
{
#if	MHTTPD_POST_MANUAL_WND
	if (ehc->no_auto_wnd)
	{
		ehc->unrecved_bytes += p->tot_len;
	}
	else
#endif
	{/* Inform TCP that we have taken the data. */
		tcp_recved(pcb, p->tot_len);
	}

//	EXT_DEBUGF(EXT_HTTPD_DATA_DEBUG, (EXT_NEW_LINE "Received %"U16_F" bytes", p->tot_len));
#ifdef	ARM
#else
//	memset(_debugBuf,0 , sizeof(_debugBuf));
	
//	pbuf_copy_partial(p, _debugBuf, p->tot_len, 0);
	EXT_DEBUGF(EXT_DBG_ON, ("recv:"EXT_NEW_LINE"'%.*s'" EXT_NEW_LINE , p->tot_len, (char *)p->payload) );
//	CONSOLE_DEBUG_MEM(p->payload, p->tot_len, 0, "RECV HTTP Data");
#endif

	if(EXT_DEBUG_HTTP_IS_ENABLE())
	{
		printf("RECV:"EXT_NEW_LINE"'%.*s'"EXT_NEW_LINE EXT_NEW_LINE, p->tot_len, (char *)p->payload);
	}

#if LWIP_EXT_HTTPD_TASK
	if(extHttpPostEvent(ehc, H_EVENT_RECV, p, pcb) == EXIT_FAILURE)
	{
		return ERR_MEM;
	}
#else

	if ( HTTPREQ_IS_WEBSOCKET(ehc) )
	{
		err_t ret = extHttpWebSocketParseFrame(ehc, p);
		if (p != NULL)
		{
			/* otherwise tcp buffer hogs */
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("[wsoc] freeing buffer"));
			pbuf_free(p);
		}
		
		if (ret == ERR_CLSD)
		{
			extHttpConnClose(ehc, pcb);
		}
		
		/* reset timeout */
		ehc->retries = 0;
		return ERR_OK;
	}


	if (ehc->postDataLeft > 0 || HTTP_IS_POST(ehc) )
	{/* reset idle counter when POST data is received */
		ehc->retries = 0;
		/* this is data for a POST, pass the complete pbuf to the application */
		extHttpPostRxDataPbuf(ehc, p);
		
		/* pbuf is passed to the application, don't free it! */
#if 0
		if (ehc->postDataLeft == 0)
#else			
		if ( 1)
#endif			
		{/* all data received, send response or close connection */
			extHttpSend(ehc);
		}

		return ERR_OK;
	}
	else
	{
		if (ehc->handle == NULL)
		{
			err_t parsed = extHttpRequestParse(ehc, p );
			EXT_ASSERT(("parse request: unexpected return value"), parsed == ERR_OK|| parsed == ERR_INPROGRESS ||parsed == ERR_ARG || parsed == ERR_USE);

			if (parsed != ERR_INPROGRESS)
			{
				/* request fully parsed or error */
				if (ehc->req != NULL)
				{
					pbuf_free(ehc->req);
					ehc->req = NULL;
				}
			}

			pbuf_free(p);
			if (parsed == ERR_OK)
			{
#if 0
				if (ehc->postDataLeft == 0)
#else			
				if ( 1)
#endif			
				{
					//EXT_DEBUGF(EXT_HTTPD_DEBUG, ("file %p len %"S32_F"", (const void*)ehc->file, ehc->left));
					extHttpSend(ehc);
				}
			}
			else if (parsed == ERR_ARG)
			{/* @todo: close on ERR_USE? */
				extHttpConnClose(ehc, pcb);
			}
		}
		else
		{
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("http_recv: already sending data"));
			/* already sending but still receiving data, we might want to RST here? */
			pbuf_free(p);
		}
	}
#endif
	
	return ERR_OK;
}

/**
 * Data has been received on this pcb.
 * For HTTP 1.0, this should normally only happen once (if the request fits in one packet).
 */
static err_t __extHttpRecv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	ExtHttpConn *ehc = NULL;
	
	EXT_DEBUGF(EXT_HTTPD_DATA_DEBUG, ("http_recv: pcb=%p pbuf=%p err=%s", (void*)pcb, (void*)p, lwip_strerr(err)));
	ehc = (ExtHttpConn *)arg;

	if((err == ERR_OK) && (p != NULL))
	{/* Data */
		EXT_ASSERT(("Data is receiving, but conn has been freed"), (ehc != NULL ));
		return __recvData(ehc, pcb, p);
	}
	else if((err == ERR_OK) && (p == NULL))
	{/* closed by peer */
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("%s Event RECV : CONN closed",(ehc)?ehc->name:"UNKNOWN") );
		if(ehc == NULL)
		{
			tcp_close(pcb);
		}
		else
		{
#if 0		
		err_t err = tcp_close(pcb);
		if (err != ERR_OK)
		{
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Error closing : %d(%s): %p"EXT_NEW_LINE, err, lwip_strerr(err), (void*)pcb));
			EXT_ERRORF( ("Error %d closing %p"EXT_NEW_LINE, err, (void*)pcb));
			/* error closing, try again later in poll */
			tcp_poll(pcb, extHttpPoll,  MHTTPD_POLL_INTERVAL);

			return err;
		}
#else		
			extHttpPostEvent( ehc, H_EVENT_CLOSE, p, pcb);
		}
#endif
	}
	else
	{/* err is not ERR_OK */
		EXT_ERRORF(("%s Event RECV error: '%s'", (ehc)?ehc->name:"UNKOWN", lwip_strerr(err)));
#if LWIP_EXT_HTTPD_TASK
//		extHttpClosePcb(ehc);
		extHttpPostEvent( ehc, H_EVENT_CLOSE, p, pcb);
//		extHttpPostEvent( ehc, H_EVENT_ERROR, p, pcb);
#else
		extHttpConnClose(ehc, pcb);
#endif
	}


#if 0
	if ((err != ERR_OK) || (p == NULL) || (ehc == NULL))
	{/* error or closed by other side? */
		if (p != NULL) 
		{/* Inform TCP that we have taken the data. */
			tcp_recved(pcb, p->tot_len);
			pbuf_free(p);
		}
		else
		{/* pbuf is null, peer close connection */
			if (ehc == NULL)
			{/* this should not happen, only to be robust */
//			EXT_ERRORF(("Error, http_recv: mhc is NULL, close"));
			}

			err_t err = tcp_close(pcb);
			if (err != ERR_OK)
			{
				EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME": Error closing : %d(%s): %p"EXT_NEW_LINE, err, lwip_strerr(err), (void*)pcb));
				EXT_ERRORF( (HTTP_CLIENT_NAME"Error %d closing %p"EXT_NEW_LINE, err, (void*)pcb));
				/* error closing, try again later in poll */
				tcp_poll(pcb, _connPoll,  HTTPC_POLL_INTERVAL);

				return err;
			}

		}
		
 		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("CONN %s is broken by peer", (ehc!=NULL)?ehc->name:"Unknown"));
 
		return ERR_OK;
	}
#endif

	return ERR_OK;
}

/* A new incoming connection has been accepted */
static err_t _extHttpAccept(void *arg, struct tcp_pcb *pcb, err_t err)
{
	ExtHttpConn *ehc;
	EXT_RUNTIME_CFG *runCfg = NULL;
	
	LWIP_UNUSED_ARG(err);
	runCfg = (EXT_RUNTIME_CFG *)arg;

	EXT_DEBUGF(EXT_DBG_OFF,("_mhttpAccept %p from %s:%d, total Connection %"FOR_U32, 
		(void*)pcb, extCmnIp4addr_ntoa((uint32_t *)&pcb->remote_ip), pcb->remote_port, runCfg->runtime.currentHttpConns ));

	if ((err != ERR_OK) || (pcb == NULL))
	{
		return ERR_VAL;
	}

	/* Set priority */
	tcp_setprio(pcb, MHTTPD_TCP_PRIO);

#if 0//LWIP_EXT_HTTPD_TASK
	extHttpPostEvent(NULL, H_EVENT_NEW, NULL, pcb);
#else
	/* Allocate memory for the structure that holds the state of the connection - initialized by that function. */
	ehc = extHttpConnAlloc(runCfg);
	if (ehc == NULL)
	{
		EXT_ERRORF(("http_accept: Out of memory, RST"));
		tcp_abort(pcb);
		return ERR_MEM;
	}

//	EXT_DEBUGF(EXT_DBG_OFF,("Accept new connection %s(on PCB %p) from %s:%d", ehc->name, pcb, extCmnIp4addr_ntoa((uint32_t *)&pcb->remote_ip), pcb->remote_port ) );
	if(EXT_DEBUG_HTTP_IS_ENABLE())
	{
		printf("Accept new connection from %s:%d"EXT_NEW_LINE, extCmnIp4addr_ntoa((uint32_t *)&pcb->remote_ip), pcb->remote_port );
	}

	runCfg->currentHttpConns++;
	
	ehc->pcb = pcb;
	/* Tell TCP that this is the structure we wish to be passed for our callbacks. */
	tcp_arg(pcb, ehc);
#endif

	/* Set up the various callback functions */
	tcp_recv(pcb, __extHttpRecv);
	tcp_err(pcb,  __extHttpErr);
	tcp_poll(pcb, extHttpPoll,	MHTTPD_POLL_INTERVAL);
	tcp_sent(pcb, __extHttpSent);

	return ERR_OK;
}

/**
 * @ingroup httpd
 * Initialize the httpd: set up a listening PCB and bind it to the defined port
 */
void extHttpSvrMain(void *data)
{
	struct tcp_pcb *pcb;
	err_t err;

	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)data;


#if	0//MHTTPD_USE_MEM_POOL
	LWIP_MEMPOOL_INIT(MHTTPD_STATE);
#if	MHTTPD_SSI
	LWIP_MEMPOOL_INIT(MHTTPD_SSI_STATE);
#endif
#endif
	EXT_DEBUGF(EXT_HTTPD_DEBUG, (__FUNCTION__));

	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	EXT_ASSERT(( "%s: tcp_new failed", __FUNCTION__), pcb != NULL);
	
	tcp_setprio(pcb, MHTTPD_TCP_PRIO);
	/* set SOF_REUSEADDR here to explicitly bind httpd to multiple interfaces */
	err = tcp_bind(pcb, IP_ANY_TYPE, runCfg->httpPort);
	
	LWIP_UNUSED_ARG(err); /* in case of LWIP_NOASSERT */
	EXT_ASSERT(( "%s: tcp_bind on port %d failed: %d",__FUNCTION__, runCfg->httpPort, err), err == ERR_OK);
	
	pcb = tcp_listen(pcb);
	EXT_ASSERT(( "%s: tcp_listen failed", __FUNCTION__), pcb != NULL);

	tcp_arg(pcb, runCfg);

	tcp_accept(pcb, _extHttpAccept);

#if LWIP_EXT_HTTPD_TASK
//	if (sys_mbox_new(&_httpdMailBox, EXT_HTTPD_MBOX_SIZE*4) != ERR_OK)
	if (sys_mbox_new(&_httpdMailBox, MEMP_NUM_HTTP_EVENT) != ERR_OK)
	{
		EXT_ASSERT(("failed to create "EXT_TASK_HTTP" mbox"), 0);
	}

	sys_thread_new(EXT_TASK_HTTP, _extHttpdTask, runCfg, EXT_NET_IF_TASK_STACK_SIZE, EXT_NET_IF_TASK_PRIORITY + 4 );
#endif
	
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
	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("http_set_ssi_handler"));

	EXT_ASSERT(("no ssi_handler given"), ssi_handler != NULL);
	g_pfnSSIHandler = ssi_handler;

#if	MHTTPD_SSI_RAW
	LWIP_UNUSED_ARG(tags);
	LWIP_UNUSED_ARG(num_tags);
#else
	EXT_ASSERT(("no tags given"), tags != NULL);
	EXT_ASSERT(("invalid number of tags"), num_tags > 0);

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
	EXT_ASSERT(("no cgis given"), cgis != NULL);
	EXT_ASSERT(("invalid number of handlers"), num_handlers > 0);

	g_pCGIs = cgis;
	g_iNumCGIs = num_handlers;
}
#endif

