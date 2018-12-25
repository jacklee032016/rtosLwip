
#include "ext.h"
#include "lwipExt.h"
#include "lwip/tcpip.h"


/** Maximum transfer unit. */
#define NET_MTU               1500

static struct netif		_guNetIf;		/* global unique netIf */


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
		extLwipStartup(&extRun);

	}
	else
	{
		printf("Network down"EXT_NEW_LINE);
	}

#if LWIP_MDNS_RESPONDER
	mdns_resp_netif_settings_changed(netif);
#endif

}

static void _tcpip_init_done(void *arg)
{
	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)arg;
//	struct ip_addr x_ip_addr, x_net_mask, x_gateway;
	ip4_addr_t x_ip_addr, x_net_mask, x_gateway;
	struct netif *_netif = NULL;

	LWIP_ASSERT(("runCfg is NULL"), (runCfg!= NULL));
	/* before network start, make FPGA work to filter flooding packets. 09.13,2018  */
//	if(EXT_IS_TX(runCfg) )
	{
//		extFpgaConfig(runCfg);
	}

	_netif = (struct netif *)runCfg->netif;
	/* Initialize lwIP. */
	_netif->hwaddr_len = 6;


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


//	EXT_DEBUGF(EXT_DBG_OFF, ("Add netif %d(%p)..."EXT_NEW_LINE, _netif->hwaddr_len, _netif));
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
		_netif->hwaddr_len, _netif , (offsetof(struct netif, rs_count)), (offsetof(struct netif, mtu)), NETIF_HWADDR_OFFSET() ) );
	EXT_LWIP_DEBUG_NETIF(_netif);
#endif

#if LWIP_IPV4
	if (NULL == netif_add(_netif, &x_ip_addr, &x_net_mask, &x_gateway, runCfg, macEthernetInit, tcpip_input))
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
//	EXT_DEBUGF(EXT_DBG_OFF, ("Setup default netif..."));
	netif_set_default(_netif);

	/* Setup callback function for netif status change */
	netif_set_status_callback(_netif, extLwipNetStatusCallback);

	/* Bring it up */
	if(EXT_DHCP_IS_ENABLE(runCfg))
	{
		/* DHCP mode. */
		EXT_DEBUGF(EXT_DBG_ON, ("DHCP Starting ..."EXT_NEW_LINE) );
		_netif->flags |= NETIF_FLAG_UP;	/* make it up to process DHCP packets. J.L. */
		if (ERR_OK != dhcp_start(_netif))
		{
			EXT_ASSERT(("ERR_OK != dhcp_start"), 0);
		}
		EXT_INFOF( ("DHCP Start..."EXT_NEW_LINE) );
	}
	else
	{/* Static mode: start up directly */
		EXT_DEBUGF(EXT_DBG_ON, ("Static IP ..."EXT_NEW_LINE) );
		netif_set_up(_netif);
	}

}


/*
 * \brief Initialize the lwIP TCP/IP stack with the network interface driver.
 */
void extBspNetStackInit(EXT_RUNTIME_CFG *runCfg)
{
	/* Initialize lwIP. */
	runCfg->netif = &_guNetIf;

	_guNetIf.name[0] = 'e';
	_guNetIf.name[1] = 'n';
	
	_guNetIf.state = runCfg;
	
	srand( sys_now() );

	/* Call tcpip_init for threaded lwIP mode. JL */
	tcpip_init(_tcpip_init_done, runCfg);

//	printf("Initializing LwIP, netif:%d(%p)..."EXT_NEW_LINE, runCfg->netif->hwaddr_len,runCfg->netif );

	/* Set hw and IP parameters, initialize MAC too. */
//	extLwipStartNic(runCfg);
}




