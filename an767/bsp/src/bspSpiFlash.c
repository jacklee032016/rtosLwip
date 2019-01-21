/*
* Read/Write interface of SPI Flash, based on bspHwSpiFlash
* mutex access is handled in hw spi flash layer
*/


#include "compact.h"
#include "bspHwSpiFlash.h"
#include "spi.h"

#define	_DEBUG_SPI_BIST		1

typedef	struct
{
	int				startSector;
	unsigned int		startAddress;

	/* fields for write */	
	unsigned short	currentSector;		/* current sector */
	unsigned short	pageIndex;			/* page index in current sector, 0~255 */
	unsigned short	leftBytesWrite;		/* left bytes in last page after last write */
	
	/* fields for read */
	unsigned int		offsetRead;
	unsigned char		readBuffer[FLASH_N250_PAGE_SIZE];	/* only used for read operation */
	unsigned int		bufSize;
}_EXT_SPI_FLASH_T;

static _EXT_SPI_FLASH_T _extSpiFlash;

/* read and write interface with continous access flash; erase operation is handled directly with hw spi flash functions */
static void _bspSpiFlashInit(_EXT_SPI_FLASH_T *_extFlash, unsigned int startSector, unsigned int startAddress, char isWrite)
{
	bspHwSpiFlashReset();

	_extFlash->startSector = startSector;
	_extFlash->currentSector = startSector;
	
	_extFlash->pageIndex = 0;
	_extFlash->leftBytesWrite = 0;
	_extFlash->offsetRead = 0;
	
	_extFlash->bufSize = sizeof(_extFlash->readBuffer);

	if(isWrite)
	{
		_extFlash->startAddress = FLASH_N250_SECTOR_ADDRESS(_extFlash->startSector) + FLASH_N250_PAGE_ADDRESS(0);
		bspHwSpiFlashEraseSector(_extFlash->currentSector);
	}
	else
	{
		_extFlash->startAddress = startAddress;
	}

}

static int	_bspSpiFlashRead(_EXT_SPI_FLASH_T *_extFlash, unsigned char *buf, unsigned int size)
{
	unsigned int address = _extFlash->startAddress + _extFlash->offsetRead;
	unsigned int length = size;// (size >= _extFlash->bufSize )? _extFlash->bufSize: size;
	
	char ret = bspHwSpiFlashRead(address, buf, length);
	if(ret == EXIT_SUCCESS)
	{
		_extFlash->offsetRead += length;
		return (int)length;
	}

	return -1;
}

/* one page */
static int	__bspSpiFlashWriteOnePage(_EXT_SPI_FLASH_T *_extFlash, unsigned char *data, unsigned int size)
{
	unsigned int	pageNo = 0;

	pageNo = FLASH_N250_SECTOR_TO_PAGE(_extFlash->currentSector) + _extFlash->pageIndex;
	if(bspHwSpiFlashWritePage(pageNo, data, size) == EXIT_FAILURE)
	{
		EXT_ERRORF(("data program error at page#:%u"EXT_NEW_LINE, pageNo ));
		return -1;
	}
	
	_extFlash->pageIndex++;
	if(_extFlash->pageIndex >= FLASH_N25Q_PAGES_IN_ONE_SECTOR)
	{
		_extFlash->currentSector++;
		if((_extFlash->currentSector >= FLASH_N25Q_PAGE_COUNT) )
		{
			EXT_ERRORF(("Sector No. %d is out of range 0~%d"EXT_NEW_LINE, _extFlash->currentSector, FLASH_N25Q_PAGE_COUNT-1) );
			return -1;
		}
		
		if(bspHwSpiFlashEraseSector(_extFlash->currentSector) == EXIT_FAILURE)
		{
			return -1;
		}
		
		_extFlash->pageIndex = 0;
	}

	return (int)size;
}

