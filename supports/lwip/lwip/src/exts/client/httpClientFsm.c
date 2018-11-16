
#include "lwipExt.h"

#include "http.h"
#include "httpClient.h"

#include "extFsm.h"

/*
* Event handlers 
*/
static unsigned char _hcEventNewReq(void *arg)
{
	HttpClient *hc = (HttpClient *)arg;
	err_t err;
	ip4_addr_t destIp;

	HcEvent *he = hc->evt;
	HttpClientReq *req = he->data;/* this pointer to RunCfg */
	EXT_ASSERT(("Req is not null for HTTP client"), req!= NULL);

	destIp.addr = req->ip;
	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME"sent #%d TCP request to %s:%d", hc->reqs++, ip4addr_ntoa(&destIp), req->port)) ;

	struct tcp_pcb *pcb;
	static u16_t localport= EXT_HTTP_CLIENT_PORT;

	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	EXT_ASSERT(( HTTP_CLIENT_NAME"failed"), pcb != NULL);
	
	tcp_setprio(pcb, MHTTPD_TCP_PRIO);

	while(tcp_bind(pcb, IP_ADDR_ANY, localport) != ERR_OK)
	{// Local port in use, use port+1
		localport++;
	}
	
	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME "#%d request bind on port %d", hc->reqs, localport));
	localport++;
	
	tcp_arg(pcb, hc);

	HTTP_CLIENT_SET_PCB(hc, pcb);
	
	err = tcp_connect(hc->pcb, &destIp, req->port,  httpClientConnected);
	if(err != ERR_OK)
	{
		EXT_ERRORF((HTTP_CLIENT_NAME"send TCP req failed: '%s': %d", lwip_strerr(err), hc->pcb->state));
		HTTP_CLIENT_CLEAR_REQ(hc);
		return EXT_STATE_CONTINUE;
	}

	extHttpClientSetRequest(hc, req);

// 	sys_timer_start(&_hcTimer, EXT_HTTP_CLIENT_TIMEOUT_NEW_CONN);
//	hc->reqs++;

	return HC_STATE_INIT;
}

static unsigned char _hcEventNewReqInOtherStates(void *arg)
{
	HttpClient *hc = (HttpClient *)arg;

	HcEvent *he = hc->evt;
	HttpClientReq *req = he->data;/* this pointer to RunCfg */
	EXT_ASSERT(("Req is not null for HTTP client"), req!= NULL);

//	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME"New REQ when current req is working")) ;
	EXT_INFOF( (HTTP_CLIENT_NAME"New REQ when current req is working")) ;

	extHttpClientNewRequest(req);
	return EXT_STATE_CONTINUE;
}



static unsigned char _hcEventTimeout(void *arg)
{
	HttpClient *hc = (HttpClient *)arg;

	if(hc->state == HC_STATE_INIT)
	{
		hc->fails++;
	}
	extHttpClientClosePcb(hc, NULL);

#if 0
//	if(hc->state != HC_STATE_WAIT)
	{
		tcp_close(hc->pcb);
	}
#endif

	snprintf(hc->msg, sizeof(hc->msg), "Timeout in state of %s", CMN_FIND_HC_STATE(hc->state) );

	return HC_STATE_WAIT;
}

static unsigned char _hcEventPoll(void *arg)
{
	HttpClient *hc = (HttpClient *)arg;

	if(hc->state == HC_STATE_WAIT)
	{
		return EXT_STATE_CONTINUE;
	}
	
	hc->retryCount ++;
	if (hc->retryCount == HTTPC_MAX_RETRIES)
	{
		return HC_STATE_WAIT;
	}
	
	return EXT_STATE_CONTINUE;
}

/* tcp err callback or err in TCP recv callback */
static unsigned char _hcEventError(void *arg)
{
	HttpClient *hc = (HttpClient *)arg;

	HTTP_CLIENT_CLEAR_REQ(hc);

	return HC_STATE_ERROR;
}

static char _findResponseType(char *response, uint32_t size)
{
	if(lwip_strnstr(response, HTTP_HDR_SDP, size) != NULL )
	{
		return HC_REQ_SDP;
	}
	else if(lwip_strnstr(response, HTTP_HDR_JSON, size) != NULL )
	{
		return HC_REQ_JSON;
	}

	return HC_REQ_UNKNOWN;

}

static unsigned char _hcEventRecv(void *arg)
{
	HttpClient *hc = (HttpClient *)arg;
	struct pbuf *p, *tmp;
//	char *data;
	u16_t reqLen;
	u16_t copied;


	EXT_ASSERT((HTTP_CLIENT_NAME"Event or pbuf is not found "), hc->evt!= NULL && hc->evt->data != NULL );
	p = (struct pbuf *)hc->evt->data;
	
//	if(p->next != NULL)
	{
		reqLen = LWIP_MIN(p->tot_len, sizeof(hc->buf));
		copied = pbuf_copy_partial(p, hc->buf, reqLen, 0);
		EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME": recv %d bytes, copied %d byte", reqLen, copied));
		hc->length = reqLen;
