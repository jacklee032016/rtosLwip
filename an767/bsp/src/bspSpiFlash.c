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
}_MUX_SPI_FLASH_T;

static _MUX_SPI_FLASH_T _muxSpiFlash;

/* read and write interface with continous access flash; erase operation is handled directly with hw spi flash functions */
static void _bspSpiFlashInit(_MUX_SPI_FLASH_T *_muxFlash, unsigned int startSector, unsigned int startAddress, char isWrite)
{
	bspHwSpiFlashReset();

	_muxFlash->startSector = startSector;
	_muxFlash->currentSector = startSector;
	
	_muxFlash->pageIndex = 0;
	_muxFlash->leftBytesWrite = 0;
	_muxFlash->offsetRead = 0;
	
	_muxFlash->bufSize = sizeof(_muxFlash->readBuffer);

	if(isWrite)
	{
		_muxFlash->startAddress = FLASH_N250_SECTOR_ADDRESS(_muxFlash->startSector) + FLASH_N250_PAGE_ADDRESS(0);
		bspHwSpiFlashEraseSector(_muxFlash->currentSector);
	}
	else
	{
		_muxFlash->startAddress = startAddress;
	}

}

static int	_bspSpiFlashRead(_MUX_SPI_FLASH_T *_muxFlash, unsigned char *buf, unsigned int size)
{
	unsigned int address = _muxFlash->startAddress + _muxFlash->offsetRead;
	unsigned int length = size;// (size >= _muxFlash->bufSize )? _muxFlash->bufSize: size;
	
	char ret = bspHwSpiFlashRead(address, buf, length);
	if(ret == EXIT_SUCCESS)
	{
		_muxFlash->offsetRead += length;
		return (int)length;
	}

	return -1;
}

/* one page */
static int	__bspSpiFlashWriteOnePage(_MUX_SPI_FLASH_T *_muxFlash, unsigned char *data, unsigned int size)
{
	unsigned int	pageNo = 0;

	pageNo = FLASH_N250_SECTOR_TO_PAGE(_muxFlash->currentSector) + _muxFlash->pageIndex;
	if(bspHwSpiFlashWritePage(pageNo, data, size) == EXIT_FAILURE)
	{
		MUX_ERRORF(("data program error at page#:%u"MUX_NEW_LINE, pageNo ));
		return -1;
	}
	
	_muxFlash->pageIndex++;
	if(_muxFlash->pageIndex >= FLASH_N25Q_PAGES_IN_ONE_SECTOR)
	{
		_muxFlash->currentSector++;
		if((_muxFlash->currentSector >= FLASH_N25Q_PAGE_COUNT) )
		{
			MUX_ERRORF(("Sector No. %d is out of range 0~%d"MUX_NEW_LINE, _muxFlash->currentSector, FLASH_N25Q_PAGE_COUNT-1) );
			return -1;
		}
		
		if(bspHwSpiFlashEraseSector(_muxFlash->currentSector) == EXIT_FAILURE)
		{
			return -1;
		}
		
		_muxFlash->pageIndex = 0;
	}

	return (int)size;
}

/* called from begin address to end  address */
static int	_bspSpiFlashWrite(_MUX_SPI_FLASH_T *_muxFlash, unsigned char *data, unsigned int size)
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

		if(__bspSpiFlashWriteOnePage(_muxFlash, data+i*FLASH_N250_PAGE_SIZE, length) == EXIT_FAILURE)
		{
			MUX_ERRORF(("data program error no. %u page"MUX_NEW_LINE, i) );
			return -1;
		}
	}

	return (int)size;
}

/* encapsulated new interface used by other modules, such as tftp or http */

/* after it is initialized, it only be used for read or write */
void bspSpiFlashInit(unsigned int startSector, unsigned int startAddress, char isWrite)
{
	_bspSpiFlashInit(&_muxSpiFlash, startSector, startAddress, isWrite);
}


int	bspSpiFlashRead( unsigned char *buf, unsigned int size)
{
	return _bspSpiFlashRead(&_muxSpiFlash, buf, size);
}

int	bspSpiFlashWrite(unsigned char *data, unsigned int size)
{
	unsigned short left = (unsigned short)size;
	unsigned int index = 0;
	int i;

	do
	{
		if(_muxSpiFlash.leftBytesWrite+ (unsigned short)left < muxRun.bufLength )
		{/* buffer not full */
			memcpy(muxRun.bufWrite+_muxSpiFlash.leftBytesWrite, data+index, left);
			_muxSpiFlash.leftBytesWrite += left;
			left = 0;
		}
		else
		{/* buffer full */
			int _copyLength = (muxRun.bufLength-_muxSpiFlash.leftBytesWrite);
			memcpy(muxRun.bufWrite+_muxSpiFlash.leftBytesWrite, data+index,  _copyLength);
			_muxSpiFlash.leftBytesWrite = muxRun.bufLength;
			left -= _copyLength;
			index += _copyLength;
		}

		for(i=0; i< _muxSpiFlash.leftBytesWrite/FLASH_N250_PAGE_SIZE; i++)
		{/* write in page size */
			if(_bspSpiFlashWrite(&_muxSpiFlash, muxRun.bufWrite+i*FLASH_N250_PAGE_SIZE, FLASH_N250_PAGE_SIZE)<0)
			{
				return -1;
			}
		}

		_muxSpiFlash.leftBytesWrite -= i*FLASH_N250_PAGE_SIZE;
		if(_muxSpiFlash.leftBytesWrite)
		{
			memcpy(muxRun.bufWrite, muxRun.bufWrite+i*FLASH_N250_PAGE_SIZE, _muxSpiFlash.leftBytesWrite);
		}
		
	}while(left > 0);
	
	return size;
}

