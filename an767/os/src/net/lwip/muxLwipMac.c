/**
 * \brief GMAC (Gigabit MAC) driver for lwIP.
 *
 */

#include "compact.h"
#include "lwipMux.h"

#include "muxOs.h"

#include "pmc.h"
#include "ethernetPhy.h"
#include "sysclk.h"

/* gmac.h contains all definations in ASF/SAM, some collisions with thirdparty. so only include it in c source file */
#include "gmac.h"

#define	MUXLAB_GMAC_TEST						0	/* test GMAC of E70 */

#define	MUXLAB_GMAC_ENABLE_MULTICAST		1

/** Network interface identifier. */
#define MUX_IF_NAME0               'e'
#define MUX_IF_NAME1               'n'


/** Network link speed. 100MB */
#define NET_LINK_SPEED        100000000

#if MUX_WITH_OS
		/* Interrupt priorities. (lowest value = highest priority) */
		/* ISRs using FreeRTOS *FromISR APIs must have priorities below or equal to */
		/* configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY. */
		#define INT_PRIORITY_GMAC     12

		/** The GMAC interrupts to enable */
		#define GMAC_INT_GROUP (GMAC_ISR_RCOMP | GMAC_ISR_ROVR | GMAC_ISR_HRESP | GMAC_ISR_TCOMP | GMAC_ISR_TUR | GMAC_ISR_TFC)

		/** The GMAC TX errors to handle */
		#define GMAC_TX_ERRORS (GMAC_TSR_TFC | GMAC_TSR_HRESP)

		/** The GMAC RX errors to handle */
		#define GMAC_RX_ERRORS (GMAC_RSR_RXOVR | GMAC_RSR_HNO)

#else
#define INT_PRIORITY_GMAC    0
#define GMAC_INT_GROUP       0
#define GMAC_TX_ERRORS       0
#define GMAC_RX_ERRORS       0
#endif

/** TX descriptor lists */
//COMPILER_ALIGNED(8)
static gmac_tx_descriptor_t gs_tx_desc_null;
/** RX descriptors lists */
//COMPILER_ALIGNED(8)
static gmac_rx_descriptor_t gs_rx_desc_null;
/**
 * GMAC driver structure.
 */
struct gmac_device
{
	/**
	 * Pointer to allocated TX buffer.
	 * Section 3.6 of AMBA 2.0 spec states that burst should not cross
	 * 1K Boundaries.
	 * Receive buffer manager writes are burst of 2 words => 3 lsb bits
	 * of the address shall be set to 0.
	 */
	/** Pointer to Rx descriptor list (must be 8-byte aligned). */
	gmac_rx_descriptor_t rx_desc[GMAC_RX_BUFFERS];
	/** Pointer to Tx descriptor list (must be 8-byte aligned). */
	gmac_tx_descriptor_t tx_desc[GMAC_TX_BUFFERS];
	/** RX pbuf pointer list. */
	struct pbuf *rx_pbuf[GMAC_RX_BUFFERS];
	/** TX buffers. */
	uint8_t tx_buf[GMAC_TX_BUFFERS][GMAC_TX_UNITSIZE];

	/** RX index for current processing TD. */
	uint32_t us_rx_idx;
	/** Circular buffer head pointer by upper layer (buffer to be sent). */
	uint32_t us_tx_idx;

	/** Reference to lwIP netif structure. */
	struct netif *netif;

#if MUX_WITH_OS
	/** RX task notification semaphore. */
	sys_sem_t rx_sem;
#endif
};

/**
 * GMAC driver instance.
 */
//COMPILER_ALIGNED(8)
static struct gmac_device gs_gmac_dev;


#if LWIP_STATS
/** Used to compute lwIP bandwidth. */
uint32_t lwip_tx_count = 0;
uint32_t lwip_rx_count = 0;
uint32_t lwip_tx_rate = 0;
uint32_t lwip_rx_rate = 0;
#endif

