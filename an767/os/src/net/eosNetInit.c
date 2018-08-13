/**
 * \file
 *
 * \brief Ethernet management for the lwIP Raw HTTP basic example.
 *
 */

#include "compact.h"
#include "lwipExt.h"

#include "eos.h"

#include "lwip/tcpip.h"

#include "ethernetPhy.h"


/** Maximum transfer unit. */
#define NET_MTU               1500


struct netif		guNetIf;		/* global unique netIf */


static void _extNetStatusCallback(struct netif *netif)
{
	int8_t c_mess[20];
	if (netif_is_up(netif))
	{
		strcpy((char*)c_mess, "IP=");
		strcat((char*)c_mess, inet_ntoa(*(struct in_addr *)&(netif->ip_addr)));
		printf("Network up, IP:%s"EXT_NEW_LINE, inet_ntoa(*(struct in_addr *)&(netif->ip_addr)) );

//		printf("Ethernet hwaddr:%d(%p)"EXT_NEW_LINE, netif->hwaddr_len, netif);

#if 0
		extNetPingInit();
		extNetPingSendNow(netif->ip_addr.addr );
#endif

#ifdef	ARM
		gmacEnableWakeOnLan(netif->ip_addr.addr);
#endif		

	}
	else
	{
		printf("Network down"EXT_NEW_LINE);
	}
}


static void _extEthernetInterfaceConfigure(struct netif *netif, EXT_RUNTIME_CFG *runCfg)
{
//	struct ip_addr x_ip_addr, x_net_mask, x_gateway;
	ip4_addr_t x_ip_addr, x_net_mask, x_gateway;

	{
		/* Set MAC hardware address length. */
		netif->hwaddr_len = NETIF_MAX_HWADDR_LEN;
		/* Set MAC hardware address. */
		netif->hwaddr[0] = runCfg->local.mac.address[0];
		netif->hwaddr[1] = runCfg->local.mac.address[1];
		netif->hwaddr[2] = runCfg->local.mac.address[2];
		netif->hwaddr[3] = runCfg->local.mac.address[3];
		netif->hwaddr[4] = runCfg->local.mac.address[4];
		netif->hwaddr[5] = runCfg->local.mac.address[5];

		/* Set maximum transfer unit. */
		netif->mtu = NET_MTU;
	}

	if(EXT_DHCP_IS_ENABLE(runCfg))
	{/* DHCP mode. */
		EXT_LWIP_INT_TO_IP(&x_ip_addr, 0);
		EXT_LWIP_INT_TO_IP(&x_net_mask, 0);
		EXT_LWIP_INT_TO_IP(&x_gateway, 0);
	}	
	else
	{/* Fixed IP mode. */
		EXT_LWIP_INT_TO_IP(&x_ip_addr, runCfg->local.ip);
		EXT_LWIP_INT_TO_IP(&x_net_mask, runCfg->ipMask);
		EXT_LWIP_INT_TO_IP(&x_gateway, runCfg->ipGateway);
	}


//	printf("Add netif %d(%p)..."EXT_NEW_LINE, netif->hwaddr_len, netif);
//	netif->flags |= NETIF_FLAG_IGMP;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP | NETIF_FLAG_ETHERNET;
	/* Add data to netif */
#if 0
	if (NULL == netif_add(netif, &x_ip_addr, &x_net_mask, &x_gateway, NULL, ethernetif_init, ethernet_input))
	{
		EXT_ASSERT(("NULL == netif_add"), 0);
	}
#else

#if EXT_LWIP_DEBUG
	EXT_DEBUGF(EXT_DBG_ON, ("before %d(%p), offset %d:%d:%d..."EXT_NEW_LINE, 
		netif->hwaddr_len, netif , NETIF_HWADDR_OFFSET(), (offsetof(struct netif, rs_count)), (offsetof(struct netif, mtu)) ) );
	EXT_LWIP_DEBUG_NETIF(netif);
#endif

	if (NULL == netif_add(netif, &x_ip_addr, &x_net_mask, &x_gateway, runCfg, ethernetif_init, tcpip_input))
	{
		EXT_ASSERT(("NULL == netif_add"), 0);
	}
#endif

	/* Make it the default interface */
//	printf("Setup default netif...\r\n");
	netif_set_default(netif);

	/* Setup callback function for netif status change */
	netif_set_status_callback(netif, _extNetStatusCallback);

	/* Bring it up */
	if(EXT_DHCP_IS_ENABLE(runCfg))
	{
		/* DHCP mode. */
		EXT_DEBUGF(EXT_DBG_ON, ("DHCP Starting %s..."EXT_NEW_LINE, "test") );
		netif->flags |= NETIF_FLAG_UP;	/* make it up to process DHCP packets. J.L. */
		if (ERR_OK != dhcp_start(netif))
		{
			EXT_ASSERT(("ERR_OK != dhcp_start"), 0);
		}
		EXT_DEBUGF(EXT_DBG_ON, ("DHCP Started"EXT_NEW_LINE) );
	}
	else
	{
		/* Static mode. */
//		printf("Setup netif...\r\n");
		netif_set_up(netif);
//		printf("Static IP Address Assigned\r\n");
	}
}

