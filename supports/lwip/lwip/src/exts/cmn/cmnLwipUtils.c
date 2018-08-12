
#include "lwipExt.h"
#include "lwip/inet.h"
#include "jsmn.h"

void extLwipDhcpDebug(	ip4_addr_t *ip, ip4_addr_t *mask, 	ip4_addr_t *gw)
{
	if(!MUX_DEBUG_IS_ENABLE(MUX_DEBUG_FLAG_DHCP) )
	{
		return;
	}
	
	printf( "DHCP Configure: IP:%3"U16_F".%3"U16_F".%3"U16_F"%3"U16_F"; MASK:%3"U16_F".%3"U16_F".%3"U16_F"%3"U16_F"; GATEWAY %3"U16_F".%3"U16_F".%3"U16_F"%3"U16_F"; "LWIP_NEW_LINE, 
		ip4_addr1_16(ip),ip4_addr2_16(ip), ip4_addr3_16(ip), ip4_addr4_16(ip), 
		ip4_addr1_16(mask),ip4_addr2_16(mask), ip4_addr3_16(mask), ip4_addr4_16(mask), 
		ip4_addr1_16(gw),ip4_addr2_16(gw), ip4_addr3_16(gw), ip4_addr4_16(gw) 
		);
}


void muxLwipIp4DebugPrint(struct pbuf *p, const char *prompt)
{
	struct ip_hdr *iphdr = (struct ip_hdr *)p->payload;

	if(!MUX_DEBUG_IS_ENABLE(MUX_DEBUG_FLAG_IP_IN|MUX_DEBUG_FLAG_IP_OUT))
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


void muxLwipIgmpDebugPrint(const ip4_addr_t *groupaddr, const char isJoin)
{
	if(!MUX_DEBUG_IS_ENABLE(MUX_DEBUG_FLAG_IGMP) )
	{
		return;
	}
	
	printf("IGMP %s group '%s'"LWIP_NEW_LINE, (isJoin==0)?"leave from":"join to",  inet_ntoa(*(ip_addr_t *)groupaddr) );
	
}


char	muxNetIsGroupAddress(unsigned int	*ipAddress)
{
	const ip4_addr_t *mcIpAddr = (ip4_addr_t *)ipAddress;
	if( ip4_addr_ismulticast(mcIpAddr))
	{
		return 1;
	}

	return 0;
}

char muxNetMulticastIP4Mac(unsigned int	*ipAddress, MUX_MAC_ADDRESS *macAddress)
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

char muxTxMulticastIP2Mac(MUX_RUNTIME_CFG *runCfg)
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

char cmnCmdLwipStats(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	stats_display(outBuffer, bufferLen);

	return MUX_FALSE;
}


char cmnCmdLwipPing(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	int	ret;
	uint32_t pingAddress;

	MUX_ASSERT(("buffer is null"MUX_NEW_LINE), outBuffer );

	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	if(argc<=1)
	{
		sprintf( outBuffer, "No IP address is defined" MUX_NEW_LINE);
		return MUX_FALSE;
	}
	
	/* Convert IP address, which may have come from a DNS lookup, to string. */
	pingAddress = ipaddr_addr( argv[1] );

	if( pingAddress != 0 )
	{
		muxNetPingInit();
		muxNetPingSendNow(pingAddress);
		ret = MUX_TRUE;
	}
	else
	{
		ret = MUX_FALSE;
	}

	if( ret == MUX_FALSE )
	{
		sprintf( outBuffer, "%s", "Could not send ping request\r\n" );
	}
#if 0	
	else
	{
		sprintf( outBuffer, "Ping sent to %s with identifier %d\r\n",  argv[1], ( int ) ret );
	}
#endif
	return MUX_FALSE;
}


char cmnCmdLwipIgmp(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	int	ret = 0;
	uint32_t address;

	MUX_ASSERT(("Buffer is null"), outBuffer );

	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	if(argc < 3)
	{
		sprintf( outBuffer, "Command '"MUX_CMD_IGMP"' with param of '"MUX_CMD_IGMP_JOIN"|"MUX_CMD_IGMP_LEAVE"' GROUP_ADDRESS" MUX_NEW_LINE);
		return MUX_FALSE;
	}
	
	if(strcasecmp(argv[1], MUX_CMD_IGMP_JOIN) == 0 )
	{
		ret = 1;
	}
	else if(strcasecmp(argv[1], MUX_CMD_IGMP_LEAVE) == 0 )
	{
		ret = 2;
	}
	else //if( ret == 0 )
	{
		sprintf( outBuffer, "'%s' is not validate IGMP command"MUX_NEW_LINE,  argv[1]);
		return MUX_TRUE;
	}
	
	/* Convert IP address, which may have come from a DNS lookup, to string. */
	address = ipaddr_addr( argv[2] );
	if( address == IPADDR_NONE )
	{
		sprintf( outBuffer, "'%s' is not validate IP address"MUX_NEW_LINE,  argv[2] );
		return MUX_TRUE;
	}

	if(ret ==1 )
	{
		ret = MUX_NET_IGMP_JOIN(address);
	}
	else
	{
		ret = MUX_NET_IGMP_LEAVE(address);
	}
	
	sprintf( outBuffer, "Command %s "MUX_NEW_LINE, ( ret == EXIT_FAILURE)?"Failed":"OK" );

	return MUX_FALSE;
}


char	cmnCmdNetInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	unsigned int address;
	struct netif *netif = &guNetIf;

	if(argc==1)
	{
		index += snprintf(outBuffer+index,bufferLen-index, "Network %s:"MUX_NEW_LINE"\tIP (%s):%s; ", netif_is_up(netif)?"Up":"Down", MUX_DHCP_IS_ENABLE(&muxRun)?"DHCP":"Static", inet_ntoa(*(struct in_addr *)&(netif->ip_addr)) );
		index += snprintf(outBuffer+index,bufferLen-index, "GW:%s"MUX_NEW_LINE, inet_ntoa(*(struct in_addr *)&(netif->gw)) );
		index += snprintf(outBuffer+index, bufferLen-index, "\tMAC:%02x:%02x:%02x:%02x:%02x:%02x"MUX_NEW_LINE, netif->hwaddr[0] , netif->hwaddr[1] , netif->hwaddr[2] , netif->hwaddr[3] , netif->hwaddr[4] , netif->hwaddr[5] );

		return MUX_FALSE;
	}

	index = (unsigned int)cmnParseGetHexIntValue(argv[1]);
	if(index != 0)
	{
		MUX_CFG_SET_DHCP(&muxRun, 1);
		index += snprintf(outBuffer+index, bufferLen-index, "\tDHCP enable after reboot"MUX_NEW_LINE );
		bspCfgSave(&muxRun, MUX_CFG_MAIN);
		return MUX_FALSE;
	}
	
	MUX_CFG_SET_DHCP(&muxRun, 0);
	index += snprintf(outBuffer+index, bufferLen-index, "\tDHCP diable"MUX_NEW_LINE);
	
	if( argc >= 3)
	{
		address = ipaddr_addr(argv[2]);
		if(address == IPADDR_NONE)
		{
			index += snprintf(outBuffer+index, bufferLen-index, "\tArgument 2 '%s' is not IP address"MUX_NEW_LINE, argv[2] );
			return MUX_FALSE;
		}
		
		muxRun.local.ip = address;
		index += snprintf(outBuffer+index, bufferLen-index, "\tIP address: %s"MUX_NEW_LINE, argv[2] );
		
		if(argc >= 4)
		{
			address = ipaddr_addr(argv[3]);
			if(address == IPADDR_NONE)
			{
				index += snprintf(outBuffer+index, bufferLen-index, "\tArgument 3 '%s' is not IP mask"MUX_NEW_LINE, argv[3] );
				return MUX_FALSE;
			}
	
			muxRun.ipMask = address;
			index += snprintf(outBuffer+index, bufferLen-index, "\tIP mask: %s"MUX_NEW_LINE, argv[3] );
			
			if(argc >= 5)
			{
				address = ipaddr_addr(argv[4]);
				if(address == IPADDR_NONE)
				{
					index += snprintf(outBuffer+index, bufferLen-index, "\tArgument 4 '%s' is not IP of gateway"MUX_NEW_LINE, argv[4] );
					return MUX_FALSE;
				}

				muxRun.ipGateway = address;
				index += snprintf(outBuffer+index, bufferLen-index, "\tGateway: %s"MUX_NEW_LINE, argv[4] );
			}
			
		}	
	}

	if(!MUX_IS_TX(&muxRun) && !muxNetIsGroupAddress(&muxRun.dest.ip) )
	{
		muxRun.dest.ip = muxRun.local.ip;
		memcpy(muxRun.dest.mac.address, muxRun.local.mac.address, MUX_MAC_ADDRESS_LENGTH);
	}

	bspCfgSave(&muxRun, MUX_CFG_MAIN);

	return MUX_FALSE;
}

