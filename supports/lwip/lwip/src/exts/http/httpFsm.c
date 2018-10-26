
#include "lwipExt.h"

#include "http.h"
#include "extFsm.h"

/*
* Event handlers 
*/
static unsigned char _httpEventRecvInReqState(void *arg)
{
	HttpEvent *he = (HttpEvent *)arg;
	ExtHttpConn *ehc = (ExtHttpConn *)he->mhc;
	
	err_t err = extHttpRequestParse(he->mhc, he->pBuf);
	if( err == ERR_INPROGRESS)
	{
		return EXT_STATE_CONTINUE;
	}
	else if(err != ERR_OK )
	{/* error to close connection */
		return H_STATE_CLOSE;
	}

#if 0
	if(extHttpFileFind(ehc) == ERR_OK)
	{
		if( (ehc->reqType == EXT_HTTP_REQ_T_UPLOAD) 
#if LWIP_EXT_NMOS
			|| (ehc->reqType == EXT_HTTP_REQ_T_REST && HTTP_IS_POST(ehc)) 
#endif			
			)
		{
			return H_STATE_DATA;
		}
	}
#endif
	
	return H_STATE_RESP;
}

static unsigned char _httpEventRecvInDataState(void *arg)
{
	HttpEvent *he = (HttpEvent *)arg;
	ExtHttpConn *ehc = (ExtHttpConn *)he->mhc;

	err_t err = extHttpPostRxDataPbuf(ehc, he->pBuf);
	if( err == ERR_INPROGRESS)
	{
		return EXT_STATE_CONTINUE;
	}
	else if(err != ERR_OK )
	{/* error to close connection */
		return H_STATE_CLOSE;
	}

	return H_STATE_RESP;
}


static unsigned char _httpEventPoll(void *arg)
{
	u8_t ret;
	HttpEvent *he = (HttpEvent *)arg;
	
	ExtHttpConn *mhc = (ExtHttpConn *)he->mhc;

	if (mhc == NULL)
	{
		/* arg is null, close. */
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("POLL: arg is NULL, close"));
		return H_STATE_CLOSE;
	}
	else
	{
		mhc->retries++;
		if (mhc->retries == MHTTPD_MAX_RETRIES)
		{
			return H_STATE_CLOSE;
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
	
	return EXT_STATE_CONTINUE;
}


static unsigned char _httpEventSend(void *arg)
{
	u8_t ret;
	HttpEvent *he = (HttpEvent *)arg;

	ExtHttpConn *ehc = (ExtHttpConn *)he->mhc;
	ehc->retries = 0;
	ret = extHttpSend( he->mhc);
	if(ret == MHTTP_NO_DATA_TO_SEND)
	{
		return H_STATE_CLOSE;
	}
	else if (ret == MHTTP_DATA_TO_SEND_BREAK)
	{
		return H_STATE_CLOSE;
	}
	return EXT_STATE_CONTINUE;
}

static unsigned char _httpEventClose(void *arg)
{
//	HttpEvent *he = (HttpEvent *)arg;
	return H_STATE_CLOSE;
}

static unsigned char _httpEventError(void *arg)
{
#if 0
	HttpEvent *he = (HttpEvent *)arg;
	if(he->mhc)
	{
		ExtHttpConn *ehc = (ExtHttpConn *)he->mhc;
//		extHttpConnClose(ehc, ehc->pcb);
//		extHttpConnFree(ehc);
	}
#endif

	return H_STATE_ERROR;
}


/* 
*  Enter handlers of every state
*/

static void _httpEnterStateResp(void *arg)
{
	HttpEvent *he = (HttpEvent *)arg;
	extHttpSend(he->mhc);
}


static void _httpEnterStateClose(void *arg)
{
	HttpEvent *he = (HttpEvent *)arg;
	extHttpConnClose(he->mhc, he->pcb);
}

/* only free resource, not abort or close connection, eg. PCB has been deallocated by TCP  */
static void _httpEnterStateError(void *arg)
{
#if 0
	HttpEvent *he = (HttpEvent *)arg;
	
	extHttpConnClose(he->mhc, he->pcb);
#endif	
}


const transition_t	_staticPageStateReq[] =
{
	{
		H_EVENT_ERROR,
		_httpEventError,
	},
	{
		H_EVENT_RECV,
		_httpEventRecvInReqState,
	},
	{
		H_EVENT_POLL,
		_httpEventPoll,
	},
	
	{
		H_EVENT_SENT,
		_httpEventSend,
	}
};

/* RX data for POST request */
const transition_t	_staticPageStateData[] =
{
	{
		H_EVENT_ERROR,
		_httpEventError,
	},
	{
		H_EVENT_RECV,
		_httpEventRecvInDataState,
	},
	{
		H_EVENT_POLL,
		_httpEventPoll,
	},
	
	{
		H_EVENT_SENT,
		_httpEventSend,
	}
};


const transition_t	_staticPageStateResp[] =
{
	{
		H_EVENT_ERROR,
		_httpEventError,
	},
	
	{
		H_EVENT_POLL,
		_httpEventPoll,
	},
	
	{
		H_EVENT_SENT,
		_httpEventSend,
	}
};

const transition_t	_httpStateClose[] =
{
#if 0
	{
		H_EVENT_ERROR,
		_httpEventError,
	}
#endif	
};

const transition_t	_httpStateError[] =
{
#if 0
	{
		H_EVENT_CLOSE,
		_httpEventClose,
	}
#endif
};




const statemachine_t	_staticPageStateMachine[] =
{
	{
		H_STATE_REQ,
		sizeof(_staticPageStateReq)/sizeof(transition_t),
		_staticPageStateReq,
		NULL
	},
	{
		H_STATE_RESP,
		sizeof(_staticPageStateResp)/sizeof(transition_t),
		_staticPageStateResp,
		_httpEnterStateResp
	},
	{
		H_STATE_CLOSE,
		sizeof(_httpStateClose)/sizeof(transition_t),
		_httpStateClose,
		_httpEnterStateClose
	},
	{
		H_STATE_ERROR,
		sizeof(_httpStateError)/sizeof(transition_t),
		_httpStateError,
		_httpEnterStateError
	},
	{
		EXT_STATE_CONTINUE,
		0,
		NULL,
		NULL
	}
};

static const statemachine_t *_httpFsmFindState(const statemachine_t *fsm, unsigned char state)
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

	EXT_ASSERT(("State %s (%d) is not found", CMN_FIND_HTTP_STATE(state), state), 0);
	return NULL;
}

