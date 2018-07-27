/*
*  Internal Flash in MCU, EFC, Embedded Flash Controller
*/ 

#include "compact.h"
#include "flashd.h"

#include <stdlib.h>

struct _flash flash;

#define __IFLASH_BUFFER_SIZE            (IFLASH_PAGE_SIZE / 4)

//CACHE_ALIGNED 
static uint8_t read_buffer[IFLASH_PAGE_SIZE];

char efcFlashRead(uint32_t pageNo, unsigned char *data, uint32_t size)
{
	int rc;
	uint32_t address = pageNo*flash.page_size;

	rc = flashd_read(&flash, address, data, size);
	if (rc != FLASH_RC_OK)
	{
		MUX_ERRORF(("IFlash Read failed (errno=%d)"MUX_NEW_LINE, rc));
		return EXIT_FAILURE;
	}

#if 0
	printf("Read complete (%u bytes): "MUX_NEW_LINE, (unsigned int)size);
	CONSOLE_DEBUG_MEM(data, size, address, "read flash");
#endif

	return EXIT_SUCCESS;
}

static void efcFlashWrite(uint32_t pageNo, const uint8_t* buffer, uint32_t size)
{
	int rc;
	uint32_t address = pageNo*flash.page_size;
	
	rc = flashd_write(&flash, address, (uint8_t*)buffer, size);
	if (rc != FLASH_RC_OK)
	{
		MUX_ERRORF(("IFlash Write failed (errno=%d)"MUX_NEW_LINE, rc));
	}
	else
	{
		printf("IFlash Write complete (%d bytes)"MUX_NEW_LINE, (int)size);
	}
}

static void _efcFlashErase(uint32_t pageNo)
{
	int rc;
	uint32_t address = pageNo*flash.page_size;
	rc = flashd_erase_block(&flash, address, flash.erase_size);
	if (rc != FLASH_RC_OK)
	{
		MUX_ERRORF(("IFlash Erase failed (errno=%d)"MUX_NEW_LINE, rc));
	}
	else
	{
		printf("IFlash Erase complete (%lu bytes on 0x%08X of page %lu)"MUX_NEW_LINE, flash.erase_size, (unsigned int)address, pageNo);
	}
}

