
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "extSysParams.h"

static unsigned char		_readBuffer[EXT_RW_BUFFER_SIZE];
static unsigned char		_writeBuffer[EXT_RW_BUFFER_SIZE];

EXT_RUNTIME_CFG			extRun;


char extSysAtoInt8(const char *str, unsigned char *value)
{
	char key;
	unsigned int dw = 0;

	*value = 0;
	for (dw = 0; dw < 2; dw++)
	{
		key = *str;

		if (key >= '0' && key <= '9')
		{
			*value = (*value * 16) + (key - '0');
		}
		else
		{
			if (key >= 'A' && key <= 'F')
			{
				*value = (*value * 16) + (key - 'A' + 10);
			}
			else
			{
				if (key >= 'a' && key <= 'f')
				{
					*value = (*value * 16) + (key - 'a' + 10);
				}
				else
				{
					printf("'%c' is not a hexa character!\n\r", key);
					return EXIT_FAILURE;
				}
			}
		}

		str++;
	}
	
//	printf("'%2x' \n\r", value);
	return EXIT_SUCCESS;
}


/*mac address in string format of "xx:xx:xx:xx:xx:xx" into structure */
char	extMacAddressParse(EXT_MAC_ADDRESS *macAddress, const char *macStr)
{
	const char *tmp = macStr;
	int i;

	for(i=0; i< EXT_MAC_ADDRESS_LENGTH; i++)
	{
		if(i != 0)
		{
			tmp = strchr(tmp, ':');
			if(tmp==NULL)
			{
				return EXIT_FAILURE;
			}
			tmp++;
		}
		
		if( tmp )
		{
			if( extSysAtoInt8(tmp, &macAddress->address[i]) )
			{
				return EXIT_FAILURE;
			}
		}
	}

	return EXIT_SUCCESS;
}

