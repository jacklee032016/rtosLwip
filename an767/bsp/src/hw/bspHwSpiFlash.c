/*
* HW SPI Flash access through FPGA, handle mutex access and spi command
*/

#include "compact.h"
#include "bspHwSpiFlash.h"
#include "spi.h"

#define	_WITH_PRIMARY_PROTECTION		0

#define	SFLASH_RESELECT()					{ SPI_RESELECT(EXT_SPI_CHIP_SEL); }


static void _bspSpiFlashHwSendAddress(unsigned int address, char size, unsigned char *addressSequence)
{
	if(size == 4)
	{
		*addressSequence++ = (U8)( (address>>24) & 0xff); /*ad31-ad24 */
	}
	
	*addressSequence++ =  (U8) ((address>>16) & 0xff);/*ad23-ad16 */
	*addressSequence++ =  (U8) ((address>>8) & 0xff);	/* ad15-ad8 */
	*addressSequence++ =  (U8) ((address>>0) & 0xff);	/* ad7-ad0 */
}


static void _bspSpiFlashHwSend(unsigned char val)
{
	unsigned short x = 0;
	spi_status_t ret;
	x = (unsigned short)val;
	
//	spi_status_t err_flag0; 
//	while (spi_is_tx_empty(SPI0) ==0 );
//	unsigned char  v[10] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa };

	ret = spi_write(SPI0, x, 1, 0);
	if(ret != SPI_OK)
	{
#ifdef	DEBUG_SPI
		bspSpiDebug(ret);
#endif
		EXT_ERRORF(("SPI Write error: %d"EXT_NEW_LINE, ret) );
	}

//	extBspSpiMasterTransfer((void *)&x, 10);
//	spi_report(err_flag0);
//	return err_flag0;
}

static char _bspSpiFlashHwIsBusy(void)
{
	unsigned char cmd, data;
	char busy = EXT_FALSE;

	cmd = NFLASH_CMD_READ_STATUS_REGISTER;
	extBspSpiWritePacket(&cmd, 1);

	extBspSpiReadPacket(&data, 1);
	
	if(data & SPI_STATUS_IS_BUSY)
	{
		busy = EXT_TRUE;
	}
	
	SFLASH_RESELECT();

//	sprintf(dbg_str, "-%x", byte_rcv );
//	Shell_PrintLine_Debug(dbg_str);

	return busy;
}

#if 0
void extBspFlashReceive(unsigned char *val)
{
	unsigned short x = 0xFF; /* 0xFF tends to be good for dummy data */
	unsigned char pcs;

#if 0
	unsigned char  d[10] ;
	extBspSpiMasterTransfer((void *)&x, 10);
	*val = x & 0xFF;
#endif

	spi_status_t ret = spi_write(SPI0, 0xff, 1, 0);
#ifdef	DEBUG_SPI
	bspSpiDebug(ret); //0xFF tends to be good for dummy data
#endif
//	while (spi_is_tx_empty(SPI0) ==0 );
//	while (spi_is_rx_ready(SPI0) ==0 );
	
	ret = spi_read(SPI0, &x, &pcs);
	bspSpiDebug(ret);
	
	*val = x & 0xFF;
}
#endif


void bspHwSpiFlashReset(void)
{
	LOCK_SPI();
	{
		SFLASH_RESELECT();

#if 0		
		printf("send ENABLE 0x66\r\n");
#endif		
		_bspSpiFlashHwSend(NFLASH_CMD_RESET_ENABLE);
		SFLASH_RESELECT();
#if 0		
		printf("send reset memory 0x99\r\n");
#endif		
		_bspSpiFlashHwSend(NFLASH_CMD_RESET_MEMORY);
		SFLASH_RESELECT();
	}
	UNLOCK_SPI();
	return;
}


/*read Flash Manufacture ID */
char  bspHwSpiFlashReadID(unsigned char *outBuffer, size_t bufferSize)
{
	char ret = EXIT_SUCCESS;
	U8 cmd = NFLASH_CMD_READ_DEVICE_ID;

	if(bufferSize< 4)
	{
		EXT_ERRORF(("buffer is too small for Flash ID: %d"EXT_NEW_LINE, bufferSize));
		return EXIT_FAILURE;
	}

	bspHwSpiFlashReset();

	LOCK_SPI();
	{
//		_bspFlashSend(NFLASH_CMD_READ_DEVICE_ID); //read device ID.
		extBspSpiWritePacket(&cmd, 1);
//		spi_send_address(0);

		extBspSpiReadPacket(outBuffer, bufferSize);

		if(*outBuffer!=FLASH_N25Q_ID_MANU || *(outBuffer+1)!=FLASH_N25Q_ID_DEVICE || *(outBuffer+2)!=FLASH_N25Q_ID_CAPACITY )
		{
			EXT_ERRORF(("Flash Manu ID Wrong:%2x:%2x:%2x"EXT_NEW_LINE, *outBuffer, *(outBuffer+1), *(outBuffer+2) ));
			ret = EXIT_FAILURE;
		}
	}

	UNLOCK_SPI();
//		SFLASH_RESELECT();

	return ret;
}


