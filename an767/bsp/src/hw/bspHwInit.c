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

/**
 *  Handler for Button 1 rising edge interrupt.
 *  Set button1 event flag (g_button_event).
 */
static void _buttonHandler(uint32_t id, uint32_t mask)
{
	if ((MUX_PUSH_BUTTON_ID == id) && (MUX_PUSH_BUTTON_PIN_MSK == mask) )
	{
//		g_button_event = 1;
		MUX_INFOF(("'%s' is pressed, now reboot..."MUX_NEW_LINE MUX_NEW_LINE, MUX_BUTTON_STRING));
		MUX_PRINTF((MUX_NEW_LINE)); /* reset the color to default before reboot */
		MUX_REBOOT();
	}
}


/**
 *  \brief Configure the push button.
 *
 *  Configure the PIOs as inputs and generate corresponding interrupt when
 *  pressed or released.
 */
void bspButtonConfig(boot_mode bMode)
{
	if(bMode == BOOT_MODE_RTOS)
	{
		/* Configure PIO clock. */
		pmc_enable_periph_clk(MUX_PUSH_BUTTON_ID );

		/* Adjust pio debounce filter parameters, uses 10 Hz filter. */
		pio_set_debounce_filter(MUX_PUSH_BUTTON_PIO, MUX_PUSH_BUTTON_PIN_MSK, 10);

		/* Initialize pios interrupt handlers, see PIO definition in board.h. */
		pio_handler_set(MUX_PUSH_BUTTON_PIO, MUX_PUSH_BUTTON_ID, MUX_PUSH_BUTTON_PIN_MSK, MUX_PUSH_BUTTON_ATTR, _buttonHandler);

		/* Enable PIO controller IRQs. */
		NVIC_EnableIRQ((IRQn_Type)MUX_PUSH_BUTTON_ID);

		/* Enable PIO line interrupts. */
		pio_enable_interrupt(MUX_PUSH_BUTTON_PIO, MUX_PUSH_BUTTON_PIN_MSK);
		gpio_configure_pin(MUX_PIN_SOFTWARE_RESET, IOPORT_DIR_INPUT );
	}
	else
	{/* bootloader */	
		gpio_configure_pin(PIO_PA30_IDX, PIO_TYPE_PIO_INPUT );
	}

	/*switch buttons */
#if 0	
	gpio_configure_pin(MUX_PIN_DIP_SW_01, IOPORT_DIR_INPUT );
	gpio_configure_pin(MUX_PIN_DIP_SW_02, IOPORT_DIR_INPUT );
	gpio_configure_pin(MUX_PIN_DIP_SW_03, IOPORT_DIR_INPUT );
	gpio_configure_pin(MUX_PIN_DIP_SW_04, IOPORT_DIR_INPUT );
#else
	gpio_configure_pin(MUX_PIN_DIP_SW_01, PIO_TYPE_PIO_INPUT );
	gpio_configure_pin(MUX_PIN_DIP_SW_02, PIO_TYPE_PIO_INPUT );
	gpio_configure_pin(MUX_PIN_DIP_SW_03, PIO_TYPE_PIO_INPUT );
	gpio_configure_pin(MUX_PIN_DIP_SW_04, PIO_TYPE_PIO_INPUT );
#endif
}


