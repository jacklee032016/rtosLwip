
#include "string.h"
#include "stdio.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"


/* common/utils/stdio */
#include "compact.h"
#include "lwipExt.h"

#include "eos.h"

#define	EXT_PERIOD_JOB_NAME		"extTimer"
#define	EXT_PERIOD_JOB_TIME		2000 	/* ms*/


static TimerHandle_t _timerHdlDelay;
static TimerHandle_t _timerHdlPeriod;

MuxDelayJob _delayJob = NULL;

static void _vTimerDelayExpired(TimerHandle_t pxTimer)
{
#if configUSE_TIMERS
	if(_delayJob == NULL)
	{
		EXT_ERRORF(("Delay Job is not defined"));
		return;
	}

	EXT_INFOF(("%s is running", pcTimerGetName(pxTimer)) );
	(_delayJob)(pvTimerGetTimerID(pxTimer) ); 
#endif
	/* remove timer */
}


static void extJobDelay(const char *name, unsigned short delayMs, MuxDelayJob func, void *data)
{
#if configUSE_TIMERS
	_delayJob = func;
	
	_timerHdlDelay = xTimerCreate(name, pdMS_TO_TICKS(delayMs), pdFALSE, /* auto reload */ (void*)0, /* timer ID */_vTimerDelayExpired);
	if (_timerHdlDelay==NULL)
	{
		EXT_ERRORF(("Delay Job can not be created"));
		return;
	}

	vTimerSetTimerID( _timerHdlDelay, data);

	if (xTimerStart(_timerHdlDelay, 0)!=pdPASS)
	{
		EXT_ERRORF(("Delay Job can not be started"));
		return;
	}
#else
	(func)(data);
#endif	
}


static char _delayReboot(void *data)
{
	EXT_REBOOT();
	return EXIT_SUCCESS;
}


void extDelayReboot(unsigned short delayMs)
{
	extJobDelay("reboot", delayMs, _delayReboot, NULL);
}

static void _periodJobCallback(TimerHandle_t pxTimer)
{
#if configUSE_TIMERS
	EXT_RUNTIME_CFG *runCfg;
//	EXT_INFOF(("%s is running", pcTimerGetName(pxTimer)) );

	runCfg = (EXT_RUNTIME_CFG *)pvTimerGetTimerID(pxTimer);

	if(runCfg->isMCast)
	{
		unsigned	int	ip = CFG_MAKEU32(bspMultiAddressFromDipSwitch(), MCAST_DEFAULT_IPADDR2, MCAST_DEFAULT_IPADDR1, MCAST_DEFAULT_IPADDR0 );
		if(ip != runCfg->dest.ip )
		{
			EXT_INFOF(("Multicast Address change to '%s'", EXT_LWIP_IPADD_TO_STR(&ip)));
			if(!EXT_IS_TX(runCfg))
			{
				EXT_NET_IGMP_LEAVE(runCfg->dest.ip);
			}
			
			runCfg->dest.ip = ip;
			if(!EXT_IS_TX(runCfg))
			{
				extTxMulticastIP2Mac(runCfg);
			}
			
			extFpgaConfig(runCfg);
		}
	}

	if(EXT_IS_TX(runCfg))
	{
		extFpgaTimerJob(runCfg);
	}
#endif	
}


/* periodical job: check switch button and FPGA register */
void extJobPeriod(EXT_RUNTIME_CFG *runCfg)
{
#if configUSE_TIMERS
	_timerHdlPeriod = xTimerCreate(EXT_PERIOD_JOB_NAME, pdMS_TO_TICKS(EXT_PERIOD_JOB_TIME), pdTRUE/* auto reload */,  (void*)0 /* timer ID */,_periodJobCallback);
	if (_timerHdlPeriod==NULL)
	{
		EXT_ERRORF(("Delay Job can not be created"));
		return;
	}

	vTimerSetTimerID( _timerHdlPeriod, runCfg);

	if (xTimerStart(_timerHdlPeriod, 0)!=pdPASS)
	{
		EXT_ERRORF(("Delay Job can not be started"));
		return;
	}
#endif	
}

