/*
 *
 */


/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

#include "FreeRTOS.h"
#include "task.h"


#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <time.h>
#include <string.h>

#include "lwip/opt.h"

#include "lwip/init.h"

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"

#include "lwip/ip_addr.h"

#include "lwip/dns.h"
#include "lwip/dhcp.h"

#include "lwip/stats.h"

#include "lwip/tcp.h"
#include "lwip/inet_chksum.h"

#include "lwip/tcpip.h"
#include "lwip/sockets.h"

#include "tapif.h"


#include "lwip/ip_addr.h"
#include "arch/perf.h"

#include "lwip/apps/tftp_server.h"

#if LWIP_RAW
#include "lwip/icmp.h"
#include "lwip/raw.h"
#endif

#include "lwipMux.h"
#include "jsmn.h"


#if LWIP_IPV4
/* (manual) host IP configuration */
static ip_addr_t ipaddr, netmask, gw;
#endif /* LWIP_IPV4 */

struct netif guNetIf;


/* nonstatic debug cmd option, exported in lwipopts.h */
unsigned char debug_flags;


#if 0
static void
tcp_debug_timeout(void *data)
{
  LWIP_UNUSED_ARG(data);
#if TCP_DEBUG
  tcp_debug_print_pcbs();
#endif /* TCP_DEBUG */
  sys_timeout(5000, tcp_debug_timeout, NULL);
}

void sntp_set_system_time(u32_t sec)
{
	char buf[32];
	struct tm current_time_val;
	time_t current_time = (time_t)sec;

	localtime_r(&current_time, &current_time_val);

	strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", &current_time_val);
	printf("SNTP time: %s\n", buf);
}

#endif

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
 	taskENTER_CRITICAL();
	{
	        printf("[ASSERT] %s:%lu\n", pcFileName, ulLine);
	        fflush(stdout);
	}
	taskEXIT_CRITICAL();
	exit(-1);
}

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	vAssertCalled( __LINE__, __FILE__ );
}

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */

		/* Call the idle task processing used by the full demo.  The simple
		blinky demo does not use the idle task hook. */
		//vFullDemoIdleFunction();
}


static void _startNetServices(void *arg)
{
	sys_sem_t *sem;
	MUX_RUNTIME_CFG *runCfg = (MUX_RUNTIME_CFG *)arg;

	LWIP_ASSERT(("runCfg is NULL"), (runCfg!= NULL));
	sem = (sys_sem_t *)runCfg->data;

#if 0//LWIP_IPV4
	netbiosns_set_name("simhost");
	netbiosns_init();
#endif /* LWIP_IPV4 */

#if 0
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
#if LWIP_DHCP
	sntp_servermode_dhcp(1); /* get SNTP server via DHCP */
#else /* LWIP_DHCP */
#if LWIP_IPV4
	sntp_setserver(0, netif_ip_gw4(&guNetIf));
#endif /* LWIP_IPV4 */
#endif /* LWIP_DHCP */
	sntp_init();

#endif

	muxLwipStartup(&guNetIf, &muxRun);

}


static void _netifStatusCb(struct netif *nif)
{
	printf("NETIF: %c%c%d is %s\n", nif->name[0], nif->name[1], nif->num, netif_is_up(nif) ? "UP" : "DOWN");
	if (netif_is_up(nif))
	{
#if LWIP_IPV4
		printf("IPV4: Host at %s ", ip4addr_ntoa(netif_ip4_addr(nif)));
		printf("mask %s ", ip4addr_ntoa(netif_ip4_netmask(nif)));
		printf("gateway %s\n", ip4addr_ntoa(netif_ip4_gw(nif)));
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
		printf("IPV6: Host at %s\n", ip6addr_ntoa(netif_ip6_addr(nif, 0)));
#endif /* LWIP_IPV6 */
#if LWIP_NETIF_HOSTNAME
		printf("FQDN: %s\n", netif_get_hostname(nif));
#endif /* LWIP_NETIF_HOSTNAME */
	}
	
#if LWIP_MDNS_RESPONDER
	mdns_resp_netif_settings_changed(nif);
#endif
}

