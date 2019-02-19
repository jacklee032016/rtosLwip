

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
		HC_UNLOCK();
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
	HttpClient *hc = (HttpClient *)arg;
	// pcb already deallocated
//	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME"HTTP Client CONN Error : '%s'", lwip_strerr(err)));
//	EXT_ERRORF(("HTTP Client CONN %s in state of %s, Error : '%s'", (hc->reqList)?hc->reqList->uri:"NULL", CMN_FIND_HC_STATE(hc->state), lwip_strerr(err)));

	/* pcb has been deallocated, so the last request has been ended. This event need not to send to new request. 02.15, 2019 */
/*
	
	if(hc->state == HC_STATE_WAIT)
		return;
	
	_httpClientPostEvent(HC_EVENT_ERROR, NULL);
*/	
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
			printf("\t#%"U32_F" CONN closed by server"EXT_NEW_LINE, hc->reqs);
		}

//		EXT_ERRORF(("HttpClient: Connect is closed by server") );
		extHttpClientClosePcb(hc, pcb);
		_httpClientPostEvent(HC_EVENT_CLOSE, NULL);
	}
	else
	{/* err is not ERR_OK */
		if(p)
		{
			pbuf_free(p);
		}
		
		EXT_ERRORF(("#%"U32_F" Event RECV error: '%s'", hc->reqs, lwip_strerr(err)));
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

	HttpClientReq *req = hc->reqList;/* this pointer to RunCfg */
	EXT_ASSERT(("Req is null for HTTP client"), req!= NULL);

	sys_timer_stop(&_hcTimer);

	EXT_ASSERT((": Client PCB is null"), hc->pcb != NULL);

	CMN_SN_PRINTF(hc->buf, size, index, "GET /%s HTTP/1.0"EXT_NEW_LINE EXT_NEW_LINE, req->uri);

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
		printf("#%"U32_F" send HTTP reqeust "EXT_NEW_LINE"'%.*s'"EXT_NEW_LINE, hc->reqs, index, hc->buf);
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

#if HTTP_CLIENT_DEBUG
static void extHttpClientReqDebug(HttpClient *hc)
{
	HttpClientReq *req = hc->reqList;
	int index = 0;

	while(req)
	{
		EXT_DEBUGF(EXT_DBG_ON, ("#%d request %p: url: '%s'", ++index, req, req->uri) );
		req = req->next;
	}
}
#endif

void extHttpClientMain(void *data)
{
	HttpClient *hc = &_httpClient;

	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)data;
	
	memset(hc, 0, sizeof(HttpClient));
	hc->reqList = NULL;
	hc->requests[0].req.type = HC_REQ_SDP_VIDEO;
	hc->requests[1].req.type = HC_REQ_SDP_AUDIO;
	hc->requests[2].req.type = HC_REQ_SDP_ANC;

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

	sys_thread_new(EXT_TASK_HTTP_CLIENT, _extHttpClientTask, hc, EXT_NET_IF_TASK_STACK_SIZE, EXT_NET_IF_TASK_PRIORITY + 4);
	
}

/* called by HTTP Client task, TCP/IP task, HTTP Svr task, Cmd Task */
void extHttpClientClearCurrentRequest(HttpClient *hc)
{
	HttpClientReq *req = hc->reqList;
	
	HC_LOCK();

	memset(hc->buf, 0, sizeof(hc->buf));
	hc->length = 0;

	EXT_ASSERT(("No Client request is need to clean"), (req != NULL) );
	if(req)
	{
		hc->reqList = req->next;
		req->next = NULL;
	}

	HC_UNLOCK();
	
	if(hc->reqList )
	{
		extHttpClientBeginRequest(hc->reqList);
	}

}

/* called by other tasks, such as sched (from http server) and console(cmd) */
static err_t extHttpClientNewRequest(HttpClient *hc, HttpClientReq *req)
{
	HttpClientReq *_newReq = NULL;

	if(req->ip == IPADDR_NONE || req->port == INVALIDATE_VALUE_U16 || IS_STRING_NULL(req->uri) )
	{
		EXT_ERRORF(("#%"U32_F" requesting parameter is error %s:%d/%s", hc->reqs, extCmnIp4addr_ntoa(&req->ip), req->port, (IS_STRING_NULL(req->uri))?"None":req->uri ));
		return ERR_VAL;
	}

	HC_LOCK();	
	
	if(req->type == HC_REQ_SDP_VIDEO)
	{
		_newReq =  (HttpClientReq *) &hc->requests[0];
	}
	else if(req->type == HC_REQ_SDP_AUDIO)
	{
		_newReq =  (HttpClientReq *) &hc->requests[1];
	}
	else
	{
		_newReq = (HttpClientReq *) &hc->requests[2];
	}

	memcpy(_newReq, req, sizeof(HttpClientReq));
	_newReq->next = NULL; //??//

	/* protect */
//	EXT_DEBUGF(EXT_DBG_OFF, ("Queuing request #%d %p: '%s:%d/%s'", hc->reqs, _newReq, extCmnIp4addr_ntoa(&_newReq->ip), _newReq->port, _newReq->uri));

	APPEND_ELEMENT(hc->reqList, _newReq, HttpClientReq);

	HC_UNLOCK();

	return ERR_OK;
}

