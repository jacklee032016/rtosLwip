/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

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

#include "netif/tapif.h"


#include "netif/tunif.h"

#if 0
#include "netif/unixif.h"
#include "netif/dropif.h"
#include "netif/pcapif.h"
#include "netif/tcpdump.h"
#endif

#ifndef LWIP_HAVE_SLIPIF
#define LWIP_HAVE_SLIPIF 0
#endif
#if LWIP_HAVE_SLIPIF
#include "netif/slipif.h"
#define SLIP_PTY_TEST 1
#endif

#if PPP_SUPPORT
#include "netif/ppp/pppos.h"
#include "lwip/sio.h"
#define PPP_PTY_TEST 1
#include <termios.h>
#endif

#include "lwip/ip_addr.h"
#include "arch/perf.h"

#if 0
#include "apps/udpecho/udpecho.h"
#include "apps/ping/ping.h"
#endif
#include "apps/chargen/chargen.h"
#include "apps/netio/netio.h"
#include "lwip/apps/netbiosns.h"
#include "lwip/apps/mdns.h"
#include "lwip/apps/sntp.h"
#include "lwip/apps/snmp.h"
#include "lwip/apps/snmp_mib2.h"
#include "apps/snmp_private_mib/private_mib.h"
#include "lwip/apps/tftp_server.h"

#if LWIP_RAW
#include "lwip/icmp.h"
#include "lwip/raw.h"
#endif

#include "lwipExt.h"
#include "jsmn.h"

#if LWIP_SNMP
static const struct snmp_mib *mibs[] = {
  &mib2,
  &mib_private
};
#endif /* LWIP_SNMP */

#if LWIP_IPV4
/* (manual) host IP configuration */
static ip_addr_t ipaddr, netmask, gw;
#endif /* LWIP_IPV4 */

static struct netif _guNetIf;

/* ping out destination cmd option */
static unsigned char ping_flag;
static ip_addr_t ping_addr;

/* nonstatic debug cmd option, exported in lwipopts.h */
unsigned char debug_flags;

/** @todo add options for selecting netif, starting DHCP client etc */
static struct option longopts[] =
{
	/* turn on debugging output (if build with LWIP_DEBUG) */
	{"rx", no_argument,        NULL, 'r'},
	{"debug", no_argument,        NULL, 'd'},
	/* help */
	{"help", no_argument, NULL, 'h'},
#if LWIP_IPV4
	/* gateway address */
	{"gateway", required_argument, NULL, 'g'},
	/* ip address */
	{"ipaddr", required_argument, NULL, 'i'},
	/* netmask */
	{"netmask", required_argument, NULL, 'm'},
	/* ping destination */
	{"ping",   required_argument, NULL, 'p'},
#endif /* LWIP_IPV4 */
	/* new command line options go here! */
	{NULL,   0,                 NULL,  0}
};

#define NUM_OPTS ((sizeof(longopts) / sizeof(struct option)) - 1)

static void init_netifs(EXT_RUNTIME_CFG *runCfg);

static void usage(void)
{
	unsigned char i;

	printf("options:\n");
	for (i = 0; i < NUM_OPTS; i++)
	{
		printf("-%c --%s\n",longopts[i].val, longopts[i].name);
	}
}

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
#endif

void sntp_set_system_time(u32_t sec)
{
	char buf[32];
	struct tm current_time_val;
	time_t current_time = (time_t)sec;

	localtime_r(&current_time, &current_time_val);

	strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", &current_time_val);
	printf("SNTP time: %s\n", buf);
}



static void simhost_tcpip_init_done(void *arg)
{
	sys_sem_t *sem;
	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)arg;

	LWIP_ASSERT(("runCfg is NULL"), (runCfg!= NULL));
	sem = (sys_sem_t *)runCfg->data;

	init_netifs(runCfg);

	extLwipStartNic(runCfg);


#if LWIP_TCP
	netio_init();
#endif


#if LWIP_SOCKET
	chargen_init();
#endif
//	extLwipStartup( runCfg);


#if LWIP_IPV4
	netbiosns_set_name("simhost");
	netbiosns_init();
