

#include "lwipExt.h"

#include "http.h"
#include "httpClient.h"

static sys_mbox_t 		_httpClientMailBox;

sys_mutex_t			_httpClientMutex;	/* protect data */

sys_timer_t			_hcTimer;			/* timer new TCP connection */

sys_sem_t			_httpClientSema;	/* wait and wake between client and its scheduler */

HttpClient 			_httpClient;


static char _httpClientPostEvent(HC_EVENT_T eventType, void *_data)
{
	HcEvent *hce;

	EXT_ASSERT(("Invalid mbox"), sys_mbox_valid_val(_httpClientMailBox));
	
	HC_LOCK();	
	HC_EVENT_ALLOC(hce);
	if (hce == NULL)
	{
		EXT_ERRORF(("No memory available for HTTP Client Event now"));
		return EXIT_FAILURE;
	}

	memset(hce, 0, sizeof(HcEvent));
	hce->event = eventType;
	hce->data = _data;
	
	HC_UNLOCK();

#if HTTP_CLIENT_DEBUG		
	if(EXT_DEBUG_HC_IS_ENABLE())
	{
		printf("\tpost event '%s'"EXT_NEW_LINE, CMN_FIND_HC_EVENT(hce->event));
	}
#endif

	if (sys_mbox_trypost(&_httpClientMailBox, hce) != ERR_OK)
	{
		EXT_ERRORF(("Post to "EXT_TASK_HTTP_CLIENT" Mailbox failed"));
		HC_EVENT_FREE(hce);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

static void _connError(void *arg, err_t err)
{
	// pcb already deallocated
//	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME"HTTP Client CONN Error : '%s'", lwip_strerr(err)));
	EXT_ERRORF(("HTTP Client CONN Error : '%s'", lwip_strerr(err)));
	
	_httpClientPostEvent(HC_EVENT_ERROR, NULL);
}

static err_t _connPoll(void *arg, struct tcp_pcb *pcb)
{
#if 0
	if(state->ConnectionTimeout > 20)
	{
		// Close the connection
		tcp_abort(pcb);
	}
#else	
	_httpClientPostEvent(HC_EVENT_POLL, pcb);
#endif
	return(ERR_OK);
}
 
/* lwip calls this function when the remote host has successfully received data (ack) */
static err_t _connSent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
#if 0
	struct hc_state *state = arg;

	// Reset connection timeout
	state->ConnectionTimeout = 0;
#endif
	_httpClientPostEvent(HC_EVENT_SENT, pcb);
	return(ERR_OK);
}
 

static err_t _connRecv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	HttpClient *hc = (HttpClient *)arg;
 
	if((err == ERR_OK) && (p != NULL))
	{
		tcp_recved(pcb, p->tot_len);

// 		EXT_DEBUGF(HTTP_CLIENT_DEBUG, ("#%d Event RECV : data %d bytes :'%.*s'", hc->reqs, p->tot_len, p->tot_len, (char *)p->payload) );
		_httpClientPostEvent(HC_EVENT_RECV, p);
	}
	else if((err == ERR_OK) && (p == NULL))
	{
		if(EXT_DEBUG_HC_IS_ENABLE())
		{
			printf("\t#%d CONN closed by server", hc->reqs);
		}

		extHttpClientClosePcb(hc, pcb);
		_httpClientPostEvent(HC_EVENT_CLOSE, NULL);
	}
	else
	{/* err is not ERR_OK */
		if(p)
		{
			pbuf_free(p);
		}
		
		EXT_ERRORF(("#%d Event RECV error: '%s'", hc->reqs, lwip_strerr(err)));
		_httpClientPostEvent(HC_EVENT_ERROR, NULL);
	}
	

	return(ERR_OK);
}
 

 
/* err always ERR_OK. No timeout is implemented now */
err_t httpClientConnected(void *arg, struct tcp_pcb *pcb, err_t err)
{
//	HttpClient *hc = (HttpClient *)arg;
//	EXT_DEBUGF(HTTP_CLIENT_DEBUG, ("#%d request connected", hc->reqs));

	if(err != ERR_OK)
	{
		EXT_ERRORF(("Error for client connection: %s", lwip_strerr(err)));
	}
	/* stop timer if connection is OK */
 	sys_timer_stop(&_hcTimer);

	_httpClientPostEvent(HC_EVENT_CONNECTED, arg);

	return(ERR_OK);
}