char	cmnCmdMacInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	struct netif *netif = &guNetIf;
	MUX_MAC_ADDRESS macAddress;

	if(argc==1)
	{
		index += snprintf(outBuffer+index, bufferLen-index, "\tMAC:%02x:%02x:%02x:%02x:%02x:%02x"MUX_NEW_LINE, netif->hwaddr[0] , netif->hwaddr[1] , netif->hwaddr[2] , netif->hwaddr[3] , netif->hwaddr[4] , netif->hwaddr[5] );
		return MUX_FALSE;
	}
	
	if(muxMacAddressParse(&macAddress, argv[1]) )
	{
		index += snprintf(outBuffer+index, bufferLen-index, "\t'%s' is not MAC address"MUX_NEW_LINE, argv[1]);
		return MUX_FALSE;
	}
	
	memcpy(&muxRun.local.mac, &macAddress, sizeof(MUX_MAC_ADDRESS));
	
	index += snprintf(outBuffer+index, bufferLen-index, "\tNew MAC:%02x:%02x:%02x:%02x:%02x:%02x"MUX_NEW_LINE, 
		muxRun.local.mac.address[0] , muxRun.local.mac.address[1] , muxRun.local.mac.address[2] , 
		muxRun.local.mac.address[3] , muxRun.local.mac.address[4] , muxRun.local.mac.address[5] );

	if(!MUX_IS_TX(&muxRun) && !muxNetIsGroupAddress(&muxRun.dest.ip) )
	{
		muxRun.dest.ip = muxRun.local.ip;
		memcpy(muxRun.dest.mac.address, muxRun.local.mac.address, MUX_MAC_ADDRESS_LENGTH);
	}
	
	bspCfgSave(&muxRun, MUX_CFG_MAIN);

	return MUX_FALSE;
}

