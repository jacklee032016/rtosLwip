
#ifndef	__BSP_HW_SPI_FLASH_H__
#define	__BSP_HW_SPI_FLASH_H__

/********** commands of Flash ***********/
/* common commands */
#define	NFLASH_CMD_RESET_ENABLE					0x66
#define	NFLASH_CMD_RESET_MEMORY					0x99
#define	NFLASH_CMD_READ_DEVICE_ID				0x9E

/* read commands */
#define	NFLASH_CMD_READ							0x03	/* 0~infinite bytes for one read command */

/* erase commands */
#define	NFLASH_CMD_ERASE_BULK					0xC7
#define	NFLASH_CMD_ERASE_SECTOR					0xD8

/* program commands */
#define	NFLASH_CMD_WRITE_ENABLE					0x06
#define	NFLASH_CMD_WRITE_DISABLE				0x04
#define	NFLASH_CMD_PAGE_PROGRAM				0x02	/* program in page size: 256 bytes */

/* register commands */
#define	NFLASH_CMD_WRITE_STATUS_REGISTER		0x01	/* status register */
#define	NFLASH_CMD_READ_STATUS_REGISTER			0x05


#define SPI_CMD_ENABLE_AUTO_ADDR_INCREMENT		0xAD

#define	NFLASH_CMD_ENTER_4B_ADDR				0xB7
#define	NFLASH_CMD_EXIT_4B_ADDR					0xE9


#define	FLASH_N25Q_ID_MANU						0x20
#define	FLASH_N25Q_ID_DEVICE						0xBA
#define	FLASH_N25Q_ID_CAPACITY					0x19
#define	FLASH_N25Q_ID_LENGTH						0x10


#define SPI_STATUS_IS_BUSY							(1<<0)
#define SPI_STATUS_IS_WRITE_ENABLED				(1<<1)
#define SPI_STATUS_BP0								(1<<2)
#define SPI_STATUS_BP1								(1<<3)
#define SPI_STATUS_BP2								(1<<4)
#define SPI_STATUS_BP3								(1<<5)
#define SPI_STATUS_AAI								(1<<6)	// Auto address increment programming mode; Otherwise, byte-program mode
#define SPI_STATUS_BPL								(1<<7)	// true if BPx are read-only


//#define _SPI_MUTEX_

#ifdef _SPI_MUTEX_
volatile xSemaphoreHandle	SPI_Mutex;
#define LOCK_SPI()		xSemaphoreTake(SPI_Mutex, portMAX_DELAY)
#define UNLOCK_SPI()		xSemaphoreGive(SPI_Mutex)
#else
#define LOCK_SPI()
#define UNLOCK_SPI()
#endif


#define	SFLASH_ERASE_SECTOR_SIZE					(64*UNIT_OF_KILO)		/* 64KB*/
#define	SFLASH_ERASE_SECTOR_COUNT				(FLASH_N25Q_CAPACITY /SFLASH_ERASE_SECTOR_SIZE)				/* 512x64KB erase sector */

#define	SFLASH_START_SECTOR_FIRST_IMAGE			(SFLASH_ADDRESS_GOLDEN_IMAGE /SFLASH_ERASE_SECTOR_SIZE)		/* No. 0 erase sector */
#define	SFLASH_START_SECTOR_SECOND_IMAGE		(SFLASH_ADDRESS_MULTIBOOT_IMAGE /SFLASH_ERASE_SECTOR_SIZE)	/* No. 64 erase sector */
#define	SFLASH_START_SECTOR_SYSTEM_PARAM		(SFLASH_ADDRESS_SYSTEM_PARAM /SFLASH_ERASE_SECTOR_SIZE)		/* No. 128 erase sector */

#define	SFLASH_START_SECTOR_TEMP_RTOS			(SFLASH_ADDRESS_TEMP_RTOS /SFLASH_ERASE_SECTOR_SIZE)			/* No. 129 erase sector */
#define	SFLASH_START_SECTOR_TEMP_FPGA			(SFLASH_ADDRESS_TEMP_FPGA /SFLASH_ERASE_SECTOR_SIZE)			/* No. 144 erase sector */

#define	SFLASH_START_SECTOR_USER_ZONE			(SFLASH_ADDRESS_USER_REGION /SFLASH_ERASE_SECTOR_SIZE)			/* No. 208 erase sector */

#define	SFLASH_ERASE_SUB_SECTOR_SIZE			SFLASH_ERASE_SECTOR_SIZE/16		/* 4KB*/
#define	SFLASH_ERASE_SUB_SECTOR_COUNT			SFLASH_ERASE_SECTOR_COUNT*16		/* 8192 4KB erase sub-sector */


#endif

