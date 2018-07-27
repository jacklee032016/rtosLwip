/*
 */

#include "compact.h"
#include "boot.h"
#include "bspHwSpiFlash.h"

#include <unistd.h>	/* for write() */


_CODE char *versionString = MUX_BL_NAME;

//CACHE_ALIGNED 
static char	cmd_buffer[CMD_BUFFER_SIZE];
static char	outBuffer[MAX_STRING_SIZE];

static volatile uint32_t inputIndex = 0;
static volatile bool cmd_complete = false;

static const char * const promptMessage = MUX_NEW_LINE"MuxLab command line, Type 'help' to view a list of commands."MUX_NEW_LINE">";

#define	BOOT_PROMPT()		\
		write(1,  ( signed char *) promptMessage, strlen(promptMessage))

		
static void _consoleHandler(uint8_t key)
{
	/* already processing a command: ignore input */
	if (cmd_complete)
	       return;

	switch (key)
	{
		case ASCII_KEY_CR:
		case ASCII_KEY_LF:
			if (inputIndex > 0)
			{
				bspConsoleEcho(key);
				cmd_buffer[inputIndex] = '\0';
				cmd_complete = MUX_TRUE;
			}
			else
			{
//				printf(promptMessage);
				BOOT_PROMPT();
			}
			break;
		case ASCII_KEY_DEL:
		case '\b':
			if (inputIndex > 0)
			{
				bspConsoleEcho(key);
				inputIndex--;
				cmd_buffer[inputIndex] = '\0';
			}
			break;
		default:
			if (inputIndex < (MUX_ARRAYSIZE(cmd_buffer) - 1))
			{
				bspConsoleEcho(key);
				cmd_buffer[inputIndex] = key;
				inputIndex++;
			}
			break;
	}
}

#if 0
static void _ledTestTask(void *param)
{
	param = param;
	while(1)
	{
		ioport_toggle_pin_level(LED0_GPIO);
	}
}
#endif

static char _consoleMonitor(void)
{
	char key;
	cmd_complete = MUX_FALSE;
	

	key = bspConsoleGetChar();

	_consoleHandler(key);

	return cmd_complete;
}

static int	__bootUpdateRtos(MUX_RUNTIME_CFG *runCfg)
{
	int	efcFlashStartPage = FLASH_START_PAGE_OS;
//	int spiOffset = 0;
	int	length= 0;
	unsigned int 	total = 0;
	char 	ret;

	if (efcFlashInit() != EXIT_SUCCESS)
	{
		printf("EFC Flash initialization error!");
		return MUX_FALSE;
	}

	/* read from SPI flash and write into EFC flash */
	bspSpiFlashInit(0, FLASH_N250_SECTOR_ADDRESS(SFLASH_START_SECTOR_TEMP_RTOS), MUX_FALSE);

	printf( ("Update RTOS"MUX_NEW_LINE));
	while(total < runCfg->firmUpdateInfo.size )
	{
		length = bspSpiFlashRead(runCfg->bufRead, runCfg->bufLength);
		if(length < 0)
		{
			MUX_ERRORF(("Update OS failed after read %d bytes"MUX_NEW_LINE, total) );
			break;
		}
		if(length != runCfg->bufLength)
		{
			MUX_INFOF(("Last read %d bytes from SPI Flash", length) );
		}
		
		ret = efcFlashProgram(efcFlashStartPage, runCfg->bufRead, length);
		if(ret == EXIT_FAILURE)
		{
			break;
		}
			
		efcFlashStartPage +=2;/* default 2 pages */

		total += length;
		printf(".");
			//putchar('.');
		
	}

	printf("" MUX_NEW_LINE);
	if( total/runCfg->bufLength != (runCfg->firmUpdateInfo.size+runCfg->bufLength-1)/runCfg->bufLength )
	{
		MUX_ERRORF( ("Update OS image with wrong image size: %d bytes(%d)"MUX_NEW_LINE, total, runCfg->firmUpdateInfo.size) )
	}
	else
	{
		printf( "Update total %d pages of EFC Flash"MUX_NEW_LINE, efcFlashStartPage-FLASH_START_PAGE_OS );
	}	

	return total;
}

