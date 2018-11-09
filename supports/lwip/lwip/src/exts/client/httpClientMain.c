

#include "lwipExt.h"

#include "http.h"
#include "httpClient.h"

static sys_mbox_t 		_httpClientMailBox;

sys_mutex_t			_httpClientMutex;

HttpClient 	_httpClient;

sys_timer_t		_hcTimer;		/* timer new TCP connection */


char _httpClientPostEvent(HC_EVENT_T eventType, void *_data)
{
	HcEvent *hce;

	EXT_ASSERT(("Invalid mbox"), sys_mbox_valid_val(_httpClientMailBox));
	
	HC_LOCK();	
	HC_EVENT_ALLOC(hce);
	if (hce == NULL)
	{
		EXT_ERRORF((HTTP_CLIENT_NAME"No memory available for HTTP Client Event now"));
		return EXIT_FAILURE;
	}

	memset(hce, 0, sizeof(HcEvent));
	hce->event = eventType;
	hce->data = _data;
	
	HC_UNLOCK();
	
	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME": post event %s", CMN_FIND_HC_EVENT(hce->event)) );

	if (sys_mbox_trypost(&_httpClientMailBox, hce) != ERR_OK)
	{
		EXT_ERRORF((HTTP_CLIENT_NAME"Post to "EXT_TASK_HTTP_CLIENT" Mailbox failed"));
		HC_EVENT_FREE(hce);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

static void _connError(void *arg, err_t err)
{
	// pcb already deallocated
	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME"HTTP Client CONN Error : '%s'", lwip_strerr(err)));
	
	_httpClientPostEvent(HC_EVENT_ERROR, NULL);
}

static err_t _connPoll(void *arg, struct tcp_pcb *pcb)
{
#if 0
	struct hc_state *state = arg;

	state->ConnectionTimeout++;
	if(state->ConnectionTimeout > 20)
	{
		// Close the connection
		tcp_abort(pcb);

		// Give err msg to callback function 
		// Call return function
		(*state->ReturnPage)(state->Num, TIMEOUT, NULL, 0);
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
#if 0
	struct hc_state *state = arg;
	char * page = NULL;
	struct pbuf * temp_p;
	hc_errormsg errormsg = GEN_ERROR;
	int i;
#endif
 
    if((err == ERR_OK) && (p != NULL))
    {
		tcp_recved(pcb, p->tot_len);

 #if 0
		// Add payload (p) to state
		temp_p = p;
		while(temp_p != NULL)
		{
			state->RecvData = realloc(state->RecvData, temp_p->len + state->Len + 1);
 
			// CHECK 'OUT OF MEM'
			if(state->RecvData == NULL)
			{
				// OUT OF MEMORY
				(*state->ReturnPage)(state->Num, OUT_MEM, NULL, 0);	
				return(ERR_OK);
			}
 
			strncpy(state->RecvData + state->Len, temp_p->payload, temp_p->len);
			state->RecvData[temp_p->len + state->Len] = '\0';			
			state->Len += temp_p->len;
 
			temp_p = temp_p->next;
		}
 
		// Removing payloads
 
		while(p != NULL)
		{
			temp_p = p->next;
			pbuf_free(p);
			p = temp_p;
		}
 #else
 		EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME": Event RECV : data %d bytes :%.*s", p->tot_len, p->tot_len, (char *)p->payload) );
		_httpClientPostEvent(HC_EVENT_RECV, p);
 #endif
    }
    else if((err == ERR_OK) && (p == NULL))
    {
#if 0    
		// Simple code for checking 200 OK
		for(i=0; i < state->Len; i++)
		{
			if(errormsg == GEN_ERROR)
			{
				// Check for 200 OK 
				if((*(state->RecvData+i) == '2') && (*(state->RecvData+ ++i) == '0') && (*(state->RecvData+ ++i) == '0')) errormsg = OK;
				if(*(state->RecvData+i) == '\n') errormsg = NOT_FOUND;
			}
			else
			{
				// Remove headers
				if((*(state->RecvData+i) == '\r') && (*(state->RecvData+ ++i) == '\n') && (*(state->RecvData+ ++i) == '\r') && (*(state->RecvData + ++i) == '\n'))
				{
					i++;
					page = malloc(strlen(state->RecvData+i));
					strcpy(page, state->RecvData+i);
					break;
				}
			}
		}
 
		if(errormsg == OK)
		{
			// Put recv data to ---> p->ReturnPage
			(*state->ReturnPage)(state->Num, OK, page, state->Len);
		}
		else
		{
			// 200 OK not found Return NOT_FOUND (WARNING: NOT_FOUND COULD ALSO BE 5xx SERVER ERROR, ...) 
			(*state->ReturnPage)(state->Num, errormsg, NULL, 0);
		}
 
        // Clear the PCB
        _hcClearPcb(pcb);
 
		// free the memory containing state
		free(state->RecvData);
		free(state);
#else
		EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME": Event RECV : CONN closed") );
		_httpClientPostEvent(HC_EVENT_CLOSE, NULL);
