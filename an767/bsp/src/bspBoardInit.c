

#include "compact.h"

#ifdef CONF_BOARD_CONFIG_MPU_AT_INIT
#include "mpu.h"

/**
 *	Default memory map
 *	Address range        Memory region      Memory type   Shareability  Cache policy
 *	0x00000000- 0x1FFFFFFF Code             Normal        Non-shareable  WT
 *	0x20000000- 0x3FFFFFFF SRAM             Normal        Non-shareable  WBWA
 *	0x40000000- 0x5FFFFFFF Peripheral       Device        Non-shareable  -
 *	0x60000000- 0x7FFFFFFF RAM              Normal        Non-shareable  WBWA
 *	0x80000000- 0x9FFFFFFF RAM              Normal        Non-shareable  WT
 *	0xA0000000- 0xBFFFFFFF Device           Device        Shareable
 *	0xC0000000- 0xDFFFFFFF Device           Device        Non Shareable
 *	0xE0000000- 0xFFFFFFFF System           -                  -
 */

/**
 * \brief Set up a memory region.
 */
static void _setup_memory_region( void )
{

	uint32_t dw_region_base_addr;
	uint32_t dw_region_attr;

	__DMB();

/**
 *	ITCM memory region --- Normal
 *	START_Addr:-  0x00000000UL
 *	END_Addr:-    0x00400000UL
 */
	dw_region_base_addr = ITCM_START_ADDRESS | MPU_REGION_VALID | MPU_DEFAULT_ITCM_REGION;

	dw_region_attr = MPU_AP_PRIVILEGED_READ_WRITE | mpu_cal_mpu_region_size(ITCM_END_ADDRESS - ITCM_START_ADDRESS) | MPU_REGION_ENABLE;

	mpu_set_region( dw_region_base_addr, dw_region_attr);

/**
 *	Internal flash memory region --- Normal read-only
 *	(update to Strongly ordered in write accesses)
 *	START_Addr:-  0x00400000UL
 *	END_Addr:-    0x00600000UL
 */

	dw_region_base_addr =	IFLASH_START_ADDRESS |MPU_REGION_VALID |MPU_DEFAULT_IFLASH_REGION;

	dw_region_attr = MPU_AP_READONLY |INNER_NORMAL_WB_NWA_TYPE( NON_SHAREABLE ) |mpu_cal_mpu_region_size(IFLASH_END_ADDRESS - IFLASH_START_ADDRESS) |MPU_REGION_ENABLE;

	mpu_set_region( dw_region_base_addr, dw_region_attr);

/**
 *	DTCM memory region --- Normal
 *	START_Addr:-  0x20000000L
 *	END_Addr:-    0x20400000UL
 */

	/* DTCM memory region */
	dw_region_base_addr =
		DTCM_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_DEFAULT_DTCM_REGION;

	dw_region_attr =
		MPU_AP_PRIVILEGED_READ_WRITE |
		mpu_cal_mpu_region_size(DTCM_END_ADDRESS - DTCM_START_ADDRESS) |
		MPU_REGION_ENABLE;

	mpu_set_region( dw_region_base_addr, dw_region_attr);

/**
 *	SRAM Cacheable memory region --- Normal
 *	START_Addr:-  0x20400000UL
 *	END_Addr:-    0x2043FFFFUL
 */
	/* SRAM memory  region */
	dw_region_base_addr =
		SRAM_FIRST_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_DEFAULT_SRAM_REGION_1;

	dw_region_attr =
		MPU_AP_FULL_ACCESS    |
		INNER_NORMAL_WB_NWA_TYPE( NON_SHAREABLE ) |
		mpu_cal_mpu_region_size(SRAM_FIRST_END_ADDRESS - SRAM_FIRST_START_ADDRESS)
		| MPU_REGION_ENABLE;

	mpu_set_region( dw_region_base_addr, dw_region_attr);


/**
 *	Internal SRAM second partition memory region --- Normal
 *	START_Addr:-  0x20440000UL
 *	END_Addr:-    0x2045FFFFUL
 */
	/* SRAM memory region */
	dw_region_base_addr =
		SRAM_SECOND_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_DEFAULT_SRAM_REGION_2;

	dw_region_attr =
		MPU_AP_FULL_ACCESS    |
		INNER_NORMAL_WB_NWA_TYPE( NON_SHAREABLE ) |
		mpu_cal_mpu_region_size(SRAM_SECOND_END_ADDRESS - SRAM_SECOND_START_ADDRESS) |
		MPU_REGION_ENABLE;

	mpu_set_region( dw_region_base_addr, dw_region_attr);

#ifdef MPU_HAS_NOCACHE_REGION
	dw_region_base_addr =
        SRAM_NOCACHE_START_ADDRESS |
        MPU_REGION_VALID |
        MPU_NOCACHE_SRAM_REGION;

    dw_region_attr =
        MPU_AP_FULL_ACCESS    |
        INNER_OUTER_NORMAL_NOCACHE_TYPE( SHAREABLE ) |
        mpu_cal_mpu_region_size(NOCACHE_SRAM_REGION_SIZE) |
        MPU_REGION_ENABLE;

    mpu_set_region( dw_region_base_addr, dw_region_attr);
#endif

/**
 *	Peripheral memory region --- DEVICE Shareable
 *	START_Addr:-  0x40000000UL
 *	END_Addr:-    0x5FFFFFFFUL
 */
	dw_region_base_addr =
		PERIPHERALS_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_PERIPHERALS_REGION;

	dw_region_attr = MPU_AP_FULL_ACCESS |
		MPU_REGION_EXECUTE_NEVER |
		SHAREABLE_DEVICE_TYPE |
		mpu_cal_mpu_region_size(PERIPHERALS_END_ADDRESS - PERIPHERALS_START_ADDRESS)
		|MPU_REGION_ENABLE;

	mpu_set_region( dw_region_base_addr, dw_region_attr);


/**
 *	External EBI memory  memory region --- Strongly Ordered
 *	START_Addr:-  0x60000000UL
 *	END_Addr:-    0x6FFFFFFFUL
 */
	dw_region_base_addr =
		EXT_EBI_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_EXT_EBI_REGION;

	dw_region_attr =
		MPU_AP_FULL_ACCESS |
		/* External memory Must be defined with 'Device' or 'Strongly Ordered' attribute for write accesses (AXI) */
		STRONGLY_ORDERED_SHAREABLE_TYPE |
		mpu_cal_mpu_region_size(EXT_EBI_END_ADDRESS - EXT_EBI_START_ADDRESS) |
		MPU_REGION_ENABLE;

	mpu_set_region( dw_region_base_addr, dw_region_attr);

/**
 *	SDRAM cacheable memory region --- Normal
 *	START_Addr:-  0x70000000UL
 *	END_Addr:-    0x7FFFFFFFUL
 */
	dw_region_base_addr =
		SDRAM_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_DEFAULT_SDRAM_REGION;

	dw_region_attr =
		MPU_AP_FULL_ACCESS    |
		INNER_NORMAL_WB_RWA_TYPE( SHAREABLE ) |
		mpu_cal_mpu_region_size(SDRAM_END_ADDRESS - SDRAM_START_ADDRESS) |
		MPU_REGION_ENABLE;

	mpu_set_region( dw_region_base_addr, dw_region_attr);

/**
 *	QSPI memory region --- Strongly ordered
 *	START_Addr:-  0x80000000UL
 *	END_Addr:-    0x9FFFFFFFUL
 */
	dw_region_base_addr =
		QSPI_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_QSPIMEM_REGION;

	dw_region_attr =
		MPU_AP_FULL_ACCESS |
		STRONGLY_ORDERED_SHAREABLE_TYPE |
		mpu_cal_mpu_region_size(QSPI_END_ADDRESS - QSPI_START_ADDRESS) |
		MPU_REGION_ENABLE;

	mpu_set_region( dw_region_base_addr, dw_region_attr);


/**
 *	USB RAM Memory region --- Device
 *	START_Addr:-  0xA0100000UL
 *	END_Addr:-    0xA01FFFFFUL
 */
	dw_region_base_addr =
		USBHSRAM_START_ADDRESS |
		MPU_REGION_VALID |
		MPU_USBHSRAM_REGION;

	dw_region_attr =
		MPU_AP_FULL_ACCESS |
		MPU_REGION_EXECUTE_NEVER |
		SHAREABLE_DEVICE_TYPE |
		mpu_cal_mpu_region_size(USBHSRAM_END_ADDRESS - USBHSRAM_START_ADDRESS) |
		MPU_REGION_ENABLE;

	mpu_set_region( dw_region_base_addr, dw_region_attr);


	/* Enable the memory management fault , Bus Fault, Usage Fault exception */
	SCB->SHCSR |= (SCB_SHCSR_MEMFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk
					| SCB_SHCSR_USGFAULTENA_Msk);

	/* Enable the MPU region */
	mpu_enable( MPU_ENABLE | MPU_PRIVDEFENA);

	__DSB();
	__ISB();
}
#endif

