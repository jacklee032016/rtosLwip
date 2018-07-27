/* 
 */

#include "flashd.h"

#define	MAX_PAGE_SIZE			512
#define	MAX_LOCK_REGIONS		128

#if 0
static void eefc_enable_frdy_it(Efc* eefc)
{
	eefc->EEFC_FMR |= EEFC_FMR_FRDY;
}
static void eefc_set_flash_wait_states(Efc* eefc, uint8_t cycles)
{
	eefc->EEFC_FMR = (eefc->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | EEFC_FMR_FWS(cycles);
}

static uint32_t eefc_get_status(Efc* eefc)
{
	return eefc->EEFC_FSR;
}
#endif

static void eefc_disable_frdy_it(Efc* eefc)
{
	eefc->EEFC_FMR &= ~EEFC_FMR_FRDY;
}


//SECTION(".ramfunc")
static int eefc_read_unique_id(Efc* eefc, uint8_t* uid)
{
	uint32_t status;
	int i;
TRACE();

	if (!uid)
	{
TRACE();
		return FLASH_RC_INVALID;
	}

	/* Disable interrupts (see following comment) */
//	arch_irq_disable();

TRACE();
	/* From this point, no code located in flash should be called: CPU
	 * cannot fetch code from flash during "Read Unique Identifier"
	 * procedure. */

	/* Send the Start Read unique Identifier command (STUI) */
	eefc->EEFC_FCR = EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_STUI;

TRACE();
	/* Wait for Unique Identifier to be ready (i.e. !FRDY) */
	do
	{
		status = eefc->EEFC_FSR;
	} while ((status & EEFC_FSR_FRDY) == EEFC_FSR_FRDY);

	/* The Unique Identifier is located in the first 128 bits of the Flash */
	for (i = 0; i < 16; i++)
	{
		uid[i] = *(uint8_t*)(IFLASH_ADDR + i);
	}

	/* Send the Stop Read unique Identifier command (SPUI) */
	eefc->EEFC_FCR = EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_SPUI;

TRACE();
	/* Wait for Stop Unique Identifier to be complete (i.e. FRDY) */
	do {
		status = eefc->EEFC_FSR;
	} while ((status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY);

TRACE();
	/* Flash is available again, re-enable interrupts */
//	arch_irq_enable();

	return FLASH_RC_OK;
}

static int eefc_perform_command(Efc* eefc, uint32_t cmd, uint32_t arg)
{
	uint32_t status;

	eefc->EEFC_FCR = EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FARG(arg) | (cmd & EEFC_FCR_FCMD_Msk);
	do
	{
		status = eefc->EEFC_FSR;
	} while ((status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY);

	if (status & EEFC_FSR_FLOCKE)
		return FLASH_RC_NOPERM;
	else if (status & EEFC_FSR_FLERR)
		return FLASH_RC_ERROR;
	else if (status & EEFC_FSR_FCMDE)
		return FLASH_RC_INVALID;

	return FLASH_RC_OK;
}

static uint32_t eefc_get_result(Efc* eefc)
{
	return eefc->EEFC_FRR;
}


static uint32_t page_buffer[MAX_PAGE_SIZE / sizeof(uint32_t)];

int flashd_initialize(struct _flash* flash, Efc* eefc)
{
	uint32_t i, rc = FLASH_RC_OK;
	uint32_t planes, plane_size;
	uint32_t lock_size;

	if (!flash)
		return FLASH_RC_INVALID;

	memset(flash, 0, sizeof(*flash));
	flash->eefc = eefc;

	eefc_disable_frdy_it(flash->eefc);

	rc = eefc_perform_command(flash->eefc, EEFC_FCR_FCMD_GETD, 0);
	if (rc != FLASH_RC_OK)
		return rc;

	/* FL_ID (skip value) */
	eefc_get_result(flash->eefc);

	/* FL_SIZE */
	flash->total_size = eefc_get_result(flash->eefc);

	/* FL_PAGE_SIZE */
	flash->page_size = eefc_get_result(flash->eefc);
	if (flash->page_size > MAX_PAGE_SIZE)
		return FLASH_RC_NOT_SUPPORT;

	/* FL_NB_PLANE (check that only 1 plane available) */
	planes = eefc_get_result(flash->eefc);
	if (planes != 1)
		return FLASH_RC_NOT_SUPPORT;

	/* FL_PLANE[0] (check that plane covers full flash) */
	plane_size = eefc_get_result(flash->eefc);
	if (plane_size != flash->total_size)
		return FLASH_RC_NOT_SUPPORT;

	/* FL_NB_LOCK (number of lock regions) */
	flash->lock_count = eefc_get_result(flash->eefc);
	if (flash->lock_count > MAX_LOCK_REGIONS)
		return FLASH_RC_NOT_SUPPORT;

	/* FL_LOCK[i] (size of lock regions, check that is coherent) */
	lock_size = flash->total_size / flash->lock_count;
	for (i = 0; i < flash->lock_count; i++)
	{
		uint32_t size = eefc_get_result(flash->eefc);
		if (size != lock_size)
			return FLASH_RC_NOT_SUPPORT;
	}

	/* FL_NB_GPNVM (number of GPNVM bits) */
	flash->gpnvm_count = eefc_get_result(flash->eefc);

	/* Report erase size (hard-coded for now) */
	/* 16 pages is the only value compatible with all sectors */
	flash->erase_size = 16 * flash->page_size;

	return FLASH_RC_OK;
}

int flashd_read_unique_id(struct _flash* flash, uint8_t* uid)
{
	int rc;

//	board_cfg_mpu_for_flash_write();
TRACE();

	rc = eefc_read_unique_id(flash->eefc, uid);

//	board_cfg_mpu_for_flash_read();
TRACE();

	return rc;
}

int flashd_get_gpnvm(struct _flash* flash, uint8_t gpnvm, bool* value)
{
	int rc;

	if (gpnvm >= flash->gpnvm_count)
		return FLASH_RC_INVALID;

	/* Get GPNVMs status */
	rc = eefc_perform_command(flash->eefc, EEFC_FCR_FCMD_GGPB, 0);
	if (rc != FLASH_RC_OK)
		return rc;

	/* Check if GPNVM is set */
	*value = ((eefc_get_result(flash->eefc) & (1 << gpnvm)) != 0);

	return 0;
}

int flashd_get_gpnvm_bits(struct _flash* flash, uint8_t gpnvm_start, uint8_t gpnvm_end, uint32_t* value)
{
	int rc, i;
	uint32_t val = 0;
	bool bit;

	if (gpnvm_start > gpnvm_end)
		return FLASH_RC_INVALID;
	if (gpnvm_start > flash->gpnvm_count)
		return FLASH_RC_INVALID;
	if (gpnvm_end > flash->gpnvm_count)
		return FLASH_RC_INVALID;

	for (i = gpnvm_end; i >= gpnvm_start; i--)
	{
		rc = flashd_get_gpnvm(flash, i, &bit);
		if (rc != FLASH_RC_OK)
			return rc;

		val <<= 1;
		if (bit)
			val |= 1;
	}

	*value = val;

	return FLASH_RC_OK;
}

int flashd_set_gpnvm(struct _flash* flash, uint8_t gpnvm, bool value)
{
	bool prev_value;
	int rc;

	rc = flashd_get_gpnvm(flash, gpnvm, &prev_value);
	if (rc != FLASH_RC_OK)
		return rc;

	if (prev_value != value)
	{
		rc = eefc_perform_command(flash->eefc, value ? EEFC_FCR_FCMD_SGPB : EEFC_FCR_FCMD_CGPB, gpnvm);
		if (rc != FLASH_RC_OK)
			return rc;
	}
	return FLASH_RC_OK;
}

int flashd_set_gpnvm_bits(struct _flash* flash, uint8_t gpnvm_start, uint8_t gpnvm_end, uint32_t value)
{
	uint32_t rc, i;

	if (gpnvm_start > gpnvm_end)
		return FLASH_RC_INVALID;
	if (gpnvm_start > flash->gpnvm_count)
		return FLASH_RC_INVALID;
	if (gpnvm_end > flash->gpnvm_count)
		return FLASH_RC_INVALID;

	for (i = gpnvm_start; i <= gpnvm_end; i++)
	{
		rc = flashd_set_gpnvm(flash, i, (value & 1) != 0);
		if (rc != FLASH_RC_OK)
			return rc;

		value >>= 1;
	}

	return FLASH_RC_OK;
}

int flashd_erase(struct _flash* flash)
{
//	cache_invalidate_region((void*)IFLASH_ADDR, flash->total_size);
	return eefc_perform_command(flash->eefc, EEFC_FCR_FCMD_EA, 0);
}

int flashd_erase_block(struct _flash* flash, uint32_t addr, uint32_t length)
{
	uint32_t page, farg;

	if (addr % flash->erase_size != 0)
		return FLASH_RC_INVALID;

	if (length != flash->erase_size)
		return FLASH_RC_INVALID;

	page = addr / flash->page_size;

	/* Get FARG field for EPA command:
	 * The first page to be erased is specified in the FARG[15:2] field of
	 * the MC_FCR register.
	 *
	 * The 2 lowest bits of the FARG field define the number of pages to
	 * be erased (FARG[1:0]).
	 */
	switch (flash->erase_size / flash->page_size) {
	case 32:
		farg = page | 3; /* 32 pages */
		break;
	case 16:
		farg = page | 2; /* 16 pages */
		break;
	case 8:
		farg = page | 1; /* 8 pages */
		break;
	case 4:
		farg = page; /* 4 pages */
		break;
	default:
		return FLASH_RC_INVALID;
	}

//	cache_invalidate_region((void*)(IFLASH_ADDR + addr), flash->erase_size);
	return eefc_perform_command(flash->eefc, EEFC_FCR_FCMD_EPA, farg);
}

int flashd_read(struct _flash* flash, uint32_t addr, uint8_t* data, uint32_t length)
{
	if ((addr + length) > flash->total_size)
		return FLASH_RC_INVALID;

	memcpy(data, (uint8_t*)(IFLASH_ADDR + addr), length);

	return FLASH_RC_OK;
}

int flashd_write(struct _flash* flash, uint32_t addr, const uint8_t* data, uint32_t length)
{
	uint32_t i, rc = FLASH_RC_OK;
	uint32_t offset = 0;

	if ((addr + length) > flash->total_size)
		return FLASH_RC_INVALID;

//	board_cfg_mpu_for_flash_write();

	while (offset < length)
	{
		/* Compute number of bytes to program in page */
		uint32_t page = (addr + offset) / flash->page_size;
		uint32_t page_offset = (addr + offset) - page * flash->page_size;
		uint32_t write_size =(length - offset)> (flash->page_size - page_offset)? (flash->page_size - page_offset):(length - offset);
		volatile uint32_t* flashptr = (uint32_t*)(IFLASH_ADDR + page * flash->page_size);

		/* if data does not cover full page, read existing data from flash */
		if (page_offset > 0 || write_size < flash->page_size)
		{
			for (i = 0; i < flash->page_size / sizeof(uint32_t); i++)
			{
				page_buffer[i] = flashptr[i];
			}
		}
		
		/* overwrite buffer with new data to flash */
		memcpy(((uint8_t*)page_buffer) + page_offset, data + offset, write_size);

		/* copy from buffer to flash, use barriers to force write order */
		for (i = 0; i < flash->page_size / sizeof(uint32_t); i++)
		{
			flashptr[i] = page_buffer[i];
//			dsb();
		}

		/* Send "erase write page" command */
		rc = eefc_perform_command(flash->eefc, EEFC_FCR_FCMD_WP, page);
		if (rc != FLASH_RC_OK)
			break;

		offset += write_size;
	}

//	board_cfg_mpu_for_flash_read();

	return rc;
}

