/*
* BSP initialization and BIST (Built-In Self-Test)
* Feb.8th, 1028  Jack Lee
*/

#include "compact.h"
#include "bsp.h"

#include "gpio.h"
#include "pio_handler.h"

/** The erase pin mask value in the PIO mode and erase mode. */
#define PIN_PIO_MODE_MSK         (matrix_get_system_io() | CCFG_SYSIO_SYSIO12)
#define PIN_ERASE_MODE_MSK       (matrix_get_system_io() & (~CCFG_SYSIO_SYSIO12))

static uint32_t _firstTime = 0;


/**
 *  Handler for Button 1 rising edge interrupt.
 *  Set button1 event flag (g_button_event).
 */
static void _buttonHandler(uint32_t id, uint32_t mask)
{

	if ((EXT_PUSH_BUTTON_ID == id) && (EXT_PUSH_BUTTON_PIN_MSK == mask) )
	{

		wakeResetInIsr();

//		g_button_event = 1;
		if(_firstTime == 0)
		{
			if(extRun.bootMode == BOOT_MODE_BOOTLOADER)
			{
				EXT_INFOF(("Bootloader now reboot..."EXT_NEW_LINE EXT_NEW_LINE) );
				EXT_PRINTF((EXT_NEW_LINE));
				EXT_REBOOT();
			}

			bspButtonConfig(BOOT_MODE_RTOS , EXT_FALSE);
			_firstTime = sys_get_ms();
//			EXT_INFOF(("'%s' is pressed at %"FOR_U32" ms:%d"EXT_NEW_LINE EXT_NEW_LINE, EXT_BUTTON_STRING, _firstTime, extRun.bootMode ));
		}
		else
		{
			unsigned int duration = (sys_get_ms()-_firstTime);
			EXT_INFOF(("'%s' is unpressed after %d ms"EXT_NEW_LINE , EXT_BUTTON_STRING, duration));
			if(duration > BTN_FACTORY_DURATION)
			{
				EXT_INFOF(("now factory reset..."EXT_NEW_LINE ));
				bspCmdFactory(NULL, NULL, 0);

//				extDelayReboot(1000);
			}
			else
			{
				EXT_INFOF(("now reboot..."EXT_NEW_LINE ) );
			}
			
			EXT_PRINTF((EXT_NEW_LINE)); /* reset the color to default before reboot */
			EXT_REBOOT();
		}
	}
}


/**
 *  \brief Configure the push button.
 *
 *  Configure the PIOs as inputs and generate corresponding interrupt when
 *  pressed or released.
 * First time, it is initialized in Rise Edge; After btn is pressed, it is initialized in Fall Edge
 */
void bspButtonConfig(boot_mode bMode, char isRiseEdge)
{
//	if(bMode == BOOT_MODE_RTOS)
	{
		unsigned int attribute = EXT_PUSH_BUTTON_ATTR_RISE_EDGE;
		if(isRiseEdge == EXT_TRUE)
			attribute = EXT_PUSH_BUTTON_ATTR_FALL_EDGE;
		
		/* Configure PIO clock. */
		pmc_enable_periph_clk(EXT_PUSH_BUTTON_ID );

		/* Adjust pio debounce filter parameters, uses 10 Hz filter. */
		pio_set_debounce_filter(EXT_PUSH_BUTTON_PIO, EXT_PUSH_BUTTON_PIN_MSK, 2 );

		/* Initialize pios interrupt handlers, see PIO definition in board.h. */
		pio_handler_set(EXT_PUSH_BUTTON_PIO, EXT_PUSH_BUTTON_ID, EXT_PUSH_BUTTON_PIN_MSK, attribute, _buttonHandler);

		/* Enable PIO controller IRQs. */
		NVIC_EnableIRQ((IRQn_Type)EXT_PUSH_BUTTON_ID);

		/* Enable PIO line interrupts. */
		pio_enable_interrupt(EXT_PUSH_BUTTON_PIO, EXT_PUSH_BUTTON_PIN_MSK);
		gpio_configure_pin(EXT_PIN_SOFTWARE_RESET, IOPORT_DIR_INPUT );
	}
#if 0
	else
	{/* bootloader */	
		gpio_configure_pin(PIO_PA30_IDX, PIO_TYPE_PIO_INPUT );
	}
#endif
	/*switch buttons */
#if 0	
	gpio_configure_pin(EXT_PIN_DIP_SW_01, IOPORT_DIR_INPUT );
	gpio_configure_pin(EXT_PIN_DIP_SW_02, IOPORT_DIR_INPUT );
	gpio_configure_pin(EXT_PIN_DIP_SW_03, IOPORT_DIR_INPUT );
	gpio_configure_pin(EXT_PIN_DIP_SW_04, IOPORT_DIR_INPUT );
#else
	gpio_configure_pin(EXT_PIN_DIP_SW_01, PIO_TYPE_PIO_INPUT );
	gpio_configure_pin(EXT_PIN_DIP_SW_02, PIO_TYPE_PIO_INPUT );
	gpio_configure_pin(EXT_PIN_DIP_SW_03, PIO_TYPE_PIO_INPUT );
	gpio_configure_pin(EXT_PIN_DIP_SW_04, PIO_TYPE_PIO_INPUT );
#endif
}