/* called from begin address to end  address */
static int	_bspSpiFlashWrite(_EXT_SPI_FLASH_T *_extFlash, unsigned char *data, unsigned int size)
{
	unsigned int 	i, pageCount;
	unsigned int	left = size %FLASH_N250_PAGE_SIZE;
	unsigned int  length = FLASH_N250_PAGE_SIZE;

	pageCount = (size+FLASH_N250_PAGE_SIZE-1)/FLASH_N250_PAGE_SIZE;

	for(i=0; i< pageCount; i++)
	{
		if(i == pageCount-1 && left != 0 )/* when last page is not 256 bytes */
		{
			length = left;
		}

		if(__bspSpiFlashWriteOnePage(_extFlash, data+i*FLASH_N250_PAGE_SIZE, length) == EXIT_FAILURE)
		{
			EXT_ERRORF(("data program error no. %u page"EXT_NEW_LINE, i) );
			return -1;
		}
	}

	return (int)size;
}

/* encapsulated new interface used by other modules, such as tftp or http */

/* after it is initialized, it only be used for read or write */
void bspSpiFlashInit(unsigned int startSector, unsigned int startAddress, char isWrite)
{
	_bspSpiFlashInit(&_extSpiFlash, startSector, startAddress, isWrite);
}


int	bspSpiFlashRead( unsigned char *buf, unsigned int size)
{
	return _bspSpiFlashRead(&_extSpiFlash, buf, size);
}

int	bspSpiFlashWrite(unsigned char *data, unsigned int size)
{
	unsigned short left = (unsigned short)size;
	unsigned int index = 0;
	int i;

	do
	{
		if(_extSpiFlash.leftBytesWrite+ (unsigned short)left < extRun.bufLength )
		{/* buffer not full */
			memcpy(extRun.bufWrite+_extSpiFlash.leftBytesWrite, data+index, left);
			_extSpiFlash.leftBytesWrite += left;
			left = 0;
		}
		else
		{/* buffer full */
			int _copyLength = (extRun.bufLength-_extSpiFlash.leftBytesWrite);
			memcpy(extRun.bufWrite+_extSpiFlash.leftBytesWrite, data+index,  _copyLength);
			_extSpiFlash.leftBytesWrite = extRun.bufLength;
			left -= _copyLength;
			index += _copyLength;
		}

		for(i=0; i< _extSpiFlash.leftBytesWrite/FLASH_N250_PAGE_SIZE; i++)
		{/* write in page size */
			if(_bspSpiFlashWrite(&_extSpiFlash, extRun.bufWrite+i*FLASH_N250_PAGE_SIZE, FLASH_N250_PAGE_SIZE)<0)
			{
				return -1;
			}
		}

		_extSpiFlash.leftBytesWrite -= i*FLASH_N250_PAGE_SIZE;
		if(_extSpiFlash.leftBytesWrite)
		{
			memcpy(extRun.bufWrite, extRun.bufWrite+i*FLASH_N250_PAGE_SIZE, _extSpiFlash.leftBytesWrite);
		}
		
	}while(left > 0);
	
	return size;
}

/* flush left data in buffer when its length is less than one page */
int	bspSpiFlashFlush(void)
{
	if(_extSpiFlash.leftBytesWrite> 0 )
	{
		_bspSpiFlashWrite(&_extSpiFlash, extRun.bufWrite, _extSpiFlash.leftBytesWrite);
	}

	return _extSpiFlash.leftBytesWrite;
}

/* BIST functions of SPI Flash */
char  bspBistSpiFlashReadDeviceID(char *outBuffer, size_t bufferSize)
{
#ifdef BSP_BIST_DEBUG
	U8 deviceID[4+FLASH_N25Q_ID_LENGTH];
#else
	U8 deviceID[4];
#endif
	int index = 0;

	outBuffer[ 0 ] = 0x00;
	
	bspHwSpiFlashReset();
	if(bspHwSpiFlashReadID(deviceID, sizeof(deviceID)) == EXIT_FAILURE)
	{
		return EXIT_FAILURE;
	}
	
	bspConsoleDumpFrame(deviceID, sizeof(deviceID));
//	printf(EXT_NEW_LINE"Flash Manu ID:%2x; Device ID:%2x; length %2x"EXT_NEW_LINE, deviceID[0], deviceID[1], deviceID[2]);
	index += snprintf( outBuffer+index, (bufferSize-index), "\t\tFlash Manu ID:'%2x'; Device ID:'%2x'; length: %2x"EXT_NEW_LINE, deviceID[0], deviceID[1], deviceID[2] );

	return EXIT_SUCCESS;
}


