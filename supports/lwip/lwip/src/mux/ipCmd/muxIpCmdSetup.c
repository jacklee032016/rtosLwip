
#include "lwipMux.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "jsmn.h"
#include "muxIpCmd.h"


typedef	enum
{
	_SETUP_TYPE_SYSTEM = 0,	/* save and reboot */
	_SETUP_TYPE_NAME,			/* save */
	_SETUP_TYPE_RS232,			/* save and setup */
	_SETUP_TYPE_PROTOCOL,
	_SETUP_TYPE_MEDIA,
	_SETUP_TYPE_CONNECT,
}_SETUP_TYPE;


MUX_RUNTIME_CFG tmp;
	char needReboot = 0;
	char	hasNewMedia = 0;
	char	needSave = 0;

static unsigned short	_setupType;


#define	SETUP_SET_TYPE( _type)	\
		( _setupType |= (1<<(_type) ) )


#define	SETUP_CHECK_TYPE( _type)	\
		(( _setupType &= (1<<(_type) ) ) != 0 )



#define	FIELD_IS_CHANGED( field)	\
		( (field)!= 0 )

#define	FIELD_REAL_VALUE(field) 	\
		( (field)-1 )


static char _checkIntegerField(unsigned int *dest, unsigned int src)
{
	if(FIELD_IS_CHANGED(src) )
	{
		if(src != *dest)
		{
			*dest = src;
			return EXIT_SUCCESS;
		}
	}

	return EXIT_FAILURE;
}

static char _checkShortField(unsigned short *dest, unsigned short src)
{
	if(FIELD_IS_CHANGED(src) )
	{
		if(src != *dest)
		{
			*dest = src;
			return EXIT_SUCCESS;
		}
	}

	return EXIT_FAILURE;
}


static char _checkBoolField(unsigned char *dest, unsigned char src, unsigned char isBool)
{
	if(FIELD_IS_CHANGED(src) )
	{
		if(isBool)
		{
			if(*dest != FIELD_REAL_VALUE(src) )
			{
				*dest = FIELD_REAL_VALUE(src);
				return EXIT_SUCCESS;
			}
		}
		else
		{
			if(*dest != src )
			{
				*dest = src;
				return EXIT_SUCCESS;
			}
		}
	}

	return EXIT_FAILURE;
}


/* comparing MAC/IP/Mask/gw/dhcp and isTx
After update, reboot is needed, so save is also needed */
static char _compareSystemCfg(MUX_RUNTIME_CFG *runCfg, MUX_RUNTIME_CFG *rxCfg)
{
	if(FIELD_IS_CHANGED(rxCfg->netMode) )
	{
		if( (runCfg->netMode) !=  FIELD_REAL_VALUE(rxCfg->netMode) )
		{
			MUX_CFG_SET_DHCP(runCfg, FIELD_REAL_VALUE(rxCfg->netMode) );
			MUX_DEBUGF(MUX_IPCMD_DEBUG, ("DHCP: %s",FIELD_REAL_VALUE(rxCfg->netMode)? "YES":"NO") );
//			needReboot = 1;
			SETUP_SET_TYPE(_SETUP_TYPE_SYSTEM);
		}
	}
	
//	if( _checkIntegerField(&runCfg->local.ip, rxCfg->local.ip) == EXIT_SUCCESS)
	if( rxCfg->local.ip != IPADDR_NONE )
	{
		runCfg->local.ip = rxCfg->local.ip;
//		needReboot = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_SYSTEM);
	}
	
//	if( _checkIntegerField(&runCfg->ipMask, rxCfg->ipMask ) == EXIT_SUCCESS)
	if(rxCfg->ipMask != IPADDR_NONE)
	{
		runCfg->ipMask = rxCfg->ipMask;
//		needReboot = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_SYSTEM);
	}
	
//	if( _checkIntegerField(&runCfg->ipGateway,rxCfg->ipGateway) == EXIT_SUCCESS)
	if(rxCfg->ipGateway != IPADDR_NONE)	
	{
		runCfg->ipGateway = rxCfg->ipGateway;
//		needReboot = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_SYSTEM);
	}

#if 0	
	{
		const ip4_addr_t *mcIpAddr;
		if(!MUX_IS_TX(rawCfg) )
		{/* RX */
			mcIpAddr = (ip4_addr_t *)&rxCfg->local.ip;
			if(! ip4_addr_ismulticast(mcIpAddr))
			{
				return EXIT_FAILURE;
			}
			
			return EXIT_SUCCESS;
		}
		else
		{
			MUX_DEBUGF(MUX_IPCMD_DEBUG, ("DHCP or local IP is not same:mode:%d=%d;IP:%d=%d;mask:%d=%d;gw:%d=%d"LWIP_NEW_LINE, 
				rawCfg->netMode, rxCfg->netMode, rawCfg->local.ip, rxCfg->local.ip, rawCfg->ipMask, rxCfg->ipMask, rawCfg->ipGateway, rxCfg->ipGateway ) );
			return EXIT_FAILURE;
		}
	}
#endif

	if(!MAC_ADDR_IS_NULL(&rxCfg->local.mac) )
	{
		if(! MAC_ADDR_IS_EQUAL(&runCfg->local.mac, &rxCfg->local.mac) )
		{
//			MUX_DEBUGF(MUX_IPCMD_DEBUG, ("MAC address is not same:%s=%s"LWIP_NEW_LINE, runCfg->local.mac.address, rxCfg->local.mac.address));
			memcpy(&runCfg->local.mac, &rxCfg->local.mac, MUX_MAC_ADDRESS_LENGTH);	
//			needReboot = 1;
			SETUP_SET_TYPE(_SETUP_TYPE_SYSTEM);
		}
	}

	if(strlen(rxCfg->name) )
	{
		if(!IS_STRING_EQUAL(runCfg->name, rxCfg->name) )
		{
			snprintf(runCfg->name, sizeof(runCfg->name), "%s", rxCfg->name);			
//			needSave = 1;
			SETUP_SET_TYPE(_SETUP_TYPE_SYSTEM);
		}
	}

	if(_checkBoolField(&runCfg->isDipOn, rxCfg->isDipOn, MUX_TRUE) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("DipOn:%s"LWIP_NEW_LINE, STR_BOOL_VALUE(runCfg->isDipOn)) );
//		needSave = 1;
//		needReboot = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_SYSTEM);
	}

	return EXIT_SUCCESS;
}