#define	MAC_ADDRESS_OUTPUT(buffer, size, index, mac) \
		{(index) += snprintf((buffer)+(index), (size)-(index), "\"%02x:%02x:%02x:%02x:%02x:%02x\""MUX_NEW_LINE,  \
			(mac)->address[0] , (mac)->address[1] , (mac)->address[2] , (mac)->address[3], (mac)->address[4], (mac)->address[5] );}



char cmnCmdParams(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	MUX_RUNTIME_CFG *runCfg = &muxRun;
	/*raw json string */

#if 0	
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_NAME"\t: %s"MUX_NEW_LINE,  runCfg->name);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_MODEL"\t: %s"MUX_NEW_LINE,  runCfg->model);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_VERSION"\t: %2d.%2d.%2d"MUX_NEW_LINE, runCfg->version.major, runCfg->version.minor, runCfg->version.revision);
	index += snprintf(outBuffer+index, bufferLen-index, "%s/%s"MUX_NEW_LINE, runCfg->user, runCfg->password);

#endif	
	index += snprintf(outBuffer+index, bufferLen-index, "MODE: %s, Multicast:%s"MUX_NEW_LINE, MUX_IS_TX(runCfg)?"TX":"RX", (runCfg->isMCast)?"YES":"NO");

#if 0
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_MAC"\t: ");
	MAC_ADDRESS_OUTPUT(outBuffer, bufferLen, index, &runCfg->macAddress);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_IP"\t: %s"MUX_NEW_LINE,  MUX_LWIP_IPADD_TO_STR(&runCfg->ipAddress));
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_MASK"\t: %s"MUX_NEW_LINE,  MUX_LWIP_IPADD_TO_STR(&runCfg->ipMask));
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_GATEWAY"\t: %s"MUX_NEW_LINE,  MUX_LWIP_IPADD_TO_STR(&runCfg->ipGateway));
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_DHCP"\t: %s"MUX_NEW_LINE, MUX_DHCP_IS_ENABLE(runCfg)?"Yes":"No");
#endif

	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_VIDEO_MAC_LOCAL"\t: ");
	MAC_ADDRESS_OUTPUT(outBuffer, bufferLen, index, &runCfg->local.mac );
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_VIDEO_IP_LOCAL"\t: %s"MUX_NEW_LINE,  MUX_LWIP_IPADD_TO_STR(&runCfg->local.ip));
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_VIDEO_PORT_LOCAL"\t: %d"MUX_NEW_LINE, runCfg->local.vport);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_AUDIO_PORT_LOCAL"\t: %d"MUX_NEW_LINE, runCfg->local.aport);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_ANC_DT_PORT_LOCAL"\t: %d"MUX_NEW_LINE, runCfg->local.dport);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_ANC_ST_PORT_LOCAL"\t: %d"MUX_NEW_LINE, runCfg->local.sport);
	
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_VIDEO_MAC_DEST"\t: ");
	MAC_ADDRESS_OUTPUT(outBuffer, bufferLen, index, &runCfg->dest.mac);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_VIDEO_IP_DEST"\t: %s"MUX_NEW_LINE,  MUX_LWIP_IPADD_TO_STR(&runCfg->dest.ip) );
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_VIDEO_PORT_DEST"\t: %d"MUX_NEW_LINE, runCfg->dest.vport);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_AUDIO_PORT_DEST"\t: %d"MUX_NEW_LINE, runCfg->dest.aport);

	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_ANC_DT_PORT_DEST"\t: %d"MUX_NEW_LINE, runCfg->dest.dport);
	index += snprintf(outBuffer+index, bufferLen-index, MUX_JSON_KEY_ANC_ST_PORT_DEST"\t: %d"MUX_NEW_LINE, runCfg->dest.sport);

	if(index>=bufferLen)
	{
//		return MUX_TRUE;
		return MUX_FALSE;
	}

	return MUX_FALSE;
}


