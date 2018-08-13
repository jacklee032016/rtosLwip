
#include "lwipExt.h"
#include "lwip/inet.h"
#include "jsmn.h"

void extLwipDhcpDebug(	ip4_addr_t *ip, ip4_addr_t *mask, 	ip4_addr_t *gw)
{
	if(!EXT_DEBUG_IS_ENABLE(EXT_DEBUG_FLAG_DHCP) )
	{
		return;
	}
	
	printf( "DHCP Configure: IP:%3"U16_F".%3"U16_F".%3"U16_F"%3"U16_F"; MASK:%3"U16_F".%3"U16_F".%3"U16_F"%3"U16_F"; GATEWAY %3"U16_F".%3"U16_F".%3"U16_F"%3"U16_F"; "LWIP_NEW_LINE, 
		ip4_addr1_16(ip),ip4_addr2_16(ip), ip4_addr3_16(ip), ip4_addr4_16(ip), 
		ip4_addr1_16(mask),ip4_addr2_16(mask), ip4_addr3_16(mask), ip4_addr4_16(mask), 
		ip4_addr1_16(gw),ip4_addr2_16(gw), ip4_addr3_16(gw), ip4_addr4_16(gw) 
		);
}


void extLwipIp4DebugPrint(struct pbuf *p, const char *prompt)
{
	struct ip_hdr *iphdr = (struct ip_hdr *)p->payload;

	if(!EXT_DEBUG_IS_ENABLE(EXT_DEBUG_FLAG_IP_IN|EXT_DEBUG_FLAG_IP_OUT))
	{
		return;
	}
	
	printf( "%s IP header:" LWIP_NEW_LINE, prompt );
	printf( "+-------------------------------+" LWIP_NEW_LINE);
	printf( "|%2"S16_F" |%2"S16_F" |  0x%02"X16_F" |     %5"U16_F"     | (v, hl, tos, len)" LWIP_NEW_LINE, (u16_t)IPH_V(iphdr), (u16_t)IPH_HL(iphdr),  (u16_t)IPH_TOS(iphdr),lwip_ntohs(IPH_LEN(iphdr)));
	printf( "+-------------------------------+" LWIP_NEW_LINE);
	printf( "|    %5"U16_F"      |%"U16_F"%"U16_F"%"U16_F"|    %4"U16_F"   | (id, flags, offset)" LWIP_NEW_LINE,
		lwip_ntohs(IPH_ID(iphdr)), (u16_t)(lwip_ntohs(IPH_OFFSET(iphdr)) >> 15 & 1), (u16_t)(lwip_ntohs(IPH_OFFSET(iphdr)) >> 14 & 1), (u16_t)(lwip_ntohs(IPH_OFFSET(iphdr)) >> 13 & 1),
		(u16_t)(lwip_ntohs(IPH_OFFSET(iphdr)) & IP_OFFMASK));
	printf( "+-------------------------------+" LWIP_NEW_LINE);
	printf( "|  %3"U16_F"  |  %3"U16_F"  |    0x%04"X16_F"     | (ttl, proto, chksum)" LWIP_NEW_LINE, (u16_t)IPH_TTL(iphdr), (u16_t)IPH_PROTO(iphdr),lwip_ntohs(IPH_CHKSUM(iphdr)));
	printf( "+-------------------------------+" LWIP_NEW_LINE);
	printf( "|  %3"U16_F"  |  %3"U16_F"  |  %3"U16_F"  |  %3"U16_F"  | (src)" LWIP_NEW_LINE, 	ip4_addr1_16(&iphdr->src),ip4_addr2_16(&iphdr->src), ip4_addr3_16(&iphdr->src), ip4_addr4_16(&iphdr->src));
	printf( "+-------------------------------+" LWIP_NEW_LINE);
	printf( "|  %3"U16_F"  |  %3"U16_F"  |  %3"U16_F"  |  %3"U16_F"  | (dest)" LWIP_NEW_LINE, 
		ip4_addr1_16(&iphdr->dest), ip4_addr2_16(&iphdr->dest), ip4_addr3_16(&iphdr->dest), ip4_addr4_16(&iphdr->dest));
	printf( "+-------------------------------+" LWIP_NEW_LINE);
}