/* update firmware from backup zone in SPI flash */
static int	_bootUpdate(MUX_RUNTIME_CFG *runCfg)
{
	int	length= 0;

	if(runCfg->firmUpdateInfo.type == MUX_FM_TYPE_RTOS)
	{
		length = __bootUpdateRtos(runCfg);
	}
	else if(runCfg->firmUpdateInfo.type == MUX_FM_TYPE_FPGA)
	{
		length = bspBootUpdateFpga(runCfg);
	}
	else
	{
		return 0;
	}
	
	printf("" MUX_NEW_LINE);
#if 0
	if( (unsigned int)length != runCfg->firmUpdateInfo.size )
	{
		MUX_ERRORF( ("Update OS image with wrong image size %d bytes, it should be %d"MUX_NEW_LINE, length, runCfg->firmUpdateInfo.size) )
	}
	else
	{
		printf( "Update total %d bytes firmware"MUX_NEW_LINE, length );
	}	
#endif

	{/* disable update when next bootup */
		memset(&runCfg->firmUpdateInfo, 0 , sizeof(MUX_FM_UPDATE));
		bspCfgSave(runCfg, MUX_CFG_MAIN);
	}

	MUX_DELAY_MS(500);
//	MUX_INFOF(("Update RTOS: %d bytes"MUX_NEW_LINE, total));

	return length;
}


int main( void )
{
#if (RESET_BTN_MODE == _RESET_BTN_STAY_IN_BOOTLOADER)
	char stayInBootloader = 0;
#endif

	bspHwInit(BOOT_MODE_BOOTLOADER);

	_bootUpdate(&muxRun);
	
#if (RESET_BTN_MODE == _RESET_BTN_STAY_IN_BOOTLOADER)
	stayInBootloader = gpio_pin_is_low(PIO_PA30_IDX);
	if(stayInBootloader)
	{
		printf("reset button is low, stay in bootloader"MUX_NEW_LINE);
	}
#if 0
	else
	{
//		printf("reset button is High"MUX_NEW_LINE);
	}
#endif

	if(!muxRun.isUpdate || stayInBootloader == 0)
#else
	if(!muxRun.isUpdate )
#endif
	{
		MUX_LOAD_OS();
	}
	
#if MUXLAB_BOARD
#else
	ioport_set_pin_level(LED0_GPIO, 0);
#endif
#if 0
	bspConsoleReset(MUX_SYSTEM_STRING);
#else
//	printf(versionString);
#endif

#if 0
	puts("Startup.....OK!"MUX_NEW_LINE);
	
	printf("Test1...\r\n" );
	printf("Test11...\n\r" );
	puts("Startup2.....OK!"MUX_NEW_LINE);
	printf("Test2..."MUX_NEW_LINE);
	printf("Test3..."MUX_NEW_LINE);
#endif
	printf(MUX_NEW_LINE"Bootloader beginning..." MUX_NEW_LINE);
	
//	printf(MUX_SYSTEM_STRING);

#if 0
	/* disable reset button in bootloader. J.L. July 25, 2018 */
	/* delay to wait releasing button */
	MUX_DELAY_MS(1000);

	bspButtonConfig(BOOT_MODE_RTOS);
#endif	
	/* Initialize flash driver */

	cmd_complete = MUX_FALSE;
	inputIndex = 0;

//	printf(promptMessage);
	BOOT_PROMPT();

	while (1)
	{
//		cpu_idle();

		if (_consoleMonitor())
		{
			char continues = MUX_FALSE;
			do
			{
				continues = bspCmdProcess((const char * const) cmd_buffer, outBuffer, sizeof(outBuffer));
				printf(outBuffer);
			}while(continues == MUX_TRUE);

			inputIndex = 0;
//			printf(promptMessage);
			BOOT_PROMPT();
			
		}
	}
	
	return 0;
}

