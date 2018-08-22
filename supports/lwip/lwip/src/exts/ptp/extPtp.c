
#include "ptpd.h"

#if LWIP_PTP

/*
* Description    : Initialize timestamping ability of ETH
* Input          : UpdateMethod:
*                       ETH_PTP_FineUpdate   : Fine Update method
*                       ETH_PTP_CoarseUpdate : Coarse Update method 
* Output         : None
* Return         : None
*/
void ETH_PTPStart(uint32_t UpdateMethod)
{
#if 0
	/* Check the parameters */
	assert_param(IS_ETH_PTP_UPDATE(UpdateMethod));

	/* Mask the Time stamp trigger interrupt by setting bit 9 in the MACIMR register. */
	ETH_MACITConfig(ETH_MAC_IT_TST, DISABLE);

	/* Program Time stamp register bit 0 to enable time stamping. */
	ETH_PTPTimeStampCmd(ENABLE);

	/* Program the Subsecond increment register based on the PTP clock frequency. */
	ETH_SetPTPSubSecondIncrement(ADJ_FREQ_BASE_INCREMENT); /* to achieve 20 ns accuracy, the value is ~ 43 */

	if (UpdateMethod == ETH_PTP_FineUpdate)
	{
		/* If you are using the Fine correction method, program the Time stamp addend register
		* and set Time stamp control register bit 5 (addend register update). */
		ETH_SetPTPTimeStampAddend(ADJ_FREQ_BASE_ADDEND);
		ETH_EnablePTPTimeStampAddend();

		/* Poll the Time stamp control register until bit 5 is cleared. */
		while(ETH_GetPTPFlagStatus(ETH_PTP_FLAG_TSARU) == SET);
	}

	/* To select the Fine correction method (if required),
	* program Time stamp control register  bit 1. */
	ETH_PTPUpdateMethodConfig(UpdateMethod);

	/* Program the Time stamp high update and Time stamp low update registers
	* with the appropriate time value. */
	ETH_SetPTPTimeStampUpdate(ETH_PTP_PositiveTime, 0, 0);

	/* Set Time stamp control register bit 2 (Time stamp init). */
	ETH_InitializePTPTimeStamp();

	/* The enhanced descriptor format is enabled and the descriptor size is
	* increased to 32 bytes (8 DWORDS). This is required when time stamping 
	* is activated above. */
	ETH_EnhancedDescriptorCmd(ENABLE);

	/* The Time stamp counter starts operation as soon as it is initialized
	* with the value written in the Time stamp update register. */
#endif

}

/*
* Function Name  : ETH_PTPTimeStampAdjFreq
* Description    : Updates time stamp addend register
* Input          : Correction value in thousandth of ppm (Adj*10^9)
* Output         : None
* Return         : None
*/
void ETH_PTPTime_AdjFreq(int32_t Adj)
{
	uint32_t addend;
	
	/* calculate the rate by which you want to speed up or slow down the system time increments */
 
	/* precise */
	/*
	int64_t addend;
	addend = Adj;
	addend *= ADJ_FREQ_BASE_ADDEND;
	addend /= 1000000000-Adj;
	addend += ADJ_FREQ_BASE_ADDEND;
	*/

	/* 32bit estimation
	ADJ_LIMIT = ((1l<<63)/275/ADJ_FREQ_BASE_ADDEND) = 11258181 = 11 258 ppm*/
	if( Adj > 5120000)
		Adj = 5120000;
	if( Adj < -5120000)
		Adj = -5120000;

	addend = ((((275LL * Adj)>>8) * (ADJ_FREQ_BASE_ADDEND>>24))>>6) + ADJ_FREQ_BASE_ADDEND;

#if 0	
	/* Reprogram the Time stamp addend register with new Rate value and set ETH_TPTSCR */
	ETH_SetPTPTimeStampAddend((uint32_t)addend);
	ETH_EnablePTPTimeStampAddend();
#endif	
}

/**
* Description    : Updates time base offset
* Input          : Time offset with sign
* Output         : None
* Return         : None
*/
void ETH_PTPTime_UpdateOffset(struct ptptime_t * timeoffset)
{
#if 0
	uint32_t Sign;
	uint32_t SecondValue;
	uint32_t NanoSecondValue;
	uint32_t SubSecondValue;
	uint32_t addend;

	/* determine sign and correct Second and Nanosecond values */
	if(timeoffset->tv_sec < 0 || (timeoffset->tv_sec == 0 && timeoffset->tv_nsec < 0))
	{
		Sign = ETH_PTP_NegativeTime;
		SecondValue = -timeoffset->tv_sec;
		NanoSecondValue = -timeoffset->tv_nsec;
	}
	else
	{
		Sign = ETH_PTP_PositiveTime;
		SecondValue = timeoffset->tv_sec;
		NanoSecondValue = timeoffset->tv_nsec;
	}

	/* convert nanosecond to subseconds */
	SubSecondValue = ETH_PTPNanoSecond2SubSecond(NanoSecondValue);

	/* read old addend register value*/
	addend = ETH_GetPTPRegister(ETH_PTPTSAR);

	while(ETH_GetPTPFlagStatus(ETH_PTP_FLAG_TSSTU) == SET);
	while(ETH_GetPTPFlagStatus(ETH_PTP_FLAG_TSSTI) == SET);

	/* Write the offset (positive or negative) in the Time stamp update high and low registers. */
	ETH_SetPTPTimeStampUpdate(Sign, SecondValue, SubSecondValue);

	/* Set bit 3 (TSSTU) in the Time stamp control register. */
	ETH_EnablePTPTimeStampUpdate();

	/* The value in the Time stamp update registers is added to or subtracted from the system */
	/* time when the TSSTU bit is cleared. */
	while(ETH_GetPTPFlagStatus(ETH_PTP_FLAG_TSSTU) == SET);      

	/* Write back old addend register value. */
	ETH_SetPTPTimeStampAddend(addend);
	ETH_EnablePTPTimeStampAddend();
#endif

}

/*
* Function Name  : ETH_PTPTimeStampSetTime
* Description    : Initialize time base
* Input          : Time with sign
* Output         : None
* Return         : None
*/
void ETH_PTPTime_SetTime(struct ptptime_t * timestamp)
{
#if 0
	uint32_t Sign;
	uint32_t SecondValue;
	uint32_t NanoSecondValue;
	uint32_t SubSecondValue;

	/* determine sign and correct Second and Nanosecond values */
	if(timestamp->tv_sec < 0 || (timestamp->tv_sec == 0 && timestamp->tv_nsec < 0))
	{
		Sign = ETH_PTP_NegativeTime;
		SecondValue = -timestamp->tv_sec;
		NanoSecondValue = -timestamp->tv_nsec;
	}
	else
	{
		Sign = ETH_PTP_PositiveTime;
		SecondValue = timestamp->tv_sec;
		NanoSecondValue = timestamp->tv_nsec;
	}

	/* convert nanosecond to subseconds */
	SubSecondValue = ETH_PTPNanoSecond2SubSecond(NanoSecondValue);

	/* Write the offset (positive or negative) in the Time stamp update high and low registers. */
	ETH_SetPTPTimeStampUpdate(Sign, SecondValue, SubSecondValue);
	/* Set Time stamp control register bit 2 (Time stamp init). */
	ETH_InitializePTPTimeStamp();
	/* The Time stamp counter starts operation as soon as it is initialized
	 * with the value written in the Time stamp update register. */
	while(ETH_GetPTPFlagStatus(ETH_PTP_FLAG_TSSTI) == SET);
#endif

}

#endif /* LWIP_PTP */

