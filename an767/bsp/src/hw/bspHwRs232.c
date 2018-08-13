


/*
* RS232, from USART1 --> FPGA UART --> RS232
* Jun.8th, 2018  Jack Lee
*/

#include "compact.h"
#include "bsp.h"

#include "gpio.h"
#include "pio_handler.h"


int extRs232Write(unsigned char *data, unsigned short size)
{
	unsigned int i;
//	unsigned char *d = data;
	
#if EXT_RS232_DEBUG
	printf("RS232: output:'");
#endif
	for(i=0; i< size; i++)
	{
		if( usart_serial_putchar(USART1, *(data+i)) == 0)
		{
			EXT_ERRORF(("Write RS232 failed on #%u", i));
			return EXIT_FAILURE;
		}
#if EXT_RS232_DEBUG
		printf("%c ", *(data+i));
#endif
	}
	
#if EXT_RS232_DEBUG
	printf("'"EXT_NEW_LINE);
#endif

	return EXIT_SUCCESS;
}


/** 
 * \brief Initializes the Usart in master mode.
 *
 * \param p_usart  Base address of the USART instance.
 * \param opt      Options needed to set up RS232 communication (see
 * \ref usart_options_t).
 */
void extHwRs232Config(EXT_RUNTIME_CFG *runCfg)
{
	sam_usart_opt_t _settings;

#if EXT_RS232_DEBUG
	printf("RS232: Parity:");
#endif
	if(runCfg->rs232Cfg.parityType == EXT_RS232_PARITY_ODD )
	{
		_settings.parity_type = US_MR_PAR_ODD;
#if EXT_RS232_DEBUG
		printf("ODD");
#endif
	}
	else if(runCfg->rs232Cfg.parityType == EXT_RS232_PARITY_EVEN )
	{
		_settings.parity_type = US_MR_PAR_EVEN;
#if EXT_RS232_DEBUG
		printf("EVEN");
#endif
	}
	else
	{/* default no parity */
		_settings.parity_type = US_MR_PAR_NO;
#if EXT_RS232_DEBUG
		printf("NO");
#endif
	}

#if EXT_RS232_DEBUG
	printf(EXT_NEW_LINE": CharBits:");
#endif
	if(runCfg->rs232Cfg.charLength == EXT_RS232_CHAR_LENGTH_5 )
	{
		_settings.char_length = US_MR_CHRL_5_BIT;
#if EXT_RS232_DEBUG
		printf("5");
#endif
	}
	else if(runCfg->rs232Cfg.charLength == EXT_RS232_CHAR_LENGTH_6 )
	{
		_settings.char_length = US_MR_CHRL_6_BIT;
#if EXT_RS232_DEBUG
		printf("6");
#endif
	}
	else if(runCfg->rs232Cfg.charLength == EXT_RS232_CHAR_LENGTH_7 )
	{
#if EXT_RS232_DEBUG
		printf("7");
#endif
		_settings.char_length = US_MR_CHRL_7_BIT;
	}
	else
	{/* default 8 bits */
#if EXT_RS232_DEBUG
		printf("8");
#endif
		_settings.char_length = US_MR_CHRL_8_BIT;
	}
	
#if EXT_RS232_DEBUG
	printf(EXT_NEW_LINE": StopBits:");
#endif
	if(runCfg->rs232Cfg.stopbits == EXT_RS232_STOP_BITS_2 )
	{
#if EXT_RS232_DEBUG
		printf("2");
#endif
		_settings.stop_bits = US_MR_NBSTOP_2_BIT;
	}
	else if(runCfg->rs232Cfg.stopbits == EXT_RS232_STOP_BITS_15 )
	{
#if EXT_RS232_DEBUG
		printf("1.5");
#endif
		_settings.stop_bits = US_MR_NBSTOP_1_5_BIT;
	}
	else
	{/* default 1 stop bit*/
#if EXT_RS232_DEBUG
		printf("1");
#endif
		_settings.stop_bits = US_MR_NBSTOP_1_BIT;
	}
	
	_settings.baudrate = runCfg->rs232Cfg.baudRate;
#if EXT_RS232_DEBUG
	printf(EXT_NEW_LINE"baudrate:%ld"EXT_NEW_LINE, _settings.baudrate );
#endif	
	_settings.channel_mode= US_MR_CHMODE_NORMAL;

	sysclk_enable_peripheral_clock(ID_USART1);

	/* Configure USART */
	usart_init_rs232(USART1, &_settings, sysclk_get_peripheral_hz())/* eg. 0x40028000U */;

	/* Enable the receiver and transmitter. */
	usart_enable_tx(USART1);
	usart_enable_rx(USART1);

}


/* called when startup. init pin and default params  */
void extHwRs232Init(EXT_RUNTIME_CFG *runCfg)
{

	/* Configure USART pins : connect to RS232 */
	ioport_set_pin_peripheral_mode(USART1_RXD_GPIO, USART1_RXD_FLAGS);
	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;
	ioport_set_pin_peripheral_mode(USART1_TXD_GPIO, USART1_TXD_FLAGS);
	
	sysclk_enable_peripheral_clock(ID_USART1);

	/* Configure USART */
	extHwRs232Config(runCfg);

	extRs232Write((unsigned char *)"RS232 OK", 8);
}