static inline void gmac_enable_loop(Gmac* p_gmac, uint8_t uc_enable)
{
	if (uc_enable)
	{
		p_gmac->GMAC_NCR |= GMAC_NCR_LBL;
	}
	else
	{
		p_gmac->GMAC_NCR &= ~GMAC_NCR_LBL;
	}
}

#if MUXLAB_GMAC_TEST
/* After successful configuration, NCR and NFR should be:  CR:0x8c; CFR:0x80003 */
static void _muxGMacStatus(Gmac* gmac)
{
	int txOcts = gmac->GMAC_FT;
	int rxOcts = gmac->GMAC_FR; 
	printf("GMAC TX:%d; RX:%d; CR:0x%x; CFR:0x%x\r\n", txOcts, rxOcts, (int)gmac->GMAC_NCR, (int)gmac->GMAC_NCFGR);
}
#endif

/**
 * \brief GMAC interrupt handler.
 */
void GMAC_Handler(void)
{
#if MUX_WITH_OS
	portBASE_TYPE xGMACTaskWoken = pdFALSE;
	volatile uint32_t ul_isr;

	/* Get interrupt status. */
	ul_isr = gmac_get_interrupt_status(GMAC);

#if MUXLAB_GMAC_TEST
	/* only for chip debug in ISR */
	printf("ISR status:0x%x"MUX_NEW_LINE, ul_isr);
	muxEtherDebug();

	_muxGMacStatus(GMAC);
#endif

	/* RX interrupts. */
	if (ul_isr & GMAC_INT_GROUP)
	{
		xSemaphoreGiveFromISR(gs_gmac_dev.rx_sem, &xGMACTaskWoken);
	}
	
	portEND_SWITCHING_ISR(xGMACTaskWoken);

#if 0
	gmac_enable_management(GMAC, false);
	gmac_enable_receive(GMAC, true);
#endif
#else
	NVIC_DisableIRQ(GMAC_IRQn);
#endif
}

/**
 * \brief Populate the RX descriptor ring buffers with pbufs.
 *
 * \note Make sure that the p->payload pointer is 32 bits aligned.
 * (since the lsb are used as status bits by GMAC).
 *
 * \param p_gmac_dev Pointer to driver data structure.
 */
static void gmac_rx_populate_queue(struct gmac_device *p_gmac_dev)
{
	uint32_t ul_index = 0;
	struct pbuf *p = 0;

	/* Set up the RX descriptors. */
	for (ul_index = 0; ul_index < GMAC_RX_BUFFERS; ul_index++)
	{
		if (p_gmac_dev->rx_pbuf[ul_index] == 0)
		{
			/* Allocate a new pbuf with the maximum size. */
			p = pbuf_alloc(PBUF_RAW, (u16_t) GMAC_FRAME_LENTGH_MAX, PBUF_POOL);
			if (p == NULL)
			{
				LWIP_DEBUGF(NETIF_DEBUG, ("gmac_rx_populate_queue: pbuf allocation failure"MUX_NEW_LINE));
				break;
			}
#if MUX_LWIP_DEBUG
			MUX_LWIP_DEBUG_PBUF(p);
			if(p->next)
			{
				MUX_LWIP_DEBUG_PBUF(p->next);
			}
#endif

			/* Make sure lwIP is well configured so one pbuf can contain the maximum packet size. */
			LWIP_ASSERT("gmac_rx_populate_queue: pbuf size too small!", (pbuf_clen(p) >= 1));

			/* Make sure that the payload buffer is properly aligned. */
			LWIP_ASSERT("gmac_rx_populate_queue: unaligned p->payload buffer address", (((uint32_t)p->payload & 0xFFFFFFFC) == (uint32_t)p->payload));

			if (ul_index == GMAC_RX_BUFFERS - 1)
				p_gmac_dev->rx_desc[ul_index].addr.val = (u32_t) p->payload | GMAC_RXD_WRAP;
			else
				p_gmac_dev->rx_desc[ul_index].addr.val = (u32_t) p->payload;

			/* Reset status value. */
			p_gmac_dev->rx_desc[ul_index].status.val = 0;

			/* Save pbuf pointer to be sent to lwIP upper layer. */
			p_gmac_dev->rx_pbuf[ul_index] = p;

//			LWIP_DEBUGF(NETIF_DEBUG|PBUF_DEBUG, ("gmac_rx_populate_queue: new pbuf allocated: %p [idx=%u]\r\n", p, (int)ul_index));
		}
	}
}

