 /**
 *
 * interface RTL8305/KSZ8081RNA PHY component.
 *
 */

#include "extSys.h"
#include "ucBsp.h"

#include "ethPhy.h"

#include "pio.h"
#include "ioport.h"
#include "gmac.h"

#define	PHY_DEBUG		EXT_DBG_ON

/** MAC PHY operation max retry count */
#define MAC_PHY_RETRY_MAX						1000000


/** Ethernet MII/RMII mode */
#define ETH_PHY_MODE                                  GMAC_PHY_RMII


/**
 * \defgroup ksz8081rna_ethernet_phy_group PHY component (KSZ8081RNA)
 * Driver for the ksz8081rna component. This driver provides access to the main features of the PHY.
 *
 * \section dependencies Dependencies
 * This driver depends on the following modules:
 * - \ref gmac_group Ethernet Media Access Controller (GMAC) module.
 */

/* Max PHY number */
#define ETH_PHY_MAX_ADDR			31

/* Ethernet PHY operation max retry count */
#define ETH_PHY_RETRY_MAX			1000000

/* Ethernet PHY operation timeout */
#define ETH_PHY_TIMEOUT			10


/**
 * \defgroup gmac_group Ethernet PHY Media Access Controller
 *
 * See \ref gmac_quickstart.
 *
 * Driver for the GMAC PHY (Ethernet Media Access Controller).
 * This file contains basic functions for the GMAC PHY.
 *
 * \section dependencies Dependencies
 * This driver does not depend on other modules.
 *
 * @{
 */

/**
 * \brief Wait PHY operation to be completed.
 *
 * \param p_gmac HW controller address.
 * \param ul_retry The retry times.
 *
 * Return GMAC_OK if the operation is completed successfully.
 */
static uint8_t gmac_phy_wait(Gmac* mac, const uint32_t retry)
{
	volatile uint32_t ul_retry_count = 0;

	while (!gmac_is_phy_idle(mac))
	{
		ul_retry_count++;

		if (ul_retry_count >= retry)
		{
			return GMAC_TIMEOUT;
		}
	}
	
	return GMAC_OK;
}

/**
 * \brief Read the PHY register.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_phy_address PHY address.
 * \param uc_address Register address.
 * \param p_value Pointer to a 32-bit location to store read data.
 *
 * \Return GMAC_OK if successfully, GMAC_TIMEOUT if timeout.
 */
uint8_t gmac_phy_read(Gmac *mac, uint8_t phyAddre, uint8_t regAddr, uint32_t *value)
{
	gmac_maintain_phy(mac, phyAddre, regAddr, 1, 0);

	if (gmac_phy_wait(mac, MAC_PHY_RETRY_MAX) == GMAC_TIMEOUT)
	{
		return GMAC_TIMEOUT;
	}
	
	*value = gmac_get_phy_data(mac);
	return GMAC_OK;
}

/**
 * Write the PHY register.
 * \Return GMAC_OK if successfully, GMAC_TIMEOUT if timeout.
 */
uint8_t gmac_phy_write(Gmac *mac, uint8_t phyAddr, uint8_t regAddr, uint32_t value)
{
	gmac_maintain_phy(mac, phyAddr, regAddr, 0, value);

	if (gmac_phy_wait(mac, MAC_PHY_RETRY_MAX) == GMAC_TIMEOUT)
	{
		return GMAC_TIMEOUT;
	}
	return GMAC_OK;
}

/**
 * \brief Find a valid PHY Address ( from addrStart to 31 ).
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_phy_addr PHY address.
 * \param uc_start_addr Start address of the PHY to be searched.
 *
 * \return 0xFF when no valid PHY address is found.
 */