/* sector number from 0~512 */
char bspHwSpiFlashEraseSector(unsigned int sectorNo)
{
	U8 cmd[8];

#if _WITH_PRIMARY_PROTECTION	
	/* bypass erase command below 4 M
	* FPGA safe boot code is located at addr 0 - 4M 
	*/
	if (sectorNo < SFLASH_START_SECTOR_SECOND_IMAGE)
	{
		EXT_ERRORF(("Sector#%u is in the zone of primary image(%d)"EXT_NEW_LINE, sectorNo, SFLASH_START_SECTOR_SECOND_IMAGE));
		return EXIT_FAILURE;
	}
#endif
	
	LOCK_SPI();
	{
		cmd[0] = NFLASH_CMD_WRITE_ENABLE;
		extBspSpiWritePacket(cmd, 1);
		SFLASH_RESELECT();

		cmd[0] = NFLASH_CMD_ERASE_SECTOR;
		_bspSpiFlashHwSendAddress((sectorNo << 16), 3, &cmd[1]);
		
		extBspSpiWritePacket(cmd, 4);
		SFLASH_RESELECT();			

		while (_bspSpiFlashHwIsBusy())
		{
			EXT_DELAY_MS(10);
		}
		
//		spi_send(NFLASH_CMD_EXIT_4B_ADDR);
	}
	UNLOCK_SPI();
	return EXIT_SUCCESS;
}


/* read any length data from flash */
char bspHwSpiFlashRead(unsigned int  address, unsigned char *data, unsigned int size)
{
	char ret = EXIT_SUCCESS;
	U8 cmd[8];
	int writeLength = 4;
	
	cmd[0] = NFLASH_CMD_READ;

	LOCK_SPI();

//	bspSpiFlashReset();

	if (address > SFLASH_ADDRESS_128M)
	{
		cmd[1] = NFLASH_CMD_ENTER_4B_ADDR;
#if 1
		_bspSpiFlashHwSendAddress(address, 4, &cmd[2]);
#else
		cmd[2] = (U8) ((address>>24) & 0xff); 	/*ad31-ad24 */
		cmd[3] = (U8) ((address>>16) & 0xff);	/*ad23-ad16 */
		cmd[4] = (U8) ((address>>8) & 0xff);	/* ad15-ad8 */
		cmd[5] = (U8) ((address>>0) & 0xff);	/* ad7-ad0 */
#endif		
		writeLength = 6;
	}
	else
	{
#if 1
		_bspSpiFlashHwSendAddress(address, 3, &cmd[1]);
#else
		cmd[1] = (U8) ((address>>16) & 0xff);	/*ad23-ad16 */
		cmd[2] = (U8) ((address>>8) & 0xff);	/* ad15-ad8 */
		cmd[3] = (U8) ((address>>0) & 0xff);	/* ad7-ad0 */
#endif
	}

#if 0//def	_DEBUG_SPI_BIST
	bspConsoleDumpFrame(cmd, writeLength);
#endif

	ret = extBspSpiWritePacket((const U8 *)&cmd, writeLength);

	ret = extBspSpiReadPacket(data, size);
	
	SFLASH_RESELECT();

	UNLOCK_SPI();

	return ret;
}

/* max is 256 byte when program command 0x02 */
char bspHwSpiFlashWritePage(unsigned int pageNo, unsigned char *data, unsigned int length)
{
	U8 cmd[8];
	int writeLength = 4;
	char ret = EXIT_SUCCESS;

	unsigned int address = FLASH_N250_PAGE_ADDRESS(pageNo);

	// Use the block mode for faster ops (1 - 256 byte )
	LOCK_SPI();
	{
		cmd[0] = NFLASH_CMD_WRITE_ENABLE;
		extBspSpiWritePacket(cmd, 1);
		SFLASH_RESELECT();
		
		cmd[0] = NFLASH_CMD_PAGE_PROGRAM; //write.

		if (address > SFLASH_ADDRESS_128M)
		{
			cmd[1] = NFLASH_CMD_ENTER_4B_ADDR;
			_bspSpiFlashHwSendAddress(address, 4, &cmd[2]);
			writeLength = 6;
		}
		else
		{
			_bspSpiFlashHwSendAddress(address, 3, &cmd[1]);
			
		}

		ret = extBspSpiWritePacket(cmd, writeLength);
		if(ret == EXIT_SUCCESS)
		{

#if 0//def	_DEBUG_SPI_BIST
			printf("send %d bytes"EXT_NEW_LINE, (int)length);
#endif
			ret = extBspSpiWritePacket(data, length);
			
			SFLASH_RESELECT();
		
			while(_bspSpiFlashHwIsBusy() )
			{
				EXT_DELAY_MS(10);
			}
		//		spi_send(NFLASH_CMD_EXIT_4B_ADDR);
		}
	}
	UNLOCK_SPI();
	
	return ret;
}


/* only called when system startup. init hw and create mutex  */
void bspHwSpiFlashInit(void)
{
	uint32_t pcs = EXT_SPI_CHIP_SEL;
//	bspSpiMasterInitialize(pcs);
	
#ifdef _SPI_MUTEX_
	SPI_Mutex = xSemaphoreCreateMutex();
#endif	

	bspHwSpiFlashReset();
	extBspSpiUnselectChip(pcs);
#if 0
	LOCK_SPI();
	{
		spi_send(NFLASH_CMD_WRITE_ENABLE); //write enable.
		spi_reselect(0);

		spi_send(NFLASH_CMD_WRITE_STATUS_REGISTER); //function.
		spi_send(0x00);	//clears all block protect flags. Does not affect the read-only BUSY, WEL or AAI flags.

		spi_reselect(0);
	}
	UNLOCK_SPI();
#endif

	return;
}