void extLwipIgmpDebugPrint(const ip4_addr_t *groupaddr, const char isJoin)
{
	if(!EXT_DEBUG_IS_ENABLE(EXT_DEBUG_FLAG_IGMP) )
	{
		return;
	}
	
	printf("IGMP %s group '%s'"LWIP_NEW_LINE, (isJoin==0)?"leave from":"join to",  inet_ntoa(*(ip_addr_t *)groupaddr) );
	
}


char	extNetIsGroupAddress(unsigned int	*ipAddress)
{
	const ip4_addr_t *mcIpAddr = (ip4_addr_t *)ipAddress;
	if( ip4_addr_ismulticast(mcIpAddr))
	{
		return 1;
	}

	return 0;
}

char extNetMulticastIP4Mac(unsigned int	*ipAddress, EXT_MAC_ADDRESS *macAddress)
{
#if 1
	const ip4_addr_t *mcIpAddr = (ip4_addr_t *)ipAddress;
	if( ip4_addr_ismulticast(mcIpAddr))
#else
	if(IP_ADDR_IS_MULTICAST(ipAddress) )
#endif
	{/* Hash IP multicast address to MAC address.*/
		macAddress->address[0] = LL_IP4_MULTICAST_ADDR_0;
		macAddress->address[1] = LL_IP4_MULTICAST_ADDR_1;
		macAddress->address[2] = LL_IP4_MULTICAST_ADDR_2;
		macAddress->address[3] = ip4_addr2(mcIpAddr) & 0x7f;
		macAddress->address[4] = ip4_addr3(mcIpAddr);
		macAddress->address[5] = ip4_addr4(mcIpAddr);

		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

char extTxMulticastIP2Mac(EXT_RUNTIME_CFG *runCfg)
{
	const ip4_addr_t *mcIpAddr = (ip4_addr_t *)&runCfg->dest.ip;
	if( ip4_addr_ismulticast(mcIpAddr) )
	{/* Hash IP multicast address to MAC address.*/
		runCfg->dest.mac.address[0] = LL_IP4_MULTICAST_ADDR_0;
		runCfg->dest.mac.address[1] = LL_IP4_MULTICAST_ADDR_1;
		runCfg->dest.mac.address[2] = LL_IP4_MULTICAST_ADDR_2;
		runCfg->dest.mac.address[3] = ip4_addr2(mcIpAddr) & 0x7f;
		runCfg->dest.mac.address[4] = ip4_addr3(mcIpAddr);
		runCfg->dest.mac.address[5] = ip4_addr4(mcIpAddr);

		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

char cmnCmdLwipStats(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	stats_display(outBuffer, bufferLen);

	return EXT_FALSE;
}


char cmnCmdLwipPing(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	int	ret;
	uint32_t pingAddress;

	EXT_ASSERT(("buffer is null"EXT_NEW_LINE), outBuffer );

	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	if(argc<=1)
	{
		sprintf( outBuffer, "No IP address is defined" EXT_NEW_LINE);
		return EXT_FALSE;
	}
	
	/* Convert IP address, which may have come from a DNS lookup, to string. */
	pingAddress = ipaddr_addr( argv[1] );

	if( pingAddress != 0 )
	{
		extNetPingInit();
		extNetPingSendNow(pingAddress);
		ret = EXT_TRUE;
	}
	else
	{
		ret = EXT_FALSE;
	}

	if( ret == EXT_FALSE )
	{
		sprintf( outBuffer, "%s", "Could not send ping request\r\n" );
	}
#if 0	
	else
	{
		sprintf( outBuffer, "Ping sent to %s with identifier %d\r\n",  argv[1], ( int ) ret );
	}
#endif
	return EXT_FALSE;
}


char cmnCmdLwipIgmp(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	int	ret = 0;
	uint32_t address;

	EXT_ASSERT(("Buffer is null"), outBuffer );

	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	if(argc < 3)
	{
		sprintf( outBuffer, "Command '"EXT_CMD_IGMP"' with param of '"EXT_CMD_IGMP_JOIN"|"EXT_CMD_IGMP_LEAVE"' GROUP_ADDRESS" EXT_NEW_LINE);
		return EXT_FALSE;
	}
	
	if(strcasecmp(argv[1], EXT_CMD_IGMP_JOIN) == 0 )
	{
		ret = 1;
	}
	else if(strcasecmp(argv[1], EXT_CMD_IGMP_LEAVE) == 0 )
	{
		ret = 2;
	}
	else //if( ret == 0 )
	{
		sprintf( outBuffer, "'%s' is not validate IGMP command"EXT_NEW_LINE,  argv[1]);
		return EXT_TRUE;
	}
	
	/* Convert IP address, which may have come from a DNS lookup, to string. */
	address = ipaddr_addr( argv[2] );
	if( address == IPADDR_NONE )
	{
		sprintf( outBuffer, "'%s' is not validate IP address"EXT_NEW_LINE,  argv[2] );
		return EXT_TRUE;
	}

	if(ret ==1 )
	{
		ret = EXT_NET_IGMP_JOIN(address);
	}
	else
	{
		ret = EXT_NET_IGMP_LEAVE(address);
	}
	
	sprintf( outBuffer, "Command %s "EXT_NEW_LINE, ( ret == EXIT_FAILURE)?"Failed":"OK" );

	return EXT_FALSE;
}


char	cmnCmdNetInfo(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	unsigned int address;
	struct netif *netif = &guNetIf;

	if(argc==1)
	{
		index += snprintf(outBuffer+index,bufferLen-index, "Network %s:"EXT_NEW_LINE"\tIP (%s):%s; ", netif_is_up(netif)?"Up":"Down", EXT_DHCP_IS_ENABLE(&extRun)?"DHCP":"Static", inet_ntoa(*(struct in_addr *)&(netif->ip_addr)) );
		index += snprintf(outBuffer+index,bufferLen-index, "GW:%s"EXT_NEW_LINE, inet_ntoa(*(struct in_addr *)&(netif->gw)) );
		index += snprintf(outBuffer+index, bufferLen-index, "\tMAC:%02x:%02x:%02x:%02x:%02x:%02x"EXT_NEW_LINE, netif->hwaddr[0] , netif->hwaddr[1] , netif->hwaddr[2] , netif->hwaddr[3] , netif->hwaddr[4] , netif->hwaddr[5] );

		return EXT_FALSE;
	}

	index = (unsigned int)cmnParseGetHexIntValue(argv[1]);
	if(index != 0)
	{
		EXT_CFG_SET_DHCP(&extRun, 1);
		index += snprintf(outBuffer+index, bufferLen-index, "\tDHCP enable after reboot"EXT_NEW_LINE );
		bspCfgSave(&extRun, EXT_CFG_MAIN);
		return EXT_FALSE;
	}
	
	EXT_CFG_SET_DHCP(&extRun, 0);
	index += snprintf(outBuffer+index, bufferLen-index, "\tDHCP diable"EXT_NEW_LINE);
	
	if( argc >= 3)
	{
		address = ipaddr_addr(argv[2]);
		if(address == IPADDR_NONE)
		{
			index += snprintf(outBuffer+index, bufferLen-index, "\tArgument 2 '%s' is not IP address"EXT_NEW_LINE, argv[2] );
			return EXT_FALSE;
		}
		
		extRun.local.ip = address;
		index += snprintf(outBuffer+index, bufferLen-index, "\tIP address: %s"EXT_NEW_LINE, argv[2] );
		
		if(argc >= 4)
		{
			address = ipaddr_addr(argv[3]);
			if(address == IPADDR_NONE)
			{
				index += snprintf(outBuffer+index, bufferLen-index, "\tArgument 3 '%s' is not IP mask"EXT_NEW_LINE, argv[3] );
				return EXT_FALSE;
			}
	
			extRun.ipMask = address;
			index += snprintf(outBuffer+index, bufferLen-index, "\tIP mask: %s"EXT_NEW_LINE, argv[3] );
			
			if(argc >= 5)
			{
				address = ipaddr_addr(argv[4]);
				if(address == IPADDR_NONE)
				{
					index += snprintf(outBuffer+index, bufferLen-index, "\tArgument 4 '%s' is not IP of gateway"EXT_NEW_LINE, argv[4] );
					return EXT_FALSE;
				}

				extRun.ipGateway = address;
				index += snprintf(outBuffer+index, bufferLen-index, "\tGateway: %s"EXT_NEW_LINE, argv[4] );
			}
			
		}	
	}

	if(!EXT_IS_TX(&extRun) && !extNetIsGroupAddress(&extRun.dest.ip) )
	{
		extRun.dest.ip = extRun.local.ip;
		memcpy(extRun.dest.mac.address, extRun.local.mac.address, EXT_MAC_ADDRESS_LENGTH);
	}

	bspCfgSave(&extRun, EXT_CFG_MAIN);

	return EXT_FALSE;
}

char	cmnCmdMacInfo(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	struct netif *netif = &guNetIf;
	EXT_MAC_ADDRESS macAddress;

	if(argc==1)
	{
		index += snprintf(outBuffer+index, bufferLen-index, "\tMAC:%02x:%02x:%02x:%02x:%02x:%02x"EXT_NEW_LINE, netif->hwaddr[0] , netif->hwaddr[1] , netif->hwaddr[2] , netif->hwaddr[3] , netif->hwaddr[4] , netif->hwaddr[5] );
		return EXT_FALSE;
	}
	
	if(extMacAddressParse(&macAddress, argv[1]) )
	{
		index += snprintf(outBuffer+index, bufferLen-index, "\t'%s' is not MAC address"EXT_NEW_LINE, argv[1]);
		return EXT_FALSE;
	}
	
	memcpy(&extRun.local.mac, &macAddress, sizeof(EXT_MAC_ADDRESS));
	
	index += snprintf(outBuffer+index, bufferLen-index, "\tNew MAC:%02x:%02x:%02x:%02x:%02x:%02x"EXT_NEW_LINE, 
		extRun.local.mac.address[0] , extRun.local.mac.address[1] , extRun.local.mac.address[2] , 
		extRun.local.mac.address[3] , extRun.local.mac.address[4] , extRun.local.mac.address[5] );

	if(!EXT_IS_TX(&extRun) && !extNetIsGroupAddress(&extRun.dest.ip) )
	{
		extRun.dest.ip = extRun.local.ip;
		memcpy(extRun.dest.mac.address, extRun.local.mac.address, EXT_MAC_ADDRESS_LENGTH);
	}
	
	bspCfgSave(&extRun, EXT_CFG_MAIN);

	return EXT_FALSE;
}

#define	MAC_ADDRESS_OUTPUT(buffer, size, index, mac) \
		{(index) += snprintf((buffer)+(index), (size)-(index), "\"%02x:%02x:%02x:%02x:%02x:%02x\""EXT_NEW_LINE,  \
			(mac)->address[0] , (mac)->address[1] , (mac)->address[2] , (mac)->address[3], (mac)->address[4], (mac)->address[5] );}



char cmnCmdParams(const struct _EXT_CLI_CMD *cmd, char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	EXT_RUNTIME_CFG *runCfg = &extRun;
	/*raw json string */

#if 0	
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_NAME"\t: %s"EXT_NEW_LINE,  runCfg->name);
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_MODEL"\t: %s"EXT_NEW_LINE,  runCfg->model);
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_VERSION"\t: %2d.%2d.%2d"EXT_NEW_LINE, runCfg->version.major, runCfg->version.minor, runCfg->version.revision);
	index += snprintf(outBuffer+index, bufferLen-index, "%s/%s"EXT_NEW_LINE, runCfg->user, runCfg->password);

#endif	
	index += snprintf(outBuffer+index, bufferLen-index, "MODE: %s, Multicast:%s"EXT_NEW_LINE, EXT_IS_TX(runCfg)?"TX":"RX", (runCfg->isMCast)?"YES":"NO");

#if 0
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_MAC"\t: ");
	MAC_ADDRESS_OUTPUT(outBuffer, bufferLen, index, &runCfg->macAddress);
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_IP"\t: %s"EXT_NEW_LINE,  EXT_LWIP_IPADD_TO_STR(&runCfg->ipAddress));
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_MASK"\t: %s"EXT_NEW_LINE,  EXT_LWIP_IPADD_TO_STR(&runCfg->ipMask));
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_GATEWAY"\t: %s"EXT_NEW_LINE,  EXT_LWIP_IPADD_TO_STR(&runCfg->ipGateway));
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_DHCP"\t: %s"EXT_NEW_LINE, EXT_DHCP_IS_ENABLE(runCfg)?"Yes":"No");
#endif

	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_VIDEO_MAC_LOCAL"\t: ");
	MAC_ADDRESS_OUTPUT(outBuffer, bufferLen, index, &runCfg->local.mac );
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_VIDEO_IP_LOCAL"\t: %s"EXT_NEW_LINE,  EXT_LWIP_IPADD_TO_STR(&runCfg->local.ip));
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_VIDEO_PORT_LOCAL"\t: %d"EXT_NEW_LINE, runCfg->local.vport);
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_AUDIO_PORT_LOCAL"\t: %d"EXT_NEW_LINE, runCfg->local.aport);
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_ANC_DT_PORT_LOCAL"\t: %d"EXT_NEW_LINE, runCfg->local.dport);
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_ANC_ST_PORT_LOCAL"\t: %d"EXT_NEW_LINE, runCfg->local.sport);
	
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_VIDEO_MAC_DEST"\t: ");
	MAC_ADDRESS_OUTPUT(outBuffer, bufferLen, index, &runCfg->dest.mac);
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_VIDEO_IP_DEST"\t: %s"EXT_NEW_LINE,  EXT_LWIP_IPADD_TO_STR(&runCfg->dest.ip) );
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_VIDEO_PORT_DEST"\t: %d"EXT_NEW_LINE, runCfg->dest.vport);
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_AUDIO_PORT_DEST"\t: %d"EXT_NEW_LINE, runCfg->dest.aport);

	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_ANC_DT_PORT_DEST"\t: %d"EXT_NEW_LINE, runCfg->dest.dport);
	index += snprintf(outBuffer+index, bufferLen-index, EXT_JSON_KEY_ANC_ST_PORT_DEST"\t: %d"EXT_NEW_LINE, runCfg->dest.sport);

	if(index>=bufferLen)
	{
//		return EXT_TRUE;
		return EXT_FALSE;
	}

	return EXT_FALSE;
}


