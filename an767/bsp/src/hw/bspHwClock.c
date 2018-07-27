
/* only for gpio */

#include "compact.h"

#include "gpio.h"
#include "muxLmh1983.h"

/* 8 bits address, and 8 bit data */
#define	LMH1983_WRITE(address, val, size)		\
	muxI2CWrite(MUX_I2C_PCA9554_CS_NONE, MUX_I2C_ADDRESS_LM1983, (address), 1, (val), size)

#define	LMH1983_READ(address, val, size)		\
	muxI2CRead(MUX_I2C_PCA9554_CS_NONE, MUX_I2C_ADDRESS_LM1983, (address), 1, (val), size)


static unsigned char	_bspHwClock(void)
{
	unsigned char		data = 0;
	/* reset PLL module: rising edge triggered */
/* although rising edge triggered, this low level is not needed. J.L. April 10,2018 */
#if 0
	gpio_set_pin_low(MUX_PIN_PLL_INIT);
	MUX_DELAY_MS(10);
#endif

	gpio_set_pin_high(MUX_PIN_PLL_INIT);
#if 1
	MUX_DELAY_MS(1);
#else
	/* enlarge delay to use osccilator */
	MUX_DELAY_MS(1000);
#endif
	gpio_set_pin_low(MUX_PIN_PLL_INIT);
	MUX_DELAY_MS(10);

	LMH1983_READ(LMH1983_REVISION_ID, &data, 1);

	return data;
}


void bspHwClockInit(void)
{
	unsigned char		data;

	data = _bspHwClock();
	if(data != 0xC0)
	{
		MUX_ERRORF(("LMH1983 RevisionID wrong: %2x", data));
	}
	else
	{
		MUX_INFOF(("LMH1983 RevisionID OK: %2x", data));
	}

	// Errata on poor duty cycle on PLL3
	data = 	0x00; /* default setting, refer to switch_clock */
	LMH1983_WRITE(LMH1983_OMODE_MISC, &data, 1);

	data = 	0x00;
	LMH1983_WRITE(LMH1983_OMODE_MISC, &data, 1);

	data = 	0x80; /* soft reset */
	LMH1983_WRITE(LMH1983_DEV_CTRL, &data, 1);
	data = 	0x23; /* enable AFD(auto format detect), force_148(PLL2=148.5Mhz; PLL3=148.45MHz); enable active outputs: all are default value for this register */
	LMH1983_WRITE(LMH1983_DEV_CTRL, &data, 1);

	data = 	0x20; /* tof always align for duty cycle issue: always align */
	/* TOP, Top Of Frame, eg, frame rate of video, 29.97 or 59.94 */
	LMH1983_WRITE(LMH1983_ALIGN_CTRL_TOF3, &data, 1);

	///	LMH1983_WRITE(LMH1983_INPUT_FMT, &data, 1);

	// wait for clock to reprogram and be stable
	MUX_DELAY_MS(100);	
}

void  bspHwClockSetDac( uint32_t pll_dac)
{
	unsigned char data[2];

	pll_dac &= 0x3ff;
	data[1] = (unsigned char )pll_dac;
	data[0] = (unsigned char )(pll_dac>>8);
	/* ??? 0x18 is only MSB of LOR control: Free-run Control Voltage */
	LMH1983_WRITE(LMH1983_LOR_CTRL_FR_MSB, data, 2 );
}

void  bspHwClockSwitchClk( unsigned char  sw)
{
	unsigned char data;
	if (sw == 0)
	{/* default value: PLL2 --> Out2; PLL3 --> Out3  */
		data = 0x00;
		LMH1983_WRITE(LMH1983_OMODE_MISC, &data, 1 );		
	}
	else
	{/* Crosspoint Output, page 34, cross PLL2 and PLL3: PLL2-->Out3; PLL3-->Out2 */	
		data = 0x04;
		LMH1983_WRITE(LMH1983_OMODE_MISC, &data, 1 );
	}
}


char  bspBistClock(char *outBuffer, size_t bufferSize)
{
	int index = 0;
	unsigned char		data;

	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	data = _bspHwClock();
	index += snprintf( outBuffer+index, (bufferSize-index), "\t\tLMH1983 RevisionID:'0x%2x' : %s"MUX_NEW_LINE, data, (data != 0xC0)?"Error":"OK" ) ;
	if(data != 0xC0)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

char  bspBistDipSwitch(char *outBuffer, size_t bufferSize)
{
	int index = 0;

	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

#if 1
//	printf( "DIP Status: SW1:%u;"MUX_NEW_LINE, gpio_pin_is_high(MUX_PIN_DIP_SW_01) );
//	index += snprintf( outBuffer+index, (bufferSize-index), "DIP Status: SW1:%u;"MUX_NEW_LINE, gpio_pin_is_high(MUX_PIN_DIP_SW_01) );
	index += snprintf( outBuffer+index, (bufferSize-index), "\t\tDIP Status: SW1:%u; SW2:%u; SW3:%u; SW4:%u"MUX_NEW_LINE, 
		MUX_DIP_STATUS_SW1(), MUX_DIP_STATUS_SW2(), MUX_DIP_STATUS_SW3(), MUX_DIP_STATUS_SW4() );
#else
	index += snprintf( outBuffer+index, (bufferSize-index), "DIP Status: SW1:; SW2:; SW3:; SW:"MUX_NEW_LINE);
#endif

	return EXIT_SUCCESS;
}


char bspMultiAddressFromDipSwitch(void)
{
	char lastDigit = 0;

	lastDigit = MUX_DIP_STATUS_SW1()|(MUX_DIP_STATUS_SW2()<<1)|(MUX_DIP_STATUS_SW3()<<2)|(MUX_DIP_STATUS_SW4()<<3);
	if(lastDigit == 0 )
	{
		lastDigit = 16;
	}

//	printf( "DIP Status: SW1:%u; SW2:%u; SW3:%u; SW4:%u, lastDigit:%u"MUX_NEW_LINE,  MUX_DIP_STATUS_SW1(), MUX_DIP_STATUS_SW2(), MUX_DIP_STATUS_SW3(), MUX_DIP_STATUS_SW4(), lastDigit );

	return lastDigit;
}

