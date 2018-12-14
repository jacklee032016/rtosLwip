/**
 * GMAC (Gigabit MAC) driver for lwIP.
 */

#include "compact.h"
#include "lwipExt.h"

#include "eos.h"

#include "pmc.h"
#include "ethernetPhy.h"
#include "sysclk.h"

/* gmac.h contains all definations in ASF/SAM, some collisions with thirdparty. so only include it in c source file */
#include "gmac.h"


/** Number of buffer for RX */
//#define GMAC_RX_BUFFERS						7	/* enlarge to decrease the risk of DMA buffers overun. Aug.8th, 2018. J.L. */
#define GMAC_RX_BUFFERS						10	/* enlarge to decrease the risk of DMA buffers overun. Dec.13th, 2018. J.L. */

/** Number of buffer for TX */
#define GMAC_TX_BUFFERS						3	/* never Overrun in TX when 3 */



#define	MUXLAB_GMAC_TEST						0	/* test GMAC of E70 */

#define	MUXLAB_GMAC_ENABLE_MULTICAST		1

/** Network interface identifier. */
#define EXT_IF_NAME0               'e'
#define EXT_IF_NAME1               'n'


/** Network link speed. 100MB */
#define NET_LINK_SPEED        100000000

#if EXT_WITH_OS
		/* Interrupt priorities. (lowest value = highest priority) */
		/* ISRs using FreeRTOS *FromISR APIs must have priorities below or equal to */
		/* configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY. */
		#define INT_PRIORITY_GMAC     12

		/** The GMAC interrupts to enable: only RECV INTs are used. Aug.8th, 2018. J.L. */
#if 1		
		#define GMAC_INT_GROUP (GMAC_ISR_RCOMP | GMAC_ISR_ROVR | GMAC_ISR_HRESP | GMAC_ISR_TCOMP | GMAC_ISR_TUR | GMAC_ISR_TFC)

		#define GMAC_INT_RECV (GMAC_ISR_RCOMP | GMAC_ISR_ROVR | GMAC_ISR_HRESP )
#else
		#define GMAC_INT_GROUP (GMAC_ISR_RCOMP | GMAC_ISR_ROVR | GMAC_ISR_HRESP )
#endif
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
struct MAC_CTRL
{
	/**
	 * Pointer to allocated TX buffer.
	 * Section 3.6 of AMBA 2.0 spec states that burst should not cross
	 * 1K Boundaries.
	 * Receive buffer manager writes are burst of 2 words => 3 lsb bits
	 * of the address shall be set to 0.
	 */

	/** Pointer to Rx descriptor list (must be 8-byte aligned). */
	gmac_rx_descriptor_t	rxDescs[GMAC_RX_BUFFERS];

	/** RX pbuf pointer list. */
	struct pbuf *rxPbuf[GMAC_RX_BUFFERS];

	/** RX index for current processing TD. */
	uint32_t		rxReadIdx;
	uint32_t		rxWriteIdx;

	/** Pointer to Tx descriptor list (must be 8-byte aligned). */
	gmac_tx_descriptor_t	txDescs[GMAC_TX_BUFFERS];
	
	/** TX buffers. */
	uint8_t				txBuf[GMAC_TX_BUFFERS][GMAC_TX_UNITSIZE];

	/** Circular buffer head pointer by upper layer (buffer to be sent). */
	uint32_t 				txIdx;

	/** Reference to lwIP netif structure. */
	struct netif			*netif;

#if EXT_WITH_OS
	/** RX task notification semaphore. */
//	sys_sem_t			rxSem;
	SemaphoreHandle_t	rxSem;
#endif

	struct MAC_STATS	*macStats;
};

/**
 * GMAC driver instance.
 */
//COMPILER_ALIGNED(8)
static struct MAC_CTRL 	_macCtrl;


struct MAC_STATS		macStats;

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
static void _extGMacStatus(Gmac* gmac)
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
#if EXT_WITH_OS
	portBASE_TYPE xGMACTaskWoken = pdFALSE;
	volatile uint32_t ul_isr;

	/* Get interrupt status. */
	ul_isr = gmac_get_interrupt_status(GMAC);