char cmdCmdDebuggable(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	unsigned int debugOption;
	int index = 0;
	EXT_ASSERT(("Buffer is null"), outBuffer );

	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	debugOption = EXT_DEBUG_FLAG_IP_IN| EXT_DEBUG_FLAG_UDP_IN|EXT_DEBUG_FLAG_IGMP|EXT_DEBUG_FLAG_CMD;
	if(argc < 2)
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'"EXT_CMD_DEBUG"' configuration is '%08X'"EXT_NEW_LINE, extRun.debugOptions);
		index += snprintf( outBuffer+index, (bufferLen-index), "%s"EXT_NEW_LINE, cmd->helpString );
		return EXT_FALSE;
	}

	if(strcasecmp(argv[1], EXT_CMD_DEBUG_ENABLE) == 0 )
	{
		EXT_DEBUG_SET_ENABLE(debugOption);
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' is Enabled"EXT_NEW_LINE,  argv[1]);
	}
	else if(strcasecmp(argv[1], EXT_CMD_DEBUG_DISABLE) == 0 )
	{
		EXT_DEBUG_SET_DISABLE(debugOption);
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' is Disabled"EXT_NEW_LINE,  argv[1]);
	}
	else
	{
		sprintf( outBuffer, "'%s' is not validate debug command"EXT_NEW_LINE,  argv[1]);
	}
	

	return EXT_FALSE;
}


