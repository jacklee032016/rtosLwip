/* startup.c */

#include "../ptpd.h"

void ptpdShutdown(PtpClock *ptpClock)
{
	netShutdown(&ptpClock->netPath);
}