char efcFlashProgram(uint32_t pageNo, const uint8_t* buffer, uint32_t size)
{
	int rc;
	uint32_t address;
	
	address = pageNo*flash.page_size;
	if(pageNo%FLASH_ERASE_COUNT == 0)
	{
		rc = flashd_erase_block(&flash, address, flash.erase_size);
		if(rc != FLASH_RC_OK)
		{
			printf("Erase failed on Page#%lu(0x%08X)"MUX_NEW_LINE, pageNo, (unsigned int)address);
			return EXIT_FAILURE;
		}
	}
	
	rc = flashd_write(&flash, address, (uint8_t*)buffer, size);
	if (rc != FLASH_RC_OK )
	{
		printf("Write failed on Page#%lu(0x%08X)"MUX_NEW_LINE, pageNo, (unsigned int)address);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


static void efcFlashTestWrite(uint32_t pageNo)
{
	unsigned int i;
	uint32_t pageBuffer[__IFLASH_BUFFER_SIZE];
	
#if 0	
	volatile	uint32_t *testAddress = (volatile uint32_t *)IFLASH_ADDR+testPage*IFLASH_PAGE_SIZE;
#else
	uint32_t *testAddress = (uint32_t *)read_buffer;
#endif
	/* Write page */
//	printf("-I- Writing test page with walking bit pattern"MUX_NEW_LINE);

	for (i = 0; i < __IFLASH_BUFFER_SIZE; i++)
	{
		pageBuffer[i] = 1 << (i % MAX_SHIFTING_NUMBER);
	}

	_efcFlashErase(pageNo);
	efcFlashWrite(pageNo, (unsigned char *)pageBuffer, IFLASH_PAGE_SIZE);

	CONSOLE_DEBUG_MEM((uint8_t *)pageBuffer, IFLASH_PAGE_SIZE, 0, "raw data");

	efcFlashRead(pageNo, (unsigned char *)testAddress, IFLASH_PAGE_SIZE);

	/* Validate page */
	printf("-I- Checking page contents ");
	for(i = 0; i < (IFLASH_PAGE_SIZE / 4); i++)
	{
		printf(".");
		if ( testAddress[i] != pageBuffer[i])
		{
			MUX_ERRORF(("data error on address %d, 0x%08X!=0x%08X"MUX_NEW_LINE, i, (unsigned int)testAddress[i], (unsigned int)pageBuffer[i]));
			return;
		}
	}
	printf("OK\n\r");
}

char	efcFlashInit(void)
{
	if (flashd_initialize(&flash, EFC) < 0)
	{
		MUX_ERRORF(("Flash initialization error!"MUX_NEW_LINE));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

char bspCmdInternalFlash(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
//	unsigned int address = 0, temp;
//	unsigned char val;
	unsigned int page = 0;

	if (flashd_initialize(&flash, EFC) < 0)
	{
		index += snprintf(outBuffer+index, bufferLen-index, "Flash initialization error!"MUX_NEW_LINE);
		return MUX_FALSE;
	}
	
	if(argc==1)
	{
		index += snprintf(outBuffer+index, bufferLen-index, "Flash Info:"MUX_NEW_LINE"\tSize:\t\t%u bytes"MUX_NEW_LINE, (unsigned)flash.total_size);
		index += snprintf(outBuffer+index, bufferLen-index, "\tPage Size:\t%u bytes"MUX_NEW_LINE, (unsigned)flash.page_size);
		index += snprintf(outBuffer+index, bufferLen-index, "\tErase Size:\t%u bytes"MUX_NEW_LINE, (unsigned)flash.erase_size);
		index += snprintf(outBuffer+index, bufferLen-index, "\tLock Count:\t%u bytes"MUX_NEW_LINE, (unsigned)flash.lock_count);
		index += snprintf(outBuffer+index, bufferLen-index, "\tGPNVM Count:\t%u bits"MUX_NEW_LINE, (unsigned)flash.gpnvm_count);
		
		index += snprintf(outBuffer+index, bufferLen-index, "%s"MUX_NEW_LINE, cmd->helpString );
	}
	else if(argc >= 2)
	{

		*outBuffer = 0; /* clean buffer */
		if(argc >= 3)
		{
			page = atoi(argv[2]);
		}

//		printf("EFC Flash Testing: '%s'"MUX_NEW_LINE, argv[1] );
		if(! strncmp(argv[1], "w", 1) )
		{
			if(page < FLASH_START_PAGE_CONFIG_BACKUP)
			{
				printf("In order to make OS usable, Page# must be %d--%d when Write"MUX_NEW_LINE, FLASH_START_PAGE_CONFIG_BACKUP, IFLASH_NB_OF_PAGES-1);
				page = FLASH_START_PAGE_CONFIG_BACKUP;
			}
			printf("EFC Flash Write Testing on page #.%u: "MUX_NEW_LINE, page);
			efcFlashTestWrite(page);
		}
		else if(! strncmp(argv[1], "r", 1) )
		{
			printf("EFC Flash Read Testing on page #.%u: "MUX_NEW_LINE, page);
			
			efcFlashRead(page, read_buffer, flash.page_size);
//			printf("Read complete (%u bytes): "MUX_NEW_LINE, (unsigned int)flash.page_size);
			CONSOLE_DEBUG_MEM(read_buffer, flash.page_size, page*flash.page_size, "read complete");
		}
		else if(! strncmp(argv[1], "e", 1) )
		{
			if(page < FLASH_START_PAGE_CONFIG_BACKUP)
			{
				printf("In order to make OS usable, Page# must be %d--%d when Erase"MUX_NEW_LINE, FLASH_START_PAGE_CONFIG_BACKUP, IFLASH_NB_OF_PAGES-1);
				page = FLASH_START_PAGE_CONFIG_BACKUP;
			}
			
			printf("EFC Flash Erase Testing on page #.%u: "MUX_NEW_LINE, page);
			_efcFlashErase(page);
			efcFlashRead(page, read_buffer, flash.page_size);
			CONSOLE_DEBUG_MEM(read_buffer, flash.page_size, page*flash.page_size, "erase complete");
		}
		else
		{
			index += snprintf(outBuffer+index, bufferLen-index, "'%s' is invalidate command: %s"MUX_NEW_LINE, argv[1], cmd->helpString );
		}
#if 0
		uint8_t unique_id[16];
		rc = flashd_read_unique_id(&flash, unique_id);
		if (rc == 0)
		{
			printf("Read UID complete: ");
			bspConsoleDumpFrame(unique_id, 16);
		}
		else
		{
			printf("Read UID failed (errno=%d)\r\n", rc);
		}
#else

#endif
	
		return MUX_FALSE;
	}
	else
	{
		index += snprintf(outBuffer+index, bufferLen-index, "'%s'"MUX_NEW_LINE, cmd->helpString );
	}

//	index += snprintf(outBuffer+index, bufferLen-index, "%s"MUX_NEW_LINE, "Internal Flash Testing" );

	return MUX_FALSE;
}

