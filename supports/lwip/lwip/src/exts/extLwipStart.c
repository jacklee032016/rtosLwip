
#include "lwipExt.h"

#include "lwip/apps/mdns.h"
#include "lwip/apps/mdns_priv.h"
#include "lwip/apps/tftp_server.h"
#include "lwip/apps/lwiperf.h"
#include "lwip/tcpip.h"

#include "jsmn.h"
#include "extUdpCmd.h"

#if LWIP_EXT_PTP
void ptpd_init(void);
#endif

#if LWIP_EXT_NMOS
#if !LWIP_EXT_HTTP
#error "sanity_check: WARNING: HTTP cannot be disabled when NMOS is enabled!"
#endif
#endif

extern	const struct tftp_context		extTftp;

EXT_JSON_PARSER  extParser;


/*
* Only involving join or leave one group. IGMP has been initialized in protocol stack
*/
char	 extLwipGroupMgr(EXT_RUNTIME_CFG *runCfg, unsigned int gAddress, unsigned char isAdd)
{
	const ip_addr_t  *ipaddr;
	ip_addr_t			ipgroup;
	err_t ret;
	struct netif *_netif = (struct netif *)runCfg->netif;

//	IP4_ADDR( &ipgroup, 239,  200,   1,   111 );
	EXT_LWIP_INT_TO_IP(&ipgroup, gAddress);

	LWIP_ERROR(("IGMP is not enabled in interface when op on group '%s'", EXT_LWIP_IPADD_TO_STR(&ipgroup)), ( (_netif->flags & NETIF_FLAG_IGMP)!=0), return ERR_VAL;);

	EXT_DEBUGF(EXT_DBG_ON, ("%s IGMP group '%s'"LWIP_NEW_LINE, (isAdd)?"Join":"Leave", EXT_LWIP_IPADD_TO_STR(&ipgroup)) );

	ipaddr = netif_ip4_addr(_netif);
	if(isAdd)
	{
		ret = igmp_joingroup(ipaddr,  &ipgroup);
	}
	else
	{
		ret = igmp_leavegroup(ipaddr, &ipgroup);
	}


	if(ret == ERR_OK)
		return EXIT_SUCCESS;

	return EXIT_FAILURE;
}


#if LWIP_MDNS_RESPONDER
static void srv_txt(struct mdns_service *service, void *txt_userdata)
{
	err_t res;
	char	name[64];
//	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)txt_userdata;

	snprintf(name, sizeof(name), "%s=%s", NMOS_API_NAME_PROTOCOL, NMOS_API_PROTOCOL_HTTP);
	res = mdns_resp_add_service_txtitem(service, name, (u8_t)strlen(name) );
	LWIP_ERROR(("mdns add API protocol failed"), (res == ERR_OK), return);

	snprintf(name, sizeof(name), "%s=%s,%s,%s", NMOS_API_NAME_VERSION, NMOS_API_VERSION_10, NMOS_API_VERSION_11, NMOS_API_VERSION_12);
	res = mdns_resp_add_service_txtitem(service, name, (u8_t)strlen(name) );
	LWIP_ERROR(("mdns add API version failed"), (res == ERR_OK), return);

}


static mdns_client_t _mdnsClient;

/* hostname used in MDNS announce packet */
static char	_extLwipMdnsResponder(EXT_RUNTIME_CFG *runCfg)
{
	char	name[64];
	struct netif *_netif = (struct netif *)runCfg->netif;
	
#define	DNS_AGING_TTL		3600

	mdns_resp_init(&_mdnsClient);

	/* this is also the hostname used to resolve IP address*/
	mdns_resp_add_netif(_netif, runCfg->name, DNS_AGING_TTL);

	/* this is servce name displayed in DNS-SD */
	snprintf(name, sizeof(name), EXT_767_MODEL"_%s_%s", EXT_IS_TX(runCfg)?"TX":"RX", EXT_LWIP_IPADD_TO_STR(&(runCfg->local.ip) ));
//	printf("IP address:%s:%s"LWIP_NEW_LINE, EXT_LWIP_IPADD_TO_STR(&(runCfg->local.ip) ), name );
	mdns_resp_add_service(_netif, name, NMOS_MDNS_NODE_SERVICE, DNSSD_PROTO_TCP, runCfg->httpPort, DNS_AGING_TTL, srv_txt, runCfg);

	return 0;
}