#if EXTLAB_BOARD
#else
static uint8_t _bspPhyFindValid(Gmac *mac, uint8_t phyAddr, uint8_t startAddr)
{
	uint32_t ul_value = 0;
	uint8_t uc_rc = 0;
	uint8_t uc_cnt;
	uint8_t uc_phy_address = phyAddr;

	gmac_enable_management(mac, true);
	uc_rc = uc_phy_address;
	
	/* Check the current PHY address */
	gmac_phy_read(mac, phyAddr, GMII_PHYID1, &ul_value);

	/* Find another one */
	if (ul_value != GMII_OUI_MSB)
	{
		uc_rc = 0xFF;
		for (uc_cnt = startAddr; uc_cnt <= ETH_PHY_MAX_ADDR; uc_cnt++)
		{
			uc_phy_address = (uc_phy_address + 1) & 0x1F;
			gmac_phy_read(mac, uc_phy_address, GMII_PHYID1, &ul_value);
			if (ul_value == GMII_OUI_MSB)
			{
				uc_rc = uc_phy_address;
				break;
			}
		}
	}

	gmac_enable_management(mac, false);

	if (uc_rc != 0xFF)
	{
		gmac_phy_read(mac, uc_phy_address, GMII_BMSR, &ul_value);
	}
	return uc_rc;
}
#endif


/**
 * \brief Issue a SW reset to reset all registers of the PHY.
 *
 * \Return GMAC_OK if successfully, GMAC_TIMEOUT if timeout.
 */
static uint8_t _bspPhyReset(Gmac *mac, uint8_t phyAddr)
{
	uint32_t ul_bmcr;
	uint32_t ul_timeout = ETH_PHY_TIMEOUT;
	uint8_t uc_rc = GMAC_TIMEOUT;

	gmac_enable_management(mac, true);

	gmac_phy_read(mac, phyAddr, GMII_BMCR, &ul_bmcr);
	EXT_DEBUGF(PHY_DEBUG, ("before reset BMCR: 0x%lx", ul_bmcr));
	
	ul_bmcr = GMII_RESET|GMII_AUTONEG;
	gmac_phy_write(mac, phyAddr, GMII_BMCR, ul_bmcr);

	do
	{
		gmac_phy_read(mac, phyAddr, GMII_BMCR, &ul_bmcr);
		ul_timeout--;
	} while ((ul_bmcr & GMII_RESET) && ul_timeout);
	EXT_DEBUGF(PHY_DEBUG, ("after reset BMCR: 0x%lx", ul_bmcr) );

	gmac_enable_management(mac, false);

	if (ul_timeout)
	{
		uc_rc = GMAC_OK;
	}

	return (uc_rc);
}

/**
 * \brief Perform a HW initialization to the PHY and set up clocks.
 *
 * This should be called only once to initialize the PHY pre-settings.
 * The PHY address is the reset status of CRS, RXD[3:0] (the emacPins' pullups).
 * The COL pin is used to select MII mode on reset (pulled up for Reduced MII).
 * The RXDV pin is used to select test mode on reset (pulled up for test mode).
 * The above pins should be predefined for corresponding settings in resetPins.
 * The GMAC peripheral pins are configured after the reset is done.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_phy_addr PHY address.
 * \param ul_mck GMAC MCK.
 *
 * Return GMAC_OK if successfully, GMAC_TIMEOUT if timeout.
 */