char cmdCmdDebuggable(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	unsigned int debugOption;
	int index = 0;
	MUX_ASSERT(("Buffer is null"), outBuffer );

	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	debugOption = MUX_DEBUG_FLAG_IP_IN| MUX_DEBUG_FLAG_UDP_IN|MUX_DEBUG_FLAG_IGMP|MUX_DEBUG_FLAG_CMD;
	if(argc < 2)
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'"MUX_CMD_DEBUG"' configuration is '%08X'"MUX_NEW_LINE, muxRun.debugOptions);
		index += snprintf( outBuffer+index, (bufferLen-index), "%s"MUX_NEW_LINE, cmd->helpString );
		return MUX_FALSE;
	}

	if(strcasecmp(argv[1], MUX_CMD_DEBUG_ENABLE) == 0 )
	{
		MUX_DEBUG_SET_ENABLE(debugOption);
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' is Enabled"MUX_NEW_LINE,  argv[1]);
	}
	else if(strcasecmp(argv[1], MUX_CMD_DEBUG_DISABLE) == 0 )
	{
		MUX_DEBUG_SET_DISABLE(debugOption);
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' is Disabled"MUX_NEW_LINE,  argv[1]);
	}
	else
	{
		sprintf( outBuffer, "'%s' is not validate debug command"MUX_NEW_LINE,  argv[1]);
	}
	

	return MUX_FALSE;
}