char cmnCmdTx(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	int index = 0;
	EXT_ASSERT(("Buffer is null"), outBuffer );

	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	if(argc < 2)
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' configuration is '%s'"EXT_NEW_LINE, cmd->name, EXT_IS_TX(&extRun)?"TX":"RX");//, extRun.isTx);
		index += snprintf( outBuffer+index, (bufferLen-index), "%s"EXT_NEW_LINE, cmd->helpString );
		return EXT_FALSE;
	}

	index = (int)cmnParseGetHexIntValue(argv[1]);
	if(( index != 0 && !EXT_IS_TX(&extRun)) || (index==0 && EXT_IS_TX(&extRun) ) )
	{
		extRun.isTx = (index==0)?EXT_FALSE:EXT_TRUE;
		bspCfgSave(&extRun, EXT_CFG_MAIN);
		sprintf( outBuffer, "'%s' configured as '%s', reboot to make it active"EXT_NEW_LINE, cmd->name, EXT_IS_TX(&extRun)?"TX":"RX" );
	}
	else
	{
		sprintf( outBuffer, "'%s' configuration is not changed"EXT_NEW_LINE, cmd->name );
	}


	
	return EXT_FALSE;
}


char cmnCmdUpdate(const struct _EXT_CLI_CMD *cmd,  char *outBuffer,  unsigned int bufferLen )
{
	int index = 0;
	
	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	if(argc < 2)
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' configuration is '%d'"EXT_NEW_LINE, cmd->name, (extRun.isUpdate) );
		index += snprintf( outBuffer+index, (bufferLen-index), "%s" EXT_NEW_LINE, cmd->helpString);
		return EXT_FALSE;
	}

	index = ( int)atoi(argv[1]);
	if(( index != 0 && extRun.isUpdate == EXT_FALSE ) || (index==0 && extRun.isUpdate== EXT_TRUE) )
	{
		extRun.isUpdate = !extRun.isUpdate;
		bspCfgSave(&extRun, EXT_CFG_MAIN);
		sprintf( outBuffer, "'%s' configured as '%d', reboot to make it active"EXT_NEW_LINE, cmd->name, extRun.isUpdate );
	}
	else
	{
		sprintf( outBuffer, "'%s' configuration is not changed"EXT_NEW_LINE, cmd->name );
	}
	
	return EXT_FALSE;
}


