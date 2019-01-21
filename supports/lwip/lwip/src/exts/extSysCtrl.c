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
	_SETUP_TYPE_SDP,
	_SETUP_TYPE_MEDIA,
	_SETUP_TYPE_CONNECT,
}_SETUP_TYPE;



EXT_RUNTIME_CFG tmpRuntime;

static unsigned short	_setupType;


#define	SETUP_SET_TYPE( _type)	\
		( _setupType |= (1<<(_type) ) )


#define	SETUP_CHECK_TYPE( _type)	\
		(( _setupType & (1<<(_type) ) ) != 0 )



#define	_checkNumU8FieldValue(dest, src, ret)	\
	if(FIELD_IS_CHANGED_U8((src)) && ((src) != *(dest)) ){ *(dest) = (src); (ret) = EXT_TRUE; }

#define	_checkNumU16FieldValue(dest, src, ret)	\
	if(FIELD_IS_CHANGED_U16((src)) && ((src) != *(dest)) ){ *(dest) = (src); (ret) = EXT_TRUE; }

#define	_checkNumU32FieldValue(dest, src, ret)	\
	if(FIELD_IS_CHANGED_U32((src)) && ((src) != *(dest)) ){ *(dest) = (src); (ret) = EXT_TRUE; }


void extSysClearConfig(EXT_RUNTIME_CFG *rxCfg)
{
	memset(rxCfg, 0, sizeof(EXT_RUNTIME_CFG));

	FIELD_INVALIDATE_U8(rxCfg->isMCast);
	FIELD_INVALIDATE_U8(rxCfg->netMode);
	FIELD_INVALIDATE_U8(rxCfg->fpgaAuto);


#if EXT_DIP_SWITCH_ON
	FIELD_INVALIDATE_U8(rxCfg->isDipOn);
#endif
	FIELD_INVALIDATE_U32(rxCfg->rs232Cfg.baudRate);
	FIELD_INVALIDATE_U8(rxCfg->rs232Cfg.charLength);
	FIELD_INVALIDATE_U8(rxCfg->rs232Cfg.parityType);
	FIELD_INVALIDATE_U8(rxCfg->rs232Cfg.stopbits);

	FIELD_INVALIDATE_U16(rxCfg->runtime.vWidth);
	FIELD_INVALIDATE_U16(rxCfg->runtime.vHeight);
	FIELD_INVALIDATE_U8(rxCfg->runtime.vFrameRate);
	FIELD_INVALIDATE_U8(rxCfg->runtime.vColorSpace);
	FIELD_INVALIDATE_U8(rxCfg->runtime.vDepth);
	FIELD_INVALIDATE_U8(rxCfg->runtime.vIsInterlaced);
//	FIELD_INVALIDATE_U8(rxCfg->runtime.vIsSegmented);

	FIELD_INVALIDATE_U8(rxCfg->runtime.aChannels);
	FIELD_INVALIDATE_U8(rxCfg->runtime.aSampleRate);
	FIELD_INVALIDATE_U8(rxCfg->runtime.aDepth);
	FIELD_INVALIDATE_U8(rxCfg->runtime.aPktSize);

	FIELD_INVALIDATE_U8(rxCfg->runtime.isConnect);

	FIELD_INVALIDATE_U8(rxCfg->runtime.reboot);
	FIELD_INVALIDATE_U8(rxCfg->runtime.blink);

	rxCfg->sdpUriVideo.ip = IPADDR_NONE;
	FIELD_INVALIDATE_U16(rxCfg->sdpUriVideo.port);

	rxCfg->sdpUriAudio.ip = IPADDR_NONE;
	FIELD_INVALIDATE_U16(rxCfg->sdpUriAudio.port);
	
	rxCfg->local.ip = IPADDR_NONE;

	rxCfg->ipGateway = IPADDR_NONE;
	rxCfg->ipMask = IPADDR_NONE;
	rxCfg->dest.ip = IPADDR_NONE;
	
	rxCfg->dest.audioIp = IPADDR_NONE;
	rxCfg->dest.ancIp = IPADDR_NONE;
#if EXT_FPGA_AUX_ON	
	rxCfg->dest.auxIp = IPADDR_NONE;
	FIELD_INVALIDATE_U16(rxCfg->dest.sport);
#endif

	FIELD_INVALIDATE_U16(rxCfg->dest.vport);
	FIELD_INVALIDATE_U16(rxCfg->dest.aport);
	FIELD_INVALIDATE_U16(rxCfg->dest.dport);

	_setupType = 0;
}


