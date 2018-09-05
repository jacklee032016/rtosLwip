
/*
* functions can be implemented in multiple platform: RTOS, Linux simulator, etc
*/


#include "lwipExt.h"

static sys_timer_t		_blinkTimer;		/* timer for connect and disconnect messages */


static void _blinkTimerCallback(void *arg)
{
	TRACE();
#ifdef ARM
	extFpgaBlinkPowerLED(EXT_TRUE);
#else
#endif
	EXT_DEBUGF(EXT_DBG_ON, ("blink starting..."));

//	sys_timer_stop(&_blinkTimer);
}


void extSysBlinkTimerInit(unsigned short milliseconds)
{
	TRACE();
	memset(&_blinkTimer, 0, sizeof(sys_timer_t));
#if EXT_TIMER_DEBUG
	snprintf(_blinkTimer.name, sizeof(_blinkTimer.name), "%s", "BlinkTimer" );
#endif
	sys_timer_new(&_blinkTimer, _blinkTimerCallback, os_timer_type_once, (void *) NULL);

	TRACE();

//	sys_timer_start(&_blinkTimer, milliseconds);
	TRACE();
}