char  bspBistSpiFlashReadWrite(char *outBuffer, size_t bufferSize)
{
	int i, j, k;
	int startSector = SFLASH_START_SECTOR_USER_ZONE;
#if 1
	int endSector = SFLASH_START_SECTOR_USER_ZONE+1;
#else
	int endSector = SFLASH_ERASE_SECTOR_COUNT;
#endif
	uint32_t pageBuffer[FLASH_N250_PAGE_SIZE/4];
	unsigned int dataSize = sizeof(pageBuffer);
	unsigned int address = 0;

	uint32_t offset = 0;
	
	unsigned char *tmp;

	_EXT_SPI_FLASH_T _spiFlashRead;	/* interface of Flash */

	bspSpiFlashInit(startSector, 0, EXT_TRUE); /* default Flash structure is for write */
	_bspSpiFlashInit(&_spiFlashRead, startSector, FLASH_N250_SECTOR_ADDRESS(startSector), EXT_FALSE);

	/* Write page */
	for (i = 0; i < FLASH_N250_PAGE_SIZE/4; i++)
	{
		pageBuffer[i] = 1 << (i % MAX_SHIFTING_NUMBER);
	}

	for(i= startSector; i< endSector; i++)
	{

		for(j=0; j<FLASH_N25Q_PAGES_IN_ONE_SECTOR; j++)
		{
			address = offset + _spiFlashRead.startAddress + _spiFlashRead.offsetRead;
			if(bspSpiFlashWrite((U8 *) pageBuffer, dataSize) < 0)
			{
				EXT_ERRORF(("SPI Flash program error at address 0x%08X(%d sector, %d page)"EXT_NEW_LINE, address, i, j ));
				return EXT_FALSE;
			}

			if(_bspSpiFlashRead(&_spiFlashRead, extRun.bufRead, dataSize )<0 )
			{
				EXT_ERRORF(("SPI Flash read error at address 0x%08X(%d sector, %d page)"EXT_NEW_LINE, address, i, j ));
				return EXT_FALSE;
			}
#if 0//	_DEBUG_SPI_BIST
			CONSOLE_DEBUG_MEM((uint8_t *)pageBuffer, dataSize, 0, "raw data");
			
			printf("Read back sector#:%d, page#%d :"EXT_NEW_LINE, i, j );
			CONSOLE_DEBUG_MEM((uint8_t *)_extSpiFlash.readBuffer, dataSize, address, "read block sector");
#endif
			tmp = (unsigned char *)pageBuffer;
			for(k = 0; k < (FLASH_N250_PAGE_SIZE); k++)
			{
//				unsigned int value = (unsigned int) _extSpiFlash.readBuffer[k*4];
				if ( *tmp != extRun.bufRead[k] )
				{
					EXT_ERRORF(("data error on address 0x%08X, 0x%02X!=0x%02X"EXT_NEW_LINE, (unsigned int)address+k, *(tmp), extRun.bufRead[k]) );
					return EXT_FALSE;
				}
				tmp++;
			}

			offset += dataSize;
			
			printf(".");
			//putchar('.');

		}
		
		printf("" EXT_NEW_LINE);
	}

#if	_DEBUG_SPI_BIST
	printf("Total %d sectors, %d pages have been tested"EXT_NEW_LINE, i -startSector, FLASH_N25Q_PAGES_IN_ONE_SECTOR*( i -startSector) );
#endif
	return EXT_FALSE;
}


