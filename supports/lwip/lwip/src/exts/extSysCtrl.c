/*
* comparing ypdated parameters and existed parameters, then control device : write to FPGA, save, and/or reboot, etc
* Used by UDP command interface and web interface (web page and REST API)
*/


#include "lwipExt.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "jsmn.h"
#include "extUdpCmd.h"
#include "extFsm.h"

typedef	enum
{
	_SETUP_TYPE_SYSTEM = 0,	/* save and reboot */
	_SETUP_TYPE_NAME,			/* save */
	_SETUP_TYPE_RS232,			/* save and setup */
	_SETUP_TYPE_PROTOCOL,
	_SETUP_TYPE_MEDIA,
	_SETUP_TYPE_CONNECT,
}_SETUP_TYPE;


EXT_RUNTIME_CFG tmpRuntime;

static unsigned short	_setupType;


#define	SETUP_SET_TYPE( _type)	\
		( _setupType |= (1<<(_type) ) )


#define	SETUP_CHECK_TYPE( _type)	\
		(( _setupType & (1<<(_type) ) ) != 0 )



#define	_checkNumFieldValue(dest, src, ret)	\
	ret = EXT_FALSE; if(FIELD_IS_CHANGED((src)) && ((src) != *(dest)) ){ *(dest) = (src); (ret) = EXT_TRUE; }



void extSysClearConfig(EXT_RUNTIME_CFG *rxCfg)
{
	memset(rxCfg, 0, sizeof(EXT_RUNTIME_CFG));

	FIELD_INVALIDATE(rxCfg->isMCast);
	FIELD_INVALIDATE(rxCfg->isDipOn);
	FIELD_INVALIDATE(rxCfg->netMode);
	
	FIELD_INVALIDATE(rxCfg->isDipOn);

	FIELD_INVALIDATE(rxCfg->rs232Cfg.parityType);

	FIELD_INVALIDATE(rxCfg->runtime.vColorSpace);
	FIELD_INVALIDATE(rxCfg->runtime.vIsInterlaced);
	FIELD_INVALIDATE(rxCfg->runtime.vIsSegmented);

	FIELD_INVALIDATE(rxCfg->runtime.isConnect);

	rxCfg->local.ip = IPADDR_NONE;

	_setupType = 0;
	
}


/* comparing MAC/IP/Mask/gw/dhcp and isTx
After update, reboot is needed, so save is also needed */
static char _compareSystemCfg(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg)
{
	char ret = EXT_FALSE, _ret = EXT_FALSE;
	if(FIELD_IS_CHANGED(rxCfg->netMode) )
	{
		if( (runCfg->netMode) !=  (rxCfg->netMode) )
		{
			EXT_CFG_SET_DHCP(runCfg, (rxCfg->netMode) );
			EXT_DEBUGF(EXT_IPCMD_DEBUG, ("DHCP: %s", (rxCfg->netMode)? "YES":"NO") );
			ret = EXT_TRUE;
		}
	}
	
	if( rxCfg->local.ip != IPADDR_NONE )
	{
		runCfg->local.ip = rxCfg->local.ip;
		ret = EXT_TRUE;
	}
	
	if(rxCfg->ipMask != IPADDR_NONE)
	{
		runCfg->ipMask = rxCfg->ipMask;
		ret = EXT_TRUE;
	}
	
	if(rxCfg->ipGateway != IPADDR_NONE)	
	{
		runCfg->ipGateway = rxCfg->ipGateway;
		SETUP_SET_TYPE(_SETUP_TYPE_SYSTEM);
	}

#if 0	
	{
		const ip4_addr_t *mcIpAddr;
		if(!EXT_IS_TX(rawCfg) )
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
			EXT_DEBUGF(EXT_IPCMD_DEBUG, ("DHCP or local IP is not same:mode:%d=%d;IP:%d=%d;mask:%d=%d;gw:%d=%d"LWIP_NEW_LINE, 
				rawCfg->netMode, rxCfg->netMode, rawCfg->local.ip, rxCfg->local.ip, rawCfg->ipMask, rxCfg->ipMask, rawCfg->ipGateway, rxCfg->ipGateway ) );
			return EXIT_FAILURE;
		}
	}
#endif

	if(!MAC_ADDR_IS_NULL(&rxCfg->local.mac) )
	{
		if(! MAC_ADDR_IS_EQUAL(&runCfg->local.mac, &rxCfg->local.mac) )
		{
//			EXT_DEBUGF(EXT_IPCMD_DEBUG, ("MAC address is not same:%s=%s"LWIP_NEW_LINE, runCfg->local.mac.address, rxCfg->local.mac.address));
			memcpy(&runCfg->local.mac, &rxCfg->local.mac, EXT_MAC_ADDRESS_LENGTH);
			runCfg->isMacConfiged = EXT_TRUE;
			ret = EXT_TRUE;
		}
	}

	if(strlen(rxCfg->name) )
	{
		if(!IS_STRING_EQUAL(runCfg->name, rxCfg->name) )
		{
			snprintf(runCfg->name, sizeof(runCfg->name), "%s", rxCfg->name);			
//			needSave = 1;
			ret = EXT_TRUE;
		}
	}