/* dest IP and ports */
static char _compareRs232Config(MUX_RUNTIME_CFG *runCfg, MUX_RUNTIME_CFG *rxCfg)
{
	if(_checkIntegerField(&runCfg->rs232Cfg.baudRate, rxCfg->rs232Cfg.baudRate) == EXIT_SUCCESS)
	{
//		hasNewMedia = 1;
//		needSave = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_RS232);
	}

	if(_checkBoolField(&runCfg->rs232Cfg.charLength, rxCfg->rs232Cfg.charLength, MUX_FALSE) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("Databits:%d"LWIP_NEW_LINE, runCfg->rs232Cfg.charLength) );
//		needSave = 1;
//		needReboot = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_RS232);
	}

	if(_checkBoolField(&runCfg->rs232Cfg.stopbits, rxCfg->rs232Cfg.stopbits, MUX_FALSE) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("Stopbits:%d"LWIP_NEW_LINE, runCfg->rs232Cfg.stopbits) );
//		needSave = 1;
//		needReboot = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_RS232);
	}

	if(_checkBoolField(&runCfg->rs232Cfg.parityType, rxCfg->rs232Cfg.parityType, MUX_TRUE) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("Databits:%d"LWIP_NEW_LINE, runCfg->rs232Cfg.parityType) );
//		needSave = 1;
//		needReboot = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_RS232);
	}
	
	return EXIT_SUCCESS;
}

/* dest IP and ports */
static char _compareProtocolConfig(MUX_RUNTIME_CFG *runCfg, MUX_RUNTIME_CFG *rxCfg)
{
	if(_checkBoolField(&runCfg->isMCast, rxCfg->isMCast, MUX_TRUE) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("isMcast:%s"LWIP_NEW_LINE, STR_BOOL_VALUE(runCfg->isMCast)) );
//		needSave = 1;
//		needReboot = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_PROTOCOL);
	}

//	if( _checkIntegerField(&runCfg->dest.ip, rxCfg->dest.ip) == EXIT_SUCCESS)
	if(rxCfg->dest.ip != IPADDR_NONE)
	{
		runCfg->dest.ip = rxCfg->dest.ip;
//		needReboot = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_PROTOCOL);
	}
	
	if( _checkShortField(&runCfg->dest.vport, rxCfg->dest.vport) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("vport:%hd"LWIP_NEW_LINE, runCfg->dest.vport) );
//		needReboot = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_PROTOCOL);
	}

	if( _checkShortField(&runCfg->dest.aport, rxCfg->dest.aport) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("vport:%hd"LWIP_NEW_LINE, runCfg->dest.aport) );
//		needReboot = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_PROTOCOL);
	}

	if( _checkShortField(&runCfg->dest.dport, rxCfg->dest.dport) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("vport:%hd"LWIP_NEW_LINE, runCfg->dest.aport) );
