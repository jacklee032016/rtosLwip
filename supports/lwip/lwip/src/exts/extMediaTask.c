
#include "lwipExt.h"
#include "jsmn.h"

#include "lwip/sys.h"
#include "lwip/memp.h"
#include "lwip/mem.h"

#include "extUdpCmd.h"
#include "extFsm.h"

typedef	struct
{
	unsigned char		type;
	void				*arg;
}media_event_t;

static sys_mbox_t 		_vmMailBox;

static sys_mutex_t		_vmLock;

unsigned char _fsmConnectEvent()
{
	TRACE();
	/* send set_param*/
	extIpCmdSendMediaData(&extParser, EXT_TRUE);
//	runCfg->runtime.paramsState = FPGA_PARAM_STATE_UPDATED;

	/* start timer */
	return EXT_MEDIA_STATE_CONNECT;
}

unsigned char _fsmDisconnEvent()
{
	TRACE();
	/* send set_param*/
	extIpCmdSendMediaData(&extParser, EXT_TRUE);

	/* start timer */
	return EXT_MEDIA_STATE_DISCONNECT;
}

unsigned char _fsmAckEvent()
{
	TRACE();
	/* stop timer */
	return EXT_STATE_CONTINUE;
}

unsigned char _fsmTimeoutEvent()
{
	TRACE();
	/* send set_param*/

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
	}
};


static ext_fsm_t	_mediaFsm;

static MuxRunTimeParam _mediaParams;

static void _extMediaControlThread(void *arg)
{
	media_event_t *event;
	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)arg;
	

	while (1)
	{
		sys_mbox_fetch(&_vmMailBox, (void **)&event);
		if (event == NULL)
		{
			EXT_DEBUGF(TCPIP_DEBUG, (EXT_TASK_NAME" thread: invalid message: NULL"));
			EXT_ASSERT((EXT_TASK_NAME" thread: invalid message"), 0);
			continue;
		}

		_mediaFsm.currentEvent = event->type;
		_mediaFsm.arg = event;
		extFsmHandle(&_mediaFsm);

		memp_free(MEMP_TCPIP_MSG_API, event);
	}
	
}


#ifdef	X86
static sys_timer_t _mediaPollingTimer;

void	_extMediaParams(MuxRunTimeParam *mediaParams, unsigned char isConn)
{
	if(isConn )
	{
		mediaParams->aChannels = 2;
		mediaParams->aDepth = 16;
		mediaParams->aSampleRate = 48000;
		
		mediaParams->vColorSpace = EXT_V_COLORSPACE_YCBCR_444;
		mediaParams->vDepth= EXT_V_DEPTH_16;
		mediaParams->vFrameRate = EXT_V_FRAMERATE_T_59;

		mediaParams->vWidth= 1920;
		mediaParams->vHeight = 1080;

		mediaParams->vIsInterlaced = 0;
		mediaParams->vIsSegmented = 1;
	}
	else
	{
		mediaParams->aChannels = 1;
		mediaParams->aDepth = 24;
		mediaParams->aSampleRate = 32000;
		
		mediaParams->vColorSpace = EXT_V_COLORSPACE_RGB;
		mediaParams->vDepth= EXT_V_DEPTH_12;
		mediaParams->vFrameRate = EXT_V_FRAMERATE_T_60;

		mediaParams->vWidth= 1920*2;
		mediaParams->vHeight = 1080*2;

		mediaParams->vIsInterlaced = 0;
		mediaParams->vIsSegmented = 1;
	}

	return;
}
 
static void pollingTimerCallback(void *arg)
{
	MuxRunTimeParam  *mediaParams = (MuxRunTimeParam  *) arg;

	// Sanity check the index.
//	if (mediaParams->isConnect == EXT_TRUE)
	{
		_extMediaParams(mediaParams, mediaParams->isConnect);
	}
//	else
	{
	}

	mediaParams->isConnect = (mediaParams->isConnect == 0);
}
#endif

void extMediaInit( void *arg)
{
	TRACE();
	
	if (sys_mbox_new(&_vmMailBox, EXT_MCTRL_MBOX_SIZE) != ERR_OK)
	{
		EXT_ASSERT(("failed to create "EXT_TASK_NAME" mbox"), 0);
	}

	if (sys_mutex_new(&_vmLock) != ERR_OK)
	{
		EXT_ASSERT(("failed to create "EXT_TASK_NAME" Lock"), 0);
	}

	_mediaFsm.currentEvent = EXT_EVENT_NONE;
	_mediaFsm.currentState = EXT_MEDIA_STATE_CONNECT;
	_mediaFsm.states = _mediaStateMachine;

#ifdef	X86
#if EXT_TIMER_DEBUG
	snprintf(_mediaPollingTimer.name, sizeof(_mediaPollingTimer.name), "%s", "PollingTimer" );
#endif
	sys_timer_new(&_mediaPollingTimer, pollingTimerCallback, os_timer_type_reload, (void *) &_mediaParams);
	sys_timer_start(&_mediaPollingTimer, 4000);
#endif

	sys_thread_new(EXT_TASK_NAME, _extMediaControlThread, arg, EXT_NET_IF_TASK_STACK_SIZE/2, EXT_NET_IF_TASK_PRIORITY-2 );
}

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

	event->type = eventType;
	event->arg = ctx;

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


void extMediaPollDevice(EXT_RUNTIME_CFG *runCfg)
{
	if(EXT_IS_TX(runCfg))
	{
#ifdef ARM
		extFpgaTimerJob(&_mediaParams);
#else
		_mediaParams.isConnect = (_mediaParams.isConnect==0);
#endif

		if(_mediaParams.isConnect == EXT_FALSE)
		{
			extMediaPostEvent(EXT_MEDIA_EVENT_DISCONNECT, NULL);
			return;
		}

		if(_extMediaCompareParams(&runCfg->runtime, &_mediaParams))	
		{
			EXT_DEBUGF(EXT_DBG_ON, ("New Media Params need to be updated now!"));
			extMediaPostEvent(EXT_MEDIA_EVENT_CONNECT, NULL);
			return;
		}

	}
}

