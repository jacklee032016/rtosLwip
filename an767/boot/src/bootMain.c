/*
 */

#include "compact.h"
#include "boot.h"
#include "bspHwSpiFlash.h"

#include <unistd.h>	/* for write() */


_CODE char *versionString = EXT_BL_NAME;

//CACHE_ALIGNED 
static char	cmd_buffer[CMD_BUFFER_SIZE];
static char	outBuffer[MAX_STRING_SIZE];

static volatile uint32_t inputIndex = 0;
static volatile bool cmd_complete = false;

static const char * const promptMessage = EXT_NEW_LINE"MuxLab command line, Type 'help' to view a list of commands."EXT_NEW_LINE">";

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
				cmd_complete = EXT_TRUE;
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
			if (inputIndex < (EXT_ARRAYSIZE(cmd_buffer) - 1))
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
	cmd_complete = EXT_FALSE;
	

	key = bspConsoleGetChar();

	_consoleHandler(key);

	return cmd_complete;
}

static int	__bootUpdateRtos(EXT_RUNTIME_CFG *runCfg)
{
	int	efcFlashStartPage = FLASH_START_PAGE_OS;
//	int spiOffset = 0;
	int	length= 0;
	unsigned int 	total = 0;
	char 	ret;

	if (efcFlashInit() != EXIT_SUCCESS)
	{
		printf("EFC Flash initialization error!");
		return EXT_FALSE;
	}

	/* read from SPI flash and write into EFC flash */
	bspSpiFlashInit(0, FLASH_N250_SECTOR_ADDRESS(SFLASH_START_SECTOR_TEMP_RTOS), EXT_FALSE);

	printf( ("Update RTOS"EXT_NEW_LINE));
	while(total < runCfg->firmUpdateInfo.size )
	{
		length = bspSpiFlashRead(runCfg->bufRead, runCfg->bufLength);
		if(length < 0)
		{
			EXT_ERRORF(("Update OS failed after read %d bytes"EXT_NEW_LINE, total) );
			break;
		}
		if(length != runCfg->bufLength)
		{
			EXT_INFOF(("Last read %d bytes from SPI Flash", length) );
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

	printf("" EXT_NEW_LINE);
	if( total/runCfg->bufLength != (runCfg->firmUpdateInfo.size+runCfg->bufLength-1)/runCfg->bufLength )
	{
		EXT_ERRORF( ("Update OS image with wrong image size: %d bytes(%d)"EXT_NEW_LINE, total, runCfg->firmUpdateInfo.size) )
	}
	else
	{
		printf( "Update total %d pages of EFC Flash"EXT_NEW_LINE, efcFlashStartPage-FLASH_START_PAGE_OS );
	}	

	return total;
}

/* update firmware from backup zone in SPI flash */
static int	_bootUpdate(EXT_RUNTIME_CFG *runCfg)
{
	int	length= 0;

	if(runCfg->firmUpdateInfo.type == EXT_FM_TYPE_RTOS)
	{
		length = __bootUpdateRtos(runCfg);
	}
	else if(runCfg->firmUpdateInfo.type == EXT_FM_TYPE_FPGA)
	{
		length = bspBootUpdateFpga(runCfg);
	}
	else
	{
		return 0;
	}
	
	printf("" EXT_NEW_LINE);
#if 0
	if( (unsigned int)length != runCfg->firmUpdateInfo.size )
	{
		EXT_ERRORF( ("Update OS image with wrong image size %d bytes, it should be %d"EXT_NEW_LINE, length, runCfg->firmUpdateInfo.size) )
	}
	else
	{
		printf( "Update total %d bytes firmware"EXT_NEW_LINE, length );
	}	
#endif

	{/* disable update when next bootup */
		memset(&runCfg->firmUpdateInfo, 0 , sizeof(EXT_FM_UPDATE));
		bspCfgSave(runCfg, EXT_CFG_MAIN);
	}

	EXT_DELAY_MS(500);
//	EXT_INFOF(("Update RTOS: %d bytes"EXT_NEW_LINE, total));

	return length;
}

uint32_t sys_get_ms(void)
{
	return 0;
}


void	wakeResetInIsr(void )
{
}

int main( void )
{
#if (RESET_BTN_MODE == _RESET_BTN_STAY_IN_BOOTLOADER)
	char stayInBootloader = 0;
#endif

	bspHwInit(BOOT_MODE_BOOTLOADER);

	_bootUpdate(&extRun);
	
#if (RESET_BTN_MODE == _RESET_BTN_STAY_IN_BOOTLOADER)
	stayInBootloader = gpio_pin_is_low(PIO_PA30_IDX);
	if(stayInBootloader)
	{
		printf("reset button is low, stay in bootloader"EXT_NEW_LINE);
	}
#if 0
	else
	{
//		printf("reset button is High"EXT_NEW_LINE);
	}
#endif

	if(!extRun.isUpdate && stayInBootloader == 0)
#else
	if(!extRun.isUpdate )
#endif
	{
		EXT_LOAD_OS();
	}
	
#if EXTLAB_BOARD
#else
	ioport_set_pin_level(LED0_GPIO, 0);
#endif
#if 0
	bspConsoleReset(EXT_SYSTEM_STRING);
#else
//	printf(versionString);
#endif

#if 0
	puts("Startup.....OK!"EXT_NEW_LINE);
	
	printf("Test1...\r\n" );
	printf("Test11...\n\r" );
	puts("Startup2.....OK!"EXT_NEW_LINE);
	printf("Test2..."EXT_NEW_LINE);
	printf("Test3..."EXT_NEW_LINE);
#endif
	printf(EXT_NEW_LINE"Bootloader beginning..." EXT_NEW_LINE);

	bspButtonConfig(BOOT_MODE_BOOTLOADER, EXT_TRUE);
	
//	printf(EXT_SYSTEM_STRING);

#if 0
	/* disable reset button in bootloader. J.L. July 25, 2018 */
	/* delay to wait releasing button */
	EXT_DELAY_MS(1000);

	bspButtonConfig(BOOT_MODE_RTOS, EXT_TRUE);
#endif	
	/* Initialize flash driver */

	cmd_complete = EXT_FALSE;
	inputIndex = 0;

//	printf(promptMessage);
	BOOT_PROMPT();

	while (1)
	{
//		cpu_idle();

		if (_consoleMonitor())
		{
			char continues = EXT_FALSE;
			do
			{
				continues = cmnCmdLineProcess((const char * const) cmd_buffer, outBuffer, sizeof(outBuffer));
				printf(outBuffer);
			}while(continues == EXT_TRUE);

			inputIndex = 0;
//			printf(promptMessage);
			BOOT_PROMPT();
			
		}
	}
	
	return 0;
}

