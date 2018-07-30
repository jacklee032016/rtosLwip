/* 
* network commands in command line
*/

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lwip/api.h"
#include "lwip/inet.h"
#include <lwip/netif.h>
#include <lwip/ip4_addr.h>

#include "muxOs.h"

#include "lwipMux.h"

char muxCmdNetPing(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
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


char muxCmdIgmp(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen )
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


char	muxCmdNetInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
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

char	muxCmdMacInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
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

#if 0
static char	_resolveOneParam( char *param, unsigned short *port)
{
	MUX_MAC_ADDRESS macAddress;
	unsigned int ip;
	int ret;

	if(muxMacAddressParse(&macAddress, param) == EXIT_SUCCESS)
	{
		printf("parsed MAC: '%s'"MUX_NEW_LINE, param );
		memcpy(&muxRun.videoMacDest, &macAddress, sizeof(MUX_MAC_ADDRESS));
		return EXIT_SUCCESS;
	}

	ip = ipaddr_addr(param);
	if(ip != IPADDR_NONE)
	{
		printf("parsed IP : '%s'"MUX_NEW_LINE, param );
		muxRun.videoIpDest = ip;
		return EXIT_SUCCESS;
	}

	ret = sscanf(param, "%"PRIu16, port);
	if (1 == ret )
	{
		printf("parsed short int: '%d'"MUX_NEW_LINE, *port );
		return EXIT_SUCCESS;
	}
	printf("parsed error short int:%d; return :%d"MUX_NEW_LINE, *port , ret);
	
	return EXIT_FAILURE;
}
#endif

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
char	muxCmdDestInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
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


#if 1
/* configure Local: MAC, IP, v/a port, both for TX/RX */
char	muxCmdLocalInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
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
#endif


char	muxCmdChangeName(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen)
{
	unsigned int index = 0;

	if(argc==1)
	{
		index += snprintf(outBuffer+index, bufferLen-index, "\tName:'%s'; model:'%s'"MUX_NEW_LINE, muxRun.name, muxRun.model );
		return MUX_FALSE;
	}
	
	memset(muxRun.name, 0, MUX_CMD_MAX_LENGTH);
	memcpy(muxRun.name, argv[1], (strlen(argv[1])>MUX_CMD_MAX_LENGTH)? MUX_CMD_MAX_LENGTH: strlen(argv[1]));
	
	bspCfgSave(&muxRun, MUX_CFG_MAIN);

	index += snprintf(outBuffer+index, bufferLen-index, "\tNew Name:'%s'"MUX_NEW_LINE, muxRun.name );

	return MUX_FALSE;
}