/**
 * \brief Set up the RX descriptor ring buffers.
 *
 * This function sets up the descriptor list used for receive packets.
 *
 * \param ps_gmac_dev Pointer to driver data structure.
 */
static void gmac_rx_init(struct gmac_device *ps_gmac_dev)
{
	uint32_t ul_index = 0;

	/* Init RX index. */
	ps_gmac_dev->us_rx_idx = 0;

	/* Set up the RX descriptors. */
	for (ul_index = 0; ul_index < GMAC_RX_BUFFERS; ul_index++)
	{
		ps_gmac_dev->rx_pbuf[ul_index] = 0;
		ps_gmac_dev->rx_desc[ul_index].addr.val = 0;
		ps_gmac_dev->rx_desc[ul_index].status.val = 0;
	}
	ps_gmac_dev->rx_desc[ul_index - 1].addr.val |= GMAC_RXD_WRAP;

	/* Build RX buffer and descriptors. */
	gmac_rx_populate_queue(ps_gmac_dev);

	/* Set receive buffer queue base address pointer. */
	gmac_set_rx_queue(GMAC, (uint32_t) &ps_gmac_dev->rx_desc[0]);
}



/**
 * \brief Set up the TX descriptor ring buffers.
 *
 * This function sets up the descriptor list used for receive packets.
 *
 * \param ps_gmac_dev Pointer to driver data structure.
 */
static void gmac_tx_init(struct gmac_device *ps_gmac_dev)
{
	uint32_t ul_index;

	/* Init TX index pointer. */
	ps_gmac_dev->us_tx_idx = 0;

	/* Set up the TX descriptors. */
	for (ul_index = 0; ul_index < GMAC_TX_BUFFERS; ul_index++)
	{
		ps_gmac_dev->tx_desc[ul_index].addr = (uint32_t)&ps_gmac_dev->tx_buf[ul_index][0];
		ps_gmac_dev->tx_desc[ul_index].status.val = GMAC_TXD_USED | GMAC_TXD_LAST;
	}
	ps_gmac_dev->tx_desc[ul_index - 1].status.val |= GMAC_TXD_WRAP;

	/* Set receive buffer queue base address pointer. */
	gmac_set_tx_queue(GMAC, (uint32_t) &ps_gmac_dev->tx_desc[0]);
}

/**
 * \brief Initialize GMAC and PHY.
 *
 * \note Called from ethernetif_init().
 *
 * \param netif the lwIP network interface structure for this ethernetif.
 */
