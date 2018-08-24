/* timer.c */


#include "ptpd.h"

#if EXT_TIMER_DEBUG
static const char *_timersDesces[TIMER_ARRAY_SIZE] =
{
	"PDelayReqInterval",
	"DelayReqInterval",
	"SyncInterval",
	"AnnounceReceipt",
	"AnnounceInterval",
	"QualifacationTimeout"

#if 0	
	"SYNC_RECEIPT",
	"DELAY_RECEIPT",
	"UNICAST_GRANT",
	"OPERATOR_MESSAGES",
	"LEAP_SECOND_PAUSE",
	"STATUSFILE_UPDATE",
	"PANIC_MODE",
	"PERIODIC_INFO_TIMER",
#ifdef PTPD_STATISTICS
	"STATISTICS_UPDATE",
#endif /* PTPD_STATISTICS */
	"ALARM_UPDATE",
	"MASTER_NETREFRESH",
	"CALIBRATION_DELAY",
	"CLOCK_UPDATE",
	"TIMINGDOMAIN_UPDATE"
#endif	
};
#endif

/* An array to hold the various system timer handles. */
static sys_timer_t ptpdTimers[TIMER_ARRAY_SIZE];
static bool ptpdTimersExpired[TIMER_ARRAY_SIZE];
 
static void timerCallback(void *arg)
{
	int index = (int) arg;

	// Sanity check the index.
	if (index < TIMER_ARRAY_SIZE)
	{
		/* Mark the indicated timer as expired. */
		ptpdTimersExpired[index] = TRUE;

		/* Notify the PTP thread of a pending operation. */
		ptpd_alert();
	}
}

void initTimer(void)
{
	int32_t i;

	DBG("initTimer");

	/* Create the various timers used in the system. */
	for (i = 0; i < TIMER_ARRAY_SIZE; i++)
	{// Mark the timer as not expired.
#if EXT_TIMER_DEBUG
		snprintf(ptpdTimers[i].name, sizeof(ptpdTimers[i].name), "%s", _timersDesces[i] );
#endif
		sys_timer_new(&ptpdTimers[i], timerCallback, os_timer_type_once, (void *) i);
		ptpdTimersExpired[i] = FALSE;
	}
}

void timerStop(int32_t index)
{
	/* Sanity check the index. */
	if (index >= TIMER_ARRAY_SIZE)
		return;

	// Cancel the timer and reset the expired flag.
#if EXT_TIMER_DEBUG
	DBGV("timerStop: stop timer %s", ptpdTimers[index].name );
#else
	DBGV("timerStop: stop timer %d", index);
#endif
	sys_timer_stop(&ptpdTimers[index]);
	ptpdTimersExpired[index] = FALSE;
}

void timerStart(int32_t index, uint32_t interval_ms)
{
	/* Sanity check the index. */
	if (index >= TIMER_ARRAY_SIZE) return;

	// Set the timer duration and start the timer.
#if EXT_TIMER_DEBUG
	DBGV("timerStart: set timer %s to %d\n", ptpdTimers[index].name, interval_ms);
#else
	DBGV("timerStart: set timer %d to %d\n", index, interval_ms);
#endif
	ptpdTimersExpired[index] = FALSE;
	sys_timer_start(&ptpdTimers[index], interval_ms);
}

bool timerExpired(int32_t index)
{
	/* Sanity check the index. */
	if (index >= TIMER_ARRAY_SIZE)
		return FALSE;

	/* Determine if the timer expired. */
	if (!ptpdTimersExpired[index])
		return FALSE;
	
#if EXT_TIMER_DEBUG
	DBGV("timerExpired: timer %s expired", ptpdTimers[index].name);
#else
	DBGV("timerExpired: timer %d expired", index);
#endif
	ptpdTimersExpired[index] = FALSE;

	return TRUE;
}

