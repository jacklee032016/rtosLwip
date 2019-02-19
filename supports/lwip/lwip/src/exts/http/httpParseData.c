/*
* parse data in HTTP request, for HTML form data and data from REST API
*/


#include "lwipExt.h"

#include "http.h"

#include "jsmn.h"

/* data from HTTP form for SDP client */
char extHttpParseSdpClientData(ExtHttpConn *ehc, EXT_RUNTIME_CFG *tmpCfg, char *key, char *value)
{
	if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_SDP_VEDIO_IP, strlen(key)))
	{
		if(cmnUtilsParseIp(value, &tmpCfg->sdpUriVideo.ip) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate IP address for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_SDP_AUDIO_IP, strlen(key)))
	{
		if(cmnUtilsParseIp(value, &tmpCfg->sdpUriAudio.ip) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate IP address for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}

	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_SDP_ANC_IP, strlen(key)))
	{
		if(cmnUtilsParseIp(value, &tmpCfg->sdpUriAnc.ip) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate IP address for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
	
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_SDP_VEDIO_PORT, strlen(key)))
	{
		if(cmnUtilsParseInt16(value, &tmpCfg->sdpUriVideo.port) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_SDP_AUDIO_PORT, strlen(key)))
	{
		if(cmnUtilsParseInt16(value, &tmpCfg->sdpUriAudio.port) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_SDP_ANC_PORT, strlen(key)))
	{
		if(cmnUtilsParseInt16(value, &tmpCfg->sdpUriAnc.port) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}

	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_SDP_VEDIO_URI, strlen(key)))
	{
		snprintf(tmpCfg->sdpUriVideo.uri, sizeof(tmpCfg->sdpUriVideo.uri), "%s", value);
//		EXT_DEBUGF(EXT_DBG_ON, ("Before unescape '%s'", tmpCfg->sdpUriVideo.uri));
		unescape_uri(tmpCfg->sdpUriVideo.uri);
//		EXT_DEBUGF(EXT_DBG_ON, ("After unescape '%s'", tmpCfg->sdpUriVideo.uri));
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_SDP_AUDIO_URI, strlen(key)))
	{
		snprintf(tmpCfg->sdpUriAudio.uri, sizeof(tmpCfg->sdpUriAudio.uri), "%s", value);
//		EXT_DEBUGF(EXT_DBG_ON, ("Before unescape '%s'", tmpCfg->sdpUriAudio.uri));
		unescape_uri(tmpCfg->sdpUriAudio.uri);
//		EXT_DEBUGF(EXT_DBG_ON, ("After unescape '%s'", tmpCfg->sdpUriAudio.uri));
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_SDP_ANC_URI, strlen(key)))
	{
		snprintf(tmpCfg->sdpUriAnc.uri, sizeof(tmpCfg->sdpUriAnc.uri), "%s", value);
//		EXT_DEBUGF(EXT_DBG_ON, ("Before unescape '%s'", tmpCfg->sdpUriAudio.uri));
		unescape_uri(tmpCfg->sdpUriAnc.uri);
//		EXT_DEBUGF(EXT_DBG_ON, ("After unescape '%s'", tmpCfg->sdpUriAudio.uri));
	}

	else
	{
//		EXT_INFOF(("Key '%s' and value '%s' is not support now", key,  value));
	}


	return EXIT_SUCCESS;
}


/* parse data from form of setting web page */
char extHttpParseData(ExtHttpConn *ehc, EXT_RUNTIME_CFG *tmpCfg, char *key, char *value)
{
	uint8_t	charVal;
	uint32_t	intVal;
//	EXT_DEBUGF(EXT_DBG_ON, ("new vDepth=%d, value:%s, '%s'='%s'", tmpCfg->runtime.vDepth, value, key, value ));

//	EXT_DEBUGF(EXT_DBG_ON, (EXT_NEW_LINE"'%s'='%s'", key, value) );extSysCfgDebugData(&tmpRuntime);
	
	if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_PRODUCT, strlen(key)))
	{
		snprintf(tmpCfg->name, sizeof(tmpCfg->name), "%s", value);
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_IP_VEDIO, strlen(key)))
	{
		if(cmnUtilsParseIp(value, &tmpCfg->dest.ip) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate IP address for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_IP_AUDIO, strlen(key)))
	{
		if(cmnUtilsParseIp(value, &tmpCfg->dest.audioIp) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate IP address for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_IP_ANC, strlen(key)))
	{
		if(cmnUtilsParseIp(value, &tmpCfg->dest.ancIp) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate IP address for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
#if EXT_FPGA_AUX_ON	
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_IP_AUX, strlen(key)))
	{
		if(cmnUtilsParseIp(value, &tmpCfg->dest.auxIp) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate IP address for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
#endif	
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_PORT_VEDIO, strlen(key)))
	{
		if(cmnUtilsParseInt16(value, &tmpCfg->dest.vport) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_PORT_AUDIO, strlen(key)))
	{
		if(cmnUtilsParseInt16(value, &tmpCfg->dest.aport) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_PORT_DATA, strlen(key)))
	{
		if(cmnUtilsParseInt16(value, &tmpCfg->dest.dport) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}

	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_FPGA_AUTO, strlen(key)))
	{
		if(IS_STRING_EQUAL(value, EXT_WEB_CFG_FIELD_FPGA_AUTO_V_AUTO) )
		{
			tmpCfg->fpgaAuto = FPGA_CFG_AUTO;
		}
		else
		{
			tmpCfg->fpgaAuto = FPGA_CFG_MANUAL;
		}
		
	}
	