static void	_extVideoConfigOutput( char *outBuffer, size_t bufferLen, EXT_VIDEO_CONFIG *vCfg)
{
	unsigned int index = 0;
	index += snprintf(outBuffer+index, bufferLen-index, "\tMAC:%02x:%02x:%02x:%02x:%02x:%02x"EXT_NEW_LINE, 
		vCfg->mac.address[0], vCfg->mac.address[1], vCfg->mac.address[2], vCfg->mac.address[3], vCfg->mac.address[4], vCfg->mac.address[5] );
	index += snprintf(outBuffer+index, bufferLen-index, "\tIP:%s, VideoPort:%d; AudioPort:%d"EXT_NEW_LINE, inet_ntoa(*(struct in_addr *)&(vCfg->ip)), vCfg->vport, vCfg->aport);
}

static char _extVideoConfigParse(char *outBuffer, size_t bufferLen, EXT_VIDEO_CONFIG *vCfg)
{
	unsigned int index = 0;
	char	ret;

	vCfg->vport = 0;
	vCfg->aport = 0;
	vCfg->ip = IPADDR_NONE;
	
	if( argc >=2 )
	{
	
		if(extMacAddressParse(&vCfg->mac, argv[1]) == EXIT_FAILURE)
		{
			index += snprintf(outBuffer+index, bufferLen-index, "\tParam:'%s' is invalidate MAC address"EXT_NEW_LINE, argv[1]);
			return EXIT_FAILURE;
		}

		if(argc >= 3 )
		{
			vCfg->ip = ipaddr_addr(argv[2]);
			if(vCfg->ip == IPADDR_NONE)
			{
				index += snprintf(outBuffer+index, bufferLen-index, "\tParam:'%s' is invalidate IP address"EXT_NEW_LINE, argv[2]);
				return EXIT_FAILURE;
			}

	
			if(argc >= 4 )
			{
				ret = sscanf(argv[3], "%hu", &vCfg->vport);
				if (1 != ret )
				{
					index += snprintf(outBuffer+index, bufferLen-index, "\tParam:'%s' is invalidate 16bit integer"EXT_NEW_LINE, argv[3]);
					return EXIT_FAILURE;
				}

				if(argc >= 5 )
				{
					ret = sscanf(argv[4], "%hu", &vCfg->aport);
					if (1 != ret )
					{
						index += snprintf(outBuffer+index, bufferLen-index, "\tParam:'%s' is invalidate 16bit integer"EXT_NEW_LINE, argv[4]);
						return EXIT_FAILURE;
					}
				}
			}	
		}
	
	}

	return EXIT_SUCCESS;
}


