
#include "muxSysParams.h"
#include "compact.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char bspCfgRead( MUX_RUNTIME_CFG *cfg, MUX_CFG_TYPE cfgType)
{
	int startPage = (cfgType== MUX_CFG_MAIN)?FLASH_START_PAGE_CONFIGURATION:FLASH_START_PAGE_CONFIG_BACKUP;
	char ret;

//	if(efcFlashRead(startPage, (unsigned char *)cfg, sizeof(MUX_RUNTIME_CFG) ) == EXIT_FAILURE)
	ret = efcFlashRead(startPage, (unsigned char *)cfg, MUX_RUNTIME_CFG_WRITE_SIZE);
	MUX_DBG_ERRORF(("Read configuration"MUX_NEW_LINE), (ret==EXIT_SUCCESS), return EXIT_FAILURE);

	if(cfg->bootMode == BOOT_MODE_BOOTLOADER)
	{/* in order to make old bootloader compatible with new RTOS */
		return EXIT_SUCCESS;
	}
	
	if(cfg->magic[0]!=MUX_MAGIC_VALUE_A ||cfg->magic[1] != MUX_MAGIC_VALUE_B ||
		cfg->endMagic[0]!=MUX_MAGIC_VALUE_B ||cfg->endMagic[1] != MUX_MAGIC_VALUE_A)
	{
		MUX_ERRORF(("Configuration data is not corrupted!!"MUX_NEW_LINE));
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

char bspCfgSave( MUX_RUNTIME_CFG *cfg, MUX_CFG_TYPE cfgType )
{
	int startPage = (cfgType== MUX_CFG_MAIN)?FLASH_START_PAGE_CONFIGURATION:FLASH_START_PAGE_CONFIG_BACKUP;
	char ret = efcFlashProgram(startPage, (unsigned char *)cfg, MUX_RUNTIME_CFG_WRITE_SIZE );

	MUX_DELAY_MS(300); /* make Flash more stable */

//	if(efcFlashProgram(startPage, (unsigned char *)cfg, sizeof(MUX_RUNTIME_CFG) ) == EXIT_FAILURE)
	MUX_DBG_ERRORF(("Save configuration"MUX_NEW_LINE), (ret==EXIT_SUCCESS), return EXIT_FAILURE);
	
	return EXIT_SUCCESS;
}