#if MUXLAB_GMAC_TEST
	/* only for chip debug in ISR */
	printf("ISR status:0x%x"EXT_NEW_LINE, ul_isr);
	extEtherDebug();

	_extGMacStatus(GMAC);
#endif

#if LWIP_STATS
	_macCtrl.macStats->isrCount++;
#endif
	/* RX interrupts. */
	if(ul_isr & GMAC_INT_RECV)
	{
		printf("ISR RECV ");
		_macCtrl.macStats->isrRecvCount++;
		xSemaphoreGiveFromISR(_macCtrl.rxSem, &xGMACTaskWoken);
	}
	
#if 0
	gmac_enable_management(GMAC, false);
	gmac_enable_receive(GMAC, true);
#endif
#else
	NVIC_DisableIRQ(GMAC_IRQn);
#endif
	
	portEND_SWITCHING_ISR(xGMACTaskWoken);
}

/**
 * Populate the RX descriptor ring buffers with pbufs.
 * \note Make sure that the p->payload pointer is 32 bits aligned.
 * (since the lsb are used as status bits by GMAC).
 */
static void _rxPopulateQueue(struct MAC_CTRL *macDev)
{
	uint32_t index = 0;
	struct pbuf *p = 0;

	/* Set up the RX descriptors. */
	for (index = 0; index < GMAC_RX_BUFFERS; index++)
	{
		if (macDev->rxPbuf[index] == 0)
		{
			/* Allocate a new pbuf with the maximum size. */
			p = pbuf_alloc(PBUF_RAW, (u16_t) GMAC_FRAME_LENTGH_MAX, PBUF_POOL);
			if (p == NULL)
			{
				EXT_DEBUGF(NETIF_DEBUG, ("pbuf allocation failure when populate RX queue"));
				EXT_ERRORF(("pbuf allocation failure when populate RX queue") );
#if LWIP_STATS
				macDev->macStats->rxErrOOM++;
#endif
				LINK_STATS_INC(link.memerr);
				break;
			}
#if EXT_LWIP_DEBUG
			EXT_LWIP_DEBUG_PBUF(p);
			if(p->next)
			{
				EXT_LWIP_DEBUG_PBUF(p->next);
			}
#endif

			/* Make sure lwIP is well configured so one pbuf can contain the maximum packet size. */
			EXT_ASSERT(("gmac_rx_populate_queue: pbuf size too small!%d",p->tot_len), (pbuf_clen(p) >= 1));

			/* Make sure that the payload buffer is properly aligned. */
			EXT_ASSERT(("gmac_rx_populate_queue: unaligned p->payload buffer address"), (((uint32_t)p->payload & 0xFFFFFFFC) == (uint32_t)p->payload));

			if (index == GMAC_RX_BUFFERS - 1)
				macDev->rxDescs[index].addr.val = (u32_t) p->payload | GMAC_RXD_WRAP;
			else
				macDev->rxDescs[index].addr.val = (u32_t) p->payload;

			/* make is can be used by DMA.  August 9, 2018 J.L.*/
			macDev->rxDescs[index].addr.val &= ~(GMAC_RXD_OWNERSHIP);


			/* Reset status value. */
			macDev->rxDescs[index].status.val = 0;

			/* Save pbuf pointer to be sent to lwIP upper layer. */
			macDev->rxPbuf[index] = p;

//			EXT_DEBUGF(NETIF_DEBUG|PBUF_DEBUG, ("gmac_rx_populate_queue: new pbuf allocated: %p [idx=%u]", p, (int)index));
		}
	}
}

/**
 * Set up the RX descriptor ring buffers.
 * This function sets up the descriptor list used for receive packets.
 */
static void _macRxInit(struct MAC_CTRL *macDev)
{
	uint32_t index = 0;

	/* Init RX index. */
	macDev->rxReadIdx = 0;
	macDev->rxWriteIdx = 0;

	/* Set up the RX descriptors. */
	for (index = 0; index < GMAC_RX_BUFFERS; index++)
	{
		macDev->rxPbuf[index] = 0;
		macDev->rxDescs[index].addr.val = 0;
		macDev->rxDescs[index].status.val = 0;
	}
	macDev->rxDescs[index - 1].addr.val |= GMAC_RXD_WRAP;

	/* Build RX buffer and descriptors. */
	_rxPopulateQueue(macDev);

	/* Set receive buffer queue base address pointer. */
	gmac_set_rx_queue(GMAC, (uint32_t) &macDev->rxDescs[0]);
}