/* configure dest: MAC, IP, v/a port, only for TX */
char	cmnCmdDestInfo(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	EXT_VIDEO_CONFIG vCfg;
//	const ip4_addr_t *mcIpAddr;
//	int i;
	char	ret;

#if 0
	if(!EXT_IS_TX(&extRun) )
	{
		index += snprintf(outBuffer+index, bufferLen-index, "'"EXT_CMD_DEST_INFO" is not used in RX!"EXT_NEW_LINE);
		return EXT_FALSE;
	}
#endif

	if(argc==1)
	{
		_extVideoConfigOutput(outBuffer, bufferLen, &extRun.dest);
		return EXT_FALSE;
	}

	ret = _extVideoConfigParse(outBuffer, bufferLen, &vCfg);
	if(ret == EXIT_FAILURE)
	{
		return EXT_FALSE;
	}

	extVideoConfigCopy(&extRun.dest, &vCfg);
	if(!EXT_IS_TX(&extRun) && !extNetIsGroupAddress(&extRun.dest.ip) )
	{/* for RX unicast, must be same */
		extVideoConfigCopy(&extRun.local, &vCfg);
	}

	bspCfgSave(&extRun, EXT_CFG_MAIN);

	_extVideoConfigOutput(outBuffer+index, bufferLen-index, &extRun.dest);

//	extFpgaConfig(&extRun);

	return EXT_FALSE;
}

