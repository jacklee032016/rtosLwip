
/*
* hardware definitions for MUX AN767 
*/

#ifndef	__HARDWARE_H__
#define	__HARDWARE_H__


// ===== System Clock (MCK) Source Options
//#define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_SLCK_RC
//#define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_SLCK_XTAL
//#define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_SLCK_BYPASS
//#define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_MAINCK_4M_RC
//#define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_MAINCK_8M_RC
//#define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_MAINCK_12M_RC
//#define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_MAINCK_XTAL
//#define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_MAINCK_BYPASS
#define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_PLLACK
//#define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_UPLLCK

// ===== Processor Clock (HCLK) Prescaler Options   (Fhclk = Fsys / (SYSCLK_PRES))
#define CONFIG_SYSCLK_PRES          SYSCLK_PRES_1
//#define CONFIG_SYSCLK_PRES          SYSCLK_PRES_2
//#define CONFIG_SYSCLK_PRES          SYSCLK_PRES_4
//#define CONFIG_SYSCLK_PRES          SYSCLK_PRES_8
//#define CONFIG_SYSCLK_PRES          SYSCLK_PRES_16
//#define CONFIG_SYSCLK_PRES          SYSCLK_PRES_32
//#define CONFIG_SYSCLK_PRES          SYSCLK_PRES_64
//#define CONFIG_SYSCLK_PRES          SYSCLK_PRES_3

// ===== System Clock (MCK) Division Options     (Fmck = Fhclk / (SYSCLK_DIV))
#define CONFIG_SYSCLK_DIV            2

// ===== PLL0 (A) Options   (Fpll = (Fclk * PLL_mul) / PLL_div)
// Use mul and div effective values here.
#define CONFIG_PLL0_SOURCE          PLL_SRC_MAINCK_XTAL
#define CONFIG_PLL0_MUL             25
#define CONFIG_PLL0_DIV             1

// ===== UPLL (UTMI) Hardware fixed at 480 MHz.

// ===== USB Clock Source Options   (Fusb = FpllX / USB_div)
// Use div effective value here.
//#define CONFIG_USBCLK_SOURCE        USBCLK_SRC_PLL0
#define CONFIG_USBCLK_SOURCE        USBCLK_SRC_UPLL
#define CONFIG_USBCLK_DIV           1

// ===== Target frequency (Processor clock)
// - XTAL frequency: 12MHz
// - System clock source: PLLA
// - System clock prescaler: 1 (divided by 1)
// - System clock divider: 2 (divided by 2)
// - PLLA source: XTAL
// - PLLA output: XTAL * 25 / 1
// - Processor clock: 12 * 25 / 1 / 1 = 300MHz
// - System clock: 300 / 2 = 150MHz
// ===== Target frequency (USB Clock)
// - USB clock source: UPLL
// - USB clock divider: 1 (not divided)
// - UPLL frequency: 480MHz
// - USB clock: 480 / 1 = 480MHz



/* memory mapping of internal flash */
#if 0
#define	AN767_MCU_MMAP_OS			0x00410000	/* 64K */
#else
/* Atmel Studio Programming tool erase in 128KB */
#define	AN767_MCU_MMAP_OS			0x00420000	/* 128K */
#endif
#define	AN767_MCU_MMAP_CONFIG		0x004FC000	/* last 16K, configuration */
#define	AN767_MCU_MMAP_BACKUP		0x004FE000	/* last 8K, backup of configuration */

#define	FLASH_START_PAGE_OS			((AN767_MCU_MMAP_OS-IFLASH_ADDR)/IFLASH_PAGE_SIZE)		/* No. 256 page when 0x420000, No.128 page when 0x410000  */
#define	FLASH_START_PAGE_CONFIG		((AN767_MCU_MMAP_CONFIG-IFLASH_ADDR)/IFLASH_PAGE_SIZE)	/* No. 2016 page */
#define	FLASH_START_PAGE_BACKUP		((AN767_MCU_MMAP_BACKUP-IFLASH_ADDR)/IFLASH_PAGE_SIZE)	/* No. 2032 page */