#ifdef CONF_BOARD_ENABLE_TCM_AT_INIT
#if defined(__GNUC__)
extern char _itcm_lma, _sitcm, _eitcm;
#endif

/** \brief  TCM memory enable
* The function enables TCM memories
*/
static inline void tcm_enable(void)
{

	__DSB();
	__ISB();
	
	SCB->ITCMCR = (SCB_ITCMCR_EN_Msk  | SCB_ITCMCR_RMW_Msk | SCB_ITCMCR_RETEN_Msk);
	SCB->DTCMCR = ( SCB_DTCMCR_EN_Msk | SCB_DTCMCR_RMW_Msk | SCB_DTCMCR_RETEN_Msk);
	
	__DSB();
	__ISB();
}
#else
/** \brief  TCM memory Disable

	The function enables TCM memories
 */
static inline void tcm_disable(void) 
{

	__DSB();
	__ISB();
	SCB->ITCMCR &= ~(uint32_t)(1UL);
	SCB->DTCMCR &= ~(uint32_t)SCB_DTCMCR_EN_Msk;
	__DSB();
	__ISB();
}
#endif


void board_init(void)
{
#ifndef CONF_BOARD_KEEP_WATCHDOG_AT_INIT
	/* Disable the watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;
#endif

#ifdef CONF_BOARD_CONFIG_MPU_AT_INIT
	_setup_memory_region();
#endif

#ifdef CONF_BOARD_ENABLE_CACHE
	/* Enabling the Cache */
	SCB_EnableICache(); 
	SCB_EnableDCache();
#endif

#ifdef CONF_BOARD_ENABLE_TCM_AT_INIT
	/* TCM Configuration */
	EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_CGPB 
					| EEFC_FCR_FARG(8));
	EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_SGPB
					| EEFC_FCR_FARG(7));
	tcm_enable();
