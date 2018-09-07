/* ptpd.c */

#include "ptpd.h"

#define PTPD_THREAD_PRIO    (tskIDLE_PRIORITY + 2)

static sys_mbox_t _ptp_alert_queue;

// Statically allocated run-time configuration data.
static RunTimeOpts	_rtOpts;

PtpClock		ptpClock;
ForeignMasterRecord ptpForeignRecords[DEFAULT_MAX_FOREIGN_RECORDS];

__IO uint32_t PTPTimer = 0;

static int16_t _ptpdStartup(PtpClock * _pptpClock)
{
	/* 9.2.2 */
	if (_pptpClock->rtOpts->slaveOnly) 
		_pptpClock->rtOpts->clockQuality.clockClass = DEFAULT_CLOCK_CLASS_SLAVE_ONLY;

	/* No negative or zero attenuation */
	if (_pptpClock->rtOpts->servo.ap < 1)
		_pptpClock->rtOpts->servo.ap = 1;
	if (_pptpClock->rtOpts->servo.ai < 1)
		_pptpClock->rtOpts->servo.ai = 1;

	DBG("event POWER UP");

	toState(_pptpClock, PTP_INITIALIZING);

	return 0;
}


static void _ptpdTask(void *arg)
{
	PtpClock	*_pptpClock = (PtpClock *)arg;

	// Initialize run-time options to default values.
	_pptpClock->rtOpts->announceInterval = DEFAULT_ANNOUNCE_INTERVAL;
	_pptpClock->rtOpts->syncInterval = DEFAULT_SYNC_INTERVAL;
	
	_pptpClock->rtOpts->clockQuality.clockAccuracy = DEFAULT_CLOCK_ACCURACY;
	_pptpClock->rtOpts->clockQuality.clockClass = DEFAULT_CLOCK_CLASS;
	_pptpClock->rtOpts->clockQuality.offsetScaledLogVariance = DEFAULT_CLOCK_VARIANCE; /* 7.6.3.3 */
	_pptpClock->rtOpts->priority1 = DEFAULT_PRIORITY1;
	_pptpClock->rtOpts->priority2 = DEFAULT_PRIORITY2;
	_pptpClock->rtOpts->domainNumber = DEFAULT_DOMAIN_NUMBER;

	_pptpClock->rtOpts->slaveOnly = SLAVE_ONLY;

	_pptpClock->rtOpts->currentUtcOffset = DEFAULT_UTC_OFFSET;
	_pptpClock->rtOpts->servo.noResetClock = DEFAULT_NO_RESET_CLOCK;
	_pptpClock->rtOpts->servo.noAdjust = NO_ADJUST;

	_pptpClock->rtOpts->servo.sDelay = DEFAULT_DELAY_S;
	_pptpClock->rtOpts->servo.sOffset = DEFAULT_OFFSET_S;
	_pptpClock->rtOpts->servo.ap = DEFAULT_AP;
	_pptpClock->rtOpts->servo.ai = DEFAULT_AI;

	_pptpClock->rtOpts->inboundLatency.nanoseconds = DEFAULT_INBOUND_LATENCY;
	_pptpClock->rtOpts->outboundLatency.nanoseconds = DEFAULT_OUTBOUND_LATENCY;

	_pptpClock->foreignMasterDS.records = ptpForeignRecords;
	_pptpClock->rtOpts->maxForeignRecords = sizeof(ptpForeignRecords) / sizeof(ptpForeignRecords[0]);
	
	_pptpClock->rtOpts->stats = PTP_TEXT_STATS;
	_pptpClock->rtOpts->delayMechanism = DEFAULT_DELAY_MECHANISM;

	_pptpClock->netPath.multicastAddr = IPADDR_ANY;

	// Initialize run time options.
	if (_ptpdStartup(_pptpClock) != 0)
	{
		EXT_ERRORF(("PTPD: startup failed"));
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
			ptpStateMachine(_pptpClock);
		}while (ptpNetSelect(&_pptpClock->netPath, 0) != 0);
		
		// Wait up to 100ms for something to do, then do something anyway.
		sys_arch_mbox_fetch(&_ptp_alert_queue, &msg, 100);
	}
}

// Notify the PTP thread of a pending operation.
void ptpd_alert(void)
{
	// Send a message to the alert queue to wake up the PTP thread.
	sys_mbox_trypost(&_ptp_alert_queue, NULL);
}

void ptpd_init(void)
{
	memset(&ptpClock, 0, sizeof(PtpClock));
	// Create the alert queue mailbox.
	if (sys_mbox_new(&_ptp_alert_queue, 8) != ERR_OK)
	{
		EXT_ERRORF(("PTPD: failed to create ptp_alert_queue mbox"));
	}
	ptpClock.rtOpts = &_rtOpts;

	// Create the PTP daemon thread.
	sys_thread_new("PTPD", _ptpdTask, &ptpClock, EXT_NET_IF_TASK_STACK_SIZE,  EXT_NET_IF_TASK_PRIORITY-2 );
}

