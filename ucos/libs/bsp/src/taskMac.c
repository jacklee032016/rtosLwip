/**
 * GMAC (Gigabit MAC) driver.
 */

#include "ext.h"

#include "lwipExt.h"

/* Standard includes. */
#include "string.h"
#include "stdio.h"

#include "pmc.h"
#include "ethPhy.h"
#include "sysclk.h"

/* gmac.h contains all definations in ASF/SAM, some collisions with thirdparty. so only include it in c source file */
#include "gmac.h"


/** Number of buffer for RX */
#define GMAC_RX_BUFFERS						7	/* enlarge to decrease the risk of DMA buffers overun. Aug.8th, 2018. J.L. */

/** Number of buffer for TX */
#define GMAC_TX_BUFFERS						3	/* never Overrun in TX when 3 */



#define	MUXLAB_GMAC_TEST						0	/* test GMAC of E70 */

#define	MUXLAB_GMAC_ENABLE_MULTICAST		1

/** Network interface identifier. */
#define EXT_IF_NAME0               'e'
#define EXT_IF_NAME1               'n'


/** Network link speed. 100MB */
#define NET_LINK_SPEED        100000000

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
//#define GMAC_RX_ERRORS (GMAC_RSR_RXOVR | GMAC_RSR_HNO)
#define GMAC_RX_ERRORS		(GMAC_RSR_RXOVR)


/** TX descriptor lists */
//COMPILER_ALIGNED(8)
static gmac_tx_descriptor_t	_txDescNull;
/** RX descriptors lists */
//COMPILER_ALIGNED(8)
static gmac_rx_descriptor_t	_rxDescNull;

#if WITH_LWIP
#else
#endif

struct _MAC_CTRL
{
	Gmac 				*mac;
	/**
	 * Pointer to allocated TX buffer.
	 * Section 3.6 of AMBA 2.0 spec states that burst should not cross 1K Boundaries.
	 * Receive buffer manager writes are burst of 2 words => 3 lsb bits of the address shall be set to 0.
	 */
	 
	/** Pointer to Rx descriptor list (must be 8-byte aligned). */
	gmac_rx_descriptor_t	rxDescs[GMAC_RX_BUFFERS];

#if WITH_LWIP	
	/** RX pbuf pointer list. */
	struct pbuf			*rxPbufs[GMAC_RX_BUFFERS];
#else
	uint8_t				rxPbufs[GMAC_RX_BUFFERS][GMAC_TX_UNITSIZE+2]; /* 4 byte alignment for performance */
#endif

	/** RX index for current processing TD. */
	uint32_t				readIdx;

	/** Pointer to Tx descriptor list (must be 8-byte aligned). */
	gmac_tx_descriptor_t	txDescs[GMAC_TX_BUFFERS];
	/** TX buffers. */
	uint8_t				txBufs[GMAC_TX_BUFFERS][GMAC_TX_UNITSIZE+2]; /* 4 byte alignment for performance */
	/** Circular buffer head pointer by upper layer (buffer to be sent). */
	uint32_t				writeIdx;

	OS_SEM				rxSema;

	struct MAC_STATS	*stats;

#if WITH_LWIP	
	/** Reference to lwIP netif structure. */
	struct netif			*netif;
#endif
//	EXT_SYS_T			*extSys;
	EXT_RUNTIME_CFG	*extSys;
}__attribute__ ((packed));


typedef	struct _MAC_CTRL MAC_CTRL;

/** Ethernet header */
struct sim_eth_hdr
{
	EXT_MAC_ADDRESS	dest;
	EXT_MAC_ADDRESS	src;
	uint16_t				type;
}__attribute__ ((packed));

#if __UCOS__
static OS_TCB			_macTaskTcb;
static CPU_STK		_macTaskStk[MAC_TASK_STK_SIZE];
#endif

//COMPILER_ALIGNED(8)
static MAC_CTRL		_macDev;

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