#endif
	}
	else
	{/* err is not ERR_OK */
		EXT_ERRORF((HTTP_CLIENT_NAME": Event RECV error: '%s'", lwip_strerr(err)));
		_httpClientPostEvent(HC_EVENT_ERROR, NULL);
	}
	

	return(ERR_OK);
}
 

 
/* err always ERR_OK */
err_t httpClientConnected(void *arg, struct tcp_pcb *pcb, err_t err)
{
#if 1
	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME": connected"));

	_httpClientPostEvent(HC_EVENT_CONNECTED, arg);
#else
	struct hc_state *state = arg;
	char  * headers;

	if(err != ERR_OK)
	{
		_hcClearPcb(pcb);

		// Call return function
		(*state->ReturnPage)(state->Num, GEN_ERROR, NULL, 0);

		// Free wc state
		free(state->RecvData);
		free(state);

		return(ERR_OK);
	}

	// Define Headers
	if(state->PostVars == NULL)
	{/* GET headers (without page)(+ \0) = 19 */
		headers = malloc(19 + strlen(state->Page));
		usprintf(headers,"GET /%s HTTP/1.0\r\n\r\n", state->Page);
	}
	else
	{/* POST headers (without PostVars or Page)(+ \0) = 91 */
		// Content-length: %d <== 						   ??? (max 10)
		headers = malloc(91 + strlen(state->PostVars) + strlen(state->Page) + 10);
		usprintf(headers, "POST /%s HTTP/1.0\r\nContent-type: application/x-www-form-urlencoded\r\nContent-length: %d\r\n\r\n%s\r\n\r\n", state->Page, strlen(state->PostVars), state->PostVars);
	}

	// Check if we are nut running out of memory
	if(headers == NULL)
	{
		_hcClearPcb(pcb);

		// Call return function
		(*state->ReturnPage)(state->Num, OUT_MEM, NULL, 0);

		// Free wc state
		free(state->RecvData);
		free(state);

		return(ERR_OK);
	}

	tcp_recv(pcb, _connRecv);
	tcp_err(pcb, _connError);

	// Setup the TCP polling function/interval	 //TCP_POLL IS NOT CORRECT DEFINED @ DOC!!!
	tcp_poll(pcb, _connPoll, 10);					 	
	tcp_sent(pcb, _connSent);

	// Send data
	tcp_write(pcb, headers, strlen(headers), 1);
	tcp_output(pcb);

	// remove headers
	free(headers);
	free(state->PostVars);
	free(state->Page);
#endif

	return(ERR_OK);
}



