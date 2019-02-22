
#include "lwipExt.h"
#include "jsmn.h"

#include "lwip/sys.h"
#include "lwip/memp.h"
#include "lwip/mem.h"

#include "extUdpCmd.h"
#include "extFsm.h"

#if EXT_POLL_TASK

typedef	struct
{
	unsigned char		type;
	void				*arg;
}media_event_t;

static sys_mbox_t 		_vmMailBox;

static sys_mutex_t		_vmLock;

static sys_timer_t		_mediaMsgTimer;		/* timer for connect and disconnect messages */

static unsigned short	_timeoutCount;

static void _sendMsgStartTimer(void )
{
	extIpCmdSendMediaData(&extParser, EXT_TRUE);
	sys_timer_start(&_mediaMsgTimer, 3000);
}



static unsigned char _fsmConnectEvent(void *arg)
{
	ext_fsm_t *fsm = (ext_fsm_t *)arg;
	
	/* stop old timer and then start a new one */
	sys_timer_stop(&_mediaMsgTimer);
	_timeoutCount = 0;

	if(fsm->currentState == EXT_MEDIA_STATE_DISCONNECT)
	{/* send set_param and start timer */
		_sendMsgStartTimer();
	}
//	runCfg->runtime.paramsState = FPGA_PARAM_STATE_UPDATED;

	/* start timer */
	return EXT_MEDIA_STATE_CONNECT;
}

static unsigned char _fsmDisconnEvent(void *arg)
{
	ext_fsm_t *fsm = (ext_fsm_t *)arg;

	/* stop old timer and then start a new one */
	sys_timer_stop(&_mediaMsgTimer);
	_timeoutCount = 0;

	/* send set_param*/
	if(fsm->currentState == EXT_MEDIA_STATE_CONNECT)
	{/* send set_param and start timer */
		_sendMsgStartTimer();
	}

	return EXT_MEDIA_STATE_DISCONNECT;
}

static unsigned char _fsmAckEvent(void *arg)
{
//	ext_fsm_t *fsm = (ext_fsm_t *)arg;
	/* stop timer */
	sys_timer_stop(&_mediaMsgTimer);
	_timeoutCount = 0;
	
	return EXT_STATE_CONTINUE;
}

static unsigned char _fsmTimeoutEvent(void *arg)
{
//	ext_fsm_t *fsm = (ext_fsm_t *)arg;
	/* maybe not compatible with sys_arch of FreeRTOS and Linux */
	sys_timer_stop(&_mediaMsgTimer);

	_timeoutCount++;

	if(_timeoutCount < 5)
	{
		_sendMsgStartTimer();
	}

	/* start timer again */
	
	return EXT_STATE_CONTINUE;
}



const transition_t	_disconnState[] =
{
	{
		EXT_MEDIA_EVENT_CONNECT,
		_fsmConnectEvent,
	},
	{
		EXT_MEDIA_EVENT_ACK,
		_fsmAckEvent,
	},
	
	{
		EXT_MEDIA_EVENT_TIMEOUT,
		_fsmTimeoutEvent,
	}
};


const transition_t	_connectState[] =
{
	{
		EXT_MEDIA_EVENT_DISCONNECT,
		_fsmDisconnEvent,
	},
	{
		EXT_MEDIA_EVENT_ACK,
		_fsmAckEvent,
	},
	
	{
		EXT_MEDIA_EVENT_TIMEOUT,
		_fsmTimeoutEvent,
	}
};


const statemachine_t	_mediaStateMachine[] =
{
	{
		EXT_MEDIA_STATE_DISCONNECT,
		sizeof(_disconnState)/sizeof(transition_t),
		_disconnState,
	},
	{
		EXT_MEDIA_STATE_CONNECT,
		sizeof(_connectState)/sizeof(transition_t),
		_connectState,
	},
	{
		EXT_STATE_CONTINUE,
		0,
		NULL,
	}
};


static ext_fsm_t	_mediaFsm;

static MuxRunTimeParam _mediaParams;

static void _extMediaControlThread(void *arg)
{
	media_event_t *event;
//	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)arg;
	
	while (1)
	{
		sys_mbox_fetch(&_vmMailBox, (void **)&event);
		if (event == NULL)
		{
			EXT_DEBUGF(EXT_DBG_ON, (EXT_TASK_NAME" thread: invalid message: NULL"));
			EXT_ASSERT((EXT_TASK_NAME" thread: invalid message"), 0);
			continue;
		}

		_mediaFsm.currentEvent = event->type;
		_mediaFsm.arg = event;
		extFsmHandle(&_mediaFsm);

		memp_free(MEMP_TCPIP_MSG_API, event);

		event = NULL;
	}
	
}