#define	_PAGE_COUNT	2
char	bspCmdSpiFlashRead(const struct _EXT_CLI_CMD *cmd, char *outBuffer,  unsigned int bufferLen )
{
	int startPage = 0;
	int count = _PAGE_COUNT;
	int i;
	unsigned int offset =0;
	unsigned int size = FLASH_N250_PAGE_SIZE;

	if( bspHwSpiFlashCheck() == EXIT_FAILURE)
	{
		snprintf( outBuffer, bufferLen, "Access SPI Flash failed: check FPGA firmware"EXT_NEW_LINE);
		return EXT_FALSE;
	}

	if(argc >= 2)
	{
		startPage = (int)atoi(argv[1]);

		if(argc == 3)
		{
			count = (int)atoi(argv[2]);
		}
	}

	if(startPage < 0 || count <=0 || startPage >= FLASH_N25Q_PAGE_COUNT )
	{
		snprintf( outBuffer, bufferLen, "Parameter error: startPage:%d, count:%d, %s"EXT_NEW_LINE, startPage, count, cmd->helpString );
		return EXT_FALSE;
	}

	bspSpiFlashInit(0, FLASH_N250_PAGE_ADDRESS(startPage), EXT_FALSE);

	*outBuffer = 0; /* clean buffer */

	for(i=startPage; i< startPage+count; i++)
	{
		if((i>= FLASH_N25Q_PAGE_COUNT) )
		{
			snprintf( outBuffer, bufferLen, "ERROR: Page#:%d is error, page is in the range of 0~%u" EXT_NEW_LINE, i, FLASH_N25Q_PAGE_COUNT-1 );
			return EXT_FALSE;
		}
		
		bspSpiFlashRead(extRun.bufRead, size);
		printf("Page#:%d: "EXT_NEW_LINE, i );
		CONSOLE_DEBUG_MEM(extRun.bufRead, size, _extSpiFlash.startAddress + _extSpiFlash.offsetRead-size, "Page #:");
		offset += size;
	}
	
	return EXT_FALSE;
}

