
#include "compact.h"
#include "boot.h"
#include <xyModem.h>

#if 0
static int _do_echo = 0;

static int _readSerialPort(char *buf, ulong len, char isFirstTime)
{
	char *p;
	char ret;
	unsigned int timeoutMs = 250;
	if(isFirstTime)
		timeoutMs = 10000;

	for (p=buf; p < buf+len; ++p)
	{
		ret = bspConsoleGetCharTimeout(p, timeoutMs);
		if(ret==MUX_FALSE)
		{/* current length */
			break;
		}
//		if (_do_echo)
//			bspConsolePutChar(c);	/* ... and echo it		*/


#if 0
		/* Check for the console hangup (if any different from serial) */
		if (gd->jt->getc != getc)
		{
			if (ctrlc())
			{
				return (-1);
			}
		}
#endif		
	}

	
	return (p - buf);
}

int cmdSerialUpload(char *outBuffer, size_t bufferLen )
{
	char	buffer[IFLASH_PAGE_SIZE];
	uint32_t	length, size;
	ulong addr;
	uint32_t total = 0;
	char *env_echo;
	int ret;
	int startPage = FLASH_START_PAGE_OS;

	printf("## Ready for serial download ...:"MUX_NEW_LINE);
	
	size = sizeof(buffer);

	do
	{
		length = _readSerialPort(buffer, size, (total==0));
		total += length;
//		printf("read %d, toal:%d"MUX_NEW_LINE, length, total);
		if(length <= 0)
		{/* EOF or timeout */
			printf("Timeout on serial port"MUX_NEW_LINE);
			break;
		}
		else if(length < sizeof(buffer))
		{/* EOF */
			printf("read %lu, total:%lu"MUX_NEW_LINE, length, total);
			bspConsoleDumpMemory(buffer, length, total);
		}

#if 0
		ret = efcFlashProgram(startPage, buffer, length);
		if(ret == EXIT_FAILURE)
		{
			break;
		}
		
		startPage++;
#endif

		if (!_do_echo)
		{	/* print a '.' every 100 lines */
			bspConsolePutChar('.');
		}

	}while(length == size );

	printf("download %lu bytes\n", total);

	return MUX_FALSE;
}
#endif

static unsigned int	 _loadSerialYmodem(unsigned int	offset, int mode)
{
	int size;
	int err;
	int res;
	connection_info_t info;
//	unsigned int	store_addr = ~0;
	unsigned int	addr = 0;
	int startPage = FLASH_START_PAGE_OS;
	char ret;

	size = 0;
	info.mode = mode;

	res = xyzModem_stream_open(&info, &err);
	if (!res)
	{
		while ((res = xyzModem_stream_read((char *)muxRun.bufRead, muxRun.bufLength, &err)) > 0)
		{
//			store_addr = addr + offset;
			size += res;
			addr += res;

			ret = efcFlashProgram(startPage, muxRun.bufRead, res);
			if(ret == EXIT_FAILURE)
			{
				break;
			}
			
			startPage +=2;
		}
	}
	else
	{
		printf("%s\n", xyzModem_error(err));
	}

	xyzModem_stream_close(&err);
	xyzModem_stream_terminate(false, bspGetcXModem);


//	flush_cache(offset, ALIGN(size, ARCH_DMA_MINALIGN));

	printf("## Total Size      = 0x%08x = %d Bytes"MUX_NEW_LINE, size, size);
//	setenv_hex("filesize", size);

	return offset;
}


static char _cmdSerialXmodemLoad(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen)
{
	unsigned int	 offset = 0;

	printf("## Ready for binary (xmodem) download...:"MUX_NEW_LINE);
	_loadSerialYmodem(offset, xyzModem_xmodem);

	return MUX_FALSE;
}


static char _cmdSerialYmodemLoad(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen)
{
	unsigned int	 offset = 0;
//	unsigned int	 addr;

	printf("## Ready for binary (ymodem) download...:"MUX_NEW_LINE );
	_loadSerialYmodem(offset, xyzModem_ymodem);

	return MUX_FALSE;
}



static char	_cmdBoot(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen )
{
	strncpy( outBuffer, "startup OS.....", bufferLen );

	MUX_LOAD_OS();
	return MUX_FALSE;
}



_CODE  MUX_CLI_CMD_T muxCmds[] =
{

#if 0
	{
		name	: MUX_CMD_LOAD,
		helpString: MUX_NEW_LINE MUX_CMD_LOAD" :"MUX_NEW_LINE" load firmware(binary) through serial port  "MUX_NEW_LINE,
		callback: cmdSerialUpload,
	},
#endif
	{
		name	: MUX_CMD_LOAD_X,
		helpString: CMD_HELP_LOADX,
		callback: _cmdSerialXmodemLoad,
	},

	{
		name	: MUX_CMD_LOAD_Y,
		helpString: CMD_HELP_LOADY,
		callback: _cmdSerialYmodemLoad,
	},
	
	{
		name	: MUX_CMD_LOAD_FPGA_X,
		helpString: CMD_HELP_LOAD_FPGA_X,
		callback: bspCmdSpiFlashXmodemLoad,
	},

	{
		name	: MUX_CMD_LOAD_FPGA_Y,
		helpString: CMD_HELP_LOAD_FPGA_Y,
		callback: bspCmdSpiFlashYmodemLoad,
	},

	{
		name	: MUX_CMD_BOOT,
		helpString: MUX_NEW_LINE MUX_CMD_BOOT":"MUX_NEW_LINE" Boot RTOS"MUX_NEW_LINE,
		callback: _cmdBoot,
	},
	
	{
		name	: MUX_CMD_EFC_FLASH,
		helpString: CMD_HELP_EFC_FLASH,
		callback: bspCmdInternalFlash,
	},

	{
		name	: MUX_CMD_SPI_FLASH_READ,
		helpString: CMD_HELP_SPI_FLASH,
		callback: bspCmdSpiFlashRead,
	},

	{
		name	: MUX_CMD_SPI_FLASH_ERASE,
		helpString: CMD_HELP_SPI_FLASH_ERASE,
		callback: bspCmdSpiFlashErase,
	},

	{
		name	: MUX_CMD_BIST,
		helpString: CMD_HELP_BIST,
		callback: bspCmdBIST,
	},

	{
		name	: MUX_CMD_UPDATE,
		helpString: CMD_HELP_UPDATE,
		callback: cmnCmdUpdate,
	},
	
	{
		name	: MUX_CMD_FACTORY,
		helpString: CMD_HELP_FACTORY,
		callback: bspCmdFactory,
	},

	{
		name	: MUX_CMD_REBOOT,
		helpString: CMD_HELP_REBOOT,
		callback: bspCmdReboot,
	},
	
	{
		name	: MUX_CMD_VERSION,
		helpString: CMD_HELP_VERSION,
		callback: cmnCmdVersion,
	},
	{
		name	: MUX_CMD_DEFAULT,
		helpString: CMD_HELP_HELP,
		callback: cmnCmdHelp,
	},
	{
		name	: NULL,
		helpString: NULL,
		callback: NULL,
	}

};