static void _extBspPinsConfig(void)
{
	//WDT->WDT_MR = WDT_MR_WDDIS;         // disable watchdog

	/* PROGRAM_B: Active Low reset to configuration logic; so output pin with high levelOUTPUT + INIT_HIGH + Drive Low Configuration */
#if 0
	gpio_configure_pin(EXT_PIN_FPGA_PROGRAM, IOPORT_DIR_OUTPUT | IOPORT_PIN_LEVEL_HIGH );//| IOPORT_MODE_PULLDOWN);
#else
//	gpio_configure_pin(EXT_PIN_FPGA_PROGRAM, IOPORT_DIR_OUTPUT | IOPORT_PIN_LEVEL_HIGH | IOPORT_MODE_PULLUP);
	gpio_configure_pin(EXT_PIN_FPGA_PROGRAM,  PIO_OUTPUT_1 | PIO_DEFAULT);
#endif
//	gpio_configure_pin(EXT_PIN_FPGA_DONE,     IOPORT_DIR_INPUT );
	gpio_configure_pin(EXT_PIN_FPGA_DONE,     PIO_TYPE_PIO_INPUT );


#if 0
	gpio_configure_pin(EXT_PIN_PLL_INIT,	IOPORT_DIR_OUTPUT | IOPORT_PIN_LEVEL_LOW | IOPORT_MODE_PULLDOWN);
#else
	gpio_configure_pin(EXT_PIN_PLL_INIT,	 PIO_OUTPUT_0 | PIO_DEFAULT);
#endif

//	gpio_configure_pin(PIN_JUMPER_SELECT,     IOPORT_DIR_INPUT );
	gpio_configure_pin(EXT_PIN_POWER_1V_OK, IOPORT_DIR_INPUT );

}


