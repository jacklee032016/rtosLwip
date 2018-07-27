/* 
*/

#ifndef __FLASHD_H__
#define __FLASHD_H__

#include "compact.h"

struct _flash
{
	Efc			*eefc;
	uint32_t		total_size;
	uint32_t		page_size;
	uint32_t		erase_size;
	uint32_t		lock_count;
	uint32_t		gpnvm_count;
};


/**
 * \brief Initializes the flash driver.
 * \param flash pointer to the flash driver structure
 * \param eefc pointer to the flash peripheral to configure
 */
int flashd_initialize(struct _flash* flash, Efc* eefc);

/**
 * \brief Read the 16-byte unique ID.
 * \param flash pointer to the flash driver structure
 * \param id pointer to a 16-byte buffer that will be updated with the unique ID.
 * \returns 0 on success; otherwise returns a negative error code.
 */
int flashd_read_unique_id(struct _flash* flash, uint8_t* uid);

/**
 * \brief Read the value of a GPNVM bit.
 * \param flash pointer to the flash driver structure
 * \param gpnvm GPNVM bit index.
 * \param value pointer to a boolean that will be updated with the GPNVM bit value.
 * \returns 0 on success; otherwise returns a negative error code.
 */
int flashd_get_gpnvm(struct _flash* flash, uint8_t gpnvm, bool* value);

/**
 * \brief Read the value of several consecutive GPNVM bits.
 * \param flash pointer to the flash driver structure
 * \param gpnvm_start GPNVM start bit index (inclusive)
 * \param gpnvm_end GPNVM end bit index (inclusive)
 * \param value pointer to a word whose lower bits will be updated with the
 *        GPNVM bit values (gpnvm_start into bit0, gpnvm_start+1 into bit1,
 *        etc.)
 * \returns 0 on success; otherwise returns a negative error code.
 */
int flashd_get_gpnvm_bits(struct _flash* flash, uint8_t gpnvm_start, uint8_t gpnvm_end, uint32_t* value);

/**
 * \brief Set the value of a GPNVM bit.
 * \param flash pointer to the flash driver structure
 * \param gpnvm GPNVM bit index.
 * \param value GPNVM bit value.
 * \returns 0 on success; otherwise returns a negative error code.
 */
int flashd_set_gpnvm(struct _flash* flash, uint8_t gpnvm, bool value);

/**
 * \brief Set the value of several consecutive GPNVM bits.
 * \param flash pointer to the flash driver structure
 * \param gpnvm_start GPNVM start bit index (inclusive)
 * \param gpnvm_end GPNVM end bit index (inclusive)
 * \param value word whose lower bits represent the GPNVM bit values to set
 *        (bit0=gpnvm_start, bit1=gpnvm_start+1, etc.)
 * \returns 0 on success; otherwise returns a negative error code.
 */
int flashd_set_gpnvm_bits(struct _flash* flash, uint8_t gpnvm_start, uint8_t gpnvm_end, uint32_t value);

/**
 * \brief Erase the whole flash
 * \param flash pointer to the flash driver structure
 * \returns 0 on success; otherwise returns a negative error code.
 */
int flashd_erase(struct _flash* flash);

/**
 * \brief Erase a single flash block
 * \param flash pointer to the flash driver structure
 * \param addr offset into the flash of the block to erase
 * \param length number of bytes to erase (block size)
 * \returns 0 on success; otherwise returns a negative error code.
 */
int flashd_erase_block(struct _flash* flash, uint32_t addr, uint32_t length);

/**
 * \brief Read data from the flash
 * \param flash pointer to the flash driver structure
 * \param addr start offset into the flash for the read operation
 * \param length number of bytes to read
 * \returns 0 on success; otherwise returns a negative error code.
 * \note The flash can also be read directly at address IFLASH_ADDR.
 */
int flashd_read(struct _flash* flash, uint32_t addr, uint8_t* data, uint32_t length);

/**
 * \brief Write data from the flash
 * \param flash pointer to the flash driver structure
 * \param addr start offset into the flash for the write operation
 * \param data pointer to the data to write into the flash
 * \param length number of bytes to write
 * \returns 0 on success; otherwise returns a negative error code.
 */
int flashd_write(struct _flash* flash, uint32_t addr, const uint8_t* data, uint32_t length);

#endif