#ifdef	X86
static sys_timer_t _mediaPollingTimer;

void	_extMediaParams(MuxRunTimeParam *mediaParams, unsigned char isConn)
{
	if(! isConn )
	{
		mediaParams->aChannels = 8;
		mediaParams->aDepth = 16;
		mediaParams->aSampleRate = EXT_A_RATE_48K;
		mediaParams->aPktSize = EXT_A_PKT_SIZE_125US;
		
		mediaParams->vColorSpace = EXT_V_COLORSPACE_YCBCR_444;
		mediaParams->vDepth= EXT_V_DEPTH_16;
		mediaParams->vFrameRate = EXT_V_FRAMERATE_T_59;

		mediaParams->vWidth= 1920;
		mediaParams->vHeight = 1080;

		mediaParams->vIsInterlaced = EXT_VIDEO_INTLC_A_PROGRESSIVE;
	}
	else
	{
		mediaParams->aChannels = 4;
		mediaParams->aDepth = 24;
		mediaParams->aSampleRate = EXT_A_RATE_48K;
		mediaParams->aPktSize = EXT_A_PKT_SIZE_125US;
		
		mediaParams->vColorSpace = EXT_V_COLORSPACE_RGB;
		mediaParams->vDepth= EXT_V_DEPTH_12;
		mediaParams->vFrameRate = EXT_V_FRAMERATE_T_60;

		mediaParams->vWidth= 1920*2;
		mediaParams->vHeight = 1080*2;

		mediaParams->vIsInterlaced = EXT_VIDEO_INTLC_INTERLACED;
	}

	return;
}
 
static void pollingTimerCallback(void *arg)
{
	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)arg;
#if 1
	MuxRunTimeParam  *mediaParams = &_mediaParams;
#else
	MuxRunTimeParam  *mediaParams = &runCfg->runtime;
#endif

	// Sanity check the index.
//	if (mediaParams->isConnect == EXT_TRUE)
	{
		_extMediaParams(mediaParams, mediaParams->isConnect);
	}
//	else
	{
	}

	mediaParams->isConnect = (mediaParams->isConnect == 0);

	extMediaPollDevice(runCfg);

}
#endif

static void _msgTimerCallback(void *arg)
{
	extMediaPostEvent(EXT_MEDIA_EVENT_TIMEOUT, NULL);
}

void extMediaInit( void *arg)
{
#ifdef	X86
	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)arg;
#endif

	if (sys_mbox_new(&_vmMailBox, EXT_MCTRL_MBOX_SIZE) != ERR_OK)
	{
		EXT_ASSERT(("failed to create "EXT_TASK_NAME" mbox"), 0);
	}

	if (sys_mutex_new(&_vmLock) != ERR_OK)
	{
		EXT_ASSERT(("failed to create "EXT_TASK_NAME" Lock"), 0);
	}

	memset(&_mediaParams, 0, sizeof(MuxRunTimeParam));
	
	memset(&_mediaFsm, 0, sizeof(ext_fsm_t));
	_mediaFsm.currentEvent = EXT_EVENT_NONE;
	_mediaFsm.currentState = EXT_MEDIA_STATE_DISCONNECT;
	_mediaFsm.states = _mediaStateMachine;

	EXT_DEBUGF(EXT_DBG_OFF, (EXT_TASK_NAME" FSM: %p:%p", _mediaStateMachine, _mediaFsm.states ));

#ifdef	X86
#if EXT_TIMER_DEBUG
	snprintf(_mediaPollingTimer.name, sizeof(_mediaPollingTimer.name), "%s", "PollingTimer" );
#endif
	sys_timer_new(&_mediaPollingTimer, pollingTimerCallback, os_timer_type_reload, (void *) runCfg);
	sys_timer_start(&_mediaPollingTimer, 14000);
#endif

#if EXT_TIMER_DEBUG
	snprintf(_mediaMsgTimer.name, sizeof(_mediaMsgTimer.name), "%s", "MsgTimer" );
#endif
	sys_timer_new(&_mediaMsgTimer, _msgTimerCallback, os_timer_type_once, (void *) &_mediaFsm);

