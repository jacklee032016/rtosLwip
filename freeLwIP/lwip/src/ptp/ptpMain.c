/* ptpd.c */

#include "ptpd.h"

#define PTPD_THREAD_PRIO    (tskIDLE_PRIORITY + 2)

static sys_mbox_t ptp_alert_queue;

// Statically allocated run-time configuration data.
static RunTimeOpts	rtOpts;

static PtpClock		_ptpClock;
ForeignMasterRecord ptpForeignRecords[DEFAULT_MAX_FOREIGN_RECORDS];

__IO uint32_t PTPTimer = 0;

static int16_t _ptpdStartup(PtpClock * ptpClock)
{
	/* 9.2.2 */
	if (ptpClock->rtOpts->slaveOnly) 
		ptpClock->rtOpts->clockQuality.clockClass = DEFAULT_CLOCK_CLASS_SLAVE_ONLY;

	/* No negative or zero attenuation */
	if (ptpClock->rtOpts->servo.ap < 1)
		ptpClock->rtOpts->servo.ap = 1;
	if (ptpClock->rtOpts->servo.ai < 1)
		ptpClock->rtOpts->servo.ai = 1;

	DBG("event POWER UP\n");

	toState(ptpClock, PTP_INITIALIZING);

	return 0;
}


static void _ptpdTask(void *arg)
{
	PtpClock	*ptpClock = (PtpClock *)arg;

	// Initialize run-time options to default values.
	ptpClock->rtOpts->announceInterval = DEFAULT_ANNOUNCE_INTERVAL;
	ptpClock->rtOpts->syncInterval = DEFAULT_SYNC_INTERVAL;
	
	ptpClock->rtOpts->clockQuality.clockAccuracy = DEFAULT_CLOCK_ACCURACY;
	ptpClock->rtOpts->clockQuality.clockClass = DEFAULT_CLOCK_CLASS;
	ptpClock->rtOpts->clockQuality.offsetScaledLogVariance = DEFAULT_CLOCK_VARIANCE; /* 7.6.3.3 */
	ptpClock->rtOpts->priority1 = DEFAULT_PRIORITY1;
	ptpClock->rtOpts->priority2 = DEFAULT_PRIORITY2;
	ptpClock->rtOpts->domainNumber = DEFAULT_DOMAIN_NUMBER;

	ptpClock->rtOpts->slaveOnly = SLAVE_ONLY;

	ptpClock->rtOpts->currentUtcOffset = DEFAULT_UTC_OFFSET;
	ptpClock->rtOpts->servo.noResetClock = DEFAULT_NO_RESET_CLOCK;
	ptpClock->rtOpts->servo.noAdjust = NO_ADJUST;

	ptpClock->rtOpts->servo.sDelay = DEFAULT_DELAY_S;
	ptpClock->rtOpts->servo.sOffset = DEFAULT_OFFSET_S;
	ptpClock->rtOpts->servo.ap = DEFAULT_AP;
	ptpClock->rtOpts->servo.ai = DEFAULT_AI;

	ptpClock->rtOpts->inboundLatency.nanoseconds = DEFAULT_INBOUND_LATENCY;
	ptpClock->rtOpts->outboundLatency.nanoseconds = DEFAULT_OUTBOUND_LATENCY;

	ptpClock->foreignMasterDS.records = ptpForeignRecords;
	ptpClock->rtOpts->maxForeignRecords = sizeof(ptpForeignRecords) / sizeof(ptpForeignRecords[0]);
	
	ptpClock->rtOpts->stats = PTP_TEXT_STATS;
	ptpClock->rtOpts->delayMechanism = DEFAULT_DELAY_MECHANISM;

	// Initialize run time options.
	if (_ptpdStartup(ptpClock) != 0)
	{
		printf("PTPD: startup failed");
		return;
	}

#ifdef USE_DHCP
	// If DHCP, wait until the default interface has an IP address.
	while (!netif_default->ip_addr.addr)
	{
		// Sleep for 500 milliseconds.
		sys_msleep(500);
	}
#endif

	// Loop forever.
	for (;;)
	{
		void *msg;

		// Process the current state.
		do
		{
			// doState() has a switch for the actions and events to be
			// checked for 'port_state'. The actions and events may or may not change
			// 'port_state' by calling toState(), but once they are done we loop around
			// again and perform the actions required for the new 'port_state'.
			ptpStateMachine(ptpClock);
		}
		while (netSelect(ptpClock->netPath, 0) > 0);
		
		// Wait up to 100ms for something to do, then do something anyway.
		sys_arch_mbox_fetch(&ptp_alert_queue, &msg, 100);
	}
}

// Notify the PTP thread of a pending operation.
void ptpd_alert(void)
{
	// Send a message to the alert queue to wake up the PTP thread.
	sys_mbox_trypost(&ptp_alert_queue, NULL);
}

void ptpd_init(void)
{
	// Create the alert queue mailbox.
	if (sys_mbox_new(&ptp_alert_queue, 8) != ERR_OK)
	{
		printf("PTPD: failed to create ptp_alert_queue mbox");
	}
	_ptpClock.rtOpts = &rtOpts;

	// Create the PTP daemon thread.
	sys_thread_new("PTPD", _ptpdTask, &_ptpClock, DEFAULT_THREAD_STACKSIZE * 2,  osPriorityAboveNormal);
}