static uint8_t _bspPhyInit(Gmac *mac, uint8_t phyAddr, uint32_t mck)
{
	uint8_t uc_rc = 0;
#if EXTLAB_BOARD
#else
	uint8_t uc_phy;
#endif

	pio_set_output(PIN_GMAC_RESET_PIO, PIN_GMAC_RESET_MASK, 1,  false, true);
#if EXTLAB_BOARD
#else
	/* external INT pin from ethernet switch chip, this pin has been routed to FPGA */
	pio_set_input(PIN_GMAC_INT_PIO, PIN_GMAC_INT_MASK, PIO_PULLUP);
#endif

	/* add this line. JL. 01.25,2018 */
//	ioport_set_port_mode(IOPORT_PIOD, PIN_GMAC_MASK, IOPORT_MODE_PULLDOWN);
	/* all pins belong to this port must be pulldown, no matter it is used in RMII or not. J.L. Jan.26, 2018 */
	ioport_set_port_mode(IOPORT_PIOD, PIN_GMAC_MASK| PIO_PD7A_GRXER |PIO_PD11A_GRX2| PIO_PD12A_GRX3 |PIO_PD14A_GRXCK|PIO_PD15A_GTX2| PIO_PD16A_GTX3, IOPORT_MODE_PULLDOWN);

	pio_set_peripheral(PIN_GMAC_PIO, PIN_GMAC_PERIPH, PIN_GMAC_MASK);


#if EXTLAB_BOARD
	if(extBspRtl8305Config() != EXIT_SUCCESS)
		uc_rc = GMAC_TIMEOUT;
#else
	uc_rc = _bspPhyReset(mac, phyAddr);
	if (uc_rc != GMAC_OK)
	{
		EXT_ABORT("PHY RESET");
		return 0;
	}
	
	/* Configure GMAC runtime clock for MDC/MDIO interface, J.L. */
	EXT_DEBUGF(PHY_DEBUG, ("set GMAC clock: %uMHz", (unsigned int)mck/1000/1000) );
	uc_rc = gmac_set_mdc_clock(mac, mck/2);
	if (uc_rc != GMAC_OK)
	{/* set clock: 300,000,000 */
		EXT_ERRORF(("ERROR: set GMAC clock error: %uMHz", (unsigned int)mck/1000/1000) );
//		EXT_ABORT("set clock");
		return 0;
	}

	/* Check PHY Address */
	uc_phy = _bspPhyFindValid(mac, phyAddr, 0);
	if (uc_phy == 0xFF)
	{
		return 0;
	}

	if (uc_phy != phyAddr)
	{
		_bspPhyReset(mac, phyAddr);
	}
#endif

	return uc_rc;
}


/**
 * \brief Get the Link & speed settings, and automatically set up the GMAC with the
 * settings.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_phy_addr PHY address.
 * \param uc_apply_setting_flag Set to 0 to not apply the PHY configurations, else to apply.
 *
 * Return GMAC_OK if successfully, GMAC_TIMEOUT if timeout.
 */
static uint8_t _bspPhySetLink(Gmac *mac, uint8_t phyAddr, uint8_t isApplySettingFlag)
{
	uint8_t uc_rc =0;
	uint8_t uc_speed, uc_fd;
#if EXTLAB_BOARD
#else
	uint32_t ul_stat1;
	uint32_t ul_stat2;
	uint8_t uc_phy_address;
#endif

	gmac_enable_management(mac, true);
#if EXTLAB_BOARD
	/* Set GMAC for 100BaseTX and Full Duplex */
	uc_speed = true;
	uc_fd = true;
#else

	uc_phy_address = phyAddr;

	uc_rc = gmac_phy_read(mac, uc_phy_address, GMII_BMSR, &ul_stat1);
	if (uc_rc != GMAC_OK)
	{/* Disable PHY management and start the GMAC transfer */
		gmac_enable_management(mac, false);
		return uc_rc;
	}

	if ((ul_stat1 & GMII_LINK_STATUS) == 0)
	{/* Disable PHY management and start the GMAC transfer */
		gmac_enable_management(mac, false);
		return GMAC_INVALID;
	}

	if (isApplySettingFlag == 0)
	{/* Disable PHY management and start the GMAC transfer */
		gmac_enable_management(mac, false);
		return uc_rc;
	}

	/* Read advertisement */
	uc_rc = gmac_phy_read(mac, uc_phy_address, GMII_PCR1, &ul_stat2);
	if (uc_rc != GMAC_OK)
	{/* Disable PHY management and start the GMAC transfer */
		gmac_enable_management(mac, false);
		return uc_rc;
	}

	if ((ul_stat1 & GMII_100BASE_TX_FD) && (ul_stat2 & GMII_OMI_100BASE_TX_FD))
	{/* Set GMAC for 100BaseTX and Full Duplex */
		uc_speed = true;
		uc_fd = true;
	}

	if ((ul_stat1 & GMII_10BASE_T_FD) && (ul_stat2 & GMII_OMI_10BASE_T_FD))
	{/* Set MII for 10BaseT and Full Duplex */
		uc_speed = false;
		uc_fd = true;
	}

	if ((ul_stat1 & GMII_100BASE_TX_HD) && (ul_stat2 & GMII_OMI_100BASE_TX_HD))
	{/* Set MII for 100BaseTX and Half Duplex */
		uc_speed = true;
		uc_fd = false;
	}

	if ((ul_stat1 & GMII_10BASE_T_HD) && (ul_stat2 & GMII_OMI_10BASE_T_HD))
	{/* Set MII for 10BaseT and Half Duplex */
		uc_speed = false;
		uc_fd = false;
	}
#endif

	gmac_set_speed(mac, uc_speed);
	gmac_enable_full_duplex(mac, uc_fd);

//	EXT_DEBUGF(EXT_DBG_OFF, ("Link Speed: %s MB, Full Duplex: %s", (uc_speed!=false)?"100":"10", (uc_fd==true)?"YES":"NO"));

#if EXTLAB_BOARD
#else
#endif

	/* Start the GMAC transfers */
	gmac_enable_management(mac, false);
	return uc_rc;
}