static void _gmac_low_level_init(struct netif *netif)
{
	volatile uint32_t ul_delay;

	/* Device capabilities. */
	netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
#if 0	
	if(MUX_DHCP_IS_ENABLE())
	{
//		netif->flags |= NETIF_FLAG_DHCP
	}
#endif

	/* Wait for PHY to be ready (CAT811: Max400ms). */
	ul_delay = sysclk_get_cpu_hz() / 1000 / 3 * 400;
	while (ul_delay--)
	{
	}

	/* Enable GMAC clock. */
	pmc_enable_periph_clk(ID_GMAC);

	/* Disable TX & RX and more. */
	gmac_network_control(GMAC, 0);
	gmac_disable_interrupt(GMAC, ~0u);

	gmac_clear_statistics(GMAC);

	/* Clear all status bits in the receive status register. */
	gmac_clear_rx_status(GMAC, GMAC_RSR_BNA | GMAC_RSR_REC | GMAC_RSR_RXOVR
			| GMAC_RSR_HNO);

	/* Clear all status bits in the transmit status register. */
	gmac_clear_tx_status(GMAC, GMAC_TSR_UBR | GMAC_TSR_COL | GMAC_TSR_RLE
			| GMAC_TSR_TXGO | GMAC_TSR_TFC | GMAC_TSR_TXCOMP
			| GMAC_TSR_HRESP);

	/* Clear interrupts. */
	gmac_get_interrupt_status(GMAC);

	/* Enable the copy of data into the buffers
	   ignore broadcasts, and not copy FCS. */
#if MUXLAB_GMAC_ENABLE_MULTICAST	   
	gmac_enable_copy_all(GMAC, true);

	gmac_enable_multicast_hash(GMAC, true);
#else
	/* not 'copy all', but broadcast still be copied */
	gmac_enable_copy_all(GMAC, false);
#endif
	gmac_disable_broadcast(GMAC, false);

	/* Set RX buffer size to 1536. */
	gmac_set_rx_bufsize(GMAC, 0x18);

	/* Clear interrupts */
	gmac_get_priority_interrupt_status(GMAC, GMAC_QUE_2);
	gmac_get_priority_interrupt_status(GMAC, GMAC_QUE_1);

	/* Set Tx Priority */
	gs_tx_desc_null.addr = (uint32_t)0xFFFFFFFF;
	gs_tx_desc_null.status.val = GMAC_TXD_WRAP | GMAC_TXD_USED;
	gmac_set_tx_priority_queue(GMAC, (uint32_t)&gs_tx_desc_null, GMAC_QUE_2);
	gmac_set_tx_priority_queue(GMAC, (uint32_t)&gs_tx_desc_null, GMAC_QUE_1);
	
	/* Set Rx Priority */
	gs_rx_desc_null.addr.val = (uint32_t)0xFFFFFFFF & GMAC_RXD_ADDR_MASK;
	gs_rx_desc_null.addr.val |= GMAC_RXD_WRAP;
	gs_rx_desc_null.status.val = 0;
	gmac_set_rx_priority_queue(GMAC, (uint32_t)&gs_rx_desc_null, GMAC_QUE_2);
	gmac_set_rx_priority_queue(GMAC, (uint32_t)&gs_rx_desc_null, GMAC_QUE_1);

	gmac_rx_init(&gs_gmac_dev);
	gmac_tx_init(&gs_gmac_dev);

	/* Enable Rx, Tx and the statistics register. */
	gmac_enable_transmit(GMAC, true);
	gmac_enable_receive(GMAC, true);
	gmac_enable_statistics_write(GMAC, true);

#if MUXLAB_GMAC_TEST	   
	/* Set up the all types of interrupts*/
	gmac_enable_interrupt(GMAC, 0xFFFFFFFF);
#else
	/* Set up only the interrupts for RX and errors. */
	gmac_enable_interrupt(GMAC, GMAC_INT_GROUP);
#endif

	/* Set GMAC address. */
	gmac_set_address(GMAC, 0, netif->hwaddr);

	/* Enable NVIC GMAC interrupt. */
	NVIC_SetPriority(GMAC_IRQn, INT_PRIORITY_GMAC);
	NVIC_EnableIRQ(GMAC_IRQn);

	/* Init MAC PHY driver. */
	LWIP_DEBUGF(NETIF_DEBUG, ("Initializing PHY_INIT:haddr:%d(%p)"MUX_NEW_LINE, netif->hwaddr_len, netif) );
	if (ethernet_phy_init(GMAC, BOARD_GMAC_PHY_ADDR, sysclk_get_cpu_hz()) != GMAC_OK)
	{
		LWIP_DEBUGF(NETIF_DEBUG, ("gmac_low_level_init: PHY init ERROR!\r\n"));
		return;
	}

	/* Auto Negotiate, work in RMII mode. */
	LWIP_DEBUGF(NETIF_DEBUG, ("Initializing PHY_AUTO_NEGOTIATE"MUX_NEW_LINE));
	if (ethernet_phy_auto_negotiate(GMAC, BOARD_GMAC_PHY_ADDR) != GMAC_OK)
	{
		LWIP_DEBUGF(NETIF_DEBUG, ("gmac_low_level_init: auto negotiate ERROR!\r\n"));
		return;
	}

	/* Establish ethernet link. */
	LWIP_DEBUGF(NETIF_DEBUG, ("Initializing PHY_SET_LINK"MUX_NEW_LINE));
	while (ethernet_phy_set_link(GMAC, BOARD_GMAC_PHY_ADDR, 1) != GMAC_OK)
	{
		LWIP_DEBUGF(NETIF_DEBUG, ("gmac_low_level_init: set link ERROR!\r\n"));
		return;
	}

	/* set loopback mode, just for test sw chip. J.L. */
#if MUXLAB_GMAC_TEST	   
//	gmac_enable_loop(GMAC, true);
#endif

	/* Set link up*/
	netif->flags |= NETIF_FLAG_LINK_UP;
}