void extLwipMdsnDestroy(struct netif *netif)
{
	mdns_resp_remove_netif(netif);

	udp_remove(_mdnsClient.udpPcb);

	LWIP_DEBUGF(EXT_DBG_ON| MDNS_DEBUG, ("Destroy MSDN Responder"LWIP_NEW_LINE) );
}

#endif



/* after netif has been set_up(make it UP), call to start services */
char extLwipStartup(EXT_RUNTIME_CFG *runCfg)
{
//	struct netif *_netif = (struct netif *)runCfg->netif;
	extParser.runCfg = runCfg;
	extJsonInit(&extParser, NULL, 0);

	EXT_INFOF(("TELNET server start..."));
	extNetRawTelnetInit(runCfg);


#if LWIP_MDNS_RESPONDER
	EXT_INFOF(("MDNS Responder start..."));
	_extLwipMdnsResponder(runCfg);

	EXT_INFOF(("MDNS Client start..."));
	mdnsClientInit(&_mdnsClient, runCfg);
#endif

#if LWIP_EXT_UDP_RX_PERF
	EXT_INFOF(("UDP RX Perf start..."));
	extUdpRxPerfStart();
#endif

#if LWIP_EXT_UDP_TX_PERF
	EXT_INFOF(("UDP TX Perf start..."));
	extUdpTxPerfTask();
#endif

#if LWIP_EXT_MQTT_CLIENT && defined(X86)
	EXT_INFOF(("MQTT Client start..."));
//	mqttClientConnect(PP_HTONL(LWIP_MAKEU32(192,168,168,102)));
#endif

	EXT_INFOF(("IP Command Daemon start..."));
	extIpCmdAgentInit(&extParser);

#if LWIP_EXT_NMOS
	EXT_INFOF(("NMOS node start..."));
	extNmosNodeInit(&nmosNode, runCfg);
#endif

#if LWIP_EXT_HTTP
	EXT_INFOF(("HTTP sever start..."));
	mHttpSvrMain(runCfg);
#endif

#if LWIP_EXT_TCP_PERF
	EXT_INFOF(("TCP Perf server start..."));
	lwiperf_start_tcp_server_default(NULL, NULL);
#endif

#ifdef	X86
#endif


#if LWIP_EXT_TFTP
	EXT_INFOF(("TFTP server start..."));
	tftp_init(&extTftp);
#endif /* LWIP_UDP */

#if LWIP_EXT_PTP
#if 0//def	X86
	EXT_INFOF(("PTP service start..."));
	ptpd_init();
#endif	
#endif

	EXT_INFOF(("FPGA Polling Service start..."));
	extMediaInit(runCfg);

	return 0;
}


#ifdef	X86
/* after basic startup of hardware, call it to read configuration from NVRAM or others */
char	extSysParamsInit(EXT_RUNTIME_CFG *runCfg)
{
	unsigned int debugOption;
	extJsonInit(&extParser, NULL, 0); /* eg. as RX */

	extCfgFromFactory(runCfg);
	runCfg->bootMode = BOOT_MODE_RTOS;

	runCfg->runtime.aChannels = 2;
	runCfg->runtime.aDepth = 16;
	runCfg->runtime.aSampleRate = 48000;
	
	runCfg->runtime.vColorSpace = EXT_V_COLORSPACE_YCBCR_444;
	runCfg->runtime.vDepth= EXT_V_DEPTH_16;
	runCfg->runtime.vFrameRate = EXT_V_FRAMERATE_T_59;

	runCfg->runtime.vWidth= 1920;
	runCfg->runtime.vHeight = 1080;

	runCfg->runtime.vIsInterlaced = 1;
	runCfg->runtime.vIsSegmented = 1;

	/* init in simhost */
//	runCfg->isTx = 1; /* TX */
	extCfgInitAfterReadFromFlash(runCfg);

#if 0
//	debugOption = EXT_DEBUG_FLAG_IP_IN| EXT_DEBUG_FLAG_UDP_IN|EXT_DEBUG_FLAG_IGMP|EXT_DEBUG_FLAG_CMD;
	debugOption = EXT_DEBUG_FLAG_IGMP|EXT_DEBUG_FLAG_CMD;
#else
	debugOption = 0;
#endif
	EXT_DEBUG_SET_ENABLE(debugOption);

	return 0;
}
#endif