/**
 * Set up the TX descriptor ring buffers.
 * This function sets up the descriptor list used for receive packets.
 */
static void _macTxInit(struct MAC_CTRL *macDev)
{
	uint32_t index;

	/* Init TX index pointer. */
	macDev->txIdx = 0;

	/* Set up the TX descriptors. */
	for (index = 0; index < GMAC_TX_BUFFERS; index++)
	{
		macDev->txDescs[index].addr = (uint32_t)&macDev->txBuf[index][0];
		macDev->txDescs[index].status.val = GMAC_TXD_USED | GMAC_TXD_LAST;
	}
	macDev->txDescs[index - 1].status.val |= GMAC_TXD_WRAP;

	/* Set receive buffer queue base address pointer. */
	gmac_set_tx_queue(GMAC, (uint32_t) &macDev->txDescs[0]);
}


/**
 * \brief Initialize GMAC and PHY.
 * \note Called from ethernetif_init().
 * \param netif the lwIP network interface structure for this ethernetif.
 */
static void _macHwInit(struct netif *netif)
{
	volatile uint32_t ul_delay;

	/* Device capabilities. */
	netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;


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
	gmac_enable_copy_all(GMAC, true);	/* must be enabled to rx multicast packets. J.L. 08.20,2018 */

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

	_macRxInit(&_macCtrl);
	_macTxInit(&_macCtrl);

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
	EXT_DEBUGF(NETIF_DEBUG, ("Initializing PHY_INIT:haddr:%d(%p)", netif->hwaddr_len, netif) );
	if (ethernet_phy_init(GMAC, BOARD_GMAC_PHY_ADDR, sysclk_get_cpu_hz()) != GMAC_OK)
	{
		EXT_ERRORF( ("gmac_low_level_init: PHY init ERROR!"));
		return;
	}

	/* Auto Negotiate, work in RMII mode. */
	EXT_DEBUGF(NETIF_DEBUG, ("Initializing PHY_AUTO_NEGOTIATE"));
	if (ethernet_phy_auto_negotiate(GMAC, BOARD_GMAC_PHY_ADDR) != GMAC_OK)
	{
		EXT_ERRORF(("gmac_low_level_init: auto negotiate ERROR!"));
		return;
	}

	/* Establish ethernet link. */
	EXT_DEBUGF(NETIF_DEBUG, ("Initializing PHY_SET_LINK"));
	while (ethernet_phy_set_link(GMAC, BOARD_GMAC_PHY_ADDR, 1) != GMAC_OK)
	{
		EXT_ERRORF( ("gmac_low_level_init: set link ERROR!"));
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
static err_t _macHwOutput(struct netif *netif, struct pbuf *p)
{
	struct MAC_CTRL *_macDev = netif->state;
	struct pbuf *q = NULL;
	uint8_t *buffer = 0;

	/* Handle GMAC underrun or AHB errors. */
	if (gmac_get_tx_status(GMAC) & GMAC_TX_ERRORS)
	{
		EXT_DEBUGF(NETIF_DEBUG, ("gmac_low_level_output: GMAC ERROR: 0x%"PRIx32", reinit TX...", gmac_get_tx_status(GMAC)));

		gmac_enable_transmit(GMAC, false);

		LINK_STATS_INC(link.err);
		LINK_STATS_INC(link.drop);

		/* Reinit TX descriptors. */
		_macTxInit(_macDev);

		/* Clear error status. */
		gmac_clear_tx_status(GMAC, GMAC_TX_ERRORS);

		gmac_enable_transmit(GMAC, true);

#if LWIP_STATS
		_macDev->macStats->txFailed++;
#endif
		return ERR_OK;
	}

	buffer = (uint8_t*)_macDev->txDescs[_macDev->txIdx].addr;

	/* Copy pbuf chain into TX buffer. */
	for (q = p; q != NULL; q = q->next)
	{
		memcpy(buffer, q->payload, q->len);
		buffer += q->len;
	}

	/* Set len and mark the buffer to be sent by GMAC. */
	_macDev->txDescs[_macDev->txIdx].status.bm.b_len = p->tot_len;
	_macDev->txDescs[_macDev->txIdx].status.bm.b_used = 0;

//	EXT_DEBUGF(NETIF_DEBUG, ("DMA buffer sent, size=%d [idx=%u]", p->tot_len,(int) _macDev->txIdx));

	_macDev->txIdx = (_macDev->txIdx + 1) % GMAC_TX_BUFFERS;

	/* Now start to transmission. */
	gmac_start_transmission(GMAC);

#if LWIP_STATS
	lwip_tx_count += p->tot_len;
	_macDev->macStats->txPackets++;
#endif
	LINK_STATS_INC(link.xmit);

	return ERR_OK;
}


static  err_t _checkRecvStatus(struct MAC_CTRL *_macDev)
{
	uint32_t index = 0;
	uint32_t status = gmac_get_rx_status(GMAC);

#if MUXLAB_GMAC_TEST
	printf("RX Status:0x%"PRIx32""EXT_NEW_LINE, status );
#endif
	/* Handle GMAC overrun or AHB errors. */
	if (status & GMAC_RX_ERRORS)
	{
		gmac_enable_receive(GMAC, false);
		
		EXT_ERRORF(("GMAC overrun: RX Status 0x%"PRIx32"", gmac_get_rx_status(GMAC) ));

		LINK_STATS_INC(link.err);
		LINK_STATS_INC(link.drop);

		/* Free all RX pbufs. */
		for (index = 0; index < GMAC_RX_BUFFERS; index++)
		{
			if (_macDev->rxPbuf[index] != 0)
			{
				pbuf_free(_macDev->rxPbuf[index]);
				_macDev->rxPbuf[index] = 0;
			}
		}

		/* Reinit RX descriptors. */
		_macRxInit(_macDev);

		/* Clear error status. */
		gmac_clear_rx_status(GMAC, GMAC_RX_ERRORS| GMAC_RSR_BNA|GMAC_RSR_REC);

		gmac_enable_receive(GMAC, true);
#if LWIP_STATS
		_macDev->macStats->rxErrOverrun++;
#endif

		return ERR_ABRT;
	}

//	if((status & (GMAC_RSR_REC|GMAC_RSR_BNA)) == 0)
	if((status & (GMAC_RSR_BNA)) != 0 && ((status & (GMAC_RSR_REC))==0 ))
	{
		EXT_ERRORF(("GMAC BNA: Buffer Not Available, status '0x%lx'", status ));
		return ERR_ABRT;
	}

	return ERR_OK;
}

static void sendToIp(struct netif *netif, struct pbuf *p)
{
	struct MAC_CTRL *_macDev = netif->state;
	struct eth_hdr *ethhdr;
	
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
			
			{
//				extLwipEthHdrDebugPrint(p->payload, "MAC input packet");
			}

			if (netif->input(p, netif) != ERR_OK)
			{
				EXT_ERRORF(("ethernetif_input: IP input error"EXT_NEW_LINE ) );
				EXT_ASSERT(("TCPIP input"), 0);
				/* Free buffer. */
				pbuf_free(p);
			}
			break;

		default:
			/* Free buffer. */
			LINK_STATS_INC(link.drop);
#if LWIP_STATS
			_macDev->macStats->rxErrFrame ++;
			EXT_DEBUGF(EXT_DBG_OFF, ("IP input ethtype:0x%x", htons(ethhdr->type) ) );
#endif			
			pbuf_free(p);
			break;
	}
}

static void getRecvPacket(struct netif *netif)
{
	uint32_t index = 0;
	gmac_rx_descriptor_t *rxDesc = NULL;
	uint32_t length = 0;
	struct pbuf *p = 0;
	struct MAC_CTRL *_macDev = netif->state;

	/* Free all RX pbufs. */
	for (index = 0; index < GMAC_RX_BUFFERS; index++)
	{
		rxDesc = &_macDev->rxDescs[index];

		if ((rxDesc->addr.val & GMAC_RXD_OWNERSHIP) == GMAC_RXD_OWNERSHIP)
		{
			/* Packet is a SOF since packet size is set to maximum. */
			length = rxDesc->status.val & GMAC_RXD_LEN_MASK;

			/* Fetch pre-allocated pbuf. */
			p = _macDev->rxPbuf[index];
			p->len = length;

			/* Remove this pbuf from its desriptor. */
			_macDev->rxPbuf[index] = 0;

			EXT_DEBUGF(NETIF_DEBUG, ("DMA(From ISR) buffer %p received, size=%u [idx=%u]"EXT_NEW_LINE, p, (int)length, (int)index));
#if EXT_LWIP_DEBUG
			EXT_LWIP_DEBUG_PBUF(p);
			if(p->next)
			{
				EXT_LWIP_DEBUG_PBUF(p->next);
			}
#endif

			/* Set pbuf total packet size. */
			p->tot_len = length;
			LINK_STATS_INC(link.recv);

			/* Mark the descriptor ready for transfer. */
			rxDesc->addr.val &= ~(GMAC_RXD_OWNERSHIP);

			_macDev->rxReadIdx = (_macDev->rxReadIdx + 1) % GMAC_RX_BUFFERS;

			/* clear these bits as data sheet. Aug,9,2018 J.L.*/
			gmac_clear_rx_status(GMAC, GMAC_RSR_BNA|GMAC_RSR_REC);

#if LWIP_STATS
			lwip_rx_count += length;
			_macDev->macStats->rxPackets++;
#endif
			sendToIp( netif, p);
		}
#if 0
		else
		{
			EXT_ERRORF(("ownership: %"PRIx32", RECV buffer index:%ld", (rxDesc->addr.val & GMAC_RXD_OWNERSHIP), _macDev->rxReadIdx));
	///		p_rx->addr.val &= ~(GMAC_RXD_OWNERSHIP);
	//		ps_gmac_dev->us_rx_idx = (ps_gmac_dev->us_rx_idx + 1) % GMAC_RX_BUFFERS;

			/* clear these bits as data sheet. Aug.9,2018 J.L.*/
			gmac_clear_rx_status(GMAC, GMAC_RSR_BNA|GMAC_RSR_REC);
#if LWIP_STATS
			_macDev->macStats->rxErrOwnership++;
#endif
		}
#endif		
	}
	
}


#if 0
/**
 * \brief Use pre-allocated pbuf as DMA source and return the incoming packet.
 * \param netif the lwIP network interface structure for this ethernetif.
 * \return a pbuf filled with the received packet (including MAC header). 
 * 0 on memory error.
 */
static struct pbuf *_macHwInput(struct netif *netif)
{
	struct MAC_CTRL *_macDev = netif->state;
	struct pbuf *p = 0;
	uint32_t length = 0;

	uint32_t index = 0;
	
	gmac_rx_descriptor_t *rxDesc = &_macDev->rxDescs[_macDev->rxReadIdx];
	uint32_t status = gmac_get_rx_status(GMAC);

#if MUXLAB_GMAC_TEST
	printf("RX Status:0x%"PRIx32""EXT_NEW_LINE, status );
#endif
	/* Handle GMAC overrun or AHB errors. */
	if (status & GMAC_RX_ERRORS)
	{
		gmac_enable_receive(GMAC, false);
		
		EXT_ERRORF(("GMAC overrun: RX Status 0x%"PRIx32"", gmac_get_rx_status(GMAC) ));

		LINK_STATS_INC(link.err);
		LINK_STATS_INC(link.drop);

		/* Free all RX pbufs. */
		for (index = 0; index < GMAC_RX_BUFFERS; index++)
		{
			if (_macDev->rxPbuf[index] != 0)
			{
				pbuf_free(_macDev->rxPbuf[index]);
				_macDev->rxPbuf[index] = 0;
			}
		}

		/* Reinit RX descriptors. */
		_macRxInit(_macDev);

		/* Clear error status. */
		gmac_clear_rx_status(GMAC, GMAC_RX_ERRORS| GMAC_RSR_BNA|GMAC_RSR_REC);

		gmac_enable_receive(GMAC, true);
#if LWIP_STATS
		_macDev->macStats->rxErrOverrun++;
#endif

		return NULL;
	}

//	if((status & (GMAC_RSR_REC|GMAC_RSR_BNA)) == 0)
	if((status & (GMAC_RSR_BNA)) != 0)
	{
		EXT_ERRORF(("GMAC BNA: Buffer Not Available, status '0x%lx'", status ));
		return NULL;
	}

#if MUXLAB_GMAC_TEST
	printf("RX frame address:0x%"PRIx32"; describer status:0x%"PRIx32""EXT_NEW_LINE, rxDesc->addr.val, rxDesc->status.val );
#endif
	/* Check that a packet has been received and processed by GMAC. */
	if ((rxDesc->addr.val & GMAC_RXD_OWNERSHIP) == GMAC_RXD_OWNERSHIP)
	{
		/* Packet is a SOF since packet size is set to maximum. */
		length = rxDesc->status.val & GMAC_RXD_LEN_MASK;

		/* Fetch pre-allocated pbuf. */
		p = _macDev->rxPbuf[_macDev->rxReadIdx];
		p->len = length;

		/* Remove this pbuf from its desriptor. */
		_macDev->rxPbuf[_macDev->rxReadIdx] = 0;

		EXT_DEBUGF(NETIF_DEBUG, ("DMA(From ISR) buffer %p received, size=%u [idx=%u]"EXT_NEW_LINE, p, (int)length, (int)_macDev->rxReadIdx));
#if EXT_LWIP_DEBUG
		EXT_LWIP_DEBUG_PBUF(p);
		if(p->next)
		{
			EXT_LWIP_DEBUG_PBUF(p->next);
		}
#endif

		/* Set pbuf total packet size. */
		p->tot_len = length;
		LINK_STATS_INC(link.recv);

		/* Fill empty descriptors with new pbufs. */
		_rxPopulateQueue(_macDev);

		/* Mark the descriptor ready for transfer. */
		rxDesc->addr.val &= ~(GMAC_RXD_OWNERSHIP);

		_macDev->rxReadIdx = (_macDev->rxReadIdx + 1) % GMAC_RX_BUFFERS;

		/* clear these bits as data sheet. Aug,9,2018 J.L.*/
		gmac_clear_rx_status(GMAC, GMAC_RSR_BNA|GMAC_RSR_REC);

#if LWIP_STATS
		lwip_rx_count += length;
		_macDev->macStats->rxPackets++;
#endif
	}
	else
	{
		EXT_ERRORF(("ownership: %"PRIx32", RECV buffer index:%ld", (rxDesc->addr.val & GMAC_RXD_OWNERSHIP), _macDev->rxReadIdx));
///		p_rx->addr.val &= ~(GMAC_RXD_OWNERSHIP);
//		ps_gmac_dev->us_rx_idx = (ps_gmac_dev->us_rx_idx + 1) % GMAC_RX_BUFFERS;

		/* clear these bits as data sheet. Aug.9,2018 J.L.*/
		gmac_clear_rx_status(GMAC, GMAC_RSR_BNA|GMAC_RSR_REC);
#if LWIP_STATS
		_macDev->macStats->rxErrOwnership++;
#endif
	}

	return p;
}
#endif

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

static void _ethernetifInput(struct netif *netif)
{
	struct MAC_CTRL *_macDev = netif->state;
#if 1
	if(_checkRecvStatus(_macDev)!= ERR_OK)
		return;

	getRecvPacket( netif);

	/* Fill empty descriptors with new pbufs. */
	_rxPopulateQueue(_macDev);

#else
	struct eth_hdr *ethhdr;
	struct pbuf *p;

	/* Move received packet into a new pbuf. */
	p = _macHwInput(netif);
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
			
			{
//				extLwipEthHdrDebugPrint(p->payload, "MAC input packet");
			}

			if (netif->input(p, netif) != ERR_OK)
			{
				EXT_ERRORF(("ethernetif_input: IP input error"EXT_NEW_LINE ) );
				EXT_ASSERT(("TCPIP input"), 0);
				/* Free buffer. */
				pbuf_free(p);
			}
			break;

		default:
			/* Free buffer. */
			LINK_STATS_INC(link.drop);
#if LWIP_STATS
			_macDev->macStats->rxErrFrame ++;
			EXT_DEBUGF(EXT_DBG_OFF, ("IP input ethtype:0x%x", htons(ethhdr->type) ) );
#endif			
			pbuf_free(p);
			break;
	}
#endif

}