/**
 * \brief This function should do the actual transmission of the packet. The
 * packet is contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * \param netif the lwIP network interface structure for this ethernetif.
 * \param p the MAC packet to send (e.g. IP packet including MAC addresses and type).
 *
 * \return ERR_OK if the packet could be sent.
 * an err_t value if the packet couldn't be sent.
 */
static err_t _gmac_low_level_output(struct netif *netif, struct pbuf *p)
{
	struct gmac_device *ps_gmac_dev = netif->state;
	struct pbuf *q = NULL;
	uint8_t *buffer = 0;

	/* Handle GMAC underrun or AHB errors. */
	if (gmac_get_tx_status(GMAC) & GMAC_TX_ERRORS)
	{
		LWIP_DEBUGF(NETIF_DEBUG, ("gmac_low_level_output: GMAC ERROR, reinit TX...\r\n"));

		gmac_enable_transmit(GMAC, false);

		LINK_STATS_INC(link.err);
		LINK_STATS_INC(link.drop);

		/* Reinit TX descriptors. */
		gmac_tx_init(ps_gmac_dev);

		/* Clear error status. */
		gmac_clear_tx_status(GMAC, GMAC_TX_ERRORS);

		gmac_enable_transmit(GMAC, true);
	}

	buffer = (uint8_t*)ps_gmac_dev->tx_desc[ps_gmac_dev->us_tx_idx].addr;

	/* Copy pbuf chain into TX buffer. */
	for (q = p; q != NULL; q = q->next)
	{
		memcpy(buffer, q->payload, q->len);
		buffer += q->len;
	}

	/* Set len and mark the buffer to be sent by GMAC. */
	ps_gmac_dev->tx_desc[ps_gmac_dev->us_tx_idx].status.bm.b_len = p->tot_len;
	ps_gmac_dev->tx_desc[ps_gmac_dev->us_tx_idx].status.bm.b_used = 0;

	LWIP_DEBUGF(NETIF_DEBUG, ("gmac_low_level_output: DMA buffer sent, size=%d [idx=%u]\r\n", p->tot_len,(int) ps_gmac_dev->us_tx_idx));

	ps_gmac_dev->us_tx_idx = (ps_gmac_dev->us_tx_idx + 1) % GMAC_TX_BUFFERS;

	/* Now start to transmission. */
	gmac_start_transmission(GMAC);

#if LWIP_STATS
	lwip_tx_count += p->tot_len;
#endif
	LINK_STATS_INC(link.xmit);

	return ERR_OK;
}

/**
 * \brief Use pre-allocated pbuf as DMA source and return the incoming packet.
 *
 * \param netif the lwIP network interface structure for this ethernetif.
 *
 * \return a pbuf filled with the received packet (including MAC header).
 * 0 on memory error.
 */