static void _extStackUp( struct netif *netif)
{
	EXT_RUNTIME_CFG *runCfg = &extRun;

	extLwipStartup(runCfg);

#ifdef	ARM
	extFpgaConfig(runCfg );

	if(EXT_IS_TX(runCfg) )
	{
		extFpgaEnable(EXT_TRUE); /* start TX and RX after IP address is configured. 09.13, 2018 */
	}

	printf(""EXT_NEW_LINE);
	
//	printf("OS Scheduler beginning..."EXT_NEW_LINE);

//	extRs232Write((unsigned char *)"OS startup", 10);

	extJobPeriod(runCfg);
#endif
}

void extLwipNetStatusCallback(struct netif *netif)
{
	EXT_INFOF(("NETIF: %c%c%d (%02x:%02x:%02x:%02x:%02x:%02x) is %s", netif->name[0], netif->name[1], netif->num, 
		netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2], netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5], netif_is_up(netif) ? "UP" : "DOWN"));
	if (netif_is_up(netif))
	{
//		printf("Ethernet hwaddr:%d(%p)"EXT_NEW_LINE, netif->hwaddr_len, netif);
#if LWIP_IPV4
#if 0
		printf("IPV4: Host at %s ", ip4addr_ntoa(netif_ip4_addr(netif)));
		printf("mask %s ", ip4addr_ntoa(netif_ip4_netmask(netif)));
		printf("gateway %s"EXT_NEW_LINE, ip4addr_ntoa(netif_ip4_gw(netif)));
#endif

		const ip4_addr_t *ip, *mask, *gw;
		ip = netif_ip4_addr(netif);
		mask = netif_ip4_netmask(netif);
		gw = netif_ip4_gw(netif);
		EXT_INFOF(("IP:%"U16_F".%"U16_F".%"U16_F".%"U16_F";"\
			" mask:%"U16_F".%"U16_F".%"U16_F".%"U16_F"; "\
			" gateway:%"U16_F".%"U16_F".%"U16_F".%"U16_F""LWIP_NEW_LINE, 
			ip4_addr1_16(ip), ip4_addr2_16(ip), ip4_addr3_16(ip), ip4_addr4_16(ip), 
			ip4_addr1_16(mask), ip4_addr2_16(mask), ip4_addr3_16(mask), ip4_addr4_16(mask), 
			ip4_addr1_16(gw), ip4_addr2_16(gw), ip4_addr3_16(gw), ip4_addr4_16(gw) ));

		
//		extRun.local.ip = ip->addr;
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
		printf("IPV6: Host at %s"EXT_NEW_LINE, ip6addr_ntoa(netif_ip6_addr(netif, 0)));
#endif /* LWIP_IPV6 */
#if LWIP_NETIF_HOSTNAME
		printf("FQDN: %s"EXT_NEW_LINE, netif_get_hostname(netif));
#endif /* LWIP_NETIF_HOSTNAME */


#ifdef	ARM
		gmacEnableWakeOnLan(netif->ip_addr.addr);
#endif		
		_extStackUp(netif);

	}
	else
	{
		printf("Network down"EXT_NEW_LINE);
	}

#if LWIP_MDNS_RESPONDER
	mdns_resp_netif_settings_changed(netif);
#endif

}

/** Maximum transfer unit. */
#define NET_MTU               1500