/* called by TCPIP task and http client task */
err_t extHttpClientClosePcb(HttpClient *hc, struct tcp_pcb *_pcb)
{
	struct tcp_pcb *pcb = _pcb;

	if(hc!= NULL)
	{
		pcb = hc->pcb;
	}

//	EXT_ASSERT((" PCB is null"), pcb!= NULL);

	if(pcb == NULL)
	{
//		EXT_ERRORF(("PCB is null now"));
		return ERR_RST;
	}
		
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


void extHttpClientBeginRequest(HttpClientReq *req)
{
	HttpClient *hc = &_httpClient;
	
	hc->reqs++;
	_httpClientPostEvent(HC_EVENT_NEW, req);
	if(EXT_DEBUG_HC_IS_ENABLE())
	{
		printf("\t#%"U32_F" starting request to %s:%d/%s"EXT_NEW_LINE, hc->reqs, extCmnIp4addr_ntoa(&req->ip), req->port, req->uri);
	}
}

err_t extHttpClientStart(EXT_RUNTIME_CFG *runCfg)
{
	err_t  ret;
	HttpClient *hc = &_httpClient;

	if(!HTTP_CLIENT_IS_NOT_REQ(hc) )
	{
		EXT_ERRORF(("HttpClient is busy now"));
		return ERR_ALREADY;
	}
	
	runCfg->sdpUriVideo.next = NULL;
	ret = extHttpClientNewRequest(hc, &runCfg->sdpUriVideo);
	
	runCfg->sdpUriAudio.next = NULL;
	ret = extHttpClientNewRequest(hc, &runCfg->sdpUriAudio);

	runCfg->sdpUriAnc.next = NULL;
	ret = extHttpClientNewRequest(hc, &runCfg->sdpUriAnc);

	ret = ret;
#if HTTP_CLIENT_DEBUG
	extHttpClientReqDebug(hc);
#endif

	if(hc->reqList )
	{
		extHttpClientBeginRequest(hc->reqList);
	}

	return ERR_OK;
}

uint16_t extHttpClientStatus(char *buf, uint16_t size)
{
	HttpClient *hc = &_httpClient;
	uint16_t index = 0;

	CMN_SN_PRINTF(buf, size, index, "\t<DIV class=\"field\"><LABEL >Video Status:</LABEL>"EXT_NEW_LINE"\t\tTotal:%"U32_F"; ConnErrors:%"U32_F"; DataErrors:%"U32_F"; Msg: '%s'</DIV>" EXT_NEW_LINE, 
			hc->requests[0].total, hc->requests[0].httpFails, hc->requests[0].dataErrors, hc->requests[0].msg);
	CMN_SN_PRINTF(buf, size, index, "\t<DIV class=\"field\"><LABEL >Audio Status:</LABEL>"EXT_NEW_LINE"\t\tTotal:%"U32_F"; ConnErrors:%"U32_F"; DataErrors:%"U32_F"; Msg: '%s'</DIV>" EXT_NEW_LINE, 
			hc->requests[1].total, hc->requests[1].httpFails, hc->requests[1].dataErrors, hc->requests[0].msg);
	CMN_SN_PRINTF(buf, size, index, "\t<DIV class=\"field\"><LABEL >ANC Status:</LABEL>"EXT_NEW_LINE"\t\tTotal:%"U32_F"; ConnErrors:%"U32_F"; DataErrors:%"U32_F"; Msg: '%s'</DIV>" EXT_NEW_LINE, 
			hc->requests[2].total, hc->requests[2].httpFails, hc->requests[2].dataErrors, hc->requests[0].msg);

	return index;
}

char	cmnCmdSdpInfo(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	HttpClient *hc = &_httpClient;
	int index = 0;
	int i = 0;
	ip4_addr_t destIp;

	index += snprintf(outBuffer+index, bufferLen-index, "MediaSet: %s; Total SDP requests: %"U32_F"; Timestamp:%s"EXT_NEW_LINE, FPGA_CFG_STR_NAME(hc->runCfg->fpgaAuto), hc->reqs, sysTimestamp() );
	for(i=0; i<3; i++)
	{
		destIp.addr = hc->requests[i].req.ip;
		index += snprintf(outBuffer+index, bufferLen-index, "\t#%d: http://%s:%d/%s:\tTotal:%"U32_F"; ConnErrors:%"U32_F"; DataErrors:%"U32_F"; Msg: '%s'"EXT_NEW_LINE, i+1,
			ip4addr_ntoa(&destIp), hc->requests[i].req.port, hc->requests[i].req.uri, hc->requests[i].total, hc->requests[i].httpFails, hc->requests[i].dataErrors, hc->requests[i].msg);
	}

	return EXT_FALSE;
}