/* comparing MAC/IP/Mask/gw/dhcp and isTx
After update, reboot is needed, so save is also needed */
static char _compareSystemCfg(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg)
{
	char ret = EXT_FALSE;
	if(rxCfg->netMode != 0XFF )
	{
		if( (runCfg->netMode) !=  (rxCfg->netMode) )
		{
			EXT_CFG_SET_DHCP(runCfg, (rxCfg->netMode) );
			EXT_DEBUGF(EXT_DBG_ON, ("DHCP: %s", (rxCfg->netMode)? "YES":"NO") );
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
			ret = EXT_TRUE;
		}
	}

#if EXT_DIP_SWITCH_ON
//	if(_checkBoolField(&runCfg->isDipOn, rxCfg->isDipOn, EXT_TRUE) == EXIT_SUCCESS)
	_checkNumU8FieldValue(&runCfg->isDipOn, rxCfg->isDipOn, _ret);
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
#endif

	return ret;
}


/* dest IP and ports */
static char _compareRs232Config(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg)
{
	char ret = EXT_FALSE;//, _ret = EXT_FALSE;
	
	_checkNumU32FieldValue(&runCfg->rs232Cfg.baudRate, rxCfg->rs232Cfg.baudRate, ret);	

	_checkNumU8FieldValue(&runCfg->rs232Cfg.charLength, rxCfg->rs232Cfg.charLength, ret);	

	_checkNumU8FieldValue(&runCfg->rs232Cfg.stopbits, rxCfg->rs232Cfg.stopbits, ret);	

	_checkNumU8FieldValue(&runCfg->rs232Cfg.parityType, rxCfg->rs232Cfg.parityType, ret);	
	
	return ret;
}



/* mac, ip, video/audio port of peer */
static char _compareMediaCfg(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg)
{
//	const ip4_addr_t *mcIpAddr;
	char ret = EXT_FALSE;

	if(FIELD_IS_CHANGED_U8(rxCfg->fpgaAuto) )
	{
		if( (runCfg->fpgaAuto) !=  (rxCfg->fpgaAuto) )
		{
			runCfg->fpgaAuto = rxCfg->fpgaAuto;
			EXT_DEBUGF(EXT_DBG_ON, ("FpgaAuto: %s", (rxCfg->fpgaAuto)? "YES":"NO") );
			ret = EXT_TRUE;
		}
	}

	/* video */
	_checkNumU16FieldValue(&runCfg->runtime.vWidth, rxCfg->runtime.vWidth, ret);	

	_checkNumU16FieldValue(&runCfg->runtime.vHeight, rxCfg->runtime.vHeight, ret);	

	_checkNumU8FieldValue(&runCfg->runtime.vFrameRate, rxCfg->runtime.vFrameRate, ret);	


	_checkNumU8FieldValue(&runCfg->runtime.vDepth, rxCfg->runtime.vDepth, ret);	

	_checkNumU8FieldValue(&runCfg->runtime.vColorSpace, rxCfg->runtime.vColorSpace, ret);	

	_checkNumU8FieldValue(&runCfg->runtime.vIsInterlaced, rxCfg->runtime.vIsInterlaced, ret);	

//	_checkNumU8FieldValue(&runCfg->runtime.vIsSegmented, rxCfg->runtime.vIsSegmented, ret);	

	/* audio */
	_checkNumU8FieldValue(&runCfg->runtime.aSampleRate, rxCfg->runtime.aSampleRate, ret);	

	_checkNumU8FieldValue(&runCfg->runtime.aChannels, rxCfg->runtime.aChannels, ret);	

	_checkNumU8FieldValue(&runCfg->runtime.aDepth, rxCfg->runtime.aDepth, ret);	

	_checkNumU8FieldValue(&runCfg->runtime.aPktSize, rxCfg->runtime.aPktSize, ret);	

	return ret;
}



