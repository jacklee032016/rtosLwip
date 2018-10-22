
#include "extSysParams.h"
#include "compact.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char bspCfgRead( EXT_RUNTIME_CFG *cfg, EXT_CFG_TYPE cfgType)
{
	int startPage = (cfgType== EXT_CFG_MAIN)?FLASH_START_PAGE_CONFIGURATION:FLASH_START_PAGE_CONFIG_BACKUP;
	char ret;

//	if(efcFlashRead(startPage, (unsigned char *)cfg, sizeof(EXT_RUNTIME_CFG) ) == EXIT_FAILURE)
	ret = efcFlashRead(startPage, (unsigned char *)cfg, EXT_RUNTIME_CFG_WRITE_SIZE);
	EXT_DBG_ERRORF(("Read configuration"EXT_NEW_LINE), (ret==EXIT_SUCCESS), return EXIT_FAILURE);

	if(cfg->bootMode == BOOT_MODE_BOOTLOADER)
	{/* in order to make old bootloader compatible with new RTOS */
		cfg->rs232Cfg.baudRate = EXT_BAUDRATE_57600;
		cfg->rs232Cfg.charLength = EXT_RS232_CHAR_LENGTH_8;	/* 6 bits */
		cfg->rs232Cfg.parityType = EXT_RS232_PARITY_NONE;
		cfg->rs232Cfg.stopbits = EXT_RS232_STOP_BITS_1;
		return EXIT_SUCCESS;
	}
	
	if(cfg->magic[0]!=EXT_MAGIC_VALUE_A ||cfg->magic[1] != EXT_MAGIC_VALUE_B ||
		cfg->endMagic[0]!=EXT_MAGIC_VALUE_B ||cfg->endMagic[1] != EXT_MAGIC_VALUE_A)
	{
		EXT_ERRORF(("Configuration data is not corrupted!!"EXT_NEW_LINE));
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

char bspCfgSave( EXT_RUNTIME_CFG *cfg, EXT_CFG_TYPE cfgType )
{
	int startPage = (cfgType== EXT_CFG_MAIN)?FLASH_START_PAGE_CONFIGURATION:FLASH_START_PAGE_CONFIG_BACKUP;
	char ret = efcFlashProgram(startPage, (unsigned char *)cfg, EXT_RUNTIME_CFG_WRITE_SIZE );

	EXT_DELAY_MS(300); /* make Flash more stable */

//	if(efcFlashProgram(startPage, (unsigned char *)cfg, sizeof(EXT_RUNTIME_CFG) ) == EXIT_FAILURE)
	EXT_DBG_ERRORF(("Save configuration"EXT_NEW_LINE), (ret==EXIT_SUCCESS), return EXIT_FAILURE);
	
	return EXIT_SUCCESS;
}


char	 bspCmdFactory(const struct _EXT_CLI_CMD *cmd, char *outBuffer,  unsigned int bufferLen )
{
	extCfgFromFactory(&extRun);
	bspCfgSave(&extRun, EXT_CFG_MAIN);

	/* this function never return to its caller, so must output directly */
	if(outBuffer && bufferLen> 0 )
	{
		snprintf( outBuffer, (bufferLen),  "Factory configuration has been restored, reboot....."EXT_NEW_LINE EXT_NEW_LINE );
	}
	else
	{
		printf( "Factory configuration has been restored, reboot....."EXT_NEW_LINE EXT_NEW_LINE);
	}

#if 0
	/* CPU is busy for delay, so no output  */
	EXT_DELAY_MS(500); /* make output (serial port or telnet) available */
#else
#endif
	if(extRun.bootMode == BOOT_MODE_BOOTLOADER)
	{/* for RTOS, it is delayed reboot */
		EXT_REBOOT();
	}
	
	return EXT_FALSE;
}


char	 bspCmdReboot(const struct _EXT_CLI_CMD *cmd, char *outBuffer, size_t bufferLen )
{
#if 0
	EXT_DELAY_MS(1000);

	if(outBuffer)
	{
		strncpy( outBuffer, "rebooting.....", bufferLen );
	}
#else
	printf("rebooting....."EXT_NEW_LINE EXT_NEW_LINE );
#endif

	EXT_REBOOT();
	return EXT_FALSE;
}