#if EXT_FPGA_AUX_ON	
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_PORT_STREM, strlen(key)))
	{
		if(cmnUtilsParseInt16(value, &tmpCfg->dest.sport) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
#endif

#if 1
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_VIDEO_PARAMS, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &charVal) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate index value for '%s'", value, key);
			return EXIT_FAILURE;
		}

		if(extCmnVideoParamPopulate(tmpCfg, charVal) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate index of video param for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
#else
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_VIDEO_WIDTH, strlen(key)))
	{
		if(cmnUtilsParseInt16(value, &tmpCfg->runtime.vWidth) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_VIDEO_HEIGHT, strlen(key)))
	{
		if(cmnUtilsParseInt16(value, &tmpCfg->runtime.vHeight) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_FRAME_RATE, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.vFrameRate) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
		EXT_DEBUGF(EXT_DBG_OFF, ("new vFrameRate=%d"EXT_NEW_LINE, tmpCfg->runtime.vFrameRate));
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_VIDEO_INTERLACE, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.vIsInterlaced) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
		if(tmpCfg->runtime.vIsInterlaced != EXT_VIDEO_INTLC_A_PROGRESSIVE && 
			tmpCfg->runtime.vIsInterlaced != EXT_VIDEO_INTLC_B_PROGRESSIVE &&
			tmpCfg->runtime.vIsInterlaced != EXT_VIDEO_INTLC_INTERLACED )
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			EXT_ERRORF(("'%s(%d)' is not validate interlaced for '%s'", value, tmpCfg->runtime.vIsInterlaced, key));
			tmpCfg->runtime.vIsInterlaced = EXT_VIDEO_INTLC_INTERLACED;
		}
	}
#endif

	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_COLOR_SPACE, strlen(key)))
	{
//	EXT_DEBUGF(EXT_DBG_ON, (EXT_NEW_LINE"'%s'='%s'", key, value) );extSysCfgDebugData(&tmpRuntime);
//		EXT_DEBUGF(EXT_DBG_ON, ("new vColorSpace=%d, vDepth:%d", tmpCfg->runtime.vColorSpace, tmpCfg->runtime.vDepth));
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.vColorSpace) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
//	EXT_DEBUGF(EXT_DBG_ON, (EXT_NEW_LINE"'%s'='%s'", key, value) );extSysCfgDebugData(&tmpRuntime);
//		EXT_DEBUGF(EXT_DBG_ON, ("new vColorSpace=%d, vDepth:%d", tmpCfg->runtime.vColorSpace, tmpCfg->runtime.vDepth));
	}

	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_COLOR_DEPTH, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.vDepth) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
		EXT_DEBUGF(EXT_DBG_OFF, ("new vDepth=%d, value:%s", tmpCfg->runtime.vDepth, value));
	}

	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_AUDIO_CHANNEL, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.aChannels) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
		EXT_DEBUGF(EXT_DBG_OFF, ("new aChannels=%d"EXT_NEW_LINE, tmpCfg->runtime.aChannels));
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_AUDIO_SAMP_RATE, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.aSampleRate) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
		EXT_DEBUGF(EXT_DBG_OFF, ("new aSampleRate=%d; aChannels=%d"EXT_NEW_LINE, tmpCfg->runtime.aSampleRate, tmpCfg->runtime.aChannels));
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_AUDIO_PKT_SIZE, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.aPktSize) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
		EXT_DEBUGF(EXT_DBG_OFF, ("new aPktSize=%d; aChannels=%d"EXT_NEW_LINE, tmpCfg->runtime.aPktSize, tmpCfg->runtime.aChannels));
	}

	/* network update */
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_IS_DHCP, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &charVal) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate integer for '%s'", value, key);
			return EXIT_FAILURE;
		}
		
		EXT_CFG_SET_DHCP(tmpCfg, (charVal==0)?EXT_FALSE:EXT_TRUE);
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_ADDRESS, strlen(key)))
	{
		if(cmnUtilsParseIp(value, &tmpCfg->local.ip) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate IP address for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_NETMASK, strlen(key)))
	{
		if(cmnUtilsParseIp(value, &tmpCfg->ipMask) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate IP address for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_GATEWAY, strlen(key)))
	{
		if(cmnUtilsParseIp(value, &tmpCfg->ipGateway) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate IP address for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}

	/* RS232 update */
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_RS232_BAUDRATE, strlen(key)))
	{
		if(cmnUtilsParseInt32(value, &intVal) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate integer for '%s'", value, key);
			return EXIT_FAILURE;
		}

		if( CHECK_BAUDRATE(intVal) )
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate baudrate for '%s'", value, key);
			return EXIT_FAILURE;
		}
		else
		{
			tmpCfg->rs232Cfg.baudRate = intVal;
		}

	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_RS232_DATABITS, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &charVal) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate integer for '%s'", value, key);
			return EXIT_FAILURE;
		}
		
		if( CHECK_DATABITS(charVal) )
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate databit for '%s'", value, key);
			return EXIT_FAILURE;
		}
		else
		{
			tmpCfg->rs232Cfg.charLength = charVal;
		}
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_RS232_PARITY, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &charVal) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate integer for '%s'", value, key);
			return EXIT_FAILURE;
		}
		tmpCfg->rs232Cfg.parityType = charVal;
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_RS232_STOPBITS, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &charVal) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate integer for '%s'", value, key);
			return EXIT_FAILURE;
		}
		tmpCfg->rs232Cfg.stopbits = charVal;
	}

	else
	{
		EXT_INFOF(("Key '%s' and value '%s' is not support now", key,  value));
	}


	return EXIT_SUCCESS;
}