#endif /* LWIP_IPV4 */

	sntp_setoperatingmode(SNTP_OPMODE_POLL);
#if LWIP_DHCP
	sntp_servermode_dhcp(1); /* get SNTP server via DHCP */
#else /* LWIP_DHCP */
#if LWIP_IPV4
	sntp_setserver(0, netif_ip_gw4(&_guNetIf));
#endif /* LWIP_IPV4 */
#endif /* LWIP_DHCP */
	sntp_init();

#if LWIP_SNMP
	lwip_privmib_init();
#if SNMP_LWIP_MIB2
#if SNMP_USE_NETCONN
	snmp_threadsync_init(&snmp_mib2_lwip_locks, snmp_mib2_lwip_synchronizer);
#endif /* SNMP_USE_NETCONN */
	snmp_mib2_set_syscontact_readonly((const u8_t*)"root", NULL);
	snmp_mib2_set_syslocation_readonly((const u8_t*)"lwIP development PC", NULL);
	snmp_mib2_set_sysdescr((const u8_t*)"simhost", NULL);
#endif /* SNMP_LWIP_MIB2 */

	snmp_set_mibs(mibs, LWIP_ARRAYSIZE(mibs));
	snmp_init();
#endif /* LWIP_SNMP */

	sys_sem_signal(sem);

	EXT_DEBUGF(EXT_DBG_ON, ("TCPIP initialization done"));
}


#if LWIP_HAVE_SLIPIF
/* (manual) host IP configuration */
#if LWIP_IPV4
static ip_addr_t ipaddr_slip, netmask_slip, gw_slip;
#endif /* LWIP_IPV4 */
struct netif slipif;
#endif /* LWIP_HAVE_SLIPIF */


#if PPP_SUPPORT
sio_fd_t ppp_sio;
ppp_pcb *ppp;
struct netif pppos_netif;

static void
pppos_rx_thread(void *arg)
{
  u32_t len;
  u8_t buffer[128];
  LWIP_UNUSED_ARG(arg);

  /* Please read the "PPPoS input path" chapter in the PPP documentation. */
  while (1) {
    len = sio_read(ppp_sio, buffer, sizeof(buffer));
    if (len > 0) {
      /* Pass received raw characters from PPPoS to be decoded through lwIP
       * TCPIP thread using the TCPIP API. This is thread safe in all cases
       * but you should avoid passing data byte after byte. */
      pppos_input_tcpip(ppp, buffer, len);
    }
  }
}