#if defined(__GNUC__)
	volatile char *dst = &_sitcm;
	volatile char *src = &_itcm_lma;
	/* copy code_TCM from flash to ITCM */
	while(dst < &_eitcm){
		*dst++ = *src++;
	}
#endif
#else
	/* TCM Configuration */
	EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_CGPB 
					| EEFC_FCR_FARG(8));
	EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_CGPB 
					| EEFC_FCR_FARG(7));
	
	tcm_disable();
#endif

	/* Initialize IOPORTs */
	ioport_init();

	/* Configure the pins connected to LED as output and set their default initial state to high (LED off).
	 */
	ioport_set_pin_dir(LED0_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED0_GPIO, LED0_INACTIVE_LEVEL);

	/* Configure Push Button pins */
	ioport_set_pin_input_mode(GPIO_PUSH_BUTTON_1, GPIO_PUSH_BUTTON_1_FLAGS, GPIO_PUSH_BUTTON_1_SENSE);




#ifdef CONF_BOARD_CAN0
	/* Configure the CAN0 TX and RX pins. */
	ioport_set_pin_peripheral_mode(PIN_CAN0_RX_IDX, PIN_CAN0_RX_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_CAN0_TX_IDX, PIN_CAN0_TX_FLAGS);
	/* Configure the transiver0 RS & EN pins. */
	ioport_set_pin_dir(PIN_CAN0_TR_RS_IDX, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_CAN0_TR_EN_IDX, IOPORT_DIR_OUTPUT);
#endif

#ifdef CONF_BOARD_CAN1
	/* Configure the CAN1 TX and RX pin. */
	ioport_set_pin_peripheral_mode(PIN_CAN1_RX_IDX, PIN_CAN1_RX_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_CAN1_TX_IDX, PIN_CAN1_TX_FLAGS);

#endif



