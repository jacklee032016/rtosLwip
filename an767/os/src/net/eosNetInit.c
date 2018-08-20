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



struct netif		guNetIf;		/* global unique netIf */

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
	extLwipStartNic(netif, runCfg);

	/* Initialize timer. */
	printf("Initializing timer..."EXT_NEW_LINE);
	sys_init_timing();

#if EXT_WITH_OS
	/*also support RAW */

//	printf("Ethernet hwaddr:%d(%p)"EXT_NEW_LINE, netif->hwaddr_len, netif);
//	netif->hwaddr_len = 6;
//	printf("Set Ethernet hwaddr:%d(%p)"EXT_NEW_LINE, netif->hwaddr_len, netif);
	extLwipStartup(netif, runCfg);
#else

	printf("Ethernet Task initializing..."EXT_NEW_LINE);
	xTaskCreate(_ethernetTask, "ethTask", EXT_TASK_ETHERNET_STACK_SIZE/* 1024*2*/, NULL, EXT_TASK_ETHERNET_PRIORITY, NULL);
#endif

}