static void _nicInterfaceConfigure(struct netif *netif, MUX_RUNTIME_CFG *runCfg)
{
/** Maximum transfer unit. */
#define NET_MTU               1500

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

	if(MUX_DHCP_IS_ENABLE(runCfg))
	{/* DHCP mode. */
		MUX_LWIP_INT_TO_IP(&x_ip_addr, 0);
		MUX_LWIP_INT_TO_IP(&x_net_mask, 0);
		MUX_LWIP_INT_TO_IP(&x_gateway, 0);
	}	
	else
	{/* Fixed IP mode. */
		MUX_LWIP_INT_TO_IP(&x_ip_addr, runCfg->local.ip);
		MUX_LWIP_INT_TO_IP(&x_net_mask, runCfg->ipMask);
		MUX_LWIP_INT_TO_IP(&x_gateway, runCfg->ipGateway);
	}


//	printf("Add netif %d(%p)..."MUX_NEW_LINE, netif->hwaddr_len, netif);
//	netif->flags |= NETIF_FLAG_IGMP;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP | NETIF_FLAG_ETHERNET;
	/* Add data to netif */
#if 0
	if (NULL == netif_add(netif, &x_ip_addr, &x_net_mask, &x_gateway, NULL, ethernetif_init, ethernet_input))
	{
		LWIP_ASSERT("NULL == netif_add", 0);
	}
#else

#if MUX_LWIP_DEBUG
	MUX_DEBUGF(MUX_DBG_ON, ("before %d(%p), offset %d:%d:%d..."MUX_NEW_LINE, 
		netif->hwaddr_len, netif , NETIF_HWADDR_OFFSET(), (offsetof(struct netif, rs_count)), (offsetof(struct netif, mtu)) ) );
	MUX_LWIP_DEBUG_NETIF(netif);
#endif

	if (NULL == netif_add(netif, &x_ip_addr, &x_net_mask, &x_gateway, runCfg, tapif_init, tcpip_input))
	{
		LWIP_ASSERT("NULL == netif_add", 0);
	}
#endif

	/* Make it the default interface */
//	printf("Setup default netif...\r\n");
	netif_set_default(netif);

	/* Setup callback function for netif status change */
	netif_set_status_callback(netif, _netifStatusCb);

	/* Bring it up */
	if(MUX_DHCP_IS_ENABLE(runCfg))
	{
		/* DHCP mode. */
		MUX_DEBUGF(MUX_DBG_ON, ("DHCP Starting %s..."MUX_NEW_LINE, "test") );
		netif->flags |= NETIF_FLAG_UP;	/* make it up to process DHCP packets. J.L. */
		if (ERR_OK != dhcp_start(netif))
		{
			LWIP_ASSERT("ERR_OK != dhcp_start", 0);
		}
		MUX_DEBUGF(MUX_DBG_ON, ("DHCP Started"MUX_NEW_LINE) );
	}
	else
	{
		/* Static mode. */
//		printf("Setup netif...\r\n");
		netif_set_up(netif);
//		printf("Static IP Address Assigned\r\n");
	}
}


int main(int argc, char **argv)
{
	int ch;
	char ip_str[IPADDR_STRLEN_MAX] = {0};

	MUX_RUNTIME_CFG *runCfg = (MUX_RUNTIME_CFG *)&muxRun;

	muxSysParamsInit(runCfg);

	/* startup defaults (may be overridden by one or more opts) */
#if LWIP_IPV4
	/* not used now, instead MuxRun . 04.26,2018*/
	IP_ADDR4(&gw,      192, 168,  166,1);
	IP_ADDR4(&netmask, 255,255,255,0);
	IP_ADDR4(&ipaddr,  192, 168, 166, 2);
#endif /* LWIP_IPV4 */

	/* use debug flags defined by debug.h */
	debug_flags = LWIP_DBG_ON;


	debug_flags |= (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT);

#if MUX_WITH_OS
	/* Call tcpip_init for threaded lwIP mode. JL */
	tcpip_init(NULL, runCfg);
#else
	lwip_init();
#endif

	TRACE();

	/* Set hw and IP parameters, initialize MAC too. */
	_nicInterfaceConfigure(&guNetIf, runCfg);

	TRACE();

	_startNetServices(runCfg);
#ifdef PERF
	perf_init("/tmp/simhost.perf");
#endif /* PERF */

	TRACE();

	printf("TCP/IP initialized.\n");
	printf("Applications started.\n");

#if LWIP_STATS
	stats_display();
#endif

	muxJobPeriod(&muxRun);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();
	for( ;; );

	return 0;
}