/**
 *  Issue an auto negotiation of the PHY.
  * Return GMAC_OK if successfully, GMAC_TIMEOUT if timeout.
 */
static uint8_t _bspPhyAutoNegotiate(Gmac *mac, uint8_t phyAddr)
{
	uint8_t uc_speed = 0;
	uint8_t uc_fd=0;
	uint8_t uc_rc =0;
	
#if EXTLAB_BOARD
#else
	uint32_t ul_retry_max = ETH_PHY_RETRY_MAX;
	uint32_t ul_value;
	uint32_t ul_phy_anar;
	uint32_t ul_phy_analpar;
	uint32_t ul_retry_count = 0;
#endif

	gmac_enable_management( mac, true);

#if EXTLAB_BOARD
	/* Set MII for 100BaseTX and Full Duplex */
	uc_speed = true;
	uc_fd = true;
#else
	/* Set up control register */
	uc_rc = gmac_phy_read( mac, phyAddr, GMII_BMCR, &ul_value);
	if (uc_rc != GMAC_OK)
	{
		gmac_enable_management( mac, false);
		return uc_rc;
	}

	ul_value &= ~(uint32_t)GMII_AUTONEG; /* Remove auto-negotiation enable */
	ul_value &= ~(uint32_t)(GMII_LOOPBACK | GMII_POWER_DOWN);
	ul_value |= (uint32_t)GMII_ISOLATE; /* Electrically isolate PHY */
	uc_rc = gmac_phy_write( mac, phyAddr, GMII_BMCR, ul_value);
	if (uc_rc != GMAC_OK)
	{
		gmac_enable_management( mac, false);
		return uc_rc;
	}

	/*
	 * Set the Auto_negotiation Advertisement Register.
	 * MII advertising for Next page.
	 * 100BaseTxFD and HD, 10BaseTFD and HD, IEEE 802.3.
	 */
	ul_phy_anar = GMII_100TX_FDX | GMII_100TX_HDX | GMII_10_FDX | GMII_10_HDX |GMII_AN_IEEE_802_3;
	uc_rc = gmac_phy_write( mac, phyAddr, GMII_ANAR, ul_phy_anar);
	if (uc_rc != GMAC_OK)
	{
		gmac_enable_management( mac, false);
		return uc_rc;
	}

	/* Read & modify control register */
	uc_rc = gmac_phy_read( mac, phyAddr, GMII_BMCR, &ul_value);
	if (uc_rc != GMAC_OK)
	{
		gmac_enable_management( mac, false);
		return uc_rc;
	}

	ul_value |= GMII_SPEED_SELECT | GMII_AUTONEG | GMII_DUPLEX_MODE;
	uc_rc = gmac_phy_write( mac, phyAddr, GMII_BMCR, ul_value);
	if (uc_rc != GMAC_OK)
	{
		gmac_enable_management( mac, false);
		return uc_rc;
	}

	/* Restart auto negotiation */
	ul_value |= (uint32_t)GMII_RESTART_AUTONEG;
	ul_value &= ~(uint32_t)GMII_ISOLATE;
	uc_rc = gmac_phy_write( mac, phyAddr, GMII_BMCR, ul_value);
	if (uc_rc != GMAC_OK)
	{
		gmac_enable_management( mac, false);
		return uc_rc;
	}

	/* Check if auto negotiation is completed */
	while (1)
	{
		uc_rc = gmac_phy_read( mac, phyAddr, GMII_BMSR, &ul_value);
		if (uc_rc != GMAC_OK)
		{
			gmac_enable_management(mac, false);
			return uc_rc;
		}
		
		/* Done successfully */
		if (ul_value & GMII_AUTONEG_COMP)
		{
			EXT_DEBUGF(PHY_DEBUG, ("AutoNegotiation finished"));
			break;
		}

		/* Timeout check */
		if (ul_retry_max)
		{
			if (++ul_retry_count >= ul_retry_max)
			{
				EXT_ERRORF(("AutoNegotiation timeout") );
				gmac_enable_management( mac, false);
				return GMAC_TIMEOUT;
			}
		}
	}

	/* Get the auto negotiate link partner base page */
	uc_rc = gmac_phy_read(mac, phyAddr, GMII_ANLPAR, &ul_phy_analpar);
	if (uc_rc != GMAC_OK)
	{
		gmac_enable_management(mac, false);
		return uc_rc;
	}


	/* Set up the GMAC link speed */
	if ((ul_phy_anar & ul_phy_analpar) & GMII_100TX_FDX)
	{/* Set MII for 100BaseTX and Full Duplex */
		uc_speed = true;
		uc_fd = true;
	}
	else if ((ul_phy_anar & ul_phy_analpar) & GMII_10_FDX)
	{/* Set MII for 10BaseT and Full Duplex */
		uc_speed = false;
		uc_fd = true;
	}
	else if ((ul_phy_anar & ul_phy_analpar) & GMII_100TX_HDX)
	{/* Set MII for 100BaseTX and half Duplex */
		uc_speed = true;
		uc_fd = false;
	}
	else if ((ul_phy_anar & ul_phy_analpar) & GMII_10_HDX)
	{/* Set MII for 10BaseT and half Duplex */
		uc_speed = false;
		uc_fd = false;
	}
#endif

	EXT_DEBUGF(EXT_DBG_ON, ("Link:%sMb; Duplex:%s", (uc_speed==false)?"10":"100", (uc_fd==false)?"Half":"Full"));
	gmac_set_speed(mac, uc_speed);
	gmac_enable_full_duplex(mac, uc_fd);

	/* Select Media Independent Interface type */
	gmac_select_mii_mode( mac, ETH_PHY_MODE);

	gmac_enable_transmit(mac, true);
	gmac_enable_receive(mac, true);

	gmac_enable_management(mac, false);
	return uc_rc;
}


uint8_t bspPhyHwStart(Gmac *mac, uint8_t phyAddr)
{
	if(_bspPhyInit(mac, phyAddr, sysclk_get_cpu_hz()) != GMAC_OK)
	{
		EXT_ERRORF( ("PHY: init ERROR!"));
		return 1;
	}

	/* Auto Negotiate, work in RMII mode. */
	EXT_DEBUGF(EXT_DBG_ON, ("PHY AUTO NEGOTIATE"));
	if (_bspPhyAutoNegotiate(mac, phyAddr) != GMAC_OK)
	{
		EXT_ERRORF(("PHY: auto negotiate ERROR!"));
		return 1;
	}

	/* Establish ethernet link. */
	EXT_DEBUGF(EXT_DBG_ON, ("PHY SET LINK"));
	while (_bspPhySetLink(mac, phyAddr, 1) != GMAC_OK)
	{
		EXT_ERRORF( ("PHY: set link ERROR!"));
		return 1;
	}

	return 0;
}