char cmnCmdTx(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	int index = 0;
	MUX_ASSERT(("Buffer is null"), outBuffer );

	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	if(argc < 2)
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' configuration is '%s'"MUX_NEW_LINE, cmd->name, MUX_IS_TX(&muxRun)?"TX":"RX");//, muxRun.isTx);
		index += snprintf( outBuffer+index, (bufferLen-index), "%s"MUX_NEW_LINE, cmd->helpString );
		return MUX_FALSE;
	}

	index = (int)cmnParseGetHexIntValue(argv[1]);
	if(( index != 0 && !MUX_IS_TX(&muxRun)) || (index==0 && MUX_IS_TX(&muxRun) ) )
	{
		muxRun.isTx = (index==0)?MUX_FALSE:MUX_TRUE;
		bspCfgSave(&muxRun, MUX_CFG_MAIN);
		sprintf( outBuffer, "'%s' configured as '%s', reboot to make it active"MUX_NEW_LINE, cmd->name, MUX_IS_TX(&muxRun)?"TX":"RX" );
	}
	else
	{
		sprintf( outBuffer, "'%s' configuration is not changed"MUX_NEW_LINE, cmd->name );
	}


	
	return MUX_FALSE;
}


char cmnCmdUpdate(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
{
	int index = 0;
	
	/* Start with an empty string. */
	outBuffer[ 0 ] = 0x00;

	if(argc < 2)
	{
		index += snprintf( outBuffer+index, (bufferLen-index), "'%s' configuration is '%d'"MUX_NEW_LINE, cmd->name, (muxRun.isUpdate) );
		index += snprintf( outBuffer+index, (bufferLen-index), "%s" MUX_NEW_LINE, cmd->helpString);
		return MUX_FALSE;
	}

	index = ( int)atoi(argv[1]);
	if(( index != 0 && muxRun.isUpdate == MUX_FALSE ) || (index==0 && muxRun.isUpdate== MUX_TRUE) )
	{
		muxRun.isUpdate = !muxRun.isUpdate;
		bspCfgSave(&muxRun, MUX_CFG_MAIN);
		sprintf( outBuffer, "'%s' configured as '%d', reboot to make it active"MUX_NEW_LINE, cmd->name, muxRun.isUpdate );
	}
	else
	{
		sprintf( outBuffer, "'%s' configuration is not changed"MUX_NEW_LINE, cmd->name );
	}
	
	return MUX_FALSE;
}


static void	_muxVideoConfigOutput( char *outBuffer, size_t bufferLen, MUX_VIDEO_CONFIG *vCfg)
{
	unsigned int index = 0;
	index += snprintf(outBuffer+index, bufferLen-index, "\tMAC:%02x:%02x:%02x:%02x:%02x:%02x"MUX_NEW_LINE, 
		vCfg->mac.address[0], vCfg->mac.address[1], vCfg->mac.address[2], vCfg->mac.address[3], vCfg->mac.address[4], vCfg->mac.address[5] );
	index += snprintf(outBuffer+index, bufferLen-index, "\tIP:%s, VideoPort:%d; AudioPort:%d"MUX_NEW_LINE, inet_ntoa(*(struct in_addr *)&(vCfg->ip)), vCfg->vport, vCfg->aport);
}

static char _muxVideoConfigParse(char *outBuffer, size_t bufferLen, MUX_VIDEO_CONFIG *vCfg)
{
	unsigned int index = 0;
	char	ret;

	vCfg->vport = 0;
	vCfg->aport = 0;
	vCfg->ip = IPADDR_NONE;
	
	if( argc >=2 )
	{
	
		if(muxMacAddressParse(&vCfg->mac, argv[1]) == EXIT_FAILURE)
		{
			index += snprintf(outBuffer+index, bufferLen-index, "\tParam:'%s' is invalidate MAC address"MUX_NEW_LINE, argv[1]);
			return EXIT_FAILURE;
		}

		if(argc >= 3 )
		{
			vCfg->ip = ipaddr_addr(argv[2]);
			if(vCfg->ip == IPADDR_NONE)
			{
				index += snprintf(outBuffer+index, bufferLen-index, "\tParam:'%s' is invalidate IP address"MUX_NEW_LINE, argv[2]);
				return EXIT_FAILURE;
			}

	
			if(argc >= 4 )
			{
				ret = sscanf(argv[3], "%"PRIu16, &vCfg->vport);
				if (1 != ret )
				{
					index += snprintf(outBuffer+index, bufferLen-index, "\tParam:'%s' is invalidate 16bit integer"MUX_NEW_LINE, argv[3]);
					return EXIT_FAILURE;
				}

				if(argc >= 5 )
				{
					ret = sscanf(argv[4], "%"PRIu16, &vCfg->aport);
					if (1 != ret )
					{
						index += snprintf(outBuffer+index, bufferLen-index, "\tParam:'%s' is invalidate 16bit integer"MUX_NEW_LINE, argv[4]);
						return EXIT_FAILURE;
					}
				}
			}	
		}
	
	}

	return EXIT_SUCCESS;
}


/* configure dest: MAC, IP, v/a port, only for TX */
char	cmnCmdDestInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	MUX_VIDEO_CONFIG vCfg;
//	const ip4_addr_t *mcIpAddr;
//	int i;
	char	ret;

#if 0
	if(!MUX_IS_TX(&muxRun) )
	{
		index += snprintf(outBuffer+index, bufferLen-index, "'"MUX_CMD_DEST_INFO" is not used in RX!"MUX_NEW_LINE);
		return MUX_FALSE;
	}
#endif

	if(argc==1)
	{
		_muxVideoConfigOutput(outBuffer, bufferLen, &muxRun.dest);
		return MUX_FALSE;
	}

	ret = _muxVideoConfigParse(outBuffer, bufferLen, &vCfg);
	if(ret == EXIT_FAILURE)
	{
		return MUX_FALSE;
	}

	muxVideoConfigCopy(&muxRun.dest, &vCfg);
	if(!MUX_IS_TX(&muxRun) && !muxNetIsGroupAddress(&muxRun.dest.ip) )
	{/* for RX unicast, must be same */
		muxVideoConfigCopy(&muxRun.local, &vCfg);
	}

	bspCfgSave(&muxRun, MUX_CFG_MAIN);

	_muxVideoConfigOutput(outBuffer+index, bufferLen-index, &muxRun.dest);

//	muxFpgaConfig(&muxRun);

	return MUX_FALSE;
}

