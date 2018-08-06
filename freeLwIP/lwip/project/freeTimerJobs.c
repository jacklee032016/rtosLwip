
#include "string.h"
#include "stdio.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"


/* common/utils/stdio */
#include "compact.h"
#include "lwipMux.h"

//#include "muxOs.h"

#define	MUX_PERIOD_JOB_NAME		"muxTimer"
#define	MUX_PERIOD_JOB_TIME		2000 	/* ms*/


static TimerHandle_t _timerHdlDelay;
static TimerHandle_t _timerHdlPeriod;

MuxDelayJob _delayJob = NULL;

static void _vTimerDelayExpired(TimerHandle_t pxTimer)
{
	if(_delayJob == NULL)
	{
		MUX_ERRORF(("Delay Job is not defined"));
		return;
	}

	MUX_INFOF(("%s is running", pcTimerGetName(pxTimer)) );
	(_delayJob)(pvTimerGetTimerID(pxTimer) ); 

	/* remove timer */
}


static void muxJobDelay(const char *name, unsigned short delayMs, MuxDelayJob func, void *data)
{
	_delayJob = func;
	
	_timerHdlDelay = xTimerCreate(name, pdMS_TO_TICKS(delayMs), pdFALSE, /* auto reload */ (void*)0, /* timer ID */_vTimerDelayExpired);
	if (_timerHdlDelay==NULL)
	{
		MUX_ERRORF(("Delay Job can not be created"));
		return;
	}

	vTimerSetTimerID( _timerHdlDelay, data);

	if (xTimerStart(_timerHdlDelay, 0)!=pdPASS)
	{
		MUX_ERRORF(("Delay Job can not be started"));
		return;
	}
}


static char _delayReboot(void *data)
{
//	MUX_REBOOT();
	return EXIT_SUCCESS;
}


void muxDelayReboot(unsigned short delayMs)
{
	muxJobDelay("reboot", delayMs, _delayReboot, NULL);
}

static void _periodJobCallback(TimerHandle_t pxTimer)
{
	MUX_RUNTIME_CFG *runCfg;
//	MUX_INFOF(("%s is running", pcTimerGetName(pxTimer)) );

	runCfg = (MUX_RUNTIME_CFG *)pvTimerGetTimerID(pxTimer);

	if(runCfg->isMCast)
	{
#if 0	
		unsigned	int	ip = CFG_MAKEU32(bspMultiAddressFromDipSwitch(), MCAST_DEFAULT_IPADDR2, MCAST_DEFAULT_IPADDR1, MCAST_DEFAULT_IPADDR0 );
		if(ip != runCfg->dest.ip )
		{
			MUX_INFOF(("Multicast Address change to '%s'", MUX_LWIP_IPADD_TO_STR(&ip)));
			muxTxMulticastIP2Mac(runCfg);
		}
#endif		
	}
}


/* periodical job: check switch button and FPGA register */
void muxJobPeriod(MUX_RUNTIME_CFG *runCfg)
{
	_timerHdlPeriod = xTimerCreate(MUX_PERIOD_JOB_NAME, pdMS_TO_TICKS(MUX_PERIOD_JOB_TIME), pdTRUE, /* auto reload */ (void*)0, /* timer ID */_periodJobCallback);
	if (_timerHdlPeriod==NULL)
	{
		MUX_ERRORF(("Delay Job can not be created"));
		return;
	}

	vTimerSetTimerID( _timerHdlPeriod, runCfg);

	if (xTimerStart(_timerHdlPeriod, 0)!=pdPASS)
	{
		MUX_ERRORF(("Delay Job can not be started"));
		return;
	}
}

