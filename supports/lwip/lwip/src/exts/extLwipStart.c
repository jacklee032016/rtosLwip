
#include "lwipExt.h"

#include "lwip/apps/mdns.h"
#include "lwip/apps/mdns_priv.h"
#include "lwip/apps/tftp_server.h"
#include "lwip/apps/lwiperf.h"

#include "jsmn.h"
#include "extUdpCmd.h"

extern	const struct tftp_context		muxTftp;

MUX_JSON_PARSER  muxParser;


char	 muxLwipGroupMgr(struct netif *netif, unsigned int gAddress, unsigned char isAdd)
{
	const ip_addr_t  *ipaddr;
	ip_addr_t			ipgroup;
	err_t ret;

//	IP4_ADDR( &ipgroup, 239,  200,   1,   111 );
	MUX_LWIP_INT_TO_IP(&ipgroup, gAddress);

	LWIP_ERROR(("IGMP is not enabled in interface"), ( (netif->flags & NETIF_FLAG_IGMP)!=0), return ERR_VAL;);

	LWIP_DEBUGF(IGMP_DEBUG, ("Register IGMP group '%s'"LWIP_NEW_LINE, MUX_LWIP_IPADD_TO_STR(&ipgroup)) );

	ipaddr = netif_ip4_addr(netif);
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
//	MUX_RUNTIME_CFG *runCfg = (MUX_RUNTIME_CFG *)txt_userdata;

	snprintf(name, sizeof(name), "%s=%s", NMOS_API_NAME_PROTOCOL, NMOS_API_PROTOCOL_HTTP);
	res = mdns_resp_add_service_txtitem(service, name, (u8_t)strlen(name) );
	LWIP_ERROR("mdns add API protocol failed\n", (res == ERR_OK), return);

	snprintf(name, sizeof(name), "%s=%s,%s,%s", NMOS_API_NAME_VERSION, NMOS_API_VERSION_10, NMOS_API_VERSION_11, NMOS_API_VERSION_12);
	res = mdns_resp_add_service_txtitem(service, name, (u8_t)strlen(name) );
	LWIP_ERROR("mdns add API version failed\n", (res == ERR_OK), return);

}


static mdns_client_t _mdnsClient;

/* hostname used in MDNS announce packet */
static char	_muxLwipMdnsResponder(struct netif *netif, MUX_RUNTIME_CFG *runCfg)
{
	char	name[64];
#define	DNS_AGING_TTL		3600

	mdns_resp_init(&_mdnsClient);

	/* this is also the hostname used to resolve IP address*/
	mdns_resp_add_netif(netif, runCfg->name, DNS_AGING_TTL);

	/* this is servce name displayed in DNS-SD */
	snprintf(name, sizeof(name), MUX_767_MODEL"_%s_%s", MUX_IS_TX(runCfg)?"TX":"RX", MUX_LWIP_IPADD_TO_STR(&(runCfg->local.ip) ));
//	printf("IP address:%s:%s"LWIP_NEW_LINE, MUX_LWIP_IPADD_TO_STR(&(runCfg->local.ip) ), name );
	mdns_resp_add_service(netif, name, NMOS_MDNS_NODE_SERVICE, DNSSD_PROTO_TCP, runCfg->httpPort, DNS_AGING_TTL, srv_txt, runCfg);

	return 0;
}

void extLwipMdsnDestroy(struct netif *netif)
{
	mdns_resp_remove_netif(netif);

	udp_remove(_mdnsClient.udpPcb);

	LWIP_DEBUGF(MUX_DBG_ON| MDNS_DEBUG, ("Destroy MSDN Responder"LWIP_NEW_LINE) );
}

#endif



/* after netif has been set_up(make it UP), call to start services */
char muxLwipStartup(struct netif *netif, MUX_RUNTIME_CFG *runCfg)
{
	muxParser.runCfg = runCfg;
	muxNetRawTelnetInit(runCfg);

#if LWIP_MDNS_RESPONDER
	_muxLwipMdnsResponder(netif, runCfg);


	mdnsClientInit(&_mdnsClient, runCfg);
#endif
#if 0

//	shell_init();
#endif
//	muxLwipGroupMgr(netif, runCfg->mcIp, 1);


#if LWIP_EXT_UDP_RX_PERF
	extUdpRxPerfStart();
#endif

#if LWIP_EXT_UDP_TX_PERF
	extUdpTxPerfTask();
#endif

	if(!MUX_IS_TX(runCfg))
	{
//		MUX_DEBUGF(IGMP_DEBUG,("Send IGMP JOIN"LWIP_NEW_LINE));
//		muxLwipGroupMgr(netif, runCfg->dest.ip, 1);

		
		muxIpCmdSendMediaData(&muxParser);
	}
	else
	{
	}

	muxIpCmdAgentInit(&muxParser);


	muxNmosNodeInit(&nmosNode, runCfg);
	mHttpSvrMain(runCfg);

	lwiperf_start_tcp_server_default(NULL, NULL);
#ifdef	X86
#endif


#if LWIP_UDP
	tftp_init(&muxTftp);
#endif /* LWIP_UDP */

	return 0;
}

#ifdef	X86
/* after basic startup of hardware, call it to read configuration from NVRAM or others */
char	muxSysParamsInit(MUX_RUNTIME_CFG *runCfg)
{
	unsigned int debugOption;
	muxJsonInit(&muxParser, NULL, 0); /* eg. as RX */

	muxCfgFromFactory(runCfg);
	runCfg->bootMode = BOOT_MODE_RTOS;

	runCfg->runtime.aChannels = 2;
	runCfg->runtime.aDepth = 16;
	runCfg->runtime.aSampleRate = 48000;
	
	runCfg->runtime.vColorSpace = MUX_V_COLORSPACE_YCBCR_444;
	runCfg->runtime.vDepth= MUX_V_DEPTH_16;
	runCfg->runtime.vFrameRate = MUX_V_FRAMERATE_T_59;

	runCfg->runtime.vWidth= 1920;
	runCfg->runtime.vHeight = 1080;

	runCfg->runtime.vIsInterlaced = 1;
	runCfg->runtime.vIsSegmented = 1;

	/* init in simhost */
//	runCfg->isTx = 1; /* TX */
	muxCfgInitAfterReadFromFlash(runCfg);

#if 1
//	debugOption = MUX_DEBUG_FLAG_IP_IN| MUX_DEBUG_FLAG_UDP_IN|MUX_DEBUG_FLAG_IGMP|MUX_DEBUG_FLAG_CMD;
	debugOption = MUX_DEBUG_FLAG_IGMP|MUX_DEBUG_FLAG_CMD;
#else
	debugOption = 0;
#endif
	MUX_DEBUG_SET_ENABLE(debugOption);

	return 0;
}
#endif