//	if(_checkBoolField(&runCfg->isDipOn, rxCfg->isDipOn, EXT_TRUE) == EXIT_SUCCESS)
	_checkNumFieldValue(&runCfg->isDipOn, rxCfg->isDipOn, _ret);
	if(_ret == EXT_TRUE)
	{
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("DipOn:%s"LWIP_NEW_LINE, STR_BOOL_VALUE(runCfg->isDipOn)) );

		if(!EXT_IS_DIP_ON(runCfg) )
		{/* DIP ON --> OFF */
			extCmnNewDestIpEffective(runCfg, runCfg->ipMulticast);
		}
		/* when DIP is changed to ON, poll job will configure new mulitcast IP from DIP switch */
#if 0
		else
		{/* DIP OFF-->ON */
		}
#endif		
		ret = EXT_TRUE;
	}

	return ret;
}


/* dest IP and ports */
static char _compareRs232Config(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg)
{
	char ret = EXT_FALSE;//, _ret = EXT_FALSE;
	
	_checkNumFieldValue(&runCfg->rs232Cfg.baudRate, rxCfg->rs232Cfg.baudRate, ret);	

	_checkNumFieldValue(&runCfg->rs232Cfg.charLength, rxCfg->rs232Cfg.charLength, ret);	

	_checkNumFieldValue(&runCfg->rs232Cfg.stopbits, rxCfg->rs232Cfg.stopbits, ret);	

	_checkNumFieldValue(&runCfg->rs232Cfg.parityType, rxCfg->rs232Cfg.parityType, ret);	
	
	return ret;
}



/* mac, ip, video/audio port of peer */
static char _compareMediaCfg(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg)
{
//	const ip4_addr_t *mcIpAddr;
	char ret = EXT_FALSE;

	/* video */
	_checkNumFieldValue(&runCfg->runtime.vWidth, rxCfg->runtime.vWidth, ret);	

	_checkNumFieldValue(&runCfg->runtime.vHeight, rxCfg->runtime.vHeight, ret);	

	_checkNumFieldValue(&runCfg->runtime.vFrameRate, rxCfg->runtime.vFrameRate, ret);	

	_checkNumFieldValue(&runCfg->runtime.vDepth, rxCfg->runtime.vDepth, ret);	

	_checkNumFieldValue(&runCfg->runtime.vColorSpace, rxCfg->runtime.vColorSpace, ret);	

	_checkNumFieldValue(&runCfg->runtime.vIsInterlaced, rxCfg->runtime.vIsInterlaced, ret);	

	_checkNumFieldValue(&runCfg->runtime.vIsSegmented, rxCfg->runtime.vIsSegmented, ret);	

	/* audio */
	_checkNumFieldValue(&runCfg->runtime.aSampleRate, rxCfg->runtime.aSampleRate, ret);	

	_checkNumFieldValue(&runCfg->runtime.aChannels, rxCfg->runtime.aChannels, ret);	

	_checkNumFieldValue(&runCfg->runtime.aDepth, rxCfg->runtime.aDepth, ret);	

	return ret;
}


/* dest IP and ports */
static char _compareProtocolConfig(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg)
{
	char ret = EXT_FALSE;

	_checkNumFieldValue(&runCfg->isMCast, rxCfg->isMCast, ret);	
	{
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("isMcast:%s"LWIP_NEW_LINE, STR_BOOL_VALUE(runCfg->isMCast)) );
		ret = EXT_TRUE;
	}

//	if( _checkIntegerField(&runCfg->dest.ip, rxCfg->dest.ip) == EXIT_SUCCESS)
	if(rxCfg->dest.ip != IPADDR_NONE && rxCfg->dest.ip != runCfg->ipMulticast )
	{
		runCfg->ipMulticast = rxCfg->dest.ip;
//		needReboot = 1;
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("New McastAddress:%s"LWIP_NEW_LINE, extCmnIp4addr_ntoa(&rxCfg->dest.ip)) );
		if(!EXT_IS_DIP_ON(runCfg) )
		{/* FPGA maybe configured twice: furst here; second, in setupParams() */
			extCmnNewDestIpEffective( runCfg, runCfg->ipMulticast);
		}