void bspHwConsoleConfig(void)
{
	usart_serial_options_t _options =
	{
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
#if EXTLAB_BOARD
	/* configure UART pins */
	ioport_set_port_mode(IOPORT_PIOA, PIO_PA9A_URXD0 | PIO_PA10A_UTXD0, IOPORT_MODE_MUX_A);
	ioport_disable_port(IOPORT_PIOA, PIO_PA9A_URXD0 | PIO_PA10A_UTXD0);
	
	sysclk_enable_peripheral_clock(ID_UART0); 	/* 7 */
	
	stdio_serial_init(UART0, &_options); /* eg. 0x400E0800U */

#else
	/* Configure USART pins */
	ioport_set_pin_peripheral_mode(USART1_RXD_GPIO, USART1_RXD_FLAGS);
	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;
	ioport_set_pin_peripheral_mode(USART1_TXD_GPIO, USART1_TXD_FLAGS);
	
	sysclk_enable_peripheral_clock(ID_USART1);		/* 14 */
	stdio_serial_init(USART1, &_options); /* eg. 0x40028000U */
#endif

}


/* configure PIN and SPI master */
static void _bspHwSpiConfig(void)
{

	ioport_set_pin_peripheral_mode(EXT_SPI_MISO_GPIO,  EXT_SPI_MISO_FLAGS);
//	gpio_configure_pin(SPI0_MISO_GPIO,		 IOPORT_DIR_INPUT | IOPORT_PIN_LEVEL_HIGH | IOPORT_MODE_PULLUP);
	ioport_set_pin_peripheral_mode(EXT_SPI_MOSI_GPIO,  EXT_SPI_MOSI_FLAGS);
	ioport_set_pin_peripheral_mode(EXT_SPI_SPCK_GPIO,  EXT_SPI_SPCK_FLAGS);

#if EXTLAB_BOARD
#else
	ioport_set_pin_peripheral_mode(EXT_SPI_NPCS0_GPIO, EXT_SPI_NPCS0_FLAGS);
#endif

	ioport_set_pin_peripheral_mode(EXT_SPI_NPCS1_GPIO,  EXT_SPI_NPCS1_FLAGS);

	bspSpiMasterInitialize(EXT_SPI_CHIP_SEL);

	bspHwSpiFlashInit();
}

SC_CTRL		scCtrl;

void bspHwInit(boot_mode bMode, uint8_t isTx)
{
	EXT_RUNTIME_CFG *runCfg = &extRun;
	memset(runCfg, 0, sizeof(EXT_RUNTIME_CFG));

	runCfg->bootMode = bMode;
	runCfg->isTx = isTx;

	runCfg->sc = &scCtrl;
	memset(runCfg->sc, 0, sizeof(SC_CTRL));
	
	sysclk_init();

	/* Disable the watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;
	/* after memory and watchdog, initialize IOPORTs */
	ioport_init();

	_extBspPinsConfig();
	

	/* UART must follow PINs configuration, so put it here */
	bspHwConsoleConfig();

#if SYS_DEBUG_STARTUP
	printf("Setup Hw Console..."EXT_NEW_LINE);
#endif
	
#if (RESET_BTN_MODE == _RESET_BTN_STAY_IN_BOOTLOADER)
	bspButtonConfig(bMode, EXT_TRUE);
#elif (RESET_BTN_MODE == _RESET_BTN_RESTORE_FACTORY)
	/* when RESET is used as restoring factory */
	if(bMode == BOOT_MODE_RTOS)
	{/* OS: first only enable INPUT button */
		bspButtonConfig(BOOT_MODE_BOOTLOADER, EXT_TRUE);
	}
	else
	{/* bootloader: configure button in ISR: reset */
		bspButtonConfig(BOOT_MODE_RTOS, EXT_TRUE);
	}
		
#else
	#error RESET button mode is not defined.
#endif

	bspHwI2cInit();

	_bspHwSpiConfig();


#if SYS_DEBUG_STARTUP
	printf("Setup Hw EFC Flash..."EXT_NEW_LINE);
#endif
	if (efcFlashInit() != EXIT_SUCCESS)
	{
		printf("EFC Flash initialization error!");
//		return EXT_FALSE;
	}

	if(bMode == BOOT_MODE_BOOTLOADER)
	{
		efcFlashUpdateGpnvm();
	}

#if SYS_DEBUG_STARTUP
	printf("read configuration from EFC Flash..."EXT_NEW_LINE);
#endif
	if(bspCfgRead(runCfg, EXT_CFG_MAIN) == EXIT_FAILURE)
	{
		EXT_INFOF(("Use factory configuration"EXT_NEW_LINE));
		extCfgFromFactory(runCfg);
	}
	runCfg->isTx = isTx;

	extCfgInitAfterReadFromFlash(runCfg);

	if(bMode == BOOT_MODE_RTOS)
	{
		bspScInit(runCfg->sc);
	}

	if( !runCfg->isMacConfiged && bMode == BOOT_MODE_RTOS )
	{
		EXT_INFOF(("Random MAC"EXT_NEW_LINE));
		bspHwTrngConfig(EXT_TRUE, RNG_MODE_MAC_ADDRESS);
	}

#if EXT_DIP_SWITCH_ON
	if( EXT_IS_MULTICAST(runCfg) && EXT_IS_DIP_ON(runCfg))
	{
		runCfg->dest.ip = CFG_MAKEU32(bspMultiAddressFromDipSwitch(), MCAST_DEFAULT_IPADDR2, MCAST_DEFAULT_IPADDR1, MCAST_DEFAULT_IPADDR0 );
	}
	else
	{
		runCfg->dest.ip = runCfg->ipMulticast;
	}
#endif

	extTxMulticastIP2Mac(runCfg);

	/* First output of UART initialized must be platform info */
	if(bMode == BOOT_MODE_RTOS)
	{
#ifdef __EXT_RELEASE__
		/* debug version is disabled */
		bspConsoleReset(versionString);
#else
		printf(versionString);
		printf(" %s", (isTx)?"TX":"RX");
#endif
	}
	else
	{
		puts(EXT_NEW_LINE);
		puts(versionString);
	}

	EXT_INFOF(("CPU Freq:%u MHz; Peripheral Freq:%u MHz; System clock:%u MHz"EXT_NEW_LINE, (int)sysclk_get_main_hz()/1000/1000, (int)sysclk_get_peripheral_hz()/1000/1000, (int)SystemCoreClock/1000/1000 ));

	if(bMode == BOOT_MODE_RTOS)
	{
#if SYS_DEBUG_STARTUP
		printf("Setup Hw RS232..."EXT_NEW_LINE);
#endif
		extHwRs232Init(runCfg);
	}
//	extBspEfcFlashReadInfo();
	
}




/* Wait for FPGA to load firmware from SPI memory */
char  bspFpgaWaitDone(unsigned int  seconds)
{
//	char data;
	uint32_t	timeout;
	char  done = EXT_FALSE;
//	wdt_opt_t watchdog_opt;
	
//	watchdog_opt.us_timeout_period = 4000000;	// timeout after 2 second
	
	timeout = seconds;

	while (1)
	{
//		wdt_enable(&watchdog_opt);
		done = FPGA_GET_DONE_SIGNAL();
		if (done== EXT_FALSE)
		{
			printf("L");
		}
		else
		{
			printf("H");
			break;
		}
		
		if (timeout == 0)
		{
			printf("Timeout in waiting FPGA Done"EXT_NEW_LINE);
			break;
		}

		timeout--;
		EXT_DELAY_MS(1000);
	}
	
	
	return done;
}


/* Reload FPGA from SPI memory */
char  bspFpgaReload(void)
{
#define	_TIMEOUT_VALUE		20 	// 60
	char		isOK = EXT_FALSE;
	unsigned char data;
	int timeoutSecond = _TIMEOUT_VALUE;	/* when compare with negative number, it must be int, not char */

	bspHwClockInit();

	EXT_INFOF(("Reconfiguring FPGA....."EXT_NEW_LINE));
	gpio_set_pin_low(EXT_PIN_FPGA_PROGRAM);
		
	EXT_DELAY_MS(10);

	gpio_set_pin_high(EXT_PIN_FPGA_PROGRAM);
	
	EXT_DELAY_MS(50);

	/* wait for FPGA done pin
	* up to 20 second is the secondary image is loaded 
	*/
	isOK = bspFpgaWaitDone(20);	/* 120 */
	
	printf("FPGA load image %s"EXT_NEW_LINE, (isOK== EXT_FALSE)?"failed":"sucessed");

	printf("Waiting FPGA ....."EXT_NEW_LINE);
	do
	{
		isOK = FPGA_I2C_READ(EXT_FPGA_REG_ETHERNET_RESET, &data, 1);
		timeoutSecond--;
		
//		EXT_ERRORF(("Timeout %d seconds when waiting FPGA status10"EXT_NEW_LINE, timeoutSecond));
		if(timeoutSecond<0)
		{
//			EXT_ERRORF(("Timeout %d seconds when waiting FPGA status0"EXT_NEW_LINE, _TIMEOUT_VALUE));
			break;
		}
		EXT_DELAY_US(1000*100);
	}while(isOK!= EXIT_SUCCESS);

	if(isOK == EXIT_SUCCESS)
	{
		printf("FPGA Ethernet Reset register %d : %2x"EXT_NEW_LINE, EXT_FPGA_REG_ETHERNET_RESET, data);

		FPGA_I2C_READ(EXT_FPGA_REG_ETHERNET_RESET, &data, 1);
		data = data||0x01;
		FPGA_I2C_WRITE(EXT_FPGA_REG_ETHERNET_RESET, &data, 1);
	}
	else
	{
		EXT_ERRORF(("Timeout %d seconds when waiting FPGA status"EXT_NEW_LINE, _TIMEOUT_VALUE));
	}
	
	return isOK;
}


