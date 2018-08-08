
bool shell_date(int argc, char **argv)
{
	char buffer[32];
	time_t seconds1900;
	struct ptptime_t ptptime;

	// Get the ethernet time values.
	ETH_PTPTime_GetTime(&ptptime);

	// Get the seconds since 1900.
	seconds1900 = (time_t) ptptime.tv_sec;

	// Format into a string.
	strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S UTC %Y\n", localtime(&seconds1900));

	// Print the string.
	telnet_puts(buffer);

	return true;
}

bool shell_ptpd(int argc, char **argv)
{
	char sign;
	const char *s;
	unsigned char *uuid;
	extern PtpClock ptpClock;

	uuid = (unsigned char*) ptpClock.parentDS.parentPortIdentity.clockIdentity;

	/* Master clock UUID */
	telnet_printf("master id: %02x%02x%02x%02x%02x%02x%02x%02x\n",
					uuid[0], uuid[1],
					uuid[2], uuid[3],
					uuid[4], uuid[5],
					uuid[6], uuid[7]);

	switch (ptpClock.portDS.portState)
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
	telnet_printf("state: %s\n", s);

	/* One way delay */
	switch (ptpClock.portDS.delayMechanism)
	{
		case E2E:
			telnet_puts("mode: end to end\n");
			telnet_printf("path delay: %d nsec\n", ptpClock.currentDS.meanPathDelay.nanoseconds);
			break;
		case P2P:
			telnet_puts("mode: peer to peer\n");
			telnet_printf("path delay: %d nsec\n", ptpClock.portDS.peerMeanPathDelay.nanoseconds);
			break;
		default:
			telnet_puts("mode: unknown\n");
			telnet_printf("path delay: unknown\n");
			/* none */
			break;
	}

	/* Offset from master */
	if (ptpClock.currentDS.offsetFromMaster.seconds)
	{
		telnet_printf("offset: %d sec\n", ptpClock.currentDS.offsetFromMaster.seconds);
	}
	else
	{
		telnet_printf("offset: %d nsec\n", ptpClock.currentDS.offsetFromMaster.nanoseconds);
	}

	/* Observed drift from master */
	sign = ' ';
	if (ptpClock.observedDrift > 0) sign = '+';
	if (ptpClock.observedDrift < 0) sign = '-';

	telnet_printf("drift: %c%d.%03d ppm\n", sign, abs(ptpClock.observedDrift / 1000), abs(ptpClock.observedDrift % 1000));

	return true;
}

#ifdef PTPD_DBG
char *ptpStateString(uint8_t state)
{
	switch (state)
	{
		case PTP_INITIALIZING: return (char *) "PTP_INITIALIZING";
		case PTP_FAULTY: return (char *) "PTP_FAULTY";
		case PTP_DISABLED: return (char *) "PTP_DISABLED";
		case PTP_LISTENING: return (char *) "PTP_LISTENING";
		case PTP_PRE_MASTER: return (char *) "PTP_PRE_MASTER";
		case PTP_MASTER: return (char *) "PTP_MASTER";
		case PTP_PASSIVE: return (char *) "PTP_PASSIVE";
		case PTP_UNCALIBRATED: return (char *) "PTP_UNCALIBRATED";
		case PTP_SLAVE: return (char *) "PTP_SLAVE";
		default: break;
	}
	return (char *) "PTP_UNKNOWN";
}
#endif