#ifdef CONF_BOARD_QSPI
	ioport_set_pin_peripheral_mode(QSPI_QSCK_GPIO, QSPI_QSCK_FLAGS);
	ioport_set_pin_peripheral_mode(QSPI_QCS_GPIO, QSPI_QCS_FLAGS);
	ioport_set_pin_peripheral_mode(QSPI_QIO0_GPIO, QSPI_QIO0_FLAGS);
	ioport_set_pin_peripheral_mode(QSPI_QIO1_GPIO, QSPI_QIO1_FLAGS);
	ioport_set_pin_peripheral_mode(QSPI_QIO2_GPIO, QSPI_QIO2_FLAGS);
	ioport_set_pin_peripheral_mode(QSPI_QIO3_GPIO, QSPI_QIO3_FLAGS);
#endif

#ifdef CONF_BOARD_PWM_LED0
	/* Configure PWM LED0 pin */
	ioport_set_pin_peripheral_mode(PIN_PWM_LED0_GPIO, PIN_PWM_LED0_FLAGS);
#endif

#ifdef CONF_BOARD_PWM_LED1
	/* Configure PWM LED1 pin */
	ioport_set_pin_peripheral_mode(PIN_PWM_LED1_GPIO, PIN_PWM_LED1_FLAGS);
#endif


#ifdef CONF_BOARD_USART_RXD
	/* Configure USART RXD pin */
	ioport_set_pin_peripheral_mode(USART0_RXD_GPIO, USART0_RXD_FLAGS);
#endif

#ifdef CONF_BOARD_USART_TXD
	/* Configure USART TXD pin */
	ioport_set_pin_peripheral_mode(USART0_TXD_GPIO, USART0_TXD_FLAGS);
#endif

#ifdef CONF_BOARD_USART_SCK
	/* Configure USART synchronous communication SCK pin */
	ioport_set_pin_peripheral_mode(PIN_USART0_SCK_IDX,PIN_USART0_SCK_FLAGS);
#endif

#ifdef CONF_BOARD_USART_CTS
	/* Configure USART synchronous communication CTS pin */
	ioport_set_pin_peripheral_mode(PIN_USART0_CTS_IDX,PIN_USART0_CTS_FLAGS);
#endif

#ifdef CONF_BOARD_USART_RTS
	/* Configure USART RTS pin */
	ioport_set_pin_peripheral_mode(PIN_USART0_RTS_IDX, PIN_USART0_RTS_FLAGS);
#endif