static struct pbuf *_gmac_low_level_input(struct netif *netif)
{
	struct gmac_device *ps_gmac_dev = netif->state;
	struct pbuf *p = 0;
	uint32_t length = 0;
	uint32_t ul_index = 0;
	gmac_rx_descriptor_t *p_rx = &ps_gmac_dev->rx_desc[ps_gmac_dev->us_rx_idx];

//	printf("RX Status:0x%x\r\n", gmac_get_rx_status(GMAC) );
	/* Handle GMAC overrun or AHB errors. */
	if (gmac_get_rx_status(GMAC) & GMAC_RX_ERRORS)
	{
		gmac_enable_receive(GMAC, false);

		LINK_STATS_INC(link.err);
		LINK_STATS_INC(link.drop);

		/* Free all RX pbufs. */
		for (ul_index = 0; ul_index < GMAC_RX_BUFFERS; ul_index++)
		{
			if (ps_gmac_dev->rx_pbuf[ul_index] != 0)
			{
				pbuf_free(ps_gmac_dev->rx_pbuf[ul_index]);
				ps_gmac_dev->rx_pbuf[ul_index] = 0;
			}
		}

		/* Reinit TX descriptors. */
		gmac_rx_init(ps_gmac_dev);

		/* Clear error status. */
		gmac_clear_rx_status(GMAC, GMAC_RX_ERRORS);

		gmac_enable_receive(GMAC, true);
	}


//	printf("RX frame address:0x%x; status:0x%x\r\n", p_rx->addr.val, p_rx->status.val );
	/* Check that a packet has been received and processed by GMAC. */
	if ((p_rx->addr.val & GMAC_RXD_OWNERSHIP) == GMAC_RXD_OWNERSHIP)
	{
		/* Packet is a SOF since packet size is set to maximum. */
		length = p_rx->status.val & GMAC_RXD_LEN_MASK;

		/* Fetch pre-allocated pbuf. */
		p = ps_gmac_dev->rx_pbuf[ps_gmac_dev->us_rx_idx];
		p->len = length;

		/* Remove this pbuf from its desriptor. */
		ps_gmac_dev->rx_pbuf[ps_gmac_dev->us_rx_idx] = 0;

		LWIP_DEBUGF(NETIF_DEBUG, ("gmac_low_level_input: DMA(From ISR) buffer %p received, size=%u [idx=%u]"MUX_NEW_LINE, p, (int)length, (int)ps_gmac_dev->us_rx_idx));
#if MUX_LWIP_DEBUG
		MUX_LWIP_DEBUG_PBUF(p);
		if(p->next)
		{
			MUX_LWIP_DEBUG_PBUF(p->next);
		}
#endif

		/* Set pbuf total packet size. */
		p->tot_len = length;
		LINK_STATS_INC(link.recv);

		/* Fill empty descriptors with new pbufs. */
		gmac_rx_populate_queue(ps_gmac_dev);

		/* Mark the descriptor ready for transfer. */
		p_rx->addr.val &= ~(GMAC_RXD_OWNERSHIP);

		ps_gmac_dev->us_rx_idx = (ps_gmac_dev->us_rx_idx + 1) % GMAC_RX_BUFFERS;

#if LWIP_STATS
		lwip_rx_count += length;
#endif
	}

	return p;
}


/* wait semaphore from ISR */
/**
 * \brief This function should be called when a packet is ready to be
 * read from the interface. It uses the function gmac_low_level_input()
 * that handles the actual reception of bytes from the network interface.
 * Then the type of the received packet is determined and the appropriate
 * input function is called.
 *
 * \param netif the lwIP network interface structure for this ethernetif.
 */