unsigned char httpClientEventConnected(void *arg)
{
	HttpClient *hc = (HttpClient *)arg;
	int index = 0;
	int size = sizeof(hc->buf);

	sys_timer_stop(&_hcTimer);

	EXT_ASSERT((": Client PCB is null"), hc->pcb != NULL);

	CMN_SN_PRINTF(hc->buf, size, index, "GET /%s HTTP/1.0"EXT_NEW_LINE EXT_NEW_LINE, hc->req->uri);

//	usprintf(headers, "POST /%s HTTP/1.0\r\nContent-type: application/x-www-form-urlencoded\r\nContent-length: %d\r\n\r\n%s\r\n\r\n", state->Page, strlen(state->PostVars), state->PostVars);

	tcp_recv(hc->pcb, _connRecv);
	tcp_err(hc->pcb, _connError);

	tcp_poll(hc->pcb, _connPoll, HTTPC_POLL_INTERVAL);					 	
	tcp_sent(hc->pcb, _connSent);

	// Send data
	tcp_write(hc->pcb, hc->buf, index, TCP_WRITE_FLAG_COPY);
	tcp_output(hc->pcb);

//	EXT_DEBUGF(HTTP_CLIENT_DEBUG, ("#%d send HTTP reqeust "EXT_NEW_LINE"'%.*s'", hc->reqs, index, hc->buf ));
	if(EXT_DEBUG_HC_IS_ENABLE())
	{
		printf("#%d send HTTP reqeust "EXT_NEW_LINE"'%.*s'"EXT_NEW_LINE, hc->reqs, index, hc->buf);
	}


	hc->contentLength = 0;
	return HC_STATE_CONN;
}

static void _hcTimerCallback(void *arg)
{
	_httpClientPostEvent(HC_EVENT_TIMEOUT, NULL);
}


static void _extHttpClientTask(void *arg)
{
	HcEvent *hce = NULL;
//	HttpClient *hc = (HttpClient *)arg;

	if (sys_mutex_new(&_httpClientMutex) != ERR_OK)
	{
		EXT_ASSERT(("failed to create HTTP Mutex"), 0);
	}

	sys_timer_new(&_hcTimer, _hcTimerCallback, os_timer_type_once, (void *)arg);


	while (1)
	{
		
//TRACE();
		sys_mbox_fetch(&_httpClientMailBox, (void **)&hce);
//TRACE();
		if (hce == NULL)
		{
			EXT_ASSERT(("task: invalid message"), 0);
			continue;
		}
//TRACE();

		httpClientFsmHandle(hce);
		
//TRACE();
		HC_LOCK();	
		HC_EVENT_FREE(hce);
		HC_UNLOCK();	
//TRACE();
	}
	
}


void extHttpClientMain(void *data)
{
	HttpClient *hc = &_httpClient;

	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)data;
	
	memset(hc, 0, sizeof(HttpClient));
	hc->req = NULL;
//	hc->req.port = -1;
	hc->state = HC_STATE_WAIT;
	hc->runCfg = runCfg;

	err_t err;
	/* Incoming packet notification semaphore. */
	err = sys_sem_new(&_httpClientSema, 1);
	EXT_ASSERT(("semaphore allocation ERROR!"), (err == ERR_OK));
	if (err == ERR_MEM)
	{
		return;// ERR_MEM;
	}