char	bspCmdSpiFlashErase(const struct _EXT_CLI_CMD *cmd, char *outBuffer,  unsigned int bufferLen )
{
	int startSector = 0;
	int countSector = 1;
	int i;

	if( bspHwSpiFlashCheck() == EXIT_FAILURE)
	{
		snprintf( outBuffer, bufferLen, "Access SPI Flash failed: check FPGA firmware"EXT_NEW_LINE);
		return EXT_FALSE;
	}
	
	if(argc >= 2)
	{
		startSector = (int)atoi(argv[1]);

		if(argc == 3)
		{
			countSector = (int)atoi(argv[2]);
		}
	}

	if(startSector < 0 || countSector <= 0 || startSector >= SFLASH_ERASE_SECTOR_COUNT)
	{
		snprintf( outBuffer, bufferLen, "Parameter error: startPage:%d, count:%d, %s"EXT_NEW_LINE, startSector, countSector, cmd->helpString );
		return EXT_FALSE;
	}

	bspHwSpiFlashReset();

	*outBuffer = 0; /* clean buffer */

	for(i=startSector; i< startSector+countSector; i++)
	{
		if((i>= SFLASH_ERASE_SECTOR_COUNT) )
		{
			snprintf( outBuffer, bufferLen, "ERROR: Sector#:%d is error, sector is in the range of 0~%u" EXT_NEW_LINE, i, SFLASH_ERASE_SECTOR_COUNT-1 );
			return EXT_FALSE;
		}
		
		if(bspHwSpiFlashEraseSector(i) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
		printf("Erase Sector#%d: "EXT_NEW_LINE, i );
	}
	
	return EXT_FALSE;
}


#include <xyModem.h>

static unsigned int	 _loadFpgaFromSerialYmodem(unsigned int	offset, int mode)
{
	int size = 0;
	int err;
	int res;
	connection_info_t info;

	/* defaultly the first image will be updated */
	int	startSector = SFLASH_START_SECTOR_FIRST_IMAGE;

	if(argc >= 2)
	{
		startSector = (int)atoi(argv[1]);
	}
	
	if(startSector < 0 )
	{
		printf("Parameter error: startSector#:%d"EXT_NEW_LINE, startSector);
		return EXT_FALSE;
	}

	info.mode = mode;

	bspSpiFlashInit(startSector, 0, EXT_TRUE);

	res = xyzModem_stream_open(&info, &err);
	if (!res)
	{		
		while ((res = xyzModem_stream_read((char *)extRun.bufRead, extRun.bufLength, &err)) > 0)
		{

#if 0
			addrInSector += res;
			if(addrInSector >= SFLASH_ERASE_SECTOR_SIZE)
			{
				startSector++;
				extBspFlashEraseSector(startSector);
				addrInSector = 0;
			}

			ret = extBspFlashProgram(addr, (unsigned char *)_buffer, res);
			if(ret == EXIT_FAILURE)
			{
				break;
			}
			addr += res;
#else
			if(bspSpiFlashWrite(extRun.bufRead, res) < 0 )
			{
				EXT_ERRORF(("SPI Flash Program error at offset %d"EXT_NEW_LINE, size ));
				break;
			}
#endif

			size += res;
			
		}
	}
	else
	{
		printf("%s\n", xyzModem_error(err));
	}

	bspSpiFlashFlush();

	xyzModem_stream_close(&err);
	xyzModem_stream_terminate(false, bspGetcXModem);


//	flush_cache(offset, ALIGN(size, ARCH_DMA_MINALIGN));

	printf("## FPGA Total Size      = 0x%08x = %d Bytes"EXT_NEW_LINE, size, size);
//	setenv_hex("filesize", size);

	return offset;
}


char bspCmdSpiFlashXmodemLoad(const struct _EXT_CLI_CMD *cmd, char *outBuffer,  unsigned int bufferLen)
{
	unsigned int	 offset = 0;

	printf("## Ready for binary (xmodem) load FPGA image...:"EXT_NEW_LINE);
	_loadFpgaFromSerialYmodem(offset, xyzModem_xmodem);

	return EXT_FALSE;
}


char bspCmdSpiFlashYmodemLoad(const struct _EXT_CLI_CMD *cmd, char *outBuffer,  unsigned int bufferLen)
{
	unsigned int	 offset = 0;
//	unsigned int	 addr;

	printf("## Ready for binary (ymodem) load FPGA image...:"EXT_NEW_LINE );
	_loadFpgaFromSerialYmodem(offset, xyzModem_ymodem);

	return EXT_FALSE;
}


int	bspBootUpdateFpga(EXT_RUNTIME_CFG *runCfg)
{
	int	length= 0;
	unsigned int 	total = 0;
	int writeLength = 0;
	_EXT_SPI_FLASH_T _spiFlashRead;	/* read interface of Flash */

	int readSector = SFLASH_START_SECTOR_TEMP_FPGA;
#if 1
	unsigned int 	writeSector = SFLASH_START_SECTOR_FIRST_IMAGE;
#else
	unsigned int 	writeSector = SFLASH_START_SECTOR_SECOND_IMAGE;
#endif

	/* read interface initialized with startAddress */
	_bspSpiFlashInit(&_spiFlashRead, readSector, FLASH_N250_SECTOR_ADDRESS(readSector), EXT_FALSE);

	/* SPI flash writing interface: initialize with sector address (to erase) at #0 */
	bspSpiFlashInit(writeSector, 0, EXT_TRUE);

	printf( ("Update FPGA"EXT_NEW_LINE) );
	while(total < runCfg->firmUpdateInfo.size )
	{
		length = _bspSpiFlashRead(&_spiFlashRead, runCfg->bufRead, runCfg->bufLength );
		if(length < 0)
		{
			EXT_ERRORF(("Update OS failed after read %d bytes"EXT_NEW_LINE, length) );
			break;
		}
		if(length != runCfg->bufLength)
		{
			EXT_INFOF(("Last read %d bytes from SPI Flash", length));
		}

		writeLength = bspSpiFlashWrite((U8 *) runCfg->bufRead, length);
		if(writeLength != length)
		{
			EXT_ERRORF(("SPI Flash program write %d bytes, it should be %d", writeLength, length) );
			return EXT_FALSE;
		}
		
		total += writeLength;
		printf(".");
			//putchar('.');
		
	}

	printf("" EXT_NEW_LINE);

	EXT_INFOF(("Update FPGA %d (%d) bytes"EXT_NEW_LINE, total,  runCfg->firmUpdateInfo.size));

	return total;
}


