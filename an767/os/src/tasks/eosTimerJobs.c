
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
#if 1
#define	EXT_PERIOD_JOB_TIME		(3000*10) 	/* ms*/
#else
#define	EXT_PERIOD_JOB_TIME		(1000*10) 	/* ms, for tests only */
#endif

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


static void _extJobDelay(const char *name, unsigned short delayMs, MuxDelayJob func, void *data)
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
	_extJobDelay("reboot", delayMs, _delayReboot, NULL);
}

static char _delayReset(void *data)
{
	extCfgFactoryKeepMac(&extRun);
	
	EXT_REBOOT();
	return EXIT_SUCCESS;
}


void extDelayReset(unsigned short delayMs)
{
	_extJobDelay("reset", delayMs, _delayReset, NULL);
}



static void _periodJobCallback(TimerHandle_t pxTimer)
{
#if configUSE_TIMERS
	EXT_RUNTIME_CFG *runCfg;
//	EXT_INFOF(("%s is running", pcTimerGetName(pxTimer)) );
	runCfg = (EXT_RUNTIME_CFG *)pvTimerGetTimerID(pxTimer);

#if EXT_DIP_SWITCH_ON
	if(EXT_IS_MULTICAST(runCfg) && EXT_IS_DIP_ON(runCfg) )
	{
		unsigned	int	ip = CFG_MAKEU32(bspMultiAddressFromDipSwitch(), MCAST_DEFAULT_IPADDR2, MCAST_DEFAULT_IPADDR1, MCAST_DEFAULT_IPADDR0 );
		if(ip != runCfg->dest.ip )
		{
			extCmnNewDestIpEffective(runCfg, ip);
		}
	}
#endif

#else
	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)pxTimer;
#endif	

	extMediaPollDevice(runCfg);

}


/* periodical job: check switch button and FPGA register */
void extJobPeriod(EXT_RUNTIME_CFG *runCfg)
{
#if configUSE_TIMERS

	if(runCfg->fpgaAuto == FPGA_CFG_SDP)
	{
		extHttpClientStart(runCfg);
	}
		
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
#else	
	_timerHdlPeriod = runCfg;
#endif	
}