//		data = hc->buf;

	EXT_DEBUGF(EXT_HTTPD_DEBUG, (HTTP_CLIENT_NAME"recv:'%.*s'" , copied, hc->buf) );
	CONSOLE_DEBUG_MEM((unsigned char *)hc->buf, copied, 0, "RECV HTTP RES Data");

	}
#if 0
	else
	{
		data = p->payload;
		reqLen = p->len;
		if (p->len != p->tot_len)
		{
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Warning: incomplete header due to chained pbufs"));
		}
	}
#endif

	uint32_t ret;
	
	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME"recv %d bytes data:"EXT_NEW_LINE"'%.*s'", hc->length, hc->length, hc->buf));

	hc->reqType = _findResponseType(hc->buf, (uint32_t)hc->length);

	ret = cmnHttpParseHeaderContentLength(hc->buf, (uint32_t)hc->length);
	if(ret <= ERR_OK )
	{
		EXT_ERRORF( (HTTP_CLIENT_NAME"Error when parsing number in Content-Length: '%.*s'", hc->length, hc->buf) );
	}
	else
	{
		char *data;
		uint32_t _dataLen = 0;
		

		data = lwip_strnstr(hc->buf, MHTTP_CRLF MHTTP_CRLF, hc->length);
		if( data != NULL )
		{
			_dataLen = hc->length - (data - hc->buf) - __HTTP_CRLF_SIZE;
			if(_dataLen == ret )
			{
				hc->dataLength = ret;
				hc->data = hc->buf + (hc->length - hc->dataLength);
			}
			else
			{
				EXT_ERRORF((HTTP_CLIENT_NAME"Content Length %d is not same as data length:%d",ret, _dataLen) );
			}
		}
	}


	while(p != NULL)
	{
		tmp = p->next;
		pbuf_free(p);
		p = tmp;
	}

	if(hc->reqType == HC_REQ_UNKNOWN)
	{
		EXT_ERRORF((HTTP_CLIENT_NAME"Response is not supported type of JSON or SDP"));
		return HC_STATE_ERROR;
	}
	
	if(hc->dataLength <= 0)
	{
		return HC_STATE_ERROR;
	}
	
	
	return HC_STATE_DATA;
}

static unsigned char _hcEventSent(void *arg)
{
	HttpClient *hc = (HttpClient*)arg;

	hc->retryCount = 0;
	return EXT_STATE_CONTINUE;
}

/* in RECV callback, when pbuf is NULL: means closed by peer */
static unsigned char _hcEventClose(void *arg)
{
	HttpClient *hc = (HttpClient *)arg;
	
 	sys_timer_stop(&_hcTimer);
	/* all resource of TCP is freed in TCP task, eg. callback of close */
	//tcp_close(hc->pcb);
	
	return HC_STATE_WAIT;
}


/* 
*  Enter handlers of every state
*/
/* clear all data */
static void _hcEnterStateWait(void *arg)
{
	HttpClient *hc = (HttpClient *)arg;

	/* stop timer which is started by entering state of DATA or ERROR */
 	sys_timer_stop(&_hcTimer);
	
	HTTP_CLIENT_CLEAR_REQ(hc);
}


/* parse data and configure to hw */
static void _hcEnterStateData(void *arg)
{
	HttpClient *hc = (HttpClient *)arg;
	
 	sys_timer_start(&_hcTimer, EXT_HTTP_CLIENT_TIMEOUT_2_WAIT);

	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME"recv %d bytes "EXT_NEW_LINE"'%.*s', data length %d:"EXT_NEW_LINE"'%.*s'", 
		hc->length, hc->length, hc->buf, hc->dataLength, hc->dataLength, hc->data) );
	
	hc->statusCode = 200;
	
}

/* TCP err callback or err_t is not ERR_OK in TCP recv callback  */
static void _hcEnterStateError(void *arg)
{
#if 0
	HttpEvent *he = (HttpEvent *)arg;
#endif	
	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME": enter ERROR state"));

 	sys_timer_start(&_hcTimer, EXT_HTTP_CLIENT_TIMEOUT_2_WAIT);
}

/* wait for request */
const transition_t	_hcStateWait[] =
{
	{
		HC_EVENT_NEW,
		_hcEventNewReq,
	},
	{
		HC_EVENT_CLOSE,
		_hcEventClose,
	}
};


/* RX data for POST request */
const transition_t	_hcStateInit[] =
{
	{
		HC_EVENT_NEW,
		_hcEventNewReqInOtherStates,
	},
	{
		HC_EVENT_NEW,
		_hcEventNewReqInOtherStates,
	},
	{
		HC_EVENT_TIMEOUT,
		_hcEventTimeout,
	},
	{
		HC_EVENT_CONNECTED,
		httpClientEventConnected,
	},
	{
		HC_EVENT_POLL,
		_hcEventPoll,
	},
	{
		HC_EVENT_SENT,
		_hcEventSent,
	},
	{
		HC_EVENT_CLOSE,
		_hcEventClose,
	},
	{
		HC_EVENT_ERROR,
		_hcEventError,
	}
};