//	if (sys_mbox_new(&_httpClientMailBox, EXT_HTTP_CLIENT_MBOX_SIZE) != ERR_OK)
	if (sys_mbox_new(&_httpClientMailBox, MEMP_NUM_HC_EVENT) != ERR_OK)
	{
		EXT_ASSERT(("failed to create "EXT_TASK_HTTP_CLIENT" mbox"), 0);
	}

	sys_thread_new(EXT_TASK_HTTP_CLIENT, _extHttpClientTask, hc, EXT_NET_IF_TASK_STACK_SIZE, EXT_NET_IF_TASK_PRIORITY -1);
	
}

/* called by HTTP Client task, TCP/IP task, HTTP Svr task, Cmd Task */
void extHttpClientClearCurrentRequest(HttpClient *hc)
{
	HttpClientReq *req = hc->req;
	
	EXT_ASSERT(("No Client request is need to clean"), (req != NULL) );

	HC_LOCK();

	memset(hc->buf, 0, sizeof(hc->buf));
	hc->length = 0;

	hc->req = req->next;
	req->next = NULL;

	HC_UNLOCK();
	
	if(hc->req )
	{
		_httpClientPostEvent(HC_EVENT_NEW, hc->req);
	}

}

/* called by other tasks, such as sched (from http server) and console(cmd) */
err_t extHttpClientNewRequest(HttpClientReq *req)
{
	HttpClient *hc = &_httpClient;

	if(req->ip == IPADDR_NONE || req->port == -1 || IS_STRING_NULL(req->uri) )
	{
		EXT_ERRORF(("#%d requesting parameter is error %s:%d/%s", hc->reqs, extCmnIp4addr_ntoa(&req->ip), req->port, (IS_STRING_NULL(req->uri))?"None":req->uri ));
		return ERR_VAL;
	}

	/* protect */
#if 1
	if(! HTTP_CLIENT_IS_NOT_REQ(hc))
	{
		EXT_DEBUGF(EXT_DBG_OFF, ("#%d is busy on requesting %s:%d/%s, new req on %s will wait", hc->reqs, extCmnIp4addr_ntoa(&hc->req->ip), hc->req->port, hc->req->uri, req->uri ));
		APPEND_ELEMENT(hc->req, req, HttpClientReq);
		return ERR_ALREADY;
	}
#endif

	APPEND_ELEMENT(hc->req, req, HttpClientReq);

	_httpClientPostEvent(HC_EVENT_NEW, req);
	if(EXT_DEBUG_HC_IS_ENABLE())
	{
		printf("\t#%d requesting %s:%d/%s"EXT_NEW_LINE, hc->reqs, extCmnIp4addr_ntoa(&hc->req->ip), hc->req->port, hc->req->uri);
	}


	return ERR_OK;
}

/* called by TCPIP task and http client task */
err_t extHttpClientClosePcb(HttpClient *hc, struct tcp_pcb *_pcb)
{
	struct tcp_pcb *pcb = pcb;

	if(hc!= NULL)
	{
		pcb = hc->pcb;
	}

	EXT_ASSERT((" PCB is null"), pcb!= NULL);
		
	err_t err = tcp_close(pcb);
	if (err != ERR_OK)
	{
//		EXT_DEBUGF(HTTP_CLIENT_DEBUG, (": Error closing : %d(%s): %p"EXT_NEW_LINE, err, lwip_strerr(err), (void*)pcb));
		EXT_ERRORF( ("Error %d closing %p"EXT_NEW_LINE, err, (void*)pcb));
		/* error closing, try again later in poll */
		tcp_poll(pcb, _connPoll,  HTTPC_POLL_INTERVAL);

		return err;
	}

	if(hc != NULL)
	{
		HTTP_CLIENT_SET_PCB(hc, NULL);
	}

	tcp_arg(pcb, NULL);
	
	tcp_recv(pcb, NULL);
	tcp_err(pcb, NULL);
	tcp_poll(pcb, NULL, 0);
	tcp_sent(pcb, NULL);

	return err;
}

