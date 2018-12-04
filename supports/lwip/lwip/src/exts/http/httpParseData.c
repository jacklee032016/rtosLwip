/*
* parse data in HTTP request, for HTML form data and data from REST API
*/


#include "lwipExt.h"

#include "http.h"

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

	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_SDP_VEDIO_URI, strlen(key)))
	{
		snprintf(tmpCfg->sdpUriVideo.uri, sizeof(tmpCfg->sdpUriVideo.uri), "%s", value);
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_SDP_AUDIO_URI, strlen(key)))
	{
		snprintf(tmpCfg->sdpUriAudio.uri, sizeof(tmpCfg->sdpUriAudio.uri), "%s", value);
	}

	else
	{
		EXT_INFOF(("Key '%s' and value '%s' is not support now", key,  value));
	}


	return EXIT_SUCCESS;
}


/* parse data from form of setting web page */
char extHttpParseData(ExtHttpConn *ehc, EXT_RUNTIME_CFG *tmpCfg, char *key, char *value)
{
	EXT_DEBUGF(EXT_DBG_ON, ("new vDepth=%d, value:%s", tmpCfg->runtime.vDepth, value));

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
			tmpCfg->fpgaAuto = EXT_TRUE;
		}
		else
		{
			tmpCfg->fpgaAuto = EXT_FALSE;
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

	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_COLOR_SPACE, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.vColorSpace) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
		EXT_DEBUGF(EXT_DBG_ON, ("new vColorSpace=%d", tmpCfg->runtime.vColorSpace));
	}

	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_COLOR_DEPTH, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.vDepth) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
		EXT_DEBUGF(EXT_DBG_ON, ("new vDepth=%d, value:%s", tmpCfg->runtime.vDepth, value));
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_FRAME_RATE, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.vFrameRate) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
		EXT_DEBUGF(EXT_DBG_ON, ("new vFrameRate=%d"EXT_NEW_LINE, tmpCfg->runtime.vFrameRate));
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_VIDEO_INTERLACE, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.vIsSegmented) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
		if(tmpCfg->runtime.vIsInterlaced != EXT_VIDEO_INTLC_PROGRESSIVE && tmpCfg->runtime.vIsInterlaced != EXT_VIDEO_INTLC_INTERLACED)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			EXT_ERRORF(("'%s' is not validate interlaced for '%s'", value, key));
			tmpCfg->runtime.vIsInterlaced = EXT_VIDEO_INTLC_INTERLACED;
		}
	}

	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_AUDIO_CHANNEL, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.aChannels) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
		EXT_DEBUGF(EXT_DBG_ON, ("new aChannels=%d"EXT_NEW_LINE, tmpCfg->runtime.aChannels));
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_AUDIO_SAMP_RATE, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.aSampleRate) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
		EXT_DEBUGF(EXT_DBG_ON, ("new aSampleRate=%d"EXT_NEW_LINE, tmpCfg->runtime.aSampleRate));
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_AUDIO_PKT_SIZE, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.aPktSize) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
		EXT_DEBUGF(EXT_DBG_ON, ("new aPktSize=%d"EXT_NEW_LINE, tmpCfg->runtime.aPktSize));
	}
	else
	{
		EXT_INFOF(("Key '%s' and value '%s' is not support now", key,  value));
	}


	return EXIT_SUCCESS;
}

