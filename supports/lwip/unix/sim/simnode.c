/*
 *
 */

#include "lwip/debug.h"

#include <unistd.h>


#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/tcp.h"
#include "lwip/priv/tcp_priv.h" /* for tcp_debug_print_pcbs() */
#include "lwip/timeouts.h"

#include "lwip/stats.h"


#include "lwip/tcpip.h"


#include "netif/unixif.h"
#include "netif/dropif.h"

#include "netif/tcpdump.h"

#include "lwip/ip_addr.h"

#include "arch/perf.h"

#include "apps/udpecho/udpecho.h"

#if LWIP_IPV4 /* @todo: IPv6 */

/* nonstatic debug cmd option, exported in lwipopts.h */
unsigned char debug_flags;

static void tcp_timeout(void *data)
{
	LWIP_UNUSED_ARG(data);
#if TCP_DEBUG && LWIP_TCP
	tcp_debug_print_pcbs();
#endif /* TCP_DEBUG */
	sys_timeout(5000, tcp_timeout, NULL);
}

struct netif netif_unix;

static void tcpip_init_done(void *arg)
{
	ip4_addr_t ipaddr, netmask, gw;
	sys_sem_t *sem;
	sem = (sys_sem_t *)arg;

	IP4_ADDR(&gw, 192,168,167, 1);
	IP4_ADDR(&ipaddr, 192,168,167, 2);
	IP4_ADDR(&netmask, 255,255,255,0);

	netif_set_default(netif_add(&netif_unix, &ipaddr, &netmask, &gw, NULL, unixif_init_client, tcpip_input));
	netif_set_up(&netif_unix);
#if LWIP_IPV6
	netif_create_ip6_linklocal_address(&netif_unix, 1);
#endif
	/*  netif_set_default(netif_add(&ipaddr, &netmask, &gw, NULL, sioslipif_init1, tcpip_input)); */

#if LWIP_NETCONN
	tcpecho_init();
	//  shell_init();
#if 0//LWIP_IPV4 && LWIP_TCP
	httpd_init();
#endif
	udpecho_init();
#endif

	printf("Applications started.\n");

	sys_timeout(5000, tcp_timeout, NULL);

	sys_sem_signal(sem);
}


static void main_thread(void *arg)
{
	sys_sem_t sem;
	LWIP_UNUSED_ARG(arg);

	if(sys_sem_new(&sem, 0) != ERR_OK)
	{
		LWIP_ASSERT(("Failed to create semaphore"), 0);
	}
	
	tcpip_init(tcpip_init_done, &sem);
	sys_sem_wait(&sem);
	printf("TCP/IP initialized.\n");

#ifdef MEM_PERF
	mem_perf_init("/tmp/memstats.client");
#endif /* MEM_PERF */

	/* Block forever. */
	sys_sem_wait(&sem);
}

int main(void)
{
#ifdef PERF
	perf_init("/tmp/client.perf");
#endif /* PERF */

#if LWIP_IPV4 && LWIP_TCP
	tcpdump_init();
#endif

	printf("System initialized.\n");

	sys_thread_new("main_thread", main_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
	pause();
	return 0;
}

#else /* LWIP_IPV4 */
int main(int argc, char **argv)
{
	LWIP_UNUSED_ARG(argc);
	LWIP_UNUSED_ARG(argv);

	printf("simnode only works with IPv4\n");

	return 0;
}

#endif /* LWIP_IPV4 */

/* dummy, because SNTP is pulled in via LWIP_DHCP_GET_NTP_SRV */
void sntp_set_system_time(u32_t sec)
{
	LWIP_UNUSED_ARG(sec);
}