/* configure Local: MAC, IP, v/a port, both for TX/RX */
char	cmnCmdLocalInfo(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	EXT_VIDEO_CONFIG vCfg;
	char	ret;
	const ip4_addr_t *mcIpAddr;

	if(argc==1)
	{
		_extVideoConfigOutput(outBuffer, bufferLen, &extRun.local);
		return EXT_FALSE;
	}

	ret = _extVideoConfigParse(outBuffer, bufferLen, &vCfg);
	if(ret == EXIT_FAILURE)
	{
		return EXT_FALSE;
	}
	
	mcIpAddr = (ip4_addr_t *)&vCfg.ip;
	if( ip4_addr_ismulticast(mcIpAddr) && EXT_IS_TX(&extRun) )
	{
		index += snprintf(outBuffer+index, bufferLen-index, "'"EXT_CMD_LOCAL_INFO" : multicast address can be used as local address for TX!"EXT_NEW_LINE);
		return EXT_FALSE;
	}

	extVideoConfigCopy(&extRun.local, &vCfg);
	
	bspCfgSave(&extRun, EXT_CFG_MAIN);

	_extVideoConfigOutput(outBuffer+index, bufferLen-index, &extRun.local);

//	extFpgaConfig(&extRun);

	return EXT_FALSE;
}



#if LWIP_EXT_UDP_TX_PERF
char	extCmdUdpTxPerf(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	unsigned int address;
	char ret;

	TRACE();
	if(argc==1)
	{
		index += snprintf(outBuffer+index, bufferLen-index, EXT_CMD_UDP_PERF" <ipaddress of UDP Perf server>:"EXT_NEW_LINE" for example, 192.168.168.102"EXT_NEW_LINE );
		return EXT_FALSE;
	}

	TRACE();
	address = ipaddr_addr(argv[1]);
	if(address == IPADDR_NONE)
	{
		index += snprintf(outBuffer+index, bufferLen-index, "\tArgument '%s' is not IP address"EXT_NEW_LINE, argv[1] );
		return EXT_FALSE;
	}

	TRACE();
	ret = extUdpTxPerfStart(address);
		
	index += snprintf(outBuffer+index, bufferLen-index, "\tUDP Perf to %s has started %s"EXT_NEW_LINE, argv[1], (ret==EXIT_SUCCESS)?"OK":"Fail" );

	return EXT_FALSE;
}
#endif