//		needReboot = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_PROTOCOL);
	}

	if( _checkShortField(&runCfg->dest.sport, rxCfg->dest.sport) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("vport:%hd"LWIP_NEW_LINE, runCfg->dest.sport) );
//		needReboot = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_PROTOCOL);
	}

	return EXIT_SUCCESS;
}



/* mac, ip, video/audio port of peer */
static char _compareMediaCfg(MUX_RUNTIME_CFG *runCfg, MUX_RUNTIME_CFG *rxCfg)
{
//	const ip4_addr_t *mcIpAddr;

	/* video */
	if( _checkShortField(&runCfg->runtime.vWidth, rxCfg->runtime.vWidth) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("vWidth:%hd"LWIP_NEW_LINE, runCfg->runtime.vWidth) );
//		needReboot = 1;
//		hasNewMedia = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_MEDIA);
	}

	if( _checkShortField(&runCfg->runtime.vHeight, rxCfg->runtime.vHeight) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("vHeight:%hd"LWIP_NEW_LINE, runCfg->runtime.vHeight) );
//		needReboot = 1;
//		hasNewMedia = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_MEDIA);
	}


	if(_checkBoolField(&runCfg->runtime.vFrameRate, rxCfg->runtime.vFrameRate, MUX_FALSE) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("vFps:%d"LWIP_NEW_LINE, runCfg->runtime.vFrameRate) );
//		needReboot = 1;
//		hasNewMedia = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_MEDIA);
	}

	if(_checkBoolField(&runCfg->runtime.vDepth, rxCfg->runtime.vDepth, MUX_FALSE) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("vDepth:%d"LWIP_NEW_LINE, runCfg->runtime.vDepth) );
//		needReboot = 1;
//		hasNewMedia = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_MEDIA);
	}


	if(_checkBoolField(&runCfg->runtime.vColorSpace, rxCfg->runtime.vColorSpace, MUX_TRUE) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("vColorSpace:%d"LWIP_NEW_LINE, runCfg->runtime.vColorSpace) );
//		needReboot = 1;
//		hasNewMedia = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_MEDIA);
	}

	if(_checkBoolField(&runCfg->runtime.vIsInterlaced, rxCfg->runtime.vIsInterlaced, MUX_TRUE) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("isInterlaced:%s"LWIP_NEW_LINE, STR_BOOL_VALUE(runCfg->runtime.vIsInterlaced) ) );
//		needReboot = 1;
//		hasNewMedia = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_MEDIA);
	}

	if(_checkBoolField(&runCfg->runtime.vIsSegmented, rxCfg->runtime.vIsSegmented, MUX_TRUE) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("isInterlaced:%s"LWIP_NEW_LINE, STR_BOOL_VALUE(runCfg->runtime.vIsSegmented) ));
//		needReboot = 1;
//		hasNewMedia = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_MEDIA);
	}

	/* audio */
	if( _checkShortField(&runCfg->runtime.aSampleRate, rxCfg->runtime.aSampleRate) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("aSample:%hd"LWIP_NEW_LINE, runCfg->runtime.aSampleRate) );
//		needReboot = 1;
//		hasNewMedia = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_MEDIA);
	}
	if(_checkBoolField(&runCfg->runtime.aChannels, rxCfg->runtime.aChannels, MUX_FALSE) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("vChannel:%d"LWIP_NEW_LINE, runCfg->runtime.aChannels) );
//		needReboot = 1;
//		hasNewMedia = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_MEDIA);
	}

	if(_checkBoolField(&runCfg->runtime.aDepth, rxCfg->runtime.aDepth, MUX_FALSE) == EXIT_SUCCESS)
	{
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("aDepth:%d"LWIP_NEW_LINE, runCfg->runtime.aDepth) );
//		needReboot = 1;
//		hasNewMedia = 1;
		SETUP_SET_TYPE(_SETUP_TYPE_MEDIA);
	}


	return EXIT_SUCCESS;
}