/* SDP IP and ports */
static char _compareSdpConfig(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg)
{
	char ret = EXT_FALSE;

	if(rxCfg->sdpUriVideo.ip != IPADDR_NONE && rxCfg->sdpUriVideo.ip != runCfg->sdpUriVideo.ip )
	{
		 runCfg->sdpUriVideo.ip = rxCfg->sdpUriVideo.ip;
		ret = EXT_TRUE;
	}
	_checkNumU16FieldValue(&runCfg->sdpUriVideo.port, rxCfg->sdpUriVideo.port, ret);
	if(( !IS_STRING_NULL(rxCfg->sdpUriVideo.uri) ) &&(!IS_STRING_EQUAL(rxCfg->sdpUriVideo.uri, runCfg->sdpUriVideo.uri) ))
	{
		snprintf(runCfg->sdpUriVideo.uri, sizeof(runCfg->sdpUriVideo.uri), "%s", rxCfg->sdpUriVideo.uri);
		ret = EXT_TRUE;
	}


	if(rxCfg->sdpUriAudio.ip != IPADDR_NONE && rxCfg->sdpUriAudio.ip != runCfg->sdpUriAudio.ip )
	{
		 runCfg->sdpUriAudio.ip = rxCfg->sdpUriAudio.ip;
		ret = EXT_TRUE;
	}
	_checkNumU16FieldValue(&runCfg->sdpUriAudio.port, rxCfg->sdpUriAudio.port, ret);	
	if(( !IS_STRING_NULL(rxCfg->sdpUriAudio.uri) ) && (!IS_STRING_EQUAL(rxCfg->sdpUriAudio.uri, runCfg->sdpUriAudio.uri) ))
	{
		snprintf(runCfg->sdpUriAudio.uri, sizeof(runCfg->sdpUriAudio.uri), "%s", rxCfg->sdpUriAudio.uri);
		ret = EXT_TRUE;
	}

	return ret;
}


/* dest IP and ports */
static char _compareProtocolConfig(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg)
{
	char ret = EXT_FALSE;

#if 0
	_checkNumU8FieldValue(&runCfg->isMCast, rxCfg->isMCast, ret);	
	{
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("isMcast:%s"LWIP_NEW_LINE, STR_BOOL_VALUE(runCfg->isMCast)) );
		ret = EXT_TRUE;
	}
#endif

//	if( _checkIntegerField(&runCfg->dest.ip, rxCfg->dest.ip) == EXIT_SUCCESS)
	if(rxCfg->dest.ip != IPADDR_NONE && rxCfg->dest.ip != runCfg->dest.ip )
	{
//		runCfg->ipMulticast = rxCfg->dest.ip;
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("New video McastAddress:%s"LWIP_NEW_LINE, extCmnIp4addr_ntoa(&rxCfg->dest.ip)) );
#if EXT_DIP_SWITCH_ON
		if(!EXT_IS_DIP_ON(runCfg) )
		{/* FPGA maybe configured twice: furst here; second, in setupParams() */
			extCmnNewDestIpEffective( runCfg, runCfg->ipMulticast);
		}
//		else

		{/* config type: system, so only save, not reconfigure FPGA */
		}
#endif		
		extLwipGroupMgr(runCfg, runCfg->dest.ip, EXT_FALSE);
		runCfg->dest.ip = rxCfg->dest.ip;
		ret = EXT_TRUE;
	}

	if(rxCfg->dest.audioIp != IPADDR_NONE && rxCfg->dest.audioIp != runCfg->dest.audioIp )
	{
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("New Audio McastAddress:%s"LWIP_NEW_LINE, extCmnIp4addr_ntoa(&rxCfg->dest.ip)) );
		extLwipGroupMgr(runCfg, runCfg->dest.audioIp, EXT_FALSE);
		
		runCfg->dest.audioIp = rxCfg->dest.audioIp;
		ret = EXT_TRUE;
	}

	if(rxCfg->dest.ancIp != IPADDR_NONE && rxCfg->dest.ancIp != runCfg->dest.ancIp )
	{
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("New ANC McastAddress:%s"LWIP_NEW_LINE, extCmnIp4addr_ntoa(&rxCfg->dest.ip)) );
		extLwipGroupMgr(runCfg, runCfg->dest.ancIp, EXT_FALSE);

		runCfg->dest.ancIp = rxCfg->dest.ancIp;
		ret = EXT_TRUE;
	}

#if EXT_FPGA_AUX_ON	
	if(rxCfg->dest.auxIp != IPADDR_NONE && rxCfg->dest.auxIp != runCfg->dest.auxIp )
	{
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("New AUX McastAddress:%s"LWIP_NEW_LINE, extCmnIp4addr_ntoa(&rxCfg->dest.ip)) );
		runCfg->dest.auxIp = rxCfg->dest.auxIp;
		ret = EXT_TRUE;
	}