unsigned char httpClientEventConnected(void *arg)
{
	HttpClient *hc = (HttpClient *)arg;
	int index = 0;
	int size = sizeof(hc->buf);

	sys_timer_stop(&_hcTimer);

	EXT_ASSERT((HTTP_CLIENT_NAME": Client PCB is null"), hc->pcb != NULL);

	CMN_SN_PRINTF(hc->buf, size, index, "GET /%s HTTP/1.0"EXT_NEW_LINE EXT_NEW_LINE, hc->req.url);

//	usprintf(headers, "POST /%s HTTP/1.0\r\nContent-type: application/x-www-form-urlencoded\r\nContent-length: %d\r\n\r\n%s\r\n\r\n", state->Page, strlen(state->PostVars), state->PostVars);

	tcp_recv(hc->pcb, _connRecv);
	tcp_err(hc->pcb, _connError);

	tcp_poll(hc->pcb, _connPoll, HTTPC_POLL_INTERVAL);					 	
	tcp_sent(hc->pcb, _connSent);

	// Send data
	tcp_write(hc->pcb, hc->buf, index, TCP_WRITE_FLAG_COPY);
	tcp_output(hc->pcb);

	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME": send HTTP reqeust "EXT_NEW_LINE"'%.*s'", index, hc->buf ));

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
		EXT_ASSERT((HTTP_CLIENT_NAME": failed to create HTTP Mutex"), 0);
	}

	sys_timer_new(&_hcTimer, _hcTimerCallback, os_timer_type_once, (void *)arg);


	while (1)
	{
		
//TRACE();
		sys_mbox_fetch(&_httpClientMailBox, (void **)&hce);
//TRACE();
		if (hce == NULL)
		{
			EXT_ASSERT((HTTP_CLIENT_NAME": task: invalid message"), 0);
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
	struct tcp_pcb *pcb;
	u16_t port= EXT_HTTP_CLIENT_PORT;
	
	HttpClient *hc = &_httpClient;

	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)data;
	
	memset(hc, 0, sizeof(HttpClient));
	hc->req.destIp = IPADDR_NONE;
	hc->req.destPort = -1;
	hc->state = HC_STATE_WAIT;
	hc->runCfg = runCfg;

	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	EXT_ASSERT(( HTTP_CLIENT_NAME": failed"), pcb != NULL);
	hc->pcb = pcb;
	
	tcp_setprio(pcb, MHTTPD_TCP_PRIO);

	while(tcp_bind(pcb, IP_ADDR_ANY, port) != ERR_OK)
	{// Local port in use, use port+1
		port++;
	}
	
	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME ": bind on port %d", port));
	
	tcp_arg(pcb, hc);

	if (sys_mbox_new(&_httpClientMailBox, EXT_HTTP_CLIENT_MBOX_SIZE) != ERR_OK)
	{
		EXT_ASSERT((HTTP_CLIENT_NAME": failed to create "EXT_TASK_HTTP_CLIENT" mbox"), 0);
	}

	sys_thread_new(EXT_TASK_HTTP_CLIENT, _extHttpClientTask, hc, EXT_NET_IF_TASK_STACK_SIZE, EXT_NET_IF_TASK_PRIORITY -1);
	
}


void extHttpClientSetRequest(HttpClient *hc, uint32_t dest, uint16_t port, char *uri)
{
	HC_LOCK();

	memset(hc->buf, 0, sizeof(hc->buf));
	hc->length = 0;
	
	hc->req.destIp = dest;
	hc->req.destPort = port;
	if(IS_STRING_NULL(uri))
	{
		memset(hc->req.url, 0, sizeof(hc->req.url) );
	}
	else 
	{
		snprintf(hc->req.url, sizeof(hc->req.url), "%s", uri);
	}

	HC_UNLOCK();
}


char extHttpClientNewRequest(uint32_t dest, uint16_t port, char *uri)
{
	HttpClient *hc = &_httpClient;

	if(! HTTP_CLIENT_IS_NOT_REQ(hc))
	{
		EXT_ERRORF((HTTP_CLIENT_NAME": has been requesting %s:%d", extCmnIp4addr_ntoa((unsigned int *)&hc->req.destIp), hc->req.destPort ));
//		return ERR_ALREADY;
		return EXIT_FAILURE;
	}

	if(dest== IPADDR_NONE || port == -1 || IS_STRING_NULL(uri) )
	{
		EXT_ERRORF((HTTP_CLIENT_NAME": requesting parameter is error %s:%d/%s", extCmnIp4addr_ntoa((unsigned int *)&dest), hc->req.destPort, (uri==NULL)?"None":uri ));
//		return ERR_VAL;
		return EXIT_FAILURE;
	}
	
	extHttpClientSetRequest(hc, dest, port, uri);

	_httpClientPostEvent(HC_EVENT_NEW, NULL);

//	return ERR_OK;
	return EXIT_SUCCESS;
}