#ifdef CONF_BOARD_SD_MMC_HSMCI
	/* Configure HSMCI pins */
	ioport_set_pin_peripheral_mode(PIN_HSMCI_MCCDA_GPIO, PIN_HSMCI_MCCDA_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_HSMCI_MCCK_GPIO, PIN_HSMCI_MCCK_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_HSMCI_MCDA0_GPIO, PIN_HSMCI_MCDA0_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_HSMCI_MCDA1_GPIO, PIN_HSMCI_MCDA1_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_HSMCI_MCDA2_GPIO, PIN_HSMCI_MCDA2_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_HSMCI_MCDA3_GPIO, PIN_HSMCI_MCDA3_FLAGS);
    /* Configure SD/MMC card detect pin */
	ioport_set_pin_dir(SD_MMC_0_CD_GPIO, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(SD_MMC_0_CD_GPIO, SD_MMC_0_CD_FLAGS);
#endif

#ifdef CONF_BOARD_ILI9488
	/**LCD pin configure on EBI*/
	pio_configure(PIN_EBI_RESET_PIO, PIN_EBI_RESET_TYPE, PIN_EBI_RESET_MASK, PIN_EBI_RESET_ATTRI);
	pio_configure(PIN_EBI_CDS_PIO, PIN_EBI_CDS_TYPE, PIN_EBI_CDS_MASK, PIN_EBI_CDS_ATTRI);
	pio_configure(PIN_EBI_DATAL_PIO, PIN_EBI_DATAL_TYPE, PIN_EBI_DATAL_MASK, PIN_EBI_DATAL_ATTRI);
	pio_configure(PIN_EBI_DATAH_0_PIO, PIN_EBI_DATAH_0_TYPE, PIN_EBI_DATAH_0_MASK, PIN_EBI_DATAH_0_ATTRI);
	pio_configure(PIN_EBI_DATAH_1_PIO, PIN_EBI_DATAH_1_TYPE, PIN_EBI_DATAH_1_MASK, PIN_EBI_DATAH_1_ATTRI);
	pio_configure(PIN_EBI_NWE_PIO, PIN_EBI_NWE_TYPE, PIN_EBI_NWE_MASK, PIN_EBI_NWE_ATTRI);
	pio_configure(PIN_EBI_NRD_PIO, PIN_EBI_NRD_TYPE, PIN_EBI_NRD_MASK, PIN_EBI_NRD_ATTRI);
	pio_configure(PIN_EBI_CS_PIO, PIN_EBI_CS_TYPE, PIN_EBI_CS_MASK, PIN_EBI_CS_ATTRI);
	pio_configure(PIN_EBI_BACKLIGHT_PIO, PIN_EBI_BACKLIGHT_TYPE, PIN_EBI_BACKLIGHT_MASK, PIN_EBI_BACKLIGHT_ATTRI);
	pio_set(PIN_EBI_BACKLIGHT_PIO, PIN_EBI_BACKLIGHT_MASK);
#endif

#if (defined CONF_BOARD_USB_PORT)
# if defined(CONF_BOARD_USB_VBUS_DETECT)
	ioport_set_pin_dir(USB_VBUS_PIN, IOPORT_DIR_INPUT);
# endif
# if defined(CONF_BOARD_USB_ID_DETECT)
	ioport_set_pin_dir(USB_ID_PIN, IOPORT_DIR_INPUT);
# endif
#endif


#ifdef CONF_BOARD_ILI9488
	/**LCD pin configure on SPI*/
	pio_configure_pin(LCD_SPI_MISO_PIO, LCD_SPI_MISO_FLAGS);
	pio_configure_pin(LCD_SPI_MOSI_PIO, LCD_SPI_MOSI_FLAGS);
	pio_configure_pin(LCD_SPI_SPCK_PIO, LCD_SPI_SPCK_FLAGS);
	pio_configure_pin(LCD_SPI_NPCS_PIO, LCD_SPI_NPCS_FLAGS);
	pio_configure_pin(LCD_SPI_RESET_PIO, LCD_SPI_RESET_FLAGS);
	pio_configure_pin(LCD_SPI_CDS_PIO, LCD_SPI_CDS_FLAGS);
	pio_configure_pin(LCD_SPI_BACKLIGHT_PIO, LCD_SPI_BACKLIGHT_FLAGS);
	pio_set_pin_high(LCD_SPI_BACKLIGHT_PIO);

#endif
}


/**
 * \brief Setup the microcontroller system.
 * Initialize the System and update the SystemFrequency variable.
 */
 void SystemInit( void )
{
	/* Set FWS according to SYS_BOARD_MCKR configuration */
	EFC->EEFC_FMR = EEFC_FMR_FWS(5);

	/* Initialize main oscillator */
	if ( !(PMC->CKGR_MOR & CKGR_MOR_MOSCSEL) )
	{
		PMC->CKGR_MOR = CKGR_MOR_KEY_PASSWD | SYS_BOARD_OSCOUNT | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN;

		while ( !(PMC->PMC_SR & PMC_SR_MOSCXTS) )
		{
		}
	}

	/* Switch to 3-20MHz Xtal oscillator */
	PMC->CKGR_MOR = CKGR_MOR_KEY_PASSWD | SYS_BOARD_OSCOUNT | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN | CKGR_MOR_MOSCSEL;

	while ( !(PMC->PMC_SR & PMC_SR_MOSCSELS) )
	{
	}

	PMC->PMC_MCKR = (PMC->PMC_MCKR & ~(uint32_t)PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS_MAIN_CLK;

	while ( !(PMC->PMC_SR & PMC_SR_MCKRDY) )
	{
	}

	/* Initialize PLLA */
	PMC->CKGR_PLLAR = SYS_BOARD_PLLAR;
	while ( !(PMC->PMC_SR & PMC_SR_LOCKA) )
	{
	}

	/* Switch to main clock */
	PMC->PMC_MCKR = (SYS_BOARD_MCKR & ~PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS_MAIN_CLK;
	while ( !(PMC->PMC_SR & PMC_SR_MCKRDY) )
	{
	}

	/* Switch to PLLA */
	PMC->PMC_MCKR = SYS_BOARD_MCKR;
	while ( !(PMC->PMC_SR & PMC_SR_MCKRDY) )
	{
	}

	SystemCoreClock = CHIP_FREQ_CPU_MAX;
}