#if EXT_POLL_TASK
	sys_thread_new(EXT_TASK_NAME, _extMediaControlThread, arg, EXT_NET_IF_TASK_STACK_SIZE, EXT_NET_IF_TASK_PRIORITY-2 );
#endif

}


#if EXT_TIMER_DEBUG
const EXT_CONST_STR  _stateStr[] = 
{
	{
		EXT_EVENT_NONE,
		"None"	
	},
	{
		EXT_EVENT_FACTORY_RESET,
		"RESET"	
	},
	{
		EXT_MEDIA_EVENT_CONNECT,
		"CONNECT"	
	},
	{
		EXT_MEDIA_EVENT_DISCONNECT,
		"DIS_CONNECT"	
	},
	{
		EXT_MEDIA_EVENT_ACK,
		"ACK"	
	},
	{
		EXT_MEDIA_EVENT_NACK,
		"NACK"	
	},
	{
		EXT_MEDIA_EVENT_TIMEOUT,
		"TIMEOUT"	
	},
};
#endif

unsigned char extMediaPostEvent(unsigned char eventType, void *ctx)
{
	media_event_t *event;

	LWIP_ASSERT(("Invalid mbox"), sys_mbox_valid_val(_vmMailBox));
	event = (media_event_t *)memp_malloc(MEMP_TCPIP_MSG_API);
	if (event == NULL)
	{
		EXT_ERRORF(("No memory available now"));
		return EXIT_FAILURE;
	}
	memset(event, 0, sizeof(media_event_t));

	event->type = eventType;
	event->arg = ctx;
#if EXT_TIMER_DEBUG
//	EXT_INFOF( ("EVENT: %s", (event->type <EXT_ARRAYSIZE(_stateStr) )? _stateStr[event->type].name: "UNKNOWN"));
#else
//	EXT_INFOF( ("EVENT: %d", event->type));
#endif
	if (sys_mbox_trypost(&_vmMailBox, event) != ERR_OK)
	{
		EXT_ERRORF(("Post failed"));
		memp_free(MEMP_TCPIP_MSG_API, event);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


static char _extMediaCompareParams(MuxRunTimeParam *dest, MuxRunTimeParam *newParam )
{
	char isDiff = EXT_FALSE;

	if(dest->isConnect== EXT_FALSE && newParam->isConnect == EXT_TRUE)
	{
		isDiff = EXT_TRUE;
	}

	if(dest->vWidth != newParam->vWidth || dest->vHeight != newParam->vHeight ||
		dest->vFrameRate != newParam->vFrameRate || dest->vColorSpace!= newParam->vColorSpace ||
		dest->vDepth != newParam->vDepth || dest->vIsSegmented != newParam->vIsSegmented )
	{
		isDiff = EXT_TRUE;
	}
	
	if(isDiff)
	{
		memcpy(dest, newParam, sizeof(MuxRunTimeParam));
	}
	
	return isDiff;
}

#endif // EXT_POLL_TASK


void extMediaPollDevice(EXT_RUNTIME_CFG *runCfg)
{
	if(EXT_IS_TX(runCfg))
	{
#if EXT_POLL_TASK


#ifdef ARM
		extFpgaTimerJob(&_mediaParams);
#else
//		_mediaParams.isConnect = (_mediaParams.isConnect==0);
#endif

		if(_mediaParams.isConnect == EXT_FALSE &&
			runCfg->runtime.isConnect == EXT_TRUE )
		{
			runCfg->runtime.isConnect = EXT_FALSE;
			extMediaPostEvent(EXT_MEDIA_EVENT_DISCONNECT, NULL);
			return;
		}
		else if(_mediaParams.isConnect == EXT_TRUE && runCfg->runtime.isConnect == EXT_FALSE )
		{
			EXT_DEBUGF(EXT_DBG_OFF, ("SDI CONNECT event"));

			if(_extMediaCompareParams(&runCfg->runtime, &_mediaParams))	
			{
				EXT_DEBUGF(EXT_DBG_OFF, ("CONNECT event : New Media Params need to be updated now!"));
				runCfg->runtime.isConnect = EXT_TRUE;
				extMediaPostEvent(EXT_MEDIA_EVENT_CONNECT, NULL);
				return;
			}
		}

#endif
	}
	else
	{/* SDP client */
		if(runCfg->fpgaAuto == FPGA_CFG_SDP)
		{
			extHttpClientStart(runCfg);
		}
	}

}