/* used in bootloader, RTOS and Linux simhost */
void extCfgFromFactory( EXT_RUNTIME_CFG *cfg )
{
	cfg->magic[0] = EXT_MAGIC_VALUE_A;
	cfg->magic[1] = EXT_MAGIC_VALUE_B;

	sprintf(cfg->name, "%s", EXT_767_PRODUCT_NAME);
	sprintf(cfg->model, "%s", EXT_767_MODEL);

	cfg->version.major = BL_VERSION_MAJOR;
	cfg->version.minor = BL_VERSION_MINOR;
	cfg->version.revision = BL_VERSION_REVISION;

	snprintf(cfg->user, EXT_USER_SIZE, "%s", EXT_USER);
	snprintf(cfg->password, EXT_PASSWORD_SIZE, "%s", EXT_PASSWORD);

	snprintf(cfg->superUser, EXT_USER_SIZE, "%s", EXT_SUPER_USER);
	snprintf(cfg->superPassword, EXT_PASSWORD_SIZE, "%s", EXT_SUPER_PASSWORD);

#if 1
	cfg->isMCast = EXT_TRUE;
#else
	cfg->isMCast = EXT_FALSE;
#endif
	cfg->ipMulticast = CFG_MAKEU32(MCAST_DEFAULT_IPADDR3, MCAST_DEFAULT_IPADDR2, MCAST_DEFAULT_IPADDR1, MCAST_DEFAULT_IPADDR0);

	cfg->isUpdate = EXT_FALSE;
	cfg->isDipOn = EXT_TRUE;
	cfg->isMacConfiged = EXT_FALSE;
	
#if 0
	cfg->macAddress.address[0] = ETHERNET_CONF_ETHADDR0;
	cfg->macAddress.address[1] = ETHERNET_CONF_ETHADDR1;
	cfg->macAddress.address[2] = ETHERNET_CONF_ETHADDR2;
	cfg->macAddress.address[3] = ETHERNET_CONF_ETHADDR3;
	cfg->macAddress.address[5] = ETHERNET_CONF_ETHADDR5;
#endif

	cfg->local.mac.address[0] = ETHERNET_CONF_ETHADDR0;
	cfg->local.mac.address[1] = ETHERNET_CONF_ETHADDR1;
	cfg->local.mac.address[2] = ETHERNET_CONF_ETHADDR2;
	cfg->local.mac.address[3] = ETHERNET_CONF_ETHADDR3;
	cfg->local.mac.address[5] = ETHERNET_CONF_ETHADDR5;

	cfg->dest.mac.address[0] = ETHERNET_CONF_ETHADDR0;
	cfg->dest.mac.address[1] = ETHERNET_CONF_ETHADDR1;
	cfg->dest.mac.address[2] = ETHERNET_CONF_ETHADDR2;
	cfg->dest.mac.address[3] = ETHERNET_CONF_ETHADDR3;
	cfg->dest.mac.address[5] = ETHERNET_CONF_ETHADDR5;
	
	if(EXT_IS_TX(cfg) )
	{
//		cfg->ipAddress = CFG_MAKEU32(ETHERNET_CONF_IPADDR3_TX, ETHERNET_CONF_IPADDR2, ETHERNET_CONF_IPADDR1, ETHERNET_CONF_IPADDR0);
		
//		extMacAddressParse(&cfg->local.mac, "00:04:25:1c:20:01" );

		cfg->local.mac.address[4] = ETHERNET_CONF_ETHADDR4_TX;
		cfg->dest.mac.address[4] = ETHERNET_CONF_ETHADDR4_RX;
		
		cfg->local.ip = CFG_MAKEU32(ETHERNET_CONF_IPADDR3_TX, ETHERNET_CONF_IPADDR2_TX, ETHERNET_CONF_IPADDR1, ETHERNET_CONF_IPADDR0);
		cfg->local.vport = EXT_MEDIA_PORT_TX_VIDEO;
		cfg->local.aport = EXT_MEDIA_PORT_TX_AUDIO;
		cfg->local.dport = EXT_MEDIA_PORT_TX_DATA;
		cfg->local.sport = EXT_MEDIA_PORT_TX_STREA;

		if(cfg->isMCast == EXT_FALSE)
		{
			cfg->dest.ip = CFG_MAKEU32(ETHERNET_CONF_IPADDR3_RX, ETHERNET_CONF_IPADDR2_TX, ETHERNET_CONF_IPADDR1, ETHERNET_CONF_IPADDR0);
		}
		else
		{
			cfg->dest.ip = CFG_MAKEU32(MCAST_DEFAULT_IPADDR3, MCAST_DEFAULT_IPADDR2, MCAST_DEFAULT_IPADDR1, MCAST_DEFAULT_IPADDR0);
		}
		cfg->ipGateway = CFG_MAKEU32(ETHERNET_CONF_GATEWAY_ADDR3, ETHERNET_CONF_GATEWAY_ADDR2_TX, ETHERNET_CONF_GATEWAY_ADDR1, ETHERNET_CONF_GATEWAY_ADDR0);

		if(IP_ADDR_IS_MULTICAST(cfg->dest.ip))
		{
			extTxMulticastIP2Mac(cfg);
		}

		cfg->dest.vport = EXT_MEDIA_PORT_RX_VIDEO;
		cfg->dest.aport = EXT_MEDIA_PORT_RX_AUDIO;
		cfg->dest.dport = EXT_MEDIA_PORT_RX_DATA;
		cfg->dest.sport = EXT_MEDIA_PORT_RX_STREA;
	}
	else
	{/* RX */
//		cfg->ipAddress = CFG_MAKEU32(ETHERNET_CONF_IPADDR3_RX, ETHERNET_CONF_IPADDR2, ETHERNET_CONF_IPADDR1, ETHERNET_CONF_IPADDR0);

//		extMacAddressParse(&cfg->local.mac, "00:04:25:1c:b0:01" );

		/* for RX, 
		* unicast: dest and local are same
		* multicast: local is MCU, dest is multicast 
		*/
		cfg->local.mac.address[4] = ETHERNET_CONF_ETHADDR4_RX;
		cfg->dest.mac.address[4] = ETHERNET_CONF_ETHADDR4_RX;

		cfg->local.ip = CFG_MAKEU32(ETHERNET_CONF_IPADDR3_RX, ETHERNET_CONF_IPADDR2_RX, ETHERNET_CONF_IPADDR1, ETHERNET_CONF_IPADDR0);
		cfg->local.vport = EXT_MEDIA_PORT_RX_VIDEO;
		cfg->local.aport = EXT_MEDIA_PORT_RX_AUDIO;
		cfg->local.dport = EXT_MEDIA_PORT_RX_DATA;
		cfg->local.sport = EXT_MEDIA_PORT_RX_STREA;
		
		if(cfg->isMCast == EXT_FALSE)
		{
			cfg->dest.ip = CFG_MAKEU32(ETHERNET_CONF_IPADDR3_RX, ETHERNET_CONF_IPADDR2_RX, ETHERNET_CONF_IPADDR1, ETHERNET_CONF_IPADDR0);
		}
		else
		{
			cfg->dest.ip = CFG_MAKEU32(MCAST_DEFAULT_IPADDR3, MCAST_DEFAULT_IPADDR2, MCAST_DEFAULT_IPADDR1, MCAST_DEFAULT_IPADDR0);
		}
		cfg->ipGateway = CFG_MAKEU32(ETHERNET_CONF_GATEWAY_ADDR3, ETHERNET_CONF_GATEWAY_ADDR2_RX, ETHERNET_CONF_GATEWAY_ADDR1, ETHERNET_CONF_GATEWAY_ADDR0);

		if(IP_ADDR_IS_MULTICAST(cfg->dest.ip))
		{
			extTxMulticastIP2Mac(cfg);
		}
		
		cfg->dest.vport = EXT_MEDIA_PORT_RX_VIDEO;
		cfg->dest.aport = EXT_MEDIA_PORT_RX_AUDIO;
		cfg->dest.dport = EXT_MEDIA_PORT_RX_DATA;
		cfg->dest.sport = EXT_MEDIA_PORT_RX_STREA;

	}

	cfg->ipSvr811 = CFG_MAKEU32(CONF_SVR811_IPADDR3, CONF_SVR811_IPADDR2, ETHERNET_CONF_IPADDR1, ETHERNET_CONF_IPADDR0);
	cfg->portSvr811 = CONF_SVR811_PORT;
	
//	memcpy(&cfg->videoMacLocal, &cfg->macAddress, EXT_MAC_ADDRESS_LENGTH);
//	cfg->videoIpLocal = cfg->ipAddress;

#if 0
	cfg->mcIp = CFG_MAKEU32(1, 0, 0, 239);
	cfg->mcPort = 3700;
	cfg->destIp = CFG_MAKEU32(59, ETHERNET_CONF_IPADDR2, ETHERNET_CONF_IPADDR1, ETHERNET_CONF_IPADDR0);

#endif

	/* little endian */
	cfg->ipMask = CFG_MAKEU32(ETHERNET_CONF_NET_MASK3, ETHERNET_CONF_NET_MASK2, ETHERNET_CONF_NET_MASK1, ETHERNET_CONF_NET_MASK0);

	/* RS232: 57600, even, 6 bit, 2 stop-bits */
	cfg->rs232Cfg.baudRate = EXT_BAUDRATE_57600;
	cfg->rs232Cfg.charLength = EXT_RS232_CHAR_LENGTH_8;	/* 6 bits */
	cfg->rs232Cfg.parityType = EXT_RS232_PARITY_NONE;
	cfg->rs232Cfg.stopbits = EXT_RS232_STOP_BITS_1;

#ifdef  ARM
	cfg->netMode = (EXT_IP_CFG_DHCP_ENABLE)|0;
#else
	cfg->netMode = 0;
#endif

	cfg->httpPort = EXT_HTTP_SVR_PORT;

	extNmosIdGenerate(&cfg->nodeID, cfg);
	extNmosIdGenerate(&cfg->deviceID, cfg);

	cfg->isConfigFpga = EXT_TRUE;
	memset(&cfg->firmUpdateInfo, 0, sizeof(EXT_FM_UPDATE));

	cfg->endMagic[0] = EXT_MAGIC_VALUE_B;
	cfg->endMagic[1] = EXT_MAGIC_VALUE_A;
}

void extCfgInitAfterReadFromFlash(EXT_RUNTIME_CFG *runCfg)
{
	runCfg->bufRead = _readBuffer;
	runCfg->bufWrite = _writeBuffer;
	runCfg->bufLength = sizeof(_readBuffer);
}