//		else

		{/* config type: system, so only save, not reconfigure FPGA */
		}
		
		ret = EXT_TRUE;
	}
	
	_checkNumFieldValue(&runCfg->dest.vport, rxCfg->dest.vport, ret);	

	_checkNumFieldValue(&runCfg->dest.aport, rxCfg->dest.aport, ret);	

	_checkNumFieldValue(&runCfg->dest.dport, rxCfg->dest.dport, ret);	

	_checkNumFieldValue(&runCfg->dest.sport, rxCfg->dest.sport, ret);	

	return ret;
}

char extSysCompareParams(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg)
{
	if(_compareSystemCfg(runCfg, rxCfg) == EXT_TRUE)
	{
		SETUP_SET_TYPE(_SETUP_TYPE_MEDIA);
	}

	if(_compareRs232Config(runCfg, rxCfg) == EXT_TRUE)
	{
		SETUP_SET_TYPE(_SETUP_TYPE_SYSTEM);
	}

	if(_compareProtocolConfig(runCfg, rxCfg) == EXT_TRUE)
	{
		SETUP_SET_TYPE(_SETUP_TYPE_PROTOCOL);
	}

	if(_compareMediaCfg(runCfg, rxCfg) == EXT_TRUE)
	{
		SETUP_SET_TYPE(_SETUP_TYPE_MEDIA);
	}
	
	return EXIT_SUCCESS;
}


char extSysConfigCtrl(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg)
{
	EXT_DEBUGF(EXT_IPCMD_DEBUG, ("config options:0x%x (0x%x)"LWIP_NEW_LINE, _setupType, SETUP_CHECK_TYPE(_SETUP_TYPE_RS232) ) );

	/* save configuration, and reboot to make it active */
	//if(needReboot || hasNewMedia  || needSave)
	if( SETUP_CHECK_TYPE(_SETUP_TYPE_SYSTEM) )	
	{
#ifdef	ARM
		bspCfgSave(runCfg, EXT_CFG_MAIN);
//		bspCmdReboot(NULL, NULL, 0);
//#else
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("New system configuration, saving configuration and reboot") );
#endif
	}

	//if(needReboot)
	if(SETUP_CHECK_TYPE(_SETUP_TYPE_RS232) || SETUP_CHECK_TYPE(_SETUP_TYPE_NAME))
	{
#ifdef	ARM
		bspCfgSave(runCfg, EXT_CFG_MAIN);
		if(SETUP_CHECK_TYPE(_SETUP_TYPE_RS232) )
		{
			extHwRs232Config(runCfg);
		}
//#else
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("RS232 save and setup") );
#endif
	}
	
	//if(hasNewMedia)
	if(SETUP_CHECK_TYPE(_SETUP_TYPE_PROTOCOL) )
	{
#ifdef	ARM
		bspCfgSave(runCfg, EXT_CFG_MAIN);
		extFpgaConfig(runCfg);
//#else
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("FPGA configuration Protocol"));
#endif
	}
	
	if( SETUP_CHECK_TYPE(_SETUP_TYPE_MEDIA) )
	{
#ifdef	ARM
		extFpgaConfigParams(&runCfg->runtime);
//#else
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("FPGA configuration Media") );
#endif
	}

	/* connect command */
	if(FIELD_IS_CHANGED(rxCfg->runtime.isConnect))
	{
		EXT_DEBUGF(EXT_DBG_ON, ("IsConnect changed: %d(Runtime:%d)", rxCfg->runtime.isConnect, runCfg->runtime.isConnect) );
//		if( parser->runCfg->runtime.isConnect != (rxCfg->runtime.isConnect-1) )
		{
			extCmdConnect( runCfg, rxCfg->runtime.isConnect-1 );
		}
	}

	return EXIT_SUCCESS;
}

static char _compareOneSdp(ExtSdpUri *dest, ExtSdpUri *src)
{
	char ret = EXT_FALSE;
	
	if( src->ip != IPADDR_NONE )
	{
		dest->ip = src->ip;
		ret = EXT_TRUE;
	}
	
	if(FIELD_IS_CHANGED(src->port) )
	{
		dest->port = src->port;
		ret = EXT_TRUE;
	}
	
	if(! IS_STRING_NULL(src->uri) )	
	{
		memcpy(dest->uri, src->uri, sizeof(dest->uri));
		ret = EXT_TRUE;
	}

	return ret;
}

char extSysConfigSdpClient(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg)
{
	char ret = EXT_FALSE, _ret = EXT_FALSE;
	
	ret = _compareOneSdp(&runCfg->sdpUriVideo, &rxCfg->sdpUriVideo);

	ret = _compareOneSdp(&runCfg->sdpUriAudio, &rxCfg->sdpUriAudio);
	
	if(ret == EXT_TRUE)
	{
#ifdef	ARM
		bspCfgSave(runCfg, EXT_CFG_MAIN);
#else
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("New SDP configuration is saved!") );
#endif
	}
	
	return ret;
}