void GMAC_Handler(void)
{
//	portBASE_TYPE xGMACTaskWoken = pdFALSE;
	OS_ERR err;
	volatile uint32_t isr;
	
	CPU_SR_ALLOC();
	CPU_CRITICAL_ENTER();
	OSIntEnter(); /* Tell the OS we're in an interrupt                    */
	CPU_CRITICAL_EXIT();

	/* Get interrupt status. */
	isr = gmac_get_interrupt_status(GMAC);

#if MUXLAB_GMAC_TEST
	/* only for chip debug in ISR */
	printf("ISR status:0x%x"EXT_NEW_LINE, isr);
	_extGMacStatus(GMAC);
#endif

	_macDev.stats->isrCount++;

	/* RX interrupts. */
	if(isr & GMAC_INT_RECV)
	{
//		printf("ISR RECV");
//		xSemaphoreGiveFromISR(gs_gmac_dev.rx_sem, &xGMACTaskWoken);

		uint32_t rxStatus = gmac_get_rx_status(GMAC);
		
	//	printf("ISR RECV:0x%lx; RX:0x%lx"EXT_NEW_LINE, isrStatus, rxStatus);
		if(rxStatus != 0)
		{/* RX status maybe be 0 even when IRQ status say it received. Dec.20, 2018 */
			OSSemPost(&_macDev.rxSema, OS_OPT_POST_1, &err);
			EXT_ASSERT(("MAC POST Sema Failed: 0x%x", err),  err== OS_ERR_NONE);
			_macDev.stats->isrRecvCount++;
		
			NVIC_DisableIRQ(GMAC_IRQn);
		}

	}
	

#if 0
	gmac_enable_management(GMAC, false);
	gmac_enable_receive(GMAC, true);
#endif
//	NVIC_DisableIRQ(GMAC_IRQn);

//	portEND_SWITCHING_ISR(xGMACTaskWoken);
	OSIntExit();
}

/**
 * Populate the RX descriptor ring buffers with pbufs.
 * \note Make sure that the p->payload pointer is 32 bits aligned.
 * (since the lsb are used as status bits by GMAC).
 */