/* flash page no. for system configuration*/
#define FLASH_START_PAGE_CONFIGURATION		(IFLASH_NB_OF_PAGES - IFLASH_LOCK_REGION_SIZE/IFLASH_PAGE_SIZE*2 )

/* flash page no. for backup of system configuration*/
#define FLASH_START_PAGE_CONFIG_BACKUP		(IFLASH_NB_OF_PAGES - IFLASH_LOCK_REGION_SIZE/IFLASH_PAGE_SIZE )


#define	FLASH_ERASE_COUNT			(IFLASH_LOCK_REGION_SIZE/IFLASH_PAGE_SIZE)

/***** button and switch pins ****/
#define	EXT_BUTTON_STRING            			"SW2"

/* pins */
#if 0
/* when with ISR to monitor these pins */
#define	EXT_PIN_DIP_SW_01				PIO_PA26
#define	EXT_PIN_DIP_SW_02				PIO_PA27
#define	EXT_PIN_DIP_SW_03				PIO_PA28
#define	EXT_PIN_DIP_SW_04				PIO_PA29
#else
/* when only read status of these pins */
#define	EXT_PIN_DIP_SW_01				PIO_PA26_IDX
#define	EXT_PIN_DIP_SW_02				PIO_PA27_IDX
#define	EXT_PIN_DIP_SW_03				PIO_PA28_IDX
#define	EXT_PIN_DIP_SW_04				PIO_PA29_IDX
#endif
#define	EXT_PIN_SOFTWARE_RESET		PIO_PA30


#define	EXT_DIP_SW_STATUS(dip)		\
			gpio_pin_is_low((dip))
			
#define	EXT_DIP_STATUS_SW1()	\
			EXT_DIP_SW_STATUS(EXT_PIN_DIP_SW_01)

#define	EXT_DIP_STATUS_SW2()	\
			EXT_DIP_SW_STATUS(EXT_PIN_DIP_SW_02)

#define	EXT_DIP_STATUS_SW3()	\
			EXT_DIP_SW_STATUS(EXT_PIN_DIP_SW_03)

#define	EXT_DIP_STATUS_SW4()	\
			EXT_DIP_SW_STATUS(EXT_PIN_DIP_SW_04)


/** Push button pin definition */
#define	EXT_PUSH_BUTTON_PIO          			PIOA
#define	EXT_PUSH_BUTTON_ID           			ID_PIOA
#define	EXT_PUSH_BUTTON_PIN_MSK				(EXT_PIN_SOFTWARE_RESET)
//#define	EXT_PUSH_BUTTON_ATTR         			(PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE)
#define	EXT_PUSH_BUTTON_ATTR         			(PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_HIGH_LEVEL)

#define	EXT_PUSH_BUTTON_ATTR_RISE_EDGE		(PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE)

#define	EXT_PUSH_BUTTON_ATTR_FALL_EDGE		(PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_FALL_EDGE)

/*
* following pins are wrong. J.L. April 11, 2018
#define	EXT_PIN_FPGA_PROGRAM			PIO_PA0A_PWMC0_PWMH0
#define	EXT_PIN_FPGA_DONE				PIO_PA2A_PWMC0_PWMH1
#define	EXT_PIN_PLL_INIT					PIO_PA1A_PWMC0_PWML0	
*/

#define	EXT_PIN_FPGA_PROGRAM		PIO_PA0_IDX
#define	EXT_PIN_FPGA_DONE			PIO_PA2_IDX
#define	EXT_PIN_PLL_INIT				PIO_PA1_IDX		/* INIT pin of LMH1983 */

#define	PIN_JUMPER_SELECT				AVR32_PIN_PA06

#define	EXT_PIN_POWER_1V_OK			PIO_PA7


/********** uart/usart and console *********************/
/** Enable Com Port. */
#define CONF_BOARD_UART_CONSOLE

/** Baudrate setting */
#define	CONF_UART_BAUDRATE			(115200UL)
/** Character length setting */
#define	CONF_UART_CHAR_LENGTH		US_MR_CHRL_8_BIT
/** Parity setting */
#define	CONF_UART_PARITY				US_MR_PAR_NO
/** Stop bits setting */
#define	CONF_UART_STOP_BITS			US_MR_NBSTOP_1_BIT


