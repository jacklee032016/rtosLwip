

#include "lwipExt.h"

#include "http.h"

#define		EXT_HTTP_CLIENT_MBOX_SIZE			2
#define		EXT_HTTP_CLIENT_PORT					50000


typedef	struct _HttpClient
{
	char					event;
	
	uint32_t				destIp;
	uint16_t				destPort;

	struct tcp_pcb 		*pcb;
	
	EXT_RUNTIME_CFG		*runCfg;
}HttpClient;

static sys_mbox_t 		_httpClientMailBox;

sys_mutex_t			_httpClientMutex;

HttpClient 	_httpClient;


static void _connError(void *arg, err_t err)
{
	// pcb already deallocated
}

static err_t _connPoll(void *arg, struct tcp_pcb *pcb)
{
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

	return(ERR_OK);
}
 
/* lwip calls this function when the remote host has successfully received data (ack) */
static err_t _connSent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
	struct hc_state *state = arg;

	// Reset connection timeout
	state->ConnectionTimeout = 0;
	return(ERR_OK);
}
 

static err_t _connRecv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	struct hc_state *state = arg;
	char * page = NULL;
	struct pbuf * temp_p;
	hc_errormsg errormsg = GEN_ERROR;
	int i;
 
    if((err == ERR_OK) && (p != NULL))
    {
		tcp_recved(pcb, p->tot_len);
 
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
 
    }
 
    // NULL packet == CONNECTION IS CLOSED(by remote host)
    else if((err == ERR_OK) && (p == NULL))
    {	
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
    }
 
    return(ERR_OK);
}
 

 
// lwip calls this function when the connection is established
static err_t _clientConnected(void *arg, struct tcp_pcb *pcb, err_t err)
{
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

	return(ERR_OK);
}


err_t extHttpClientNewConn(HttpClient  *hc)
{
	err_t err;
	ip_addr_t destIp;

	destIp.addr = hc->destPort;
	err = tcp_connect(hc->pcb, &destIp, hc->destPort,  _clientConnected);
 
	return err;
}

static void _extHttpClientTask(void *arg)
{
	HttpEvent	*he;
#if 1	
//	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)arg;
#else
	EXT_JSON_PARSER  *parser = (EXT_JSON_PARSER *)arg;
#endif

	if (sys_mutex_new(&_httpClientMutex) != ERR_OK)
	{
		EXT_ASSERT(("failed to create HTTP Mutex"), 0);
	}


	while (1)
	{
		ExtHttpConn *ehc = NULL;
		
//TRACE();
		sys_mbox_fetch(&_httpClientMailBox, (void **)&he);
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
			if(ehc && ehc->eventCount==0 && (ehc->state == H_STATE_FREE||ehc->state == H_STATE_CLOSE||ehc->state == H_STATE_ERROR))
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


void extHttpClientMain(void *data)
{
	struct tcp_pcb *pcb;
	u16_t port= EXT_HTTP_CLIENT_PORT;
	
	HttpClient *hc = &_httpClient;

	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)data;
	
	memset(hc, 0, sizeof(HttpClient));
	hc->runCfg = runCfg;
#if 0//EXT_HTTPD_DEBUG	

	LWIP_MEMPOOL_INIT(MHTTPD_STATE);
#if	MHTTPD_SSI
	LWIP_MEMPOOL_INIT(MHTTPD_SSI_STATE);
#endif
#endif

	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	EXT_ASSERT(( "%s: HTTP Client failed", __FUNCTION__), pcb != NULL);
	
	tcp_setprio(pcb, MHTTPD_TCP_PRIO);

	while(tcp_bind(pcb, IP_ADDR_ANY, port) != ERR_OK)
	{// Local port in use, use port+1
		port++;
	}
	
	LWIP_UNUSED_ARG(err); /* in case of LWIP_NOASSERT */
	EXT_DEBUGF(EXT_DBG_ON, ( "Client bind on port %d f", port));
	
	tcp_arg(pcb, hc);

	if (sys_mbox_new(&_httpClientMailBox, EXT_HTTP_CLIENT_MBOX_SIZE) != ERR_OK)
	{
		EXT_ASSERT(("failed to create "EXT_TASK_HTTP_CLIENT" mbox"), 0);
	}

	sys_thread_new(EXT_TASK_HTTP_CLIENT, _extHttpClientTask, runCfg, EXT_NET_IF_TASK_STACK_SIZE, EXT_NET_IF_TASK_PRIORITY -1);
	
}