static void _rxPopulateQueue(MAC_CTRL *macDev)
{
	uint32_t index = 0;
#if WITH_LWIP	
	struct pbuf *p = 0;
#endif

	/* Set up the RX descriptors. */
	for(index = 0; index < GMAC_RX_BUFFERS; index++)
	{
#if WITH_LWIP	
		if (macDev->rxPbufs[index] == 0)
		{
			/* Allocate a new pbuf with the maximum size. */
			p = pbuf_alloc(PBUF_RAW, (u16_t) GMAC_FRAME_LENTGH_MAX, PBUF_POOL);
			if (p == NULL)
			{
				EXT_DEBUGF(NETIF_DEBUG, ("gmac_rx_populate_queue: pbuf allocation failure"));
				EXT_ERRORF(("gmac_rx_populate_queue: pbuf allocation failure") );
#if LWIP_STATS
				macDev->stats->rxErrOOM++;
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
			EXT_ASSERT(("pbuf size too small!%d",p->tot_len), (pbuf_clen(p) >= 1));

			/* Make sure that the payload buffer is properly aligned. */
			EXT_ASSERT(("unaligned p->payload buffer address"), (((uint32_t)p->payload & 0xFFFFFFFC) == (uint32_t)p->payload));

			if (index == GMAC_RX_BUFFERS - 1)
				macDev->rxDescs[index].addr.val = (u32_t) p->payload | GMAC_RXD_WRAP;
			else
				macDev->rxDescs[index].addr.val = (u32_t) p->payload;

			/* make is can be used by DMA.  August 9, 2018 J.L.*/
			macDev->rxDescs[index].addr.val &= ~(GMAC_RXD_OWNERSHIP);

			/* Reset status value. */
			macDev->rxDescs[index].status.val = 0;

			/* Save pbuf pointer to be sent to lwIP upper layer. */
			macDev->rxPbufs[index] = p;

//			EXT_DEBUGF(NETIF_DEBUG|PBUF_DEBUG, ("gmac_rx_populate_queue: new pbuf allocated: %p [idx=%u]", p, (int)ul_index));
		}
#else
		if(macDev->rxDescs[index].addr.val == 0)
		{
			if (index == GMAC_RX_BUFFERS - 1)
				macDev->rxDescs[index].addr.val = (((uint32_t)&macDev->rxPbufs[index][0])|GMAC_RXD_WRAP);
			else
				macDev->rxDescs[index].addr.val = (uint32_t)&macDev->rxPbufs[index][0];

			/* make is can be used by DMA.  August 9, 2018 J.L.*/
			macDev->rxDescs[index].addr.val &= ~(GMAC_RXD_OWNERSHIP);
			
			EXT_DEBUGF(EXT_DBG_ON, ("Load #%ld RX buffer on 0x%lx", index, macDev->rxDescs[index].addr.val));
		}
#endif

	}
}

/**
 * Set up the RX descriptor ring buffers.
 * This function sets up the descriptor list used for receive packets.
 */
static void _rxInit(MAC_CTRL *macDev)
{
	uint32_t index = 0;

	macDev->readIdx = 0;

	for (index = 0; index < GMAC_RX_BUFFERS; index++)
	{
#if WITH_LWIP
		macDev->rxPbufs[index] = 0;
		macDev->rxDescs[index].addr.val = 0;
#else
		macDev->rxDescs[index].addr.val = 0;
#endif
		macDev->rxDescs[index].status.val = 0;
	}
//	macDev->rxDescs[index - 1].addr.val |= GMAC_RXD_WRAP;

	_rxPopulateQueue(macDev);

	/* Set receive buffer queue base address pointer. ? no buffer now? */
	gmac_set_rx_queue(macDev->mac, (uint32_t)&macDev->rxDescs[0] );
}



/**
 * Set up the TX descriptor ring buffers.
 * This function sets up the descriptor list used for receive packets.
 */
static void _txInit(MAC_CTRL *macDev)
{
	uint32_t index;

	macDev->writeIdx = 0;

	/* Set up the TX descriptors. */
	for (index = 0; index < GMAC_TX_BUFFERS; index++)
	{
		macDev->txDescs[index].addr = (uint32_t)&macDev->txBufs[index][0];
		macDev->txDescs[index].status.val = GMAC_TXD_USED | GMAC_TXD_LAST;
	}
	
	macDev->txDescs[index - 1].status.val |= GMAC_TXD_WRAP;

	/* Set receive buffer queue base address pointer. */
	gmac_set_tx_queue(macDev->mac, (uint32_t) &macDev->txDescs[0]);
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
#if WITH_LWIP
static err_t _HwOutput(struct netif *netif, struct pbuf *p)
#else
static int _HwOutput(struct netif *netif, uint8_t *p)
#endif
{
	MAC_CTRL *macDev = (MAC_CTRL *)&_macDev;
#if WITH_LWIP
	struct pbuf *q = NULL;
	uint8_t *buffer = 0;
#endif

	uint32_t status = gmac_get_tx_status(macDev->mac);

	/* Handle GMAC underrun or AHB errors. */
	if ( (status& GMAC_TX_ERRORS) )
	{
		EXT_ERRORF( ("GMAC ERROR: 0x%lx, reinit TX...", status));

		gmac_enable_transmit(GMAC, false);

#if WITH_LWIP
		LINK_STATS_INC(link.err);
		LINK_STATS_INC(link.drop);
#endif

		/* Reinit TX descriptors. */
		_txInit(macDev);

		/* Clear error status. */
		gmac_clear_tx_status(macDev->mac, GMAC_TX_ERRORS);

		gmac_enable_transmit(macDev->mac, true);

#if LWIP_STATS
		macDev->stats->txFailed++;
#endif
#if WITH_LWIP
		return ERR_OK;
#else
		return -1;
#endif
	}


#if WITH_LWIP
	buffer = (uint8_t*)macDev->txDescs[macDev->writeIdx].addr;

	/* Copy pbuf chain into TX buffer. */
	for (q = p; q != NULL; q = q->next)
	{
		memcpy(buffer, q->payload, q->len);
		buffer += q->len;
	}

	/* Set len and mark the buffer to be sent by GMAC. */
	macDev->txDescs[macDev->writeIdx].status.bm.b_len = p->tot_len;
	macDev->txDescs[macDev->writeIdx].status.bm.b_used = 0;

//	EXT_DEBUGF(EXT_DBG_OFF, ("DMA buffer sent, size=%d [idx=%u]", p->tot_len,(int)macDev->writeIdx));
#endif

	macDev->writeIdx = (macDev->writeIdx + 1) % GMAC_TX_BUFFERS;

	/* Now start to transmission. */
	gmac_start_transmission(macDev->mac);

#if LWIP_STATS
	lwip_tx_count += p->tot_len;
	macDev->stats->txPackets++;
#endif

#if WITH_LWIP
	LINK_STATS_INC(link.xmit);
	return ERR_OK;
#else
	return 0;
#endif
}


static uint8_t __checkInput(MAC_CTRL *macDev)
{
#if WITH_LWIP
//	uint32_t	length = 0;
	uint32_t 	index = 0;
#endif

	uint32_t	status = gmac_get_rx_status(macDev->mac);

	/* Handle GMAC overrun or AHB errors. */
	if (status & GMAC_RX_ERRORS)
	{
		gmac_enable_receive(macDev->mac, false);
		
		EXT_ERRORF(("GMAC overrun: RX Status 0x%lx", status) );

#if WITH_LWIP
		LINK_STATS_INC(link.err);
		LINK_STATS_INC(link.drop);

		/* Free all RX pbufs. */
		for (index = 0; index < GMAC_RX_BUFFERS; index++)
		{
			if (macDev->rxPbufs[index] != 0)
			{
				pbuf_free(macDev->rxPbufs[index]);
				macDev->rxPbufs[index] = 0;
			}
		}
#endif

		/* Reinit RX descriptors. */
		_rxInit(macDev);

		/* Clear error status. */
//		gmac_clear_rx_status(macDev->mac, GMAC_RX_ERRORS| GMAC_RSR_BNA|GMAC_RSR_REC);
		gmac_clear_rx_status(macDev->mac, GMAC_RX_ERRORS);

		gmac_enable_receive(macDev->mac, true);
#if LWIP_STATS
		macDev->stats->rxErrOverrun++;
#endif

		return EXIT_FAILURE;
	}

	if((status & (GMAC_RSR_BNA)) != 0)
	{
		EXT_INFOF(("MAC BNA: Buffer Not Available"));
		gmac_clear_rx_status(macDev->mac, GMAC_RSR_BNA);
//		return EXIT_FAILURE;
	}

	if((status & (GMAC_RSR_HNO)) != 0)
	{
		EXT_INFOF(("MAC HNO: HResp Not OK"));
		gmac_clear_rx_status(macDev->mac, GMAC_RSR_HNO);
//		return EXIT_FAILURE;
	}

	if( (status &GMAC_RSR_REC) != 0)
	{
		return EXIT_SUCCESS;
	}

	EXT_ERRORF(("MAC RX IRQ: 0x%lx", status));
	return EXIT_FAILURE;
}

#if WITH_LWIP
#else
void extEthHdrDebugPrint(void *ethHdr, const char *prompt)
{
	struct sim_eth_hdr *ethhdr = (struct sim_eth_hdr *)ethHdr;
	unsigned short type = NS2HS(ethhdr->type);

	printf("%s ethernet header: '%s(%04x)' type ", prompt, (type==0x0800U)?"IP":(type==0x0806U)?"ARP":"Other", type);
	printf(": dest:%02x:%02x:%02x:%02x:%02x:%02x, src:%02x:%02x:%0x2:%02x:%02x:%02x, type:%04x"EXT_NEW_LINE,
		(unsigned char)ethhdr->dest.address[0], (unsigned)ethhdr->dest.address[1], (unsigned)ethhdr->dest.address[2],
		(unsigned char)ethhdr->dest.address[3], (unsigned)ethhdr->dest.address[4], (unsigned)ethhdr->dest.address[5],
		(unsigned char)ethhdr->src.address[0],  (unsigned)ethhdr->src.address[1],  (unsigned)ethhdr->src.address[2],
		(unsigned char)ethhdr->src.address[3],  (unsigned)ethhdr->src.address[4],  (unsigned)ethhdr->src.address[5],
		type );
}
#endif


/**
 * \brief This function should be called when a packet is ready to be
 * read from the interface. It uses the function gmac_low_level_input()
 * that handles the actual reception of bytes from the network interface.
 * Then the type of the received packet is determined and the appropriate
 * input function is called.
 *
 * \param netif the lwIP network interface structure for this ethernetif.
 */
#if WITH_LWIP
static void _send2Ip(MAC_CTRL *macDev, struct pbuf *p)
#else
static void _send2Ip(MAC_CTRL *macDev, uint8_t *p)
#endif
{
	uint16_t type;
#if WITH_LWIP
	struct netif *netif = macDev->netif;
	struct eth_hdr	*ethhdr;
#else
 	struct sim_eth_hdr *ethhdr;
#endif

	if (p == NULL)
		return;

	/* Points to packet payload, which starts with an Ethernet header. */
#if WITH_LWIP
	ethhdr = p->payload;
#else
	ethhdr = (struct sim_eth_hdr *)p;
#endif
	type = NS2HS(ethhdr->type);
	switch (type)
	{
		err_t err;
		case 0x0800U:/* IP */
//			EXT_DEBUGF(EXT_DBG_ON, ("IP packet") );
#if WITH_LWIP
#else
			break;
#endif
		case 0x0806U: /* ARP */
//			EXT_DEBUGF(EXT_DBG_ON, ("ARP packet") );
			/* Send packet to lwIP for processing. */
			/* call tcpip_input() */
			
			{
//				extLwipEthHdrDebugPrint(p->payload, "MAC input packet");
			}
#if WITH_LWIP
			err = netif->input(p, netif);
			if (err != ERR_OK)
			{
				EXT_ERRORF(("IP input error :'%s(%d)'", lwip_strerr(err), err ) );
//				EXT_ASSERT(("TCPIP input"), 0);
				/* Free buffer. */
				pbuf_free(p);
			}
#endif			
			break;

		default:
			/* Free buffer. */
#if WITH_LWIP
			LINK_STATS_INC(link.drop);
#endif
#if LWIP_STATS
			macDev->stats->rxErrFrame ++;
#endif			
//			EXT_INFOF(("Unknow ethernet frame type:0x%x", type) );
#if WITH_LWIP
			pbuf_free(p);
#endif
			break;
	}
}

static uint8_t __inputPackets(MAC_CTRL *macDev)
{
	uint32_t	length = 0;
	uint32_t 	index = 0;
	gmac_rx_descriptor_t *rxDesc = NULL;
#if WITH_LWIP
	struct pbuf *p = 0;
#else
	uint8_t *p;
#endif

	for (index = 0; index < GMAC_RX_BUFFERS; index++)
	{
		rxDesc = &macDev->rxDescs[index];

		/* Check that a packet has been received and processed by GMAC. */
		if ((rxDesc->addr.val & GMAC_RXD_OWNERSHIP) == GMAC_RXD_OWNERSHIP)
		{
			/* Packet is a SOF since packet size is set to maximum. */
			length = rxDesc->status.val & GMAC_RXD_LEN_MASK;

			/* Fetch pre-allocated pbuf. */
			p = macDev->rxPbufs[index];

#if WITH_LWIP
			p->len = length;
			/* Remove this pbuf from its desriptor. */
			macDev->rxPbufs[index] = 0;

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
#endif
			EXT_DEBUGF(EXT_DBG_OFF, ("DMA(From ISR) buffer %p received, size=%u [idx=%u]"EXT_NEW_LINE, p, (int)length, (int)index) );
			_send2Ip(macDev, p);

			/* Mark the descriptor ready for transfer. */
//			rxDesc->addr.val &= ~(GMAC_RXD_OWNERSHIP);
			rxDesc->addr.val = 0;

			macDev->readIdx = (macDev->readIdx + 1) % GMAC_RX_BUFFERS;

//			/* clear these bits as data sheet. Aug,9,2018 J.L.*/
//			gmac_clear_rx_status(macDev->mac, GMAC_RSR_BNA|GMAC_RSR_REC);

#if LWIP_STATS
			lwip_rx_count += length;
#endif
			macDev->stats->rxPackets++;
		}
		else
		{
//			EXT_ERRORF(("ownership: %lx, RECV buffer index:%ld", (rxDesc->addr.val & GMAC_RXD_OWNERSHIP), index));
	///		p_rx->addr.val &= ~(GMAC_RXD_OWNERSHIP);
	//		ps_gmac_dev->us_rx_idx = (ps_gmac_dev->us_rx_idx + 1) % GMAC_RX_BUFFERS;

#if LWIP_STATS
			macDev->stats->rxErrOwnership++;
#endif
		}
	}

	return 0;
}


/**
 * GMAC task function. This function waits for the notification
 * semaphore from the interrupt, processes the incoming packet and then
 * passes it to the lwIP stack.
 */
static void _macTaskMain(void *pvParameters)
{
	MAC_CTRL *macDev = pvParameters;
	OS_ERR	err;
	CPU_TS	ts;

	while (1)
	{
		/* Wait for the RX notification semaphore. */
		OSSemPend(&macDev->rxSema, (OS_TICK) EXT_TIME_MS_2_TICK(500), OS_OPT_PEND_BLOCKING, &ts, &err);
		if(err == OS_ERR_NONE)
		{/* Process the incoming packet. */
			EXT_DEBUGF(EXT_DBG_OFF, ("Recv RX IRQ"));
			if(__checkInput(macDev) == EXIT_SUCCESS)
			{
				__inputPackets(macDev);
				
				/* Fill empty descriptors with new pbufs. */
				_rxPopulateQueue(macDev);
				
				/* clear as soon as possible. Dev.15, 2018 JL */
				/* clear these bits as data sheet. Aug.9,2018 J.L.*/
			//	gmac_clear_rx_status(macDev->mac, GMAC_RSR_BNA|GMAC_RSR_REC);
				gmac_clear_rx_status(macDev->mac, GMAC_RSR_REC);
				
			}
			else
			{
				EXT_DEBUGF(EXT_DBG_ON, ("No packet for RX IRQ"));
			}

			NVIC_EnableIRQ(GMAC_IRQn);
		}
		else if(err == OS_ERR_TIMEOUT)
		{
//			EXT_INFOF(("Recv timeout"));
		}
		else
		{
			EXT_ERRORF(("MacTask wait failed: 0x%x", err) );
		}
	}
}



/**
 * Initialize GMAC and PHY.
 * \note Called from ethernetif_init().
 */
static void _macHwInit(MAC_CTRL *macDev)
{
	volatile uint32_t ul_delay;

#if WITH_LWIP
	struct netif *netif = macDev->netif;
	/* Device capabilities. */
	netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
#endif

	/* Wait for PHY to be ready (CAT811: Max400ms). */
	ul_delay = sysclk_get_cpu_hz() / 1000 / 3 * 400;
	while (ul_delay--)
	{
	}

	/* Enable GMAC clock. */
	pmc_enable_periph_clk(ID_GMAC);

	/* Disable TX & RX and more. */
	gmac_network_control(macDev->mac, 0);
	gmac_disable_interrupt(macDev->mac, ~0u);

	gmac_clear_statistics(macDev->mac);

	/* Clear all status bits in the receive status register. */
	gmac_clear_rx_status(macDev->mac, GMAC_RSR_BNA | GMAC_RSR_REC | GMAC_RSR_RXOVR|GMAC_RSR_HNO);

	/* Clear all status bits in the transmit status register. */
	gmac_clear_tx_status(macDev->mac, GMAC_TSR_UBR | GMAC_TSR_COL | GMAC_TSR_RLE
			| GMAC_TSR_TXGO | GMAC_TSR_TFC | GMAC_TSR_TXCOMP
			| GMAC_TSR_HRESP);

	/* Clear interrupts. */
	gmac_get_interrupt_status(macDev->mac);

	/* Enable the copy of data into the buffers
	   ignore broadcasts, and not copy FCS. */
#if MUXLAB_GMAC_ENABLE_MULTICAST	   
	gmac_enable_copy_all(macDev->mac, true);	/* must be enabled to rx multicast packets. J.L. 08.20,2018 */

	gmac_enable_multicast_hash(macDev->mac, true);
#else
	/* not 'copy all', but broadcast still be copied */
	gmac_enable_copy_all(macDev->mac, false);
#endif
	gmac_disable_broadcast(macDev->mac, false);

	/* Set RX buffer size to 1536. */
	gmac_set_rx_bufsize(macDev->mac, 0x18);

	/* Clear interrupts */
	gmac_get_priority_interrupt_status(macDev->mac, GMAC_QUE_2);
	gmac_get_priority_interrupt_status(macDev->mac, GMAC_QUE_1);

	/* Set Tx Priority */
	_txDescNull.addr = (uint32_t)0xFFFFFFFF;
	_txDescNull.status.val = GMAC_TXD_WRAP | GMAC_TXD_USED;
	gmac_set_tx_priority_queue(macDev->mac, (uint32_t)&_txDescNull, GMAC_QUE_2);
	gmac_set_tx_priority_queue(macDev->mac, (uint32_t)&_txDescNull, GMAC_QUE_1);
	
	/* Set Rx Priority */
	_rxDescNull.addr.val = (uint32_t)0xFFFFFFFF & GMAC_RXD_ADDR_MASK;
	_rxDescNull.addr.val |= GMAC_RXD_WRAP;
	_rxDescNull.status.val = 0;
	gmac_set_rx_priority_queue(macDev->mac, (uint32_t)&_rxDescNull, GMAC_QUE_2);
	gmac_set_rx_priority_queue(macDev->mac, (uint32_t)&_rxDescNull, GMAC_QUE_1);

	_rxInit(macDev);
	_txInit(macDev);

	/* Enable Rx, Tx and the statistics register. */
	gmac_enable_transmit(macDev->mac, true);
	gmac_enable_receive(macDev->mac, true);
	gmac_enable_statistics_write(macDev->mac, true);

#if MUXLAB_GMAC_TEST	   
	/* Set up the all types of interrupts*/
	gmac_enable_interrupt(macDev->mac, 0xFFFFFFFF);
#else
	/* Set up only the interrupts for RX and errors. */
	gmac_enable_interrupt(macDev->mac, GMAC_INT_GROUP);
#endif

	/* Set GMAC address. */
#if WITH_LWIP
	gmac_set_address(macDev->mac, 0, netif->hwaddr);
#else
	gmac_set_address(macDev->mac, 0, macDev->extSys->macAddress.address);
#endif
	/* Enable NVIC GMAC interrupt. */
	NVIC_SetPriority(GMAC_IRQn, INT_PRIORITY_GMAC);
	NVIC_EnableIRQ(GMAC_IRQn);

	/* Init MAC PHY driver. */
	bspPhyHwStart(macDev->mac, BOARD_GMAC_PHY_ADDR);
	
#if MUXLAB_GMAC_TEST	   
	/* set loopback mode, just for test sw chip. J.L. */
//	gmac_enable_loop(GMAC, true);
#endif

	/* Set link up*/
#if WITH_LWIP
	netif->flags |= NETIF_FLAG_LINK_UP;
#endif
}


int macTaskStart(EXT_RUNTIME_CFG *runCfg )
{
	OS_ERR err;
	MAC_CTRL *macDev = &_macDev;

	runCfg->macCtrl = macDev;
	macDev->stats = &runCfg->macStats;
	macDev->extSys = runCfg;
	macDev->mac = GMAC;
	
	memset(macDev->stats, 0, sizeof(struct MAC_STATS));

	OSSemCreate(&macDev->rxSema, "MacRxSema", (OS_SEM_CTR)0, &err);
	EXT_ASSERT(("MacRxSema can't be created: %x", err), (err==OS_ERR_NONE) );


	EXT_DEBUGF(EXT_DBG_ON, ("Initializing MAC task" ) );
	OSTaskCreate(
		(OS_TCB *)&_macTaskTcb, 
		(CPU_CHAR *)"MacCtrl", 
		(OS_TASK_PTR )_macTaskMain, 
		(void *)macDev,	/* arg to task's routine */
		(OS_PRIO )MAC_TASK_PRIO, /* lower number is high priority */
		(CPU_STK *)_macTaskStk, /* base address of stack */
		(CPU_STK_SIZE )MAC_TASK_STK_SIZE /10,  /* watermark of stack */
		(CPU_STK_SIZE )MAC_TASK_STK_SIZE, 
		(OS_MSG_QTY )MAC_TASK_Q_SIZE,
		(OS_TICK)0,  
		(void *)0,  
		(OS_OPT )(OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR)/* OS_OPT_NONE*/ , 
		(OS_ERR *)&err);
	if (err != OS_ERR_NONE)
	{
		EXT_ERRORF(("macTask failed for Err = %x",  (int)err));
		return -1;
	}

	return 0;
}


/* start hw to start IRQ after task is ready */
err_t macEthernetInit(struct netif *_netif)
{
	MAC_CTRL *macDev = &_macDev;
	
#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname. */
	_netif->hostname = "gmacdev";
#endif /* LWIP_NETIF_HOSTNAME */
//	_netif->state = macDev;
	macDev->netif = _netif;

#if EXT_LWIP_DEBUG
	EXT_DEBUGF(NETIF_DEBUG, ("macEthernetInit" ) );
	EXT_LWIP_DEBUG_NETIF(_netif);
#endif

	/*
	 * Initialize the snmp variables and counters inside the struct netif.
	 * The last argument should be replaced with your link speed, in units
	 * of bits per second.
	 */
#if LWIP_SNMP
	NETIF_INIT_SNMP(_netif, snmp_ifType_ethernet_csmacd, NET_LINK_SPEED);
#endif /* LWIP_SNMP */

	/* We directly use etharp_output() here to save a function call.
	 * You can instead declare your own function an call etharp_output()
	 * from it if you have to do some checks before sending (e.g. if link
	 * is available...) */
	_netif->output = etharp_output;
	_netif->linkoutput = _HwOutput;

	/* Initialize the hardware */
	_macHwInit(macDev);

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

