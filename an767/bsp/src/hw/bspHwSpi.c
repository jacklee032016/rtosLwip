

#include "compact.h"
#include "bsp.h"

#include "spi.h"


/* it must be (0,1) or (1, 0) for CPOL and NCPHA. Refers to datasheet of both chip
*/


/* SPI clock setting (Hz). Nicro flash only support 108MHz in STR mode */
static uint32_t gs_ul_spi_clock = 5000000;

#if 0
/* SPI clock configuration. */
static const uint32_t gs_ul_clock_configurations[] = { 500000, 1000000, 2000000, 5000000 };
#endif



/* Initialize SPI as master */
void bspSpiMasterInitialize(uint32_t pcs)
{
	Spi *spi = SPI_MASTER_BASE;
	
//	puts("-I- Initialize SPI as master\r");

	/* Configure an SPI peripheral. */
	spi_enable_clock(spi);

	spi_disable(spi);
	spi_reset(spi);
	
//	spi_set_lastxfer(spi);
	spi_set_master_mode(spi);
#if 1
	spi_disable_mode_fault_detect(spi);
#else
	spi_enable_mode_fault_detect(spi);
#endif

	spi_set_fixed_peripheral_select(spi);

	spi_set_peripheral_chip_select_value(spi, spi_get_pcs(pcs));

	/* CPOL:CPHA=0:0, means SPI mode 1: refer. page 972 table 40-4. JL. */
	spi_set_clock_polarity(spi, pcs, EXT_SPI_CLK_POLARITY);
	spi_set_clock_phase(spi, pcs, EXT_SPI_CLK_PHASE);

	spi_set_bits_per_transfer(spi,pcs, SPI_CSR_BITS_8_BIT);
	
	spi_set_baudrate_div(spi, pcs, sysclk_get_peripheral_hz()/ gs_ul_spi_clock) ;

	spi_set_transfer_delay(spi, pcs, EXT_SPI_DLYBS, EXT_SPI_DLYBCT);

	/* keep CS low, otherwise flash goes to standby */
	spi_configure_cs_behavior(spi, pcs, SPI_CS_KEEP_LOW);

#if 0
	/* test SPI master mode only */
	spi_enable_loopback(spi);
#endif

	spi_enable(spi);
}

/**
 * \brief Perform SPI master transfer.
 *
 * \param pbuf Pointer to buffer to transfer.
 * \param size Size of the buffer.
 */
void extBspSpiMasterTransfer(void *buf, uint32_t size)
{
	Spi *spi = SPI_MASTER_BASE;
	uint32_t i;
	uint8_t uc_pcs;
	static uint16_t data;
	uint8_t *p_buffer;

#ifdef DEBUG_SPI
	spi_status_t err;
#endif

	p_buffer = buf;

	for (i = 0; i < size; i++)
	{
#ifdef DEBUG_SPI
		err = spi_write(spi, p_buffer[i], spi_get_pcs(1), 0);
//		EXT_DBG_ERRORF(("SPI write failed"), (err==SPI_OK), while(1){});
#else
		spi_write(spi, p_buffer[i], 1, 1);
#endif
		/* Wait transfer done. */
		while ((spi_read_status(spi) & SPI_SR_RDRF) == 0);
		
#ifdef DEBUG_SPI
		err = spi_read(spi, &data, &uc_pcs);
//		EXT_DBG_ERRORF(("SPI read failed"), (err==SPI_OK), while(1){});
#else
		spi_read(spi, &data, &uc_pcs);
#endif
		p_buffer[i] = data;
	}
}