/******** I2C **************/

/** TWI0 pins definition */
#define	TWIHS0_DATA_GPIO				PIO_PA3_IDX
#define	TWIHS0_DATA_FLAGS				(IOPORT_MODE_MUX_A)
#define	TWIHS0_CLK_GPIO				PIO_PA4_IDX
#define	TWIHS0_CLK_FLAGS				(IOPORT_MODE_MUX_A)


#define	BOARD_TWIHS_CLK				(400000UL)

/* following devices attached on I2C of MCU directly */
#define	EXT_I2C_PCA9554_ADDRESS				(0xE0 >> 1)

#define	EXT_I2C_ADDRESS_LM1983				(0xCA >> 1)		/* ADDR pin is Tie low */
#define	EXT_I2C_ADDRESS_LM1983_FLOAT		(0xCE >> 1)
#define	EXT_I2C_ADDRESS_FPGA					(0x60 >> 1)
#define	EXT_I2C_ADDRESS_FPGA_B				(0x62 >> 1)

/* folllowing devices attached on I2C of LM1983 */
#define	EXT_I2C_ADDRESS_SENSOR             		(0x30 >> 1)
#define	EXT_I2C_ADDRESS_AT24MAC             		(0xAE >> 1)
#define	EXT_I2C_ADDRESS_XC7A75T             		(0xFF >> 1)	/* T.B.D. */
#define	EXT_I2C_ADDRESS_RTL8035				0x54


/* address of EEROM */
#define	EEPROM_AT24MAC402_ADDRESS			(0xBE >> 1)	/* Evaludation board AT24MAC, 4Kb */
#define	EEPROM_AT24C04BN_ADDRESS			(0xAC >> 1)	/* 4Kb, on channel-1, shared with Sensor */
#define	EEPROM_24AA02E48T_ADDRESS			(0xA0 >> 1)	/* 2Kb, on channel-0 */

#define	EEPROM_AT24MAC402_SIZE				(128)	/* bytes, 1Kb  not include the second half */
#define	EEPROM_AT24C04BN_SIZE				(512)	/* 4Kb,  */
#define	EEPROM_24AA02E48T_SIZE				(256)	/* 2Kb */

#define	EEPROM_AT24MAC402_PAGE_SIZE			(16)		/* 16 page of 16 byte for whole 2kbit */
#define	EEPROM_AT24C04BN_PAGE_SIZE			(16)		/* 16byte/page, total 32 pages */
#define	EEPROM_24AA02E48T_PAGE_SIZE			(8)		/* 8bytes/page, total 32 pages */


#define	EXT_I2C_PCA9554_CS_MAC				(4)	/* CS0, MAC address; not used */
#define	EXT_I2C_PCA9554_CS_SENSOR			(5)	/* CS1, sensor and EEPROM  */
#define	EXT_I2C_PCA9554_CS_FPGA				(6)	/* CS2, FPGA */
#define	EXT_I2C_PCA9554_CS_BOOTROM			(7)	/* CS3, Ethernet switch */

#define	EXT_I2C_PCA9554_CS_NONE				(0xFF) /* connect to I2C of MCU directly */

#define	EXT_EEPROM_4K							1