void ethernetif_input(struct netif *netif)
{
	struct eth_hdr *ethhdr;
	struct pbuf *p;

	/* Move received packet into a new pbuf. */
	p = _gmac_low_level_input(netif);
	if (p == NULL)
		return;

	/* Points to packet payload, which starts with an Ethernet header. */
	ethhdr = p->payload;

	switch (htons(ethhdr->type))
	{
		case ETHTYPE_IP:
		case ETHTYPE_ARP:
#if PPPOE_SUPPORT
		case ETHTYPE_PPPOEDISC:
		case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
			/* Send packet to lwIP for processing. */
			/* call tcpip_input() */
			if (netif->input(p, netif) != ERR_OK)
			{
				LWIP_DEBUGF(NETIF_DEBUG, (ANSI_COLOR_RED "ethernetif_input: IP input error" ERROR_TEXT_END MUX_NEW_LINE ));
				LWIP_ASSERT(("TCPIP input"), 0);
				/* Free buffer. */
				pbuf_free(p);
			}
			break;

		default:
			/* Free buffer. */
			pbuf_free(p);
			break;
	}
}

#if MUX_WITH_OS
/**
 * \brief GMAC task function. This function waits for the notification
 * semaphore from the interrupt, processes the incoming packet and then
 * passes it to the lwIP stack.
 *
 * \param pvParameters A pointer to the gmac_device instance.
 */
static void gmac_task(void *pvParameters)
{
	struct gmac_device *ps_gmac_dev = pvParameters;

	while (1)
	{

		/* Wait for the RX notification semaphore. */
		sys_arch_sem_wait(&ps_gmac_dev->rx_sem, 0);

		/* Process the incoming packet. */
		ethernetif_input(ps_gmac_dev->netif);
	}
}
#endif



/**
 * \brief Should be called at the beginning of the program to set up the
 * network interface. It calls the function gmac_low_level_init() to do the
 * actual setup of the hardware.
 *
 * \param netif the lwIP network interface structure for this ethernetif.
 *
 * \return ERR_OK if the loopif is initialized.
 * ERR_MEM if private data couldn't be allocated.
 * any other err_t on error.
 */
err_t ethernetif_init(struct netif *netif)
{
	LWIP_ASSERT("netif != NULL", (netif != NULL));

	gs_gmac_dev.netif = netif;

#if MUX_LWIP_DEBUG
	LWIP_DEBUGF(NETIF_DEBUG, ("Initializing ethernetif_init"MUX_NEW_LINE ) );
	MUX_LWIP_DEBUG_NETIF(netif);
#endif

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname. */
	netif->hostname = "gmacdev";
#endif /* LWIP_NETIF_HOSTNAME */

	/*
	 * Initialize the snmp variables and counters inside the struct netif.
	 * The last argument should be replaced with your link speed, in units
	 * of bits per second.
	 */
#if LWIP_SNMP
	NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, NET_LINK_SPEED);
#endif /* LWIP_SNMP */

	netif->state = &gs_gmac_dev;
	netif->name[0] = MUX_IF_NAME0;
	netif->name[1] = MUX_IF_NAME1;

	/* We directly use etharp_output() here to save a function call.
	 * You can instead declare your own function an call etharp_output()
	 * from it if you have to do some checks before sending (e.g. if link
	 * is available...) */
	netif->output = etharp_output;
	netif->linkoutput = _gmac_low_level_output;
	/* Initialize the hardware */
	_gmac_low_level_init(netif);

#if MUX_WITH_OS
	sys_thread_t id;

	err_t err;
	/* Incoming packet notification semaphore. */
	err = sys_sem_new(&gs_gmac_dev.rx_sem, 0);
	LWIP_ASSERT("ethernetif_init: GMAC RX semaphore allocation ERROR!\r\n", (err == ERR_OK));
	if (err == ERR_MEM)
		return ERR_MEM;
	
	id = sys_thread_new(MUX_TASK_MAC, gmac_task, &gs_gmac_dev, netifINTERFACE_TASK_STACK_SIZE, netifINTERFACE_TASK_PRIORITY);
//	id = sys_thread_new(MUX_TASK_MAC, gmac_task, &gs_gmac_dev, netifINTERFACE_TASK_STACK_SIZE*4, MUX_TASK_ETHERNET_PRIORITY);
	LWIP_ASSERT("ethernetif_init: GMAC Task allocation ERROR!\r\n", (id != 0));
	if (id == 0)
		return ERR_MEM;
#endif

	return ERR_OK;
}