#if EXT_WITH_OS
/**
 * \brief GMAC task function. This function waits for the notification
 * semaphore from the interrupt, processes the incoming packet and then
 * passes it to the lwIP stack.
 *
 * \param pvParameters A pointer to the gmac_device instance.
 */
static void gmac_task(void *pvParameters)
{
	struct MAC_CTRL *_macDev = pvParameters;
	const TickType_t	blockTime = pdMS_TO_TICKS( 500 );

	while (1)
	{
#if 0	
		/* Wait for the RX notification semaphore. */
		sys_arch_sem_wait(&_macDev->rxSem, 0);
		/* Process the incoming packet. */
		_ethernetifInput(_macDev->netif);
#else		
		if(xSemaphoreTake(_macDev->rxSem, blockTime) == pdPASS)
		{
			_ethernetifInput(_macDev->netif);
		}
		else
		{/* error handler */
//			EXT_ERRORF(("Timeout in semaphore"));
		}
#endif
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
	EXT_ASSERT(("netif != NULL"), (netif != NULL));

	memset(&macStats, 0, sizeof(struct MAC_STATS) );
	_macCtrl.netif = netif;
	_macCtrl.macStats = &macStats;

#if EXT_LWIP_DEBUG
	EXT_DEBUGF(NETIF_DEBUG, ("Initializing ethernetif_init"EXT_NEW_LINE ) );
	EXT_LWIP_DEBUG_NETIF(netif);
#endif

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname. */
	netif->hostname = "macdev";
#endif /* LWIP_NETIF_HOSTNAME */

	/*
	 * Initialize the snmp variables and counters inside the struct netif.
	 * The last argument should be replaced with your link speed, in units
	 * of bits per second.
	 */
#if LWIP_SNMP
	NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, NET_LINK_SPEED);
#endif /* LWIP_SNMP */

	netif->state = &_macCtrl;
	netif->name[0] = EXT_IF_NAME0;
	netif->name[1] = EXT_IF_NAME1;

	/* We directly use etharp_output() here to save a function call.
	 * You can instead declare your own function an call etharp_output()
	 * from it if you have to do some checks before sending (e.g. if link
	 * is available...) */
	netif->output = etharp_output;
	netif->linkoutput = _macHwOutput;
	/* Initialize the hardware */
	_macHwInit(netif);

#if EXT_WITH_OS
	sys_thread_t id;

#if 0	
	err_t err;
	/* Incoming packet notification semaphore. */
	err = sys_sem_new(&_macCtrl.rx_sem, 0);
	EXT_ASSERT(("ethernetif_init: GMAC RX semaphore allocation ERROR!"), (err == ERR_OK));
	if (err == ERR_MEM)
		return ERR_MEM;
#else
	_macCtrl.rxSem = xSemaphoreCreateCounting(GMAC_RX_BUFFERS, 0);
	EXT_ASSERT(("ethernetif_init: GMAC RX semaphore allocation ERROR!"), (_macCtrl.rxSem != NULL) );
#endif
	id = sys_thread_new(EXT_TASK_MAC, gmac_task, &_macCtrl, EXT_NET_IF_TASK_STACK_SIZE, EXT_NET_IF_TASK_PRIORITY + 4 );
//	id = sys_thread_new(EXT_TASK_MAC, gmac_task, &gs_gmac_dev, netifINTERFACE_TASK_STACK_SIZE*4, EXT_TASK_ETHERNET_PRIORITY);
	EXT_ASSERT(("ethernetif_init: GMAC Task allocation ERROR!"), (id != 0));
	if (id == 0)
		return ERR_MEM;
#endif

	return ERR_OK;
}


void gmacEnableWakeOnLan(unsigned int ipAddr)
{
	Gmac* _pgmac = GMAC;
	_pgmac->GMAC_WOL = GMAC_WOL_MAG|GMAC_WOL_ARP|GMAC_WOL_SA1|GMAC_WOL_MTI|(GMAC_WOL_IP_Msk & ipAddr);
}


/* disable multicast and boardcast for update firmware */
void gmacBMCastEnable(unsigned char enable)
{
	gmac_enable_multicast_hash(GMAC, enable);
	gmac_enable_copy_all(GMAC, enable);
	gmac_disable_broadcast(GMAC, (enable == EXT_FALSE));
}

