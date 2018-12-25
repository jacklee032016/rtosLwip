/* sys.c */

#include "ptpd.h"

void displayStats(const PtpClock *ptpClock)
{
	const char *s;
	unsigned char *uuid;
	char sign;

	uuid = (unsigned char*) ptpClock->parentDS.parentPortIdentity.clockIdentity;

	/* Master clock UUID */
	printf("%02X%02X:%02X%02X:%02X%02X:%02X%02X\n",
					uuid[0], uuid[1],
					uuid[2], uuid[3],
					uuid[4], uuid[5],
					uuid[6], uuid[7]);

	switch (ptpClock->portDS.portState)
	{
		case PTP_INITIALIZING:  s = "init";  break;
		case PTP_FAULTY:        s = "faulty";   break;
		case PTP_LISTENING:     s = "listening";  break;
		case PTP_PASSIVE:       s = "passive";  break;
		case PTP_UNCALIBRATED:  s = "uncalibrated";  break;
		case PTP_SLAVE:         s = "slave";   break;
		case PTP_PRE_MASTER:    s = "pre master";  break;
		case PTP_MASTER:        s = "master";   break;
		case PTP_DISABLED:      s = "disabled";  break;
		default:                s = "?";     break;
	}

	/* State of the PTP */
	printf("state: %s\n", s);

	/* One way delay */
	switch (ptpClock->portDS.delayMechanism)
	{
		case E2E:
			printf("path delay: %"U32_F" nsec\n", ptpClock->currentDS.meanPathDelay.nanoseconds);
			break;
		case P2P:
			printf("path delay: %"U32_F" nsec\n", ptpClock->portDS.peerMeanPathDelay.nanoseconds);
			break;
		default:
			printf("path delay: unknown\n");
			/* none */
			break;
	}

	/* Offset from master */
	if (ptpClock->currentDS.offsetFromMaster.seconds)
	{
		printf("offset: %"U32_F" sec\n", ptpClock->currentDS.offsetFromMaster.seconds);
	}
	else
	{
		printf("offset: %"U32_F" nsec\n", ptpClock->currentDS.offsetFromMaster.nanoseconds);
	}

	/* Observed drift from master */
	sign = ' ';
	if (ptpClock->observedDrift > 0) sign = '+';
	if (ptpClock->observedDrift < 0) sign = '-';

	printf("drift: %c%d.%03d ppm\n", sign, abs(ptpClock->observedDrift / 1000), abs(ptpClock->observedDrift % 1000));
}

void getTime(TimeInternal *time)
{
#ifdef ARM	
	struct ptptime_t timestamp;
	ETH_PTPTime_GetTime(&timestamp);
	time->seconds = timestamp.tv_sec;
	time->nanoseconds = timestamp.tv_nsec;
#else
	struct timeval tv;
	gettimeofday(&tv, 0);
	time->seconds = tv.tv_sec;
	time->nanoseconds = tv.tv_usec * 1000;
#endif	
}

void setTime(const TimeInternal *time)
{
	struct ptptime_t ts;

	ts.tv_sec = time->seconds;
	ts.tv_nsec = time->nanoseconds;
	ETH_PTPTime_SetTime(&ts);
	DBG("resetting system clock to %"U32_F" sec %"U32_F" nsec\n", time->seconds, time->nanoseconds);
}

void updateTime(const TimeInternal *time)
{
	struct ptptime_t timeoffset;

	DBGV("updateTime: %"U32_F" sec %"U32_F" nsec\n", time->seconds, time->nanoseconds);

	timeoffset.tv_sec = -time->seconds;
	timeoffset.tv_nsec = -time->nanoseconds;

	/* Coarse update method */
	ETH_PTPTime_UpdateOffset(&timeoffset);
	DBGV("updateTime: updated\n");
}

uint32_t getRand(uint32_t randMax)
{
	return rand() % randMax;
}

bool  adjFreq(int32_t adj)
{
	DBGV("adjFreq %"U32_F, adj);

	if (adj > ADJ_FREQ_MAX)
		adj = ADJ_FREQ_MAX;
	else if (adj < -ADJ_FREQ_MAX)
		adj = -ADJ_FREQ_MAX;

	/* Fine update method */
	ETH_PTPTime_AdjFreq(adj);

	return TRUE;
}