#if EXTLAB_BOARD
#if EXT_EEPROM_4K
#define	EXT_I2C_EEPROM_ADDRESS				EEPROM_AT24C04BN_ADDRESS	/*default, 4K, channel-1*/
#define	EXT_I2C_EEPROM_CHANNEL				EXT_I2C_PCA9554_CS_SENSOR	/* channel-1 */
#define	EXT_I2C_EEPROM_SIZE					EEPROM_AT24C04BN_SIZE
#define	EXT_I2C_EEPROM_PAGE_SIZE				EEPROM_AT24C04BN_PAGE_SIZE
#else
#define	EXT_I2C_EEPROM_ADDRESS				EEPROM_24AA02E48T_ADDRESS
#define	EXT_I2C_EEPROM_CHANNEL				EXT_I2C_PCA9554_CS_MAC	/* channel-0 */
#define	EXT_I2C_EEPROM_SIZE					EEPROM_24AA02E48T_SIZE
#define	EXT_I2C_EEPROM_PAGE_SIZE				EEPROM_24AA02E48T_PAGE_SIZE
#endif
#else
#define	EXT_I2C_EEPROM_ADDRESS				EEPROM_AT24MAC402_ADDRESS
#define	EXT_I2C_EEPROM_CHANNEL				EXT_I2C_PCA9554_CS_SENSOR	/* not used in XPLD board */
#define	EXT_I2C_EEPROM_SIZE					EEPROM_AT24MAC402_SIZE
#define	EXT_I2C_EEPROM_PAGE_SIZE			EEPROM_AT24MAC402_PAGE_SIZE
#endif

#define	EXT_I2C_EEPROM_PAGE_NUMBER			(EXT_I2C_EEPROM_SIZE/EXT_I2C_EEPROM_PAGE_SIZE)


/* registers for LM95245 sensor */
#define	EXT_I2C_SENSOR_LOCAL_TEMP_MSB		0x00
#define	EXT_I2C_SENSOR_LOCAL_TEMP_LSB		0x30
#define	EXT_I2C_SENSOR_MANUFACTURE_ID		0xFE
#define	EXT_I2C_SENSOR_REVISION_ID			0xFF


typedef	enum
{
	EXT_EEPROM_TYPE_4K = 0,
	EXT_EEPROM_TYPE_2K,
}EXT_EEPROM_TYPE;


/********************* SPI ******************/
/** SPI0 pins definition */
#define	EXT_SPI_MISO_GPIO				PIO_PD20_IDX
#define	EXT_SPI_MISO_FLAGS			(IOPORT_MODE_MUX_B)
#define	EXT_SPI_MOSI_GPIO				PIO_PD21_IDX
#define	EXT_SPI_MOSI_FLAGS			(IOPORT_MODE_MUX_B)
#define	EXT_SPI_NPCS0_GPIO			PIO_PB2_IDX
#define	EXT_SPI_NPCS0_FLAGS			(IOPORT_MODE_MUX_D)
#define	EXT_SPI_NPCS1_GPIO			PIO_PD25_IDX
#define	EXT_SPI_NPCS1_FLAGS			(IOPORT_MODE_MUX_B)

#define	EXT_SPI_SPCK_GPIO				PIO_PD22_IDX
#define	EXT_SPI_SPCK_FLAGS			(IOPORT_MODE_MUX_B)


/** SPI base address for SPI master mode*/
#define SPI_MASTER_BASE			SPI0


/** SPI pins definition
 *  - VCC -- VCC
 *  - NPCS0: PB02, evaluation board

 *  - NPCS0: PD25, MuxLab board
 *  - MISO: PD20
 *  - MOSI: PD21
 *  - SPCK: PD22
 *  - GND -- GND
 */
#define SPI_Handler				SPI0_Handler
#define SPI_IRQn					SPI0_IRQn


#define	SPI_MASTER_SPEED  			(10000000L)

/* configuration options for SPI controller */
/* Clock polarity. */
#define EXT_SPI_CLK_POLARITY			0

/* Clock phase. */
#define EXT_SPI_CLK_PHASE				1

/* Delay Before SPCK. */
#define EXT_SPI_DLYBS					0//0x40

/* Delay Between Consecutive Transfers. */
#define EXT_SPI_DLYBCT					0//0x10

/* Chip select. */
#define	EXT_SPI_CHIP_SEL						1
#define	EXT_SPI_CHIP_PCS						spi_get_pcs(EXT_SPI_CHIP_SEL)

#define	EXT_SPI_MASTER_DUMMY				0xFF


#define	SPI_RESELECT(a)							{ extBspSpiUnselectChip(a); extBspSpiSelectChip(a); }




/* Micro NOR FLASH */