void	httpFsmHandle(HttpEvent *he)
{
	unsigned char	i;
	unsigned char newState = EXT_STATE_CONTINUE;
	const statemachine_t *_fsm = _staticPageStateMachine;
	const statemachine_t *_state = NULL;
	
	ExtHttpConn *ehc = he->mhc;

	EXT_ASSERT(("HttpConnect is null in state machine"), ehc != NULL);

	if(he->type == EXT_EVENT_NONE || _fsm == NULL)
	{
		EXT_ERRORF(("Invalidate params in state Machine"));
		return;
	}

	if(ehc == NULL)
	{
		EXT_ERRORF(("Invalidate CONN in state Machine"));
		return;
	}

	_state = _httpFsmFindState(_fsm, ehc->state);
	
//	EXT_DEBUGF(EXT_DBG_ON, ("states %p ", _states)) ;
//	EXT_DEBUGF(EXT_DBG_ON, ("state %d in FSM, current state is %d", _states->state, fsm->currentState)) ;
	const transition_t *handle = _state->eventHandlers;

	for(i=0; i < _state->size; i++)
	{
		if(he->type == handle->event )
		{
#if EXT_HTTPD_DEBUG
			EXT_INFOF(("%s handle event %s in state %s", ehc->name, CMN_FIND_HTTP_EVENT(he->type), CMN_FIND_HTTP_STATE(ehc->state)));
#endif

			newState = (handle->handle)(he);
			//fsm->currentEvent = EXT_EVENT_NONE;
			
			if(newState!= EXT_STATE_CONTINUE && newState != ehc->state )
			{
#if EXT_HTTPD_DEBUG
				EXT_INFOF(("%s from state %s enter into state %s", ehc->name, CMN_FIND_HTTP_STATE(ehc->state), CMN_FIND_HTTP_STATE(newState)));
#endif

				_state = _httpFsmFindState(_fsm, newState);
				if(_state->enter_handle )
				{
					(_state->enter_handle)(he);
				}
				
				ehc->state = newState;
			}
			
			return;
		}

		handle++;
		
	}
	
#if EXT_HTTPD_DEBUG
	EXT_INFOF(("%s no handle for event %s in state %s", ehc->name, CMN_FIND_HTTP_EVENT(he->type), CMN_FIND_HTTP_STATE(ehc->state)));
#else
	EXT_INFOF(("State Machine no handle for event %d in state %s", he->type, ehc->state));
#endif

	return;
}