/* flush left data in buffer when its length is less than one page */
int	bspSpiFlashFlush(void)
{
	if(_muxSpiFlash.leftBytesWrite> 0 )
	{
		_bspSpiFlashWrite(&_muxSpiFlash, muxRun.bufWrite, _muxSpiFlash.leftBytesWrite);
	}

	return _muxSpiFlash.leftBytesWrite;
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
//	printf(MUX_NEW_LINE"Flash Manu ID:%2x; Device ID:%2x; length %2x"MUX_NEW_LINE, deviceID[0], deviceID[1], deviceID[2]);
	index += snprintf( outBuffer+index, (bufferSize-index), "\t\tFlash Manu ID:'%2x'; Device ID:'%2x'; length: %2x"MUX_NEW_LINE, deviceID[0], deviceID[1], deviceID[2] );

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

	_MUX_SPI_FLASH_T _spiFlashRead;	/* interface of Flash */

	bspSpiFlashInit(startSector, 0, MUX_TRUE); /* default Flash structure is for write */
	_bspSpiFlashInit(&_spiFlashRead, startSector, FLASH_N250_SECTOR_ADDRESS(startSector), MUX_FALSE);

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
				MUX_ERRORF(("SPI Flash program error at address 0x%08X(%d sector, %d page)"MUX_NEW_LINE, address, i, j ));
				return MUX_FALSE;
			}

			if(_bspSpiFlashRead(&_spiFlashRead, muxRun.bufRead, dataSize )<0 )
			{
				MUX_ERRORF(("SPI Flash read error at address 0x%08X(%d sector, %d page)"MUX_NEW_LINE, address, i, j ));
				return MUX_FALSE;
			}
#if 0//	_DEBUG_SPI_BIST
			CONSOLE_DEBUG_MEM((uint8_t *)pageBuffer, dataSize, 0, "raw data");
			
			printf("Read back sector#:%d, page#%d :"MUX_NEW_LINE, i, j );
			CONSOLE_DEBUG_MEM((uint8_t *)_muxSpiFlash.readBuffer, dataSize, address, "read block sector");
#endif
			tmp = (unsigned char *)pageBuffer;
			for(k = 0; k < (FLASH_N250_PAGE_SIZE); k++)
			{
//				unsigned int value = (unsigned int) _muxSpiFlash.readBuffer[k*4];
				if ( *tmp != muxRun.bufRead[k] )
				{
					MUX_ERRORF(("data error on address 0x%08X, 0x%02X!=0x%02X"MUX_NEW_LINE, (unsigned int)address+k, *(tmp), muxRun.bufRead[k]) );
					return MUX_FALSE;
				}
				tmp++;
			}

			offset += dataSize;
			
			printf(".");
			//putchar('.');

		}
		
		printf("" MUX_NEW_LINE);
	}

#if	_DEBUG_SPI_BIST
	printf("Total %d sectors, %d pages have been tested"MUX_NEW_LINE, i -startSector, FLASH_N25Q_PAGES_IN_ONE_SECTOR*( i -startSector) );
#endif
	return MUX_FALSE;
}


#define	_PAGE_COUNT	2
char	bspCmdSpiFlashRead(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen )
{
	int startPage = 0;
	int count = _PAGE_COUNT;
	int i;
	unsigned int offset =0;
	unsigned int size = FLASH_N250_PAGE_SIZE;

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
		snprintf( outBuffer, bufferLen, "Parameter error: startPage:%d, count:%d, %s"MUX_NEW_LINE, startPage, count, cmd->helpString );
		return MUX_FALSE;
	}

	bspSpiFlashInit(0, FLASH_N250_PAGE_ADDRESS(startPage), MUX_FALSE);

	*outBuffer = 0; /* clean buffer */

	for(i=startPage; i< startPage+count; i++)
	{
		if((i>= FLASH_N25Q_PAGE_COUNT) )
		{
			snprintf( outBuffer, bufferLen, "ERROR: Page#:%d is error, page is in the range of 0~%u" MUX_NEW_LINE, i, FLASH_N25Q_PAGE_COUNT-1 );
			return MUX_FALSE;
		}
		
		bspSpiFlashRead(muxRun.bufRead, size);
		printf("Page#:%d: "MUX_NEW_LINE, i );
		CONSOLE_DEBUG_MEM(muxRun.bufRead, size, _muxSpiFlash.startAddress + _muxSpiFlash.offsetRead-size, "Page #:");
		offset += size;
	}
	
	return MUX_FALSE;
}