#define	SFLASH_ADDRESS_128M						(0xffffff)	/*address of 128 Mb(16MB) */
#define	SFLASH_ADDRESS_GOLDEN_IMAGE				(0x000000)	/*address for Golden image FPGA firmware, 4MB */
#define	SFLASH_ADDRESS_MULTIBOOT_IMAGE			(0x400000)	/*address for Multiboot image FPGA firmware, the second 4MB */

#define	SFLASH_ADDRESS_SYSTEM_PARAM			(0x800000)	/*address for system parameter, 8MB. Not used now */
#define	SFLASH_ADDRESS_TEMP_RTOS				(0x810000)	/*address for temp RTPOS, 8MB+64KB */
#define	SFLASH_ADDRESS_TEMP_FPGA				(0x900000)	/*address for temp FPGA, 9MB */
#define	SFLASH_ADDRESS_USER_REGION				(0xD00000)	/*address for user region, 13MB. empty */


#define	SFLASH_PAGE_ADDRESS						(8)
#define	FLASH_N250_PAGE_SIZE						(1<<SFLASH_PAGE_ADDRESS)		/* 256 bytes/page  */

#define	FLASH_N25Q_CAPACITY						(32*UNIT_OF_MEGA)			/*256Mb, 32 MB */
#define	FLASH_N25Q_PAGE_COUNT					(FLASH_N25Q_CAPACITY/FLASH_N250_PAGE_SIZE)	/* 65536x2=131072 pages */


#define	FLASH_N25Q_PAGES_IN_ONE_SECTOR				(SFLASH_ERASE_SECTOR_SIZE/FLASH_N250_PAGE_SIZE)	/* 256 pages in one erase sector */

/* page# to address */
#define	FLASH_N250_PAGE_ADDRESS(pageNo)				((pageNo)<<SFLASH_PAGE_ADDRESS)	/*2^^8=256 */

/* page# to sector# */
#define	FLASH_N250_PAGE_TO_SECTOR(pageNo)			((pageNo)/FLASH_N25Q_PAGES_IN_ONE_SECTOR)	 /* page#/256 */


/* sector#. to address */
#define	FLASH_N250_SECTOR_ADDRESS(sectorNo)			((sectorNo)<<16)		/*2^^16=64K */

/* sector#. to page# */
#define	FLASH_N250_SECTOR_TO_PAGE(sectorNo)			((sectorNo)<<SFLASH_PAGE_ADDRESS)		/*2^^8= pages, 256 pages per sector */


#ifdef TEST_SDRAM
#define CONFIG_SLEEPMGR_ENABLE

#define CONF_BOARD_SDRAMC
#endif

/* Test page start address. */
#define TEST_PAGE_ADDRESS		(IFLASH_ADDR + IFLASH_SIZE - IFLASH_PAGE_SIZE * 4)


#define	EXT_FPGA_REG_ENABLE					3


#define	EXT_FPGA_REG_ETHERNET_RESET			0


/* for both TX/RX */
#define	EXT_FPGA_REG_VERSION						38
#define	EXT_FPGA_REG_REVISION					39
#define	EXT_FPGA_REG_MODEL						40

#define	EXT_FPGA_REG_YEAR						41
#define	EXT_FPGA_REG_MONTH						42
#define	EXT_FPGA_REG_DAY							43
#define	EXT_FPGA_REG_HOUR						44
#define	EXT_FPGA_REG_MINUTE						45


#define	EXT_FPGA_REG_IP							52
#define	EXT_FPGA_REG_MAC							56
#define	EXT_FPGA_REG_PORT_VIDEO					62
#define	EXT_FPGA_REG_PORT_AUDIO					76

#define	EXT_FPGA_REG_PORT_ANC_DT				80
#define	EXT_FPGA_REG_PORT_ANC_ST				84

/* for only TX */
#define	EXT_FPGA_REG_DEST_IP						64
#define	EXT_FPGA_REG_DEST_MAC					68
#define	EXT_FPGA_REG_DEST_PORT_VIDEO			74
#define	EXT_FPGA_REG_DEST_PORT_AUDIO			78

#define	EXT_FPGA_REG_DEST_PORT_ANC_DT			82
#define	EXT_FPGA_REG_DEST_PORT_ANC_ST			86


#endif