static void _muxBspPinsConfig(void)
{
	//WDT->WDT_MR = WDT_MR_WDDIS;         // disable watchdog

	/* PROGRAM_B: Active Low reset to configuration logic; so output pin with high levelOUTPUT + INIT_HIGH + Drive Low Configuration */
#if 0
	gpio_configure_pin(MUX_PIN_FPGA_PROGRAM, IOPORT_DIR_OUTPUT | IOPORT_PIN_LEVEL_HIGH );//| IOPORT_MODE_PULLDOWN);
#else
//	gpio_configure_pin(MUX_PIN_FPGA_PROGRAM, IOPORT_DIR_OUTPUT | IOPORT_PIN_LEVEL_HIGH | IOPORT_MODE_PULLUP);
	gpio_configure_pin(MUX_PIN_FPGA_PROGRAM,  PIO_OUTPUT_1 | PIO_DEFAULT);
#endif
//	gpio_configure_pin(MUX_PIN_FPGA_DONE,     IOPORT_DIR_INPUT );
	gpio_configure_pin(MUX_PIN_FPGA_DONE,     PIO_TYPE_PIO_INPUT );


#if 0
	gpio_configure_pin(MUX_PIN_PLL_INIT,	IOPORT_DIR_OUTPUT | IOPORT_PIN_LEVEL_LOW | IOPORT_MODE_PULLDOWN);
#else
	gpio_configure_pin(MUX_PIN_PLL_INIT,	 PIO_OUTPUT_0 | PIO_DEFAULT);
#endif

//	gpio_configure_pin(PIN_JUMPER_SELECT,     IOPORT_DIR_INPUT );
	gpio_configure_pin(MUX_PIN_POWER_1V_OK, IOPORT_DIR_INPUT );

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
#if MUXLAB_BOARD
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

	ioport_set_pin_peripheral_mode(MUX_SPI_MISO_GPIO,  MUX_SPI_MISO_FLAGS);
//	gpio_configure_pin(SPI0_MISO_GPIO,		 IOPORT_DIR_INPUT | IOPORT_PIN_LEVEL_HIGH | IOPORT_MODE_PULLUP);
	ioport_set_pin_peripheral_mode(MUX_SPI_MOSI_GPIO,  MUX_SPI_MOSI_FLAGS);
	ioport_set_pin_peripheral_mode(MUX_SPI_SPCK_GPIO,  MUX_SPI_SPCK_FLAGS);

#if MUXLAB_BOARD
#else
	ioport_set_pin_peripheral_mode(MUX_SPI_NPCS0_GPIO, MUX_SPI_NPCS0_FLAGS);
#endif

	ioport_set_pin_peripheral_mode(MUX_SPI_NPCS1_GPIO,  MUX_SPI_NPCS1_FLAGS);

	bspSpiMasterInitialize(MUX_SPI_CHIP_SEL);

	bspHwSpiFlashInit();
}

void bspHwInit(boot_mode bMode)
{
	MUX_RUNTIME_CFG *runCfg = &muxRun;
	runCfg->bootMode = bMode;
	
	sysclk_init();

	/* Disable the watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;
	/* after memory and watchdog, initialize IOPORTs */
	ioport_init();

	_muxBspPinsConfig();
	
	/* UART must follow PINs configuration, so put it here */
	bspHwConsoleConfig();

	
#if (RESET_BTN_MODE == _RESET_BTN_STAY_IN_BOOTLOADER)
	bspButtonConfig(bMode);
#elif (RESET_BTN_MODE == _RESET_BTN_RESTORE_FACTORY)
	/* when RESET is used as restoring factory */
	if(bMode == BOOT_MODE_RTOS)
	{/* OS: first only enable INPUT button */
		bspButtonConfig(BOOT_MODE_BOOTLOADER);
	}
	else
	{/* bootloader: configure button in ISR: reset */
		bspButtonConfig(BOOT_MODE_RTOS);
	}
		
#else
	#error RESET button mode is not defined.
#endif

	bspHwI2cInit();

	_bspHwSpiConfig();


	if (efcFlashInit() != EXIT_SUCCESS)
	{
		printf("EFC Flash initialization error!");
//		return MUX_FALSE;
	}

	memset(runCfg, 0, sizeof(MUX_RUNTIME_CFG));
	if(bspCfgRead(runCfg, MUX_CFG_MAIN) == EXIT_FAILURE)
	{
		MUX_INFOF(("Use factory configuration"MUX_NEW_LINE));
		muxCfgFromFactory(runCfg);
	}

	muxCfgInitAfterReadFromFlash(runCfg);

	if(runCfg->isMCast)
	{
		runCfg->dest.ip = CFG_MAKEU32(bspMultiAddressFromDipSwitch(), MCAST_DEFAULT_IPADDR2, MCAST_DEFAULT_IPADDR1, MCAST_DEFAULT_IPADDR0 );
		muxTxMulticastIP2Mac(runCfg);
	}

	/* First output of UART initialized must be platform info */
	if(bMode == BOOT_MODE_RTOS)
	{
#ifdef __MUX_RELEASE__
		/* debug version is disabled */
		bspConsoleReset(versionString);
#endif
	}
	else
	{
		puts(MUX_NEW_LINE);
		puts(versionString);
	}

	MUX_INFOF(("CPU Freq:%u MHz; Peripheral Freq:%u MHz; System clock:%u MHz"MUX_NEW_LINE, (int)sysclk_get_main_hz()/1000/1000, (int)sysclk_get_peripheral_hz()/1000/1000, (int)SystemCoreClock/1000/1000 ));

	muxHwRs232Init(runCfg);
//	muxBspEfcFlashReadInfo();
	
//	muxFpgaInit();
	
}