#if	EXT_WITH_OS

#else
/* Timer structure for calling lwIP tmr functions without RTOS. */
typedef struct timers_info
{
	uint32_t		timer;
	uint32_t		timer_interval;
	void			(*timer_func)(void);
} timers_info_t;

/* LwIP tmr functions list. */
static timers_info_t gs_timers_table[] =
{
	{0, TCP_TMR_INTERVAL, tcp_tmr},
	{0, IP_TMR_INTERVAL, ip_reass_tmr},
#if 0
	/* LWIP_TCP */
	{0, TCP_FAST_INTERVAL, tcp_fasttmr},
	{0, TCP_SLOW_INTERVAL, tcp_slowtmr},
#endif
	/* LWIP_ARP */
	{0, ARP_TMR_INTERVAL, etharp_tmr},
	/* LWIP_DHCP */
#if LWIP_DHCP
	{0, DHCP_COARSE_TIMER_MSECS, dhcp_coarse_tmr},
	{0, DHCP_FINE_TIMER_MSECS, dhcp_fine_tmr},
#endif
};


/**
 * \brief Timer management function.
 */
static void timers_update(void)
{
	static uint32_t ul_last_time;
	uint32_t ul_cur_time, ul_time_diff, ul_idx_timer;
	timers_info_t *p_tmr_inf;

	ul_cur_time = sys_get_ms();
	if (ul_cur_time >= ul_last_time)
	{
		ul_time_diff = ul_cur_time - ul_last_time;
	}
	else
	{
		ul_time_diff = 0xFFFFFFFF - ul_last_time + ul_cur_time;
	}

	if (ul_time_diff)
	{
		ul_last_time = ul_cur_time;
		for (ul_idx_timer = 0; ul_idx_timer < (sizeof(gs_timers_table) / sizeof(timers_info_t)); ul_idx_timer++)
		{
			p_tmr_inf = &gs_timers_table[ul_idx_timer];
			p_tmr_inf->timer += ul_time_diff;
			if (p_tmr_inf->timer > p_tmr_inf->timer_interval)
			{
				if (p_tmr_inf->timer_func)
				{
					p_tmr_inf->timer_func();
				}

				p_tmr_inf->timer -= p_tmr_inf->timer_interval;
			}
		}
	}
}

/**
 * \brief Process incoming ethernet frames, then update timers.
 */
void ethernet_task(void)
{
	/* Poll the network interface driver for incoming ethernet frames. */
	ethernetif_input(&guNetIf);

	/* Update the periodic timer. */
	timers_update();
}

static void _ethernetTask(void *param)
{
	param = param;
	while (1)
	{
		ethernet_task();
	}
}
#endif


/**
 * \brief Initialize the lwIP TCP/IP stack with the network interface driver.
 */
void extBspNetStackInit(EXT_RUNTIME_CFG *runCfg)
{
	struct netif *netif = &guNetIf;
	/* Initialize lwIP. */
	netif->hwaddr_len = 6;

	srand( sys_now() );
	
	/* Initialize lwIP. */
#if EXT_WITH_OS
	/* Call tcpip_init for threaded lwIP mode. JL */
	tcpip_init(NULL, NULL);
#else
	lwip_init();
#endif

//	printf("Initializing LwIP, netif:%d(%p)..."EXT_NEW_LINE, netif->hwaddr_len, netif );

	/* Set hw and IP parameters, initialize MAC too. */
	_extEthernetInterfaceConfigure(netif, runCfg);

	/* Initialize timer. */
	printf("Initializing timer...\r\n");
	sys_init_timing();

#if EXT_WITH_OS
	/*also support RAW */

//	printf("Ethernet hwaddr:%d(%p)"EXT_NEW_LINE, netif->hwaddr_len, netif);
//	netif->hwaddr_len = 6;
//	printf("Set Ethernet hwaddr:%d(%p)"EXT_NEW_LINE, netif->hwaddr_len, netif);

//	extNetShellInit();

//	extNetTelnetInit();

//	_httpThreadInit(runCfg);
	extLwipStartup(netif, runCfg);
#else

	printf("Ethernet Task initializing...\n\r");
	xTaskCreate(_ethernetTask, "ethTask", EXT_TASK_ETHERNET_STACK_SIZE/* 1024*2*/, NULL, EXT_TASK_ETHERNET_PRIORITY, NULL);
#endif

}