char extBspSpiSelectChip(uint32_t pcs)
{
	Spi *spi = SPI_MASTER_BASE;
	/* Assert all lines; no peripheral is selected. */
	spi->SPI_MR |= SPI_MR_PCS_Msk;

#if 0
	EXT_DBG_ERRORF(("PCSDEC=1\r\n"), (!(spi->SPI_MR & SPI_MR_PCSDEC)), return EXIT_FAILURE);
	spi_set_peripheral_chip_select_value(spi, spi_get_pcs(pcs));
#else
	if (spi->SPI_MR & SPI_MR_PCSDEC)
	{/* use CSRx reguster to Chip Select */
		/* The signal is decoded; allow up to 15 chips. */
#if 0//def DEBUG_SPI
		EXT_ASSERT("PCSDEC=1\r\n");
#endif
#if 0		
		if (chip > 14)
		{
			return SPI_ERROR_ARGUMENT;
		}

		spi->SPI_MR &= ~SPI_MR_PCS_Msk| 	(chip << SPI_MR_PCS_Pos);
#endif		
		return EXIT_FAILURE;
	}
	else
	{/* CS connect to device directly */
#if 0
		if (chip > 3)
		{
			return SPI_ERROR_ARGUMENT;
		}
#endif

#if 0//def DEBUG_SPI
		printf("PCSDEC=0 CS selected\r\n");
#endif
		spi_set_peripheral_chip_select_value(spi, spi_get_pcs(pcs));
	}
#endif

	return EXIT_SUCCESS;
}

char extBspSpiUnselectChip(uint32_t pcs)
{
	Spi *spi = SPI_MASTER_BASE;
	uint32_t timeout = SPI_TIMEOUT;

	while (!(spi->SPI_SR & SPI_SR_TXEMPTY ))
	{/* waiting TR is empty */
		if (!timeout--)
		{
			EXT_ERRORF(("timeout"));
			return EXIT_FAILURE;
		}
	}

#if 0//def DEBUG_SPI
	printf("CS unselected\r\n");
#endif
	/* Assert all lines; no peripheral is selected. */
	spi->SPI_MR |= SPI_MR_PCS_Msk;

	/* Last transfer, so de-assert the current NPCS if CSAAT is set. */
//	spi->SPI_CR = SPI_CR_LASTXFER;
	spi_set_lastxfer(spi);

	return EXIT_SUCCESS;
}


char extBspSpiReadPacket(unsigned char *data, unsigned int len)
{
	Spi *spi = SPI_MASTER_BASE;
	unsigned int timeout = SPI_TIMEOUT;
	size_t i=0;
	
	while(len)
	{
		timeout = SPI_TIMEOUT;
		
		while (!spi_is_tx_ready(spi))
		{
			if (!timeout--)
			{
				EXT_ERRORF(("Timeout in SPI read packet"));
				return EXIT_FAILURE;
			}
		}
		
		spi_put(spi, (uint16_t)EXT_SPI_MASTER_DUMMY);
		timeout = SPI_TIMEOUT;
		while (!spi_is_rx_ready(spi))
		{
			if (!timeout--)
			{
				EXT_ERRORF(("Timeout in SPI read packet2"));
				return EXIT_FAILURE;
			}
		}
		
		data[i] = (unsigned char) spi_get(spi);

		i++;
		len--;
	}
	
	return EXIT_SUCCESS;
}


char 	extBspSpiWritePacket(const unsigned char *data, unsigned int len)
{
	Spi *spi = SPI_MASTER_BASE;
	unsigned int timeout = SPI_TIMEOUT;
	size_t i=0;
	uint16_t val;

	while(len)
	{
		timeout = SPI_TIMEOUT;
		while (!spi_is_tx_ready(spi))
		{
			if (!timeout--)
			{
				EXT_ERRORF(("Timeout in SPI write packet"));
				return EXIT_FAILURE;
			}
		}
		
		val = (uint16_t)data[i];
		spi_put(spi, val);
		i++;
		len--;
	}
	
	return EXIT_SUCCESS;
}



#ifdef	DEBUG_SPI

void bspSpiDebug(spi_status_t stat)
{
	switch (stat)
	{
		case SPI_OK:
			return;
			break;
		case SPI_ERROR:
			printf( "Error: Generic SPI Error\n\r");
			break;
		case SPI_ERROR_TIMEOUT:
			printf("Error: SPI Timeout\n\r");
			break;
		case SPI_ERROR_ARGUMENT:
			printf("Error: SPI Argument\n\r");
			break;
		case SPI_ERROR_OVERRUN:
			printf("Error: SPI Overrun\n\r");
			break;
		case SPI_ERROR_MODE_FAULT:
			printf("Error: SPI Mode Fault\n\r");
			break;
		case SPI_ERROR_OVERRUN_AND_MODE_FAULT:
			printf("Error: SPI Overrun and Mode Fault\n\r");
			break;
		}
	return;
}
#endif


