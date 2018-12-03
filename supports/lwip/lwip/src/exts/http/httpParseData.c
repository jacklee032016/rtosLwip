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

void extHttpDebugData(EXT_RUNTIME_CFG *cfg)
{
	EXT_DEBUGF(EXT_DBG_ON, ("Video:Width:%d; height:%d; fps:%d; colorspce:%d; depth=%d, interlaced:%d", cfg->runtime.vWidth, cfg->runtime.vHeight, cfg->runtime.vFrameRate, cfg->runtime.vColorSpace, cfg->runtime.vDepth, cfg->runtime.vIsInterlaced));
	EXT_DEBUGF(EXT_DBG_ON, ("Audio:Chans:%d; depth:%d; pktSize:%d; sampleRate:%d", cfg->runtime.aChannels, cfg->runtime.aDepth, cfg->runtime.aPktSize, cfg->runtime.aSampleRate));
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
	}

	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_COLOR_DEPTH, strlen(key)))
	{
		EXT_DEBUGF(EXT_DBG_ON, ("new vDepth=%d, value:%s", tmpCfg->runtime.vDepth, value));
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
#if 0
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_VIDEO_SEGMENTED, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.vIsSegmented) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
#endif

	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_AUDIO_CHANNEL, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.aChannels) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_AUDIO_SAMP_RATE, strlen(key)))
	{
		if(cmnUtilsParseInt16(value, &tmpCfg->runtime.aSampleRate) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
	else if( lwip_strnstr(key, EXT_WEB_CFG_FIELD_AUDIO_PKT_SIZE, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &tmpCfg->runtime.aPktSize) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate port for '%s'", value, key);
			return EXIT_FAILURE;
		}
	}
	else
	{
		EXT_INFOF(("Key '%s' and value '%s' is not support now", key,  value));
	}


	return EXIT_SUCCESS;
}