const transition_t	_hcStateConn[] =
{
	{
		HC_EVENT_NEW,
		_hcEventNewReqInOtherStates,
	},
	{
		HC_EVENT_RECV,
		_hcEventRecv,
	},
	
	{
		HC_EVENT_POLL,
		_hcEventPoll,
	},
	{
		HC_EVENT_SENT,
		_hcEventSent,
	},
	{
		HC_EVENT_CLOSE,
		_hcEventClose,
	},
	{
		HC_EVENT_ERROR,
		_hcEventError,
	}
};


const transition_t	_hcStateData[] =
{
	{
		HC_EVENT_NEW,
		_hcEventNewReqInOtherStates,
	},
	{
		HC_EVENT_TIMEOUT,
		_hcEventTimeout,
	},
	{
		HC_EVENT_POLL,
		_hcEventPoll,
	},
	{
		HC_EVENT_SENT,
		_hcEventSent,
	},
	{
		HC_EVENT_CLOSE,
		_hcEventClose,
	},
	{
		HC_EVENT_ERROR,
		_hcEventError,
	}
};


const transition_t	_hcStateError[] =
{
	{
		HC_EVENT_NEW,
		_hcEventNewReqInOtherStates,
	},
	{
		HC_EVENT_TIMEOUT,
		_hcEventTimeout,
	},
	{
		HC_EVENT_POLL,
		_hcEventPoll,
	},
	{
		HC_EVENT_SENT,
		_hcEventSent,
	},
	{
		HC_EVENT_CLOSE,
		_hcEventClose,
	},
	{
		HC_EVENT_ERROR,
		_hcEventError,
	}
};


const statemachine_t	_hcStateMachine[] =
{
	{
		HC_STATE_WAIT,
		sizeof(_hcStateWait)/sizeof(transition_t),
		_hcStateWait,
		_hcEnterStateWait
	},
	
	{
		HC_STATE_INIT,
		sizeof(_hcStateInit)/sizeof(transition_t),
		_hcStateInit,
		NULL
	},
	{
		HC_STATE_CONN,
		sizeof(_hcStateConn)/sizeof(transition_t),
		_hcStateConn,
		NULL
	},

	{
		HC_STATE_DATA,
		sizeof(_hcStateData)/sizeof(transition_t),
		_hcStateData,
		_hcEnterStateData
	},

	{
		HC_STATE_ERROR,
		sizeof(_hcStateError)/sizeof(transition_t),
		_hcStateError,
		_hcEnterStateError
	},
	{
		EXT_STATE_CONTINUE,
		0,
		NULL,
		NULL
	}
};

static const statemachine_t *_hcFsmFindState(const statemachine_t *fsm, unsigned char state)
{
	const statemachine_t *_states = fsm;
	
	while(_states->state != EXT_STATE_CONTINUE )
	{
		if( _states->state == state )
		{
			return _states;
		}
		_states++;
	}

	EXT_ASSERT((HTTP_CLIENT_NAME"State %d is not found", state), 0);
	return NULL;
}

void	httpClientFsmHandle(HcEvent *hce)
{
	unsigned char	i;
	unsigned char newState = EXT_STATE_CONTINUE;
	const statemachine_t *_fsm = _hcStateMachine;
	const statemachine_t *_state = NULL;
	
	HttpClient *hc = &_httpClient;

	EXT_ASSERT((HTTP_CLIENT_NAME"HcEvent is null"), hce != NULL);

	if(hce->event == EXT_EVENT_NONE || _fsm == NULL)
	{
		EXT_ERRORF((HTTP_CLIENT_NAME"Invalidate params in state Machine"));
		return;
	}


	_state = _hcFsmFindState(_fsm, hc->state);
	
	const transition_t *handle = _state->eventHandlers;

	for(i=0; i < _state->size; i++)
	{
		if(hce->event == handle->event )
		{
			EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME"#%d handle event %s in state %s",hc->reqs, CMN_FIND_HC_EVENT(hce->event), CMN_FIND_HC_STATE(hc->state)));
			hc->evt = hce;

			newState = (handle->handle)(hc);
			//fsm->currentEvent = EXT_EVENT_NONE;
			
			if(newState!= EXT_STATE_CONTINUE && newState != hc->state )
			{
#if EXT_HTTPC_DEBUG
				EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME"#%d request from state %s enter into state %s", hc->reqs, CMN_FIND_HC_STATE(hc->state), CMN_FIND_HC_STATE(newState)));
#endif

				_state = _hcFsmFindState(_fsm, newState);
				if(_state->enter_handle )
				{
					(_state->enter_handle)(hc);
				}
				
				hc->state = newState;
			}
			
			return;
		}

		handle++;
		
	}
	
#if EXT_HTTPC_DEBUG
	EXT_INFOF((HTTP_CLIENT_NAME"State Machine no handle for event %s in state %s", CMN_FIND_HC_EVENT(hce->event), CMN_FIND_HC_STATE(hc->state)));
#endif

	return;
}