#endif

	_checkNumU16FieldValue(&runCfg->dest.vport, rxCfg->dest.vport, ret);	

	_checkNumU16FieldValue(&runCfg->dest.aport, rxCfg->dest.aport, ret);	

	_checkNumU16FieldValue(&runCfg->dest.dport, rxCfg->dest.dport, ret);	

#if EXT_FPGA_AUX_ON	
	_checkNumU16FieldValue(&runCfg->dest.sport, rxCfg->dest.sport, ret);	
#endif

	return ret;
}

char extSysCompareParams(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg)
{
//	DEBUG_CFG_PARAMS();

	if(_compareSystemCfg(runCfg, rxCfg) == EXT_TRUE)
	{
		EXT_DEBUGF(EXT_DBG_ON, ("System params changing") );
		SETUP_SET_TYPE(_SETUP_TYPE_SYSTEM);
	}

	if(_compareProtocolConfig(runCfg, rxCfg) == EXT_TRUE)
	{
		EXT_DEBUGF(EXT_DBG_ON, ("Protocol params changing") );
		SETUP_SET_TYPE(_SETUP_TYPE_PROTOCOL);
	}
	
	if(_compareMediaCfg(runCfg, rxCfg) == EXT_TRUE)
	{
		EXT_DEBUGF(EXT_DBG_ON, ("Media params changing") );
		SETUP_SET_TYPE(_SETUP_TYPE_MEDIA);
	}


	if(_compareSdpConfig(runCfg, rxCfg) == EXT_TRUE)
	{
		EXT_DEBUGF(EXT_DBG_ON, ("SDP params changing") );
		SETUP_SET_TYPE(_SETUP_TYPE_SDP);
	}

	if(_compareRs232Config(runCfg, rxCfg) == EXT_TRUE)
	{
		EXT_DEBUGF(EXT_DBG_ON, ("RS232 params changing") );
		SETUP_SET_TYPE(_SETUP_TYPE_RS232);
	}

	if(FIELD_IS_CHANGED_U8(rxCfg->runtime.blink) && (rxCfg->runtime.blink != runCfg->runtime.blink) ) 
	{
		 runCfg->runtime.blink = rxCfg->runtime.blink;
#ifdef ARM
		extFpgaBlinkPowerLED(runCfg->runtime.blink);
#endif
	}

	if(FIELD_IS_CHANGED_U8(rxCfg->runtime.reboot) && (rxCfg->runtime.reboot != runCfg->runtime.reboot) ) 
	{
		 runCfg->runtime.reboot = rxCfg->runtime.reboot;

#ifdef ARM
		extDelayReboot(2000);
#endif
	}

	if(EXT_DEBUG_HTTP_IS_ENABLE() || EXT_DEBUG_HC_IS_ENABLE())
	{
		printf(EXT_NEW_LINE"After configured, Runtime Configuration:"EXT_NEW_LINE);
		extSysCfgDebugData(&extRun);
	}
	return EXIT_SUCCESS;
}


