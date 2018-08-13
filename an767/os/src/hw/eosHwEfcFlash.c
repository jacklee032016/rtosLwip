/*
* This module is not used now, only bspHwEfcFlash is used
*/

#include "eos.h"

/** Flash wait state number. */
#define	IFLASH_WAIT_STATE_NBR		(6)
#define 	__BUFFER_SIZE            (IFLASH_PAGE_SIZE / 4)


void extBspEfcFlashReadInfo(void)
{
	uint32_t ul_rc;
	uint32_t unique_id[4];

	uint32_t descriptor[16];

	/* Initialize Flash service */
	ul_rc = flash_init(FLASH_ACCESS_MODE_128, 4);
	if (ul_rc != FLASH_RC_OK)
	{
		printf("-F- Initialization error %lu\n\r", (unsigned long)ul_rc);
		return;
	}

	/* Read the unique ID */
	puts("-I- Reading 128 bits Unique Identifier\r");
	ul_rc = flash_read_unique_id(unique_id, 4);
	if (ul_rc != FLASH_RC_OK)
	{
		printf("-F- Read the Unique Identifier error %lu\n\r", (unsigned long)ul_rc);
		return;
	}


	printf("-I- ID: 0x%08lu, 0x%08lu, 0x%08lu, 0x%08lu\n\r", (unsigned long)unique_id[0], (unsigned long)unique_id[1], (unsigned long)unique_id[2], (unsigned long)unique_id[3]);

	ul_rc= flash_get_descriptor(IFLASH_ADDR, descriptor, 16);

	printf("Flash: pages: %lu, regions:%lu,  pages/region:%lu\n\r", flash_get_page_count(descriptor), flash_get_region_count(descriptor), flash_get_page_count_per_region(descriptor) );

}



static char extBspEfcFlashWrite(uint32_t pageAddress,  const void *buffer, uint32_t size)
{
//	volatile	uint32_t *p_test_page_data;
	uint32_t ret;
	
//	uint32_t ul_idx;
//	uint8_t uc_key;
	
	/* Initialize flash: 6 wait states for flash writing. */
	ret = flash_init(FLASH_ACCESS_MODE_128, IFLASH_WAIT_STATE_NBR );
	if (ret != FLASH_RC_OK)
	{
		printf("-F- Initialization error %lu\n\r", (unsigned long)ret);
		return EXIT_FAILURE;
	}

	/* Unlock page */
	printf("-I- Unlocking test page: 0x%08lx\r\n", pageAddress);
	ret = flash_unlock(pageAddress, pageAddress + size - 1, 0, 0);
	if (ret != FLASH_RC_OK)
	{
		printf("-F- Unlock error %lu\n\r", (unsigned long)ret);
		return EXIT_FAILURE;
	}


	/* The EWP command is not supported for non-8KByte sectors in all devices
	 *  SAM4 series, so an erase command is requried before the write operation.
	 */
	ret = flash_erase_sector(pageAddress);
	if (ret != FLASH_RC_OK)
	{
		printf("-F- Flash programming error %lu\n\r", (unsigned long)ret);
		return EXIT_FAILURE;
	}

	ret = flash_write(pageAddress, buffer, size, 0);
	if (ret != FLASH_RC_OK)
	{
		printf("-F- Flash programming error %lu\n\r", (unsigned long)ret);
		return EXIT_FAILURE;
	}

#if 0
	/* Lock page */
	printf("-I- Locking test page\n\r");
	ret = flash_lock(pageAddress, pageAddress + size - 1, 0, 0);
	if (ret != FLASH_RC_OK)
	{
		printf("-F- Flash locking error %lu\n\r", (unsigned long)ret);
		return EXIT_FAILURE;
	}

	/* Set security bit */
	printf("-I- Setting security bit \n\r");
	ret = flash_enable_security_bit();
	if (ret != FLASH_RC_OK)
	{
		printf("-F- Set security bit error %lu\n\r", (unsigned long)ret);
	}
#endif

	return EXIT_SUCCESS;
}

#if 0
static void extBspEfcFlashRead(uint32_t address,  const void *buffer, uint32_t size)
{
	unsigned int i;
	uint32_t *pageBuffer= (uint32_t *)buffer;
	volatile	uint32_t *testPage = (volatile uint32_t *)address;

	if( (address< IFLASH_ADDR) || (address> IFLASH_ADDR + IFLASH_SIZE) )	
	{
		printf("\n\r-F- Invalidate address 0x%08lu\n\r", address);
		return;
	}

	for (i = 0; i < size/4; i++)
	{
		pageBuffer[i] = testPage[i];
	}

}
#endif

void extBspEfcFlashTestWrite(void)
{
	unsigned int i;
	uint32_t pageBuffer[__BUFFER_SIZE];
	
	volatile	uint32_t *testPage = (volatile uint32_t *)TEST_PAGE_ADDRESS;

	/* Write page */
	printf("-I- Writing test page with walking bit pattern\n\r");
	for (i = 0; i < __BUFFER_SIZE; i++)
	{
		pageBuffer[i] = 1 << (i % MAX_SHIFTING_NUMBER);
	}

	extBspEfcFlashWrite(TEST_PAGE_ADDRESS, pageBuffer, IFLASH_PAGE_SIZE);

	/* Validate page */
	printf("-I- Checking page contents ");
	for(i = 0; i < (IFLASH_PAGE_SIZE / 4); i++)
	{
		printf(".");
		if ( testPage[i] != pageBuffer[i])
		{
			printf("\n\r-F- data error on address %d\n\r", i);
			return;
		}
	}
	printf("OK\n\r");
}