char muxIpCmdSetupParams(MUX_JSON_PARSER  *parser)
{
	char ret;
	MUX_RUNTIME_CFG *rxCfg = &tmp;

	ret = muxIpCmdIsLocal(parser);
	if(ret == EXIT_FAILURE)
		return ret;

	memset(rxCfg, 0, sizeof(MUX_RUNTIME_CFG));
	_setupType = 0;
//	tmp.local.ip = IPADDR_NONE;
//	tmp.dest.ip = IPADDR_NONE;

	if(muxJsonRequestParse(parser, rxCfg) == EXIT_FAILURE)
	{
		parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
		return EXIT_FAILURE;
	}



#ifdef	X86
	muxDebugCfg(rxCfg, LWIP_NEW_LINE"RECEIVED Cfg:");
	muxDebugCfg(parser->runCfg,LWIP_NEW_LINE "Before update Cfg:");
#endif

	_compareSystemCfg(parser->runCfg, rxCfg);

	_compareRs232Config(parser->runCfg, rxCfg);

	_compareProtocolConfig(parser->runCfg, rxCfg);

	_compareMediaCfg(parser->runCfg, rxCfg);

	/* reply cmd with updated params */
	muxIpCmdSendMediaData(parser);

#ifdef	X86
	muxDebugCfg(parser->runCfg, LWIP_NEW_LINE"updated Cfg:");
#endif

	muxJsonResponsePrintConfig(parser);

	/* save configuration, and reboot to make it active */
	//if(needReboot || hasNewMedia  || needSave)
	if( SETUP_CHECK_TYPE(_SETUP_TYPE_SYSTEM) )	
	{
#ifdef	ARM
		bspCfgSave(parser->runCfg, MUX_CFG_MAIN);
		bspCmdReboot(NULL, NULL, 0);
#else
		printf("New system configuration, saving configuration and reboot"LWIP_NEW_LINE);
#endif
	}

	//if(needReboot)
	if(SETUP_CHECK_TYPE(_SETUP_TYPE_RS232) || SETUP_CHECK_TYPE(_SETUP_TYPE_NAME))
	{
#ifdef	ARM
		bspCfgSave(parser->runCfg, MUX_CFG_MAIN);
		if(SETUP_CHECK_TYPE(_SETUP_TYPE_RS232) )
		{
			muxHwRs232Config(parser->runCfg);
		}
#else
		printf("RS232 save and setup"LWIP_NEW_LINE);
#endif
	}
	
	//if(hasNewMedia)
	if(SETUP_CHECK_TYPE(_SETUP_TYPE_PROTOCOL) || SETUP_CHECK_TYPE(_SETUP_TYPE_MEDIA) )
	{
#ifdef	ARM
		if(SETUP_CHECK_TYPE(_SETUP_TYPE_PROTOCOL))
		{
			bspCfgSave(parser->runCfg, MUX_CFG_MAIN);
		}
		muxFpgaConfig(parser->runCfg);
#else
		printf("FPGA configuration(Protocol|Media)"LWIP_NEW_LINE);
#endif
	}

	/* connect command */
	if(FIELD_IS_CHANGED(rxCfg->runtime.isConnect))
	{
		if( parser->runCfg->runtime.isConnect != rxCfg->runtime.isConnect )
		{
			muxCmdConnect( parser->runCfg);
		}
	}

	return EXIT_SUCCESS;
}



void muxVideoConfigCopy(MUX_VIDEO_CONFIG *dest, MUX_VIDEO_CONFIG *src)
{
	const ip4_addr_t *mcIpAddr;
	
	memcpy(dest->mac.address, src->mac.address, MUX_MAC_ADDRESS_LENGTH);
	if(src->ip != IPADDR_NONE)
	{
		dest->ip = src->ip;
	}

	mcIpAddr = (ip4_addr_t *)&dest->ip;
	if( ip4_addr_ismulticast(mcIpAddr))
	{
		muxNetMulticastIP4Mac(&dest->ip, &dest->mac);
	}

	if(src->vport != 0)
	{
		dest->vport = src->vport;
	}
	
	if(src->aport != 0)
	{
		dest->aport = src->aport;
	}
}

extern	struct netif			muxNetIf;

char muxCmdConnect(MUX_RUNTIME_CFG  *runCfg)
{
	char newStatus;
//	char action[32];
//	int index = 0;

	newStatus = (runCfg->runtime.isConnect==0);

	if(MUX_IS_TX(runCfg) )
	{
#ifdef	ARM
		muxFpgaEnable(newStatus);
#endif
	}
	else
	{/* RX leave igmp group */
		char ret = EXIT_SUCCESS;
		if(IP_ADDR_IS_MULTICAST(runCfg->dest.ip))
		{
#ifdef	ARM
			ret = muxLwipGroupMgr(&muxNetIf, runCfg->dest.ip, newStatus);
#endif
			MUX_DEBUGF(MUX_IPCMD_DEBUG, ("IGMP group %s %s: %s"LWIP_NEW_LINE, (newStatus==0)?"leave":"join", MUX_LWIP_IPADD_TO_STR(&runCfg->dest.ip), (ret==EXIT_SUCCESS)?"OK":"Fail" ));
		}
	}

	runCfg->runtime.isConnect = newStatus;

	return EXIT_SUCCESS;
}