/* configure Local: MAC, IP, v/a port, both for TX/RX */
char	cmnCmdLocalInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	MUX_VIDEO_CONFIG vCfg;
	char	ret;
	const ip4_addr_t *mcIpAddr;

	if(argc==1)
	{
		_muxVideoConfigOutput(outBuffer, bufferLen, &muxRun.local);
		return MUX_FALSE;
	}

	ret = _muxVideoConfigParse(outBuffer, bufferLen, &vCfg);
	if(ret == EXIT_FAILURE)
	{
		return MUX_FALSE;
	}
	
	mcIpAddr = (ip4_addr_t *)&vCfg.ip;
	if( ip4_addr_ismulticast(mcIpAddr) && MUX_IS_TX(&muxRun) )
	{
		index += snprintf(outBuffer+index, bufferLen-index, "'"MUX_CMD_LOCAL_INFO" : multicast address can be used as local address for TX!"MUX_NEW_LINE);
		return MUX_FALSE;
	}

	muxVideoConfigCopy(&muxRun.local, &vCfg);
	
	bspCfgSave(&muxRun, MUX_CFG_MAIN);

	_muxVideoConfigOutput(outBuffer+index, bufferLen-index, &muxRun.local);

//	muxFpgaConfig(&muxRun);

	return MUX_FALSE;
}



#if LWIP_EXT_UDP_TX_PERF
char	muxCmdUdpTxPerf(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;
	unsigned int address;
	char ret;

	TRACE();
	if(argc==1)
	{
		index += snprintf(outBuffer+index, bufferLen-index, MUX_CMD_UDP_PERF" <ipaddress of UDP Perf server>:"MUX_NEW_LINE" for example, 192.168.168.102"MUX_NEW_LINE );
		return MUX_FALSE;
	}

	TRACE();
	address = ipaddr_addr(argv[1]);
	if(address == IPADDR_NONE)
	{
		index += snprintf(outBuffer+index, bufferLen-index, "\tArgument '%s' is not IP address"MUX_NEW_LINE, argv[1] );
		return MUX_FALSE;
	}

	TRACE();
	ret = extUdpTxPerfStart(address);
		
	index += snprintf(outBuffer+index, bufferLen-index, "\tUDP Perf to %s has started %s"MUX_NEW_LINE, argv[1], (ret==EXIT_SUCCESS)?"OK":"Fail" );

	return MUX_FALSE;
}
#endif