char	bspCmdSpiFlashErase(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen )
{
	int startSector = 0;
	int countSector = 1;
	int i;

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
		snprintf( outBuffer, bufferLen, "Parameter error: startPage:%d, count:%d, %s"MUX_NEW_LINE, startSector, countSector, cmd->helpString );
		return MUX_FALSE;
	}

	bspHwSpiFlashReset();

	*outBuffer = 0; /* clean buffer */

	for(i=startSector; i< startSector+countSector; i++)
	{
		if((i>= SFLASH_ERASE_SECTOR_COUNT) )
		{
			snprintf( outBuffer, bufferLen, "ERROR: Sector#:%d is error, sector is in the range of 0~%u" MUX_NEW_LINE, i, SFLASH_ERASE_SECTOR_COUNT-1 );
			return MUX_FALSE;
		}
		
		if(bspHwSpiFlashEraseSector(i) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
		printf("Erase Sector#%d: "MUX_NEW_LINE, i );
	}
	
	return MUX_FALSE;
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
		printf("Parameter error: startSector#:%d"MUX_NEW_LINE, startSector);
		return MUX_FALSE;
	}

	info.mode = mode;

	bspSpiFlashInit(startSector, 0, MUX_TRUE);

	res = xyzModem_stream_open(&info, &err);
	if (!res)
	{		
		while ((res = xyzModem_stream_read((char *)muxRun.bufRead, muxRun.bufLength, &err)) > 0)
		{

#if 0
			addrInSector += res;
			if(addrInSector >= SFLASH_ERASE_SECTOR_SIZE)
			{
				startSector++;
				muxBspFlashEraseSector(startSector);
				addrInSector = 0;
			}

			ret = muxBspFlashProgram(addr, (unsigned char *)_buffer, res);
			if(ret == EXIT_FAILURE)
			{
				break;
			}
			addr += res;
#else
			if(bspSpiFlashWrite(muxRun.bufRead, res) < 0 )
			{
				MUX_ERRORF(("SPI Flash Program error at offset %d"MUX_NEW_LINE, size ));
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

	printf("## FPGA Total Size      = 0x%08x = %d Bytes"MUX_NEW_LINE, size, size);
//	setenv_hex("filesize", size);

	return offset;
}


char bspCmdSpiFlashXmodemLoad(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen)
{
	unsigned int	 offset = 0;

	printf("## Ready for binary (xmodem) load FPGA image...:"MUX_NEW_LINE);
	_loadFpgaFromSerialYmodem(offset, xyzModem_xmodem);

	return MUX_FALSE;
}


char bspCmdSpiFlashYmodemLoad(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen)
{
	unsigned int	 offset = 0;
//	unsigned int	 addr;

	printf("## Ready for binary (ymodem) load FPGA image...:"MUX_NEW_LINE );
	_loadFpgaFromSerialYmodem(offset, xyzModem_ymodem);

	return MUX_FALSE;
}


int	bspBootUpdateFpga(MUX_RUNTIME_CFG *runCfg)
{
	int	length= 0;
	unsigned int 	total = 0;
	int writeLength = 0;
	_MUX_SPI_FLASH_T _spiFlashRead;	/* read interface of Flash */

	int readSector = SFLASH_START_SECTOR_TEMP_FPGA;

	/* read interface initialized with startAddress */
	_bspSpiFlashInit(&_spiFlashRead, readSector, FLASH_N250_SECTOR_ADDRESS(readSector), MUX_FALSE);

	/* SPI flash writing interface: initialize with sector address (to erase) at #0 */
	bspSpiFlashInit(SFLASH_START_SECTOR_FIRST_IMAGE, 0, MUX_TRUE);

	printf( ("Update FPGA"MUX_NEW_LINE) );
	while(total < runCfg->firmUpdateInfo.size )
	{
		length = _bspSpiFlashRead(&_spiFlashRead, runCfg->bufRead, runCfg->bufLength );
		if(length < 0)
		{
			MUX_ERRORF(("Update OS failed after read %d bytes"MUX_NEW_LINE, length) );
			break;
		}
		if(length != runCfg->bufLength)
		{
			MUX_INFOF(("Last read %d bytes from SPI Flash", length));
		}

		writeLength = bspSpiFlashWrite((U8 *) runCfg->bufRead, length);
		if(writeLength != length)
		{
			MUX_ERRORF(("SPI Flash program write %d bytes, it should be %d", writeLength, length) );
			return MUX_FALSE;
		}
		
		total += writeLength;
		printf(".");
			//putchar('.');
		
	}

	printf("" MUX_NEW_LINE);

	MUX_INFOF(("Update FPGA %d (%d) bytes"MUX_NEW_LINE, total,  runCfg->firmUpdateInfo.size));

	return total;
}