static void
ppp_link_status_cb(ppp_pcb *pcb, int err_code, void *ctx)
{
    struct netif *pppif = ppp_netif(pcb);
    LWIP_UNUSED_ARG(ctx);

    switch(err_code) {
    case PPPERR_NONE:               /* No error. */
        {
#if LWIP_DNS
        const ip_addr_t *ns;
#endif /* LWIP_DNS */
        fprintf(stderr, "ppp_link_status_cb: PPPERR_NONE\n\r");
#if LWIP_IPV4
        fprintf(stderr, "   our_ip4addr = %s\n\r", ip4addr_ntoa(netif_ip4_addr(pppif)));
        fprintf(stderr, "   his_ipaddr  = %s\n\r", ip4addr_ntoa(netif_ip4_gw(pppif)));
        fprintf(stderr, "   netmask     = %s\n\r", ip4addr_ntoa(netif_ip4_netmask(pppif)));
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
        fprintf(stderr, "   our_ip6addr = %s\n\r", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif /* LWIP_IPV6 */

#if LWIP_DNS
        ns = dns_getserver(0);
        fprintf(stderr, "   dns1        = %s\n\r", ipaddr_ntoa(ns));
        ns = dns_getserver(1);
        fprintf(stderr, "   dns2        = %s\n\r", ipaddr_ntoa(ns));
#endif /* LWIP_DNS */
#if PPP_IPV6_SUPPORT
        fprintf(stderr, "   our6_ipaddr = %s\n\r", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif /* PPP_IPV6_SUPPORT */
        }
        break;

    case PPPERR_PARAM:             /* Invalid parameter. */
        printf("ppp_link_status_cb: PPPERR_PARAM\n");
        break;

    case PPPERR_OPEN:              /* Unable to open PPP session. */
        printf("ppp_link_status_cb: PPPERR_OPEN\n");
        break;

    case PPPERR_DEVICE:            /* Invalid I/O device for PPP. */
        printf("ppp_link_status_cb: PPPERR_DEVICE\n");
        break;

    case PPPERR_ALLOC:             /* Unable to allocate resources. */
        printf("ppp_link_status_cb: PPPERR_ALLOC\n");
        break;

    case PPPERR_USER:              /* User interrupt. */
        printf("ppp_link_status_cb: PPPERR_USER\n");
        break;

    case PPPERR_CONNECT:           /* Connection lost. */
        printf("ppp_link_status_cb: PPPERR_CONNECT\n");
        break;

    case PPPERR_AUTHFAIL:          /* Failed authentication challenge. */
        printf("ppp_link_status_cb: PPPERR_AUTHFAIL\n");
        break;

    case PPPERR_PROTOCOL:          /* Failed to meet protocol. */
        printf("ppp_link_status_cb: PPPERR_PROTOCOL\n");
        break;

    case PPPERR_PEERDEAD:          /* Connection timeout. */
        printf("ppp_link_status_cb: PPPERR_PEERDEAD\n");
        break;

    case PPPERR_IDLETIMEOUT:       /* Idle Timeout. */
        printf("ppp_link_status_cb: PPPERR_IDLETIMEOUT\n");
        break;

    case PPPERR_CONNECTTIME:       /* PPPERR_CONNECTTIME. */
        printf("ppp_link_status_cb: PPPERR_CONNECTTIME\n");
        break;

    case PPPERR_LOOPBACK:          /* Connection timeout. */
        printf("ppp_link_status_cb: PPPERR_LOOPBACK\n");
        break;

    default:
        printf("ppp_link_status_cb: unknown errCode %d\n", err_code);
        break;
    }
}

static u32_t ppp_output_cb(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx)
{
	LWIP_UNUSED_ARG(pcb);
	LWIP_UNUSED_ARG(ctx);
	return sio_write(ppp_sio, data, len);
}
#endif


static void init_netifs(EXT_RUNTIME_CFG *runCfg)
{
#if LWIP_HAVE_SLIPIF
#if SLIP_PTY_TEST
	u8_t siodev_slip = 3;
#else
	u8_t siodev_slip = 0;
#endif

#if LWIP_IPV4
	netif_add(&slipif, ip_2_ip4(&ipaddr_slip), ip_2_ip4(&netmask_slip), ip_2_ip4(&gw_slip), (void*)&siodev_slip, slipif_init, tcpip_input);
#else /* LWIP_IPV4 */
	netif_add(&slipif, (void*)&siodev_slip, slipif_init, tcpip_input);
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
	netif_create_ip6_linklocal_address(&slipif, 1);
#endif

#if LWIP_NETIF_STATUS_CALLBACK
	netif_set_status_callback(&slipif, extLwipNetStatusCallback);
#endif /* LWIP_NETIF_STATUS_CALLBACK */

	netif_set_link_up(&slipif);
	netif_set_up(&slipif);
#endif /* LWIP_HAVE_SLIPIF */

#if PPP_SUPPORT
#if PPP_PTY_TEST
	ppp_sio = sio_open(2);
#else
	ppp_sio = sio_open(0);
#endif
	if(!ppp_sio)
	{
		perror("Error opening device: ");
		exit(1);
	}

	ppp = pppos_create(&pppos_netif, ppp_output_cb, ppp_link_status_cb, NULL);
	if (!ppp)
	{
		printf("Could not create PPP control interface");
		exit(1);
	}

#ifdef LWIP_PPP_CHAP_TEST
	ppp_set_auth(ppp, PPPAUTHTYPE_CHAP, "lwip", "mysecret");
#endif

	ppp_connect(ppp, 0);

#if LWIP_NETIF_STATUS_CALLBACK
	netif_set_status_callback(&pppos_netif, extLwipNetStatusCallback);
#endif /* LWIP_NETIF_STATUS_CALLBACK */
#endif /* PPP_SUPPORT */

#if 0
	/* Only used for testing purposes: */
	netif_add(&ipaddr, &netmask, &gw, NULL, pcapif_init, tcpip_input);
#endif
	/*  sys_timeout(5000, tcp_debug_timeout, NULL);*/
}


static void main_thread(void *arg)
{
	sys_sem_t sem;

	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)arg;

	extSysParamsInit(runCfg);
	extCfgInitAfterReadFromFlash(runCfg);

TRACE();
	if(sys_sem_new(&sem, 0) != ERR_OK)
	{
		LWIP_ASSERT(("Failed to create semaphore"), 0);
	}

	runCfg->data = &sem;
TRACE();
	tcpip_init(simhost_tcpip_init_done, runCfg);
	
TRACE();
	sys_sem_wait(&sem);

	printf("TCP/IP initialized.\n");


#if 0
	stats_display(NULL, 0);
#endif
	printf("Applications started"LWIP_NEW_LINE);

#ifdef MEM_PERF
	mem_perf_init("/tmp/memstats.client");
#endif /* MEM_PERF */

#if PPP_SUPPORT
	/* Block forever. */
	sys_thread_new("pppos_rx_thread", pppos_rx_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
	sys_sem_wait(&sem);
#endif

	printf("main thread quit"LWIP_NEW_LINE);

}


int main(int argc, char **argv)
{
	int ch;
	char ip_str[IPADDR_STRLEN_MAX] = {0};

	/* startup defaults (may be overridden by one or more opts) */
#if LWIP_IPV4
	/* not used now, instead MuxRun . 04.26,2018*/
	IP_ADDR4(&gw,      192, 168,  166,1);
	IP_ADDR4(&netmask, 255,255,255,0);
	IP_ADDR4(&ipaddr,  192, 168, 166, 2);
#if LWIP_HAVE_SLIPIF
	IP_ADDR4(&gw_slip,      192, 168,  2,  1);
	IP_ADDR4(&netmask_slip, 255,255,255,255);
	IP_ADDR4(&ipaddr_slip,  192,168,  2,  2);
#endif
#endif /* LWIP_IPV4 */

	ping_flag = 0;
	/* use debug flags defined by debug.h */
	debug_flags = LWIP_DBG_ON;

	memset(&extRun, 0, sizeof(EXT_RUNTIME_CFG) );
#if 0	
	extRun.isTx = 0; /* run as RX default, 192.168.166.3 */
#else
	extRun.isTx = 1; /* run as TX default, 192.168.166.2 */
#endif

	extRun.netif = &_guNetIf;
	while ((ch = getopt_long(argc, argv, "rdhg:i:m:p:", longopts, NULL)) != -1)
	{
		switch (ch)
		{
			case 'r':
				extRun.isTx = 0;	/* 192.168.166.3 */
				break;
			case 'd':
				debug_flags |= (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT);
				break;
			case 'h':
				usage();
				exit(0);
				break;
#if LWIP_IPV4
			case 'g':
				ipaddr_aton(optarg, &gw);
				break;
			case 'i':
				ipaddr_aton(optarg, &ipaddr);
				break;
			case 'm':
				ipaddr_aton(optarg, &netmask);
				break;
#endif /* LWIP_IPV4 */
			case 'p':
				ping_flag = !0;
				ipaddr_aton(optarg, &ping_addr);
				strncpy(ip_str,ipaddr_ntoa(&ping_addr),sizeof(ip_str));
				ip_str[sizeof(ip_str)-1] = 0; /* ensure \0 termination */
				printf("Using %s to ping\n", ip_str);
				break;
			default:
				usage();
				break;
		}
	}
	printf("System initialized, '%s' run as '%s'"LWIP_NEW_LINE, argv[0], EXT_IS_TX(&extRun)?"TX":"RX");
	
	argc -= optind;
	argv += optind;

	debug_flags |= (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT);

#ifdef PERF
	perf_init("/tmp/simhost.perf");
#endif /* PERF */


	sys_thread_new("simMain", main_thread, &extRun, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
	pause();
	return 0;
}