#if 0
static void _initTask(void *param)
{
	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)param;
	struct netif *_netif = (struct netif *)runCfg->netif;
	
	/* Bring it up */
	if(EXT_DHCP_IS_ENABLE(runCfg))
	{
		/* DHCP mode. */
		EXT_DEBUGF(EXT_DBG_OFF, ("DHCP Starting ..."EXT_NEW_LINE) );
		_netif->flags |= NETIF_FLAG_UP;	/* make it up to process DHCP packets. J.L. */
		if (ERR_OK != dhcp_start(netif))
		{
			EXT_ASSERT(("ERR_OK != dhcp_start"), 0);
		}
		EXT_INFOF( ("DHCP Start..."EXT_NEW_LINE) );
	}
	else
	{/* Static mode: start up directly */
		netif_set_up(_netif);
	}

	EXT_ERRORF(("Init Task exut"));
	while(1){};
}
#endif



void extLwipStartNic(EXT_RUNTIME_CFG *runCfg)
{
//	struct ip_addr x_ip_addr, x_net_mask, x_gateway;
	ip4_addr_t x_ip_addr, x_net_mask, x_gateway;
	struct netif *_netif = (struct netif *)runCfg->netif;

	/* Set MAC hardware address length. */
	_netif->hwaddr_len = NETIF_MAX_HWADDR_LEN;
	/* Set MAC hardware address. */
	_netif->hwaddr[0] = runCfg->local.mac.address[0];
	_netif->hwaddr[1] = runCfg->local.mac.address[1];
	_netif->hwaddr[2] = runCfg->local.mac.address[2];
	_netif->hwaddr[3] = runCfg->local.mac.address[3];
	_netif->hwaddr[4] = runCfg->local.mac.address[4];
	_netif->hwaddr[5] = runCfg->local.mac.address[5];

	/* Set maximum transfer unit. */
	_netif->mtu = NET_MTU;

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
	/* start multicast and IGMP */
#ifdef X86
	_netif->flags = NETIF_FLAG_IGMP;
#else
#if 1
	/* when random MAC address is used, ARP must be enabled. 08.26.2018 */
	_netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP | NETIF_FLAG_ETHERNET;
#else
	_netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_IGMP;
#endif
#endif

#if EXT_LWIP_DEBUG
	EXT_DEBUGF(EXT_DBG_ON, ("before %d(%p), offset %d:%d:%d..."EXT_NEW_LINE, 
		_netif->hwaddr_len, _netif , NETIF_HWADDR_OFFSET(), (offsetof(struct netif, rs_count)), (offsetof(struct netif, mtu)) ) );
	EXT_LWIP_DEBUG_NETIF(netif);
#endif

#if LWIP_IPV4
	if (NULL == netif_add(_netif, &x_ip_addr, &x_net_mask, &x_gateway, runCfg, ethernetif_init, tcpip_input))
#else /* LWIP_IPV4 */
	if(NULL== netif_add(_netif, runCfg, tapif_init, tcpip_input))
#endif /* LWIP_IPV4 */
	{
		EXT_ASSERT(("NULL == netif_add"), 0);
	}

#if LWIP_IPV6
	netif_create_ip6_linklocal_address(_netif, 1);
	_netif->ip6_autoconfig_enabled = 1;
#endif

	/* Make it the default interface */
//	printf("Setup default netif...\r\n");
	netif_set_default(_netif);


	/* Setup callback function for netif status change */
	netif_set_status_callback(_netif, extLwipNetStatusCallback);

#if 1
	/* Bring it up */
	if(EXT_DHCP_IS_ENABLE(runCfg))
	{
		/* DHCP mode. */
		EXT_DEBUGF(EXT_DBG_OFF, ("DHCP Starting ..."EXT_NEW_LINE) );
		_netif->flags |= NETIF_FLAG_UP;	/* make it up to process DHCP packets. J.L. */
		if (ERR_OK != dhcp_start(_netif))
		{
			EXT_ASSERT(("ERR_OK != dhcp_start"), 0);
		}
		EXT_INFOF( ("DHCP Start..."EXT_NEW_LINE) );
	}
	else
	{/* Static mode: start up directly */
		netif_set_up(_netif);
	}
#else
	sys_thread_new("init", _initTask, runCfg, 512, 2);
#endif

}