char extSysConfigCtrl(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg)
{
//	EXT_DEBUGF(EXT_DBG_ON, ("config options:0x%x (0x%x)"LWIP_NEW_LINE, _setupType, SETUP_CHECK_TYPE(_SETUP_TYPE_RS232) ) );

	/* save configuration, and reboot to make it active */
	//if(needReboot || hasNewMedia  || needSave)
	if( SETUP_CHECK_TYPE(_SETUP_TYPE_SYSTEM) )	
	{
#ifdef	ARM
		bspCfgSave(runCfg, EXT_CFG_MAIN);
//		bspCmdReboot(NULL, NULL, 0);
//#else
		EXT_DEBUGF(EXT_DBG_ON, ("New system configuration, saving configuration and reboot") );
#endif
	}

	if( SETUP_CHECK_TYPE(_SETUP_TYPE_SDP) )	
	{
#ifdef	ARM
		bspCfgSave(runCfg, EXT_CFG_MAIN);
//		bspCmdReboot(NULL, NULL, 0);
//#else
		EXT_DEBUGF(EXT_DBG_ON, ("New SDP parameters") );
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
		EXT_DEBUGF(EXT_DBG_ON, ("RS232 save and setup") );
#endif
	}
	
	//if(hasNewMedia)
	if(SETUP_CHECK_TYPE(_SETUP_TYPE_PROTOCOL) )
	{
#ifdef	ARM
//		extIgmpGroupMgr(rxCfg, EXT_FALSE);
//		memcpy(&runCfg->dest,  &rxCfg->dest, sizeof(EXT_VIDEO_CONFIG));

		bspCfgSave(runCfg, EXT_CFG_MAIN);

		extFpgaConfig(runCfg);
//#else
		EXT_DEBUGF(EXT_DBG_ON, ("FPGA configuration Protocol"));
#endif
	}
	
	if( SETUP_CHECK_TYPE(_SETUP_TYPE_MEDIA) )
	{
#ifdef	ARM

		bspCfgSave(runCfg, EXT_CFG_MAIN);
		extFpgaConfigParams(runCfg);
//#else
		EXT_DEBUGF(EXT_DBG_ON, ("FPGA configuration Media") );
#endif
	}

#if 0
	/* connect command */
	if(FIELD_IS_CHANGED(rxCfg->runtime.isConnect))
	{
		EXT_DEBUGF(EXT_DBG_ON, ("IsConnect changed: %d(Runtime:%d)", rxCfg->runtime.isConnect, runCfg->runtime.isConnect) );
//		if( parser->runCfg->runtime.isConnect != (rxCfg->runtime.isConnect-1) )
		{
			extCmdConnect( runCfg, rxCfg->runtime.isConnect-1 );
		}
	}
#endif

	return EXIT_SUCCESS;
}

static char _compareOneSdp(HttpClientReq *dest, HttpClientReq *src)
{
	char ret = EXT_FALSE;
	
	if( src->ip != IPADDR_NONE )
	{
		dest->ip = src->ip;
		ret = EXT_TRUE;
	}
	
	if(FIELD_IS_CHANGED_U16(src->port) )
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
	char ret = EXT_FALSE;//, _ret = EXT_FALSE;
	
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

void extSysCfgDebugData(EXT_RUNTIME_CFG *cfg)
{
	printf("\tname:%s; IP:%s"EXT_NEW_LINE, cfg->name, EXT_LWIP_IPADD_TO_STR(&cfg->local.ip));
	printf("\tVideo IP:%s; Port:%d;"EXT_NEW_LINE, EXT_LWIP_IPADD_TO_STR(&cfg->dest.ip), cfg->dest.vport);
	printf("\tAudio IP:%s; Port:%d;"EXT_NEW_LINE,EXT_LWIP_IPADD_TO_STR(&cfg->dest.audioIp), cfg->dest.aport);
	printf("\tANC IP:%s; Port:%d;"EXT_NEW_LINE,EXT_LWIP_IPADD_TO_STR(&cfg->dest.ancIp), cfg->dest.dport);

	printf("\tfpgaAuto:%d; "EXT_NEW_LINE, cfg->fpgaAuto);
	
	printf("\tVideo:Width:%d; height:%d; fps:%d; colorspce:%d; depth=%d, interlaced:%d"EXT_NEW_LINE, 
		cfg->runtime.vWidth, cfg->runtime.vHeight, cfg->runtime.vFrameRate, cfg->runtime.vColorSpace, cfg->runtime.vDepth, cfg->runtime.vIsInterlaced);
	printf("\tAudio:Chans:%d; pktSize:%d; sampleRate:%d"EXT_NEW_LINE, cfg->runtime.aChannels, cfg->runtime.aPktSize, cfg->runtime.aSampleRate);
//	printf("Audio:Chans:%d; depth:%d; pktSize:%d; sampleRate:%d", cfg->runtime.aChannels, cfg->runtime.aDepth, cfg->runtime.aPktSize, cfg->runtime.aSampleRate));

	printf("\tVideo SDP IP:%s; Port:%d; URI:%s"EXT_NEW_LINE,EXT_LWIP_IPADD_TO_STR(&cfg->sdpUriVideo.ip), cfg->sdpUriVideo.port, cfg->sdpUriVideo.uri);
	printf("\tAudio SDP IP:%s; Port:%d; URI:%s"EXT_NEW_LINE,EXT_LWIP_IPADD_TO_STR(&cfg->sdpUriAudio.ip), cfg->sdpUriAudio.port, cfg->sdpUriAudio.uri);
}

