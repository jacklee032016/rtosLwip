
#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"
#include "httpClient.h"

/* section 5 of RFC4566*/
#define	SDP_VERSION		"0"

#define	SDP_USER_NAME		"-"	/*host which not support user ID */

#define	SDP_NET_TYPE		"IN"
#define	SDP_ADDR_TYPE		"IP4"

/* start/stop are 0, session is permanent */
#define	SDP_TIME_START		"0"
#define	SDP_TIME_STOP		"0"

#define	SDP_MEDIA_VIDEO							"m=video"
#define	SDP_MEDIA_AUDIO							"m=audio"

#define	SDP_MEDIA_RTP_MAP							"a=rtpmap:"

#define	SDP_MEDIA_FORMAP_PARAMS					"a=fmtp:"


#define	SDP_MEDIA_TEXT							"text"
#define	SDP_MEDIA_APPLICATION						"application"
#define	SDP_MEDIA_MESSAGE						"message"

#define	SDP_MEDIA_PROTOCOL_AVP					"RTP/AVP"

/* 2110 definitions */
#define	SDP_2110_VIDEO_FORMAT					"raw/90000"
#define	SDP_2110_PM_GENERAL						"2110GPM"	/* general packing mode */
#define	SDP_2110_PM_BLOCK						"2110BPM"	/* general packing mode */

#define	SDP_2110_VIDEO_INTERLACE					"interlace"
#define	SDP_2110_VIDEO_SEGMENTED					"segmented"

#define	SDP_2110_VKEY_SAMPLING					"sampling"
#define	SDP_2110_VKEY_WIDTH						"width"
#define	SDP_2110_VKEY_HEIGHT						"height"
#define	SDP_2110_VKEY_FRAME_RATE					"exactframerate"
#define	SDP_2110_VKEY_DEPTH						"depth"
#define	SDP_2110_VKEY_TCS							"TCS"		/* Transfer Characteritic System */
#define	SDP_2110_VKEY_COLORIMETRY				"colorimetry"

#define	SDP_2110_VKEY_PM							"PM"				/* Packing Mode */
#define	SDP_2110_VKEY_SSN							"SSN"			/* SMPTE Standard Number  */
#define	SDP_2110_VKEY_TP							"TP"  			/*  TP=2110TPN; is not defined in specs */


/* private definitions of SDP */
#define	SDP_P_SESSION_VERSION						1

#define	SDP_P_MEDIA_FORMAT_VIDEO				96
#define	SDP_P_MEDIA_FORMAT_AUDIO				100

#define	SDP_P_TTL									64

/* p.21 of AES67-2015 */
#define	SDP_P_AUDIO_DEPTH							"L24"
#define	SDP_P_AUDIO_SAMPLE_RATE					48000
#define	SDP_P_AUDIO_CHANNELS						16

#define	SDP_P_AUDIO_PACK_TIME_125MS				"0.12"


static uint16_t _extSdpSessionDescription(ExtHttpConn  *ehc, char *dataBuf, uint16_t size, char isVideo)
{
	int index = 0;
	EXT_RUNTIME_CFG	*runCfg = ehc->runCfg;
	struct netif *_netif = (struct netif *)runCfg->netif;

	/* version */
	CMN_SN_PRINTF(dataBuf, size, index, "v="SDP_VERSION  EXT_NEW_LINE ); 
	/* originator */
	/* session ID: 1|0(TX|RX)+1|0(VIDEO|AUDIO)+current_time() */
	CMN_SN_PRINTF(dataBuf, size, index, "o="SDP_USER_NAME" %d%d%"FOR_U32" %d "SDP_NET_TYPE" "SDP_ADDR_TYPE" %s" EXT_NEW_LINE,
		(EXT_IS_TX(runCfg))?1:0, (isVideo)?1:0, sys_now(), SDP_P_SESSION_VERSION, inet_ntoa(*(struct in_addr *)&(_netif->ip_addr))  ); 

	/* session name */
	CMN_SN_PRINTF(dataBuf, size, index, "s=MuxLab %s 2110"EXT_NEW_LINE, runCfg->name );
	/**/
	CMN_SN_PRINTF(dataBuf, size, index, "t="SDP_TIME_START" "SDP_TIME_STOP EXT_NEW_LINE);

	return index;
}

uint16_t extHttpSdpVideo(ExtHttpConn  *ehc, void *pageHandle)
{
	int index = 0;
	EXT_RUNTIME_CFG	*runCfg = ehc->runCfg;

	char *dataBuf = (char *)ehc->data+ehc->headerLength;
	uint16_t size = sizeof(ehc->data) - ehc->headerLength;

	index = _extSdpSessionDescription(ehc, dataBuf, size, EXT_TRUE);

	/**** media stream description ******/
	/* media description*/
	CMN_SN_PRINTF(dataBuf, size, index, SDP_MEDIA_VIDEO" %d "SDP_MEDIA_PROTOCOL_AVP" %d" EXT_NEW_LINE, runCfg->dest.vport, SDP_P_MEDIA_FORMAT_VIDEO);

	/*connection */
	CMN_SN_PRINTF(dataBuf, size, index, "c="SDP_NET_TYPE" "SDP_ADDR_TYPE" %s/%d" EXT_NEW_LINE, EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.ip)), SDP_P_TTL );

	/* RTP map attribute */
	CMN_SN_PRINTF(dataBuf, size, index, SDP_MEDIA_RTP_MAP"%d "SDP_2110_VIDEO_FORMAT EXT_NEW_LINE, SDP_P_MEDIA_FORMAT_VIDEO);

	/* format parameters 'fmtp', page 15~20, specs 2110-20:2017 */
	CMN_SN_PRINTF(dataBuf, size, index, SDP_MEDIA_FORMAP_PARAMS"%d "SDP_2110_VKEY_SAMPLING"=%s; "SDP_2110_VKEY_WIDTH"=%d; "SDP_2110_VKEY_HEIGHT"=%d; "SDP_2110_VKEY_FRAME_RATE"=%s; ",
		SDP_P_MEDIA_FORMAT_VIDEO, CMN_FIND_V_COLORSPACE(runCfg->runtime.vColorSpace), runCfg->runtime.vWidth, runCfg->runtime.vHeight, 
		CMN_FIND_V_FRAME_RATE(runCfg->runtime.vFrameRate) );

	/* PM: Packing Mode; TCP: Transfer Characteritic System; SSN: SMPTE Standard Number */
	/*  TP=2110TPN; is not defined in specs */
	CMN_SN_PRINTF(dataBuf, size, index, SDP_2110_VKEY_DEPTH"=%d; "SDP_2110_VKEY_TCS"=SDR; "SDP_2110_VKEY_COLORIMETRY"=BT709; "SDP_2110_VKEY_PM"=%s; "SDP_2110_VKEY_SSN"=ST2110-20:2017;",
		runCfg->runtime.vDepth, SDP_2110_PM_BLOCK );
	if(runCfg->runtime.vIsInterlaced )
	{/* p.17 in specs */
		CMN_SN_PRINTF(dataBuf, size, index, " %s;", SDP_2110_VIDEO_INTERLACE);
		if(runCfg->runtime.vIsSegmented )
		{
			CMN_SN_PRINTF(dataBuf, size, index, " %s;", SDP_2110_VIDEO_SEGMENTED);
		}
	}
	CMN_SN_PRINTF(dataBuf, size, index, ""EXT_NEW_LINE);

	/* time and clock. see specs 2110-10  */
//	CMN_SN_PRINTF(dataBuf, size, index, "a=ts-refclk:ptp=IEEE1588-2008:"EXT_NEW_LINE);
	/* not support PTP now */
	CMN_SN_PRINTF(dataBuf, size, index, "a=ts-refclk:localmac=%02x-%02x-%02x-%02x-%02x-%02x"EXT_NEW_LINE, 
		runCfg->local.mac.address[0], runCfg->local.mac.address[1], runCfg->local.mac.address[2], 
		runCfg->local.mac.address[3], runCfg->local.mac.address[4], runCfg->local.mac.address[5]);

	/* RFC7273 clause 5.2 and p.22 of AES67-2015; offset between media clock and RTP clock */
	CMN_SN_PRINTF(dataBuf, size, index, "a=mediaclk:direct=0"EXT_NEW_LINE);

	CMN_SN_PRINTF(dataBuf, size, index, "a=mid:VID"EXT_NEW_LINE);

	ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	return index;
}


uint16_t extHttpSdpAudio(ExtHttpConn  *ehc, void *pageHandle)
{
	int index = 0;
	EXT_RUNTIME_CFG	*runCfg = ehc->runCfg;
	char *dataBuf = (char *)ehc->data+ehc->headerLength;
	uint16_t size = sizeof(ehc->data) - ehc->headerLength;

	index = _extSdpSessionDescription(ehc, dataBuf, size, EXT_FALSE);

	/* media stream */
	CMN_SN_PRINTF(dataBuf, size, index, SDP_MEDIA_AUDIO" %d "SDP_MEDIA_PROTOCOL_AVP" %d" EXT_NEW_LINE, 
		runCfg->dest.aport, SDP_P_MEDIA_FORMAT_AUDIO);

	/* connection */
	CMN_SN_PRINTF(dataBuf, size, index, "c="SDP_NET_TYPE" "SDP_ADDR_TYPE" %s/%d" EXT_NEW_LINE , EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.audioIp)), SDP_P_TTL);

	/* attribute of RTP map */
	CMN_SN_PRINTF(dataBuf, size, index, SDP_MEDIA_RTP_MAP":%d %s/%d/%d"EXT_NEW_LINE, 
		SDP_P_MEDIA_FORMAT_AUDIO, SDP_P_AUDIO_DEPTH, SDP_P_AUDIO_SAMPLE_RATE, SDP_P_AUDIO_CHANNELS);

	/* channel order: see specs 2110-30 */
	CMN_SN_PRINTF(dataBuf, size, index, SDP_MEDIA_FORMAP_PARAMS"%d channel-order=SMPTE2110.(SGRP,SGRP,SGRP,SGRP)" EXT_NEW_LINE, SDP_P_MEDIA_FORMAT_AUDIO);
	/* packing time. table 4 in p.21  */
	CMN_SN_PRINTF(dataBuf, size, index, "a=ptime:%s"EXT_NEW_LINE, SDP_P_AUDIO_PACK_TIME_125MS );

	/* clock */
//	CMN_SN_PRINTF(dataBuf, size, index, "a=ts-refclk:ptp=IEEE1588-2008:"EXT_NEW_LINE);
	/* not support PTP now */
	CMN_SN_PRINTF(dataBuf, size, index, "a=ts-refclk:localmac=%02x-%02x-%02x-%02x-%02x-%02x"EXT_NEW_LINE, 
		runCfg->local.mac.address[0], runCfg->local.mac.address[1], runCfg->local.mac.address[2], 
		runCfg->local.mac.address[3], runCfg->local.mac.address[4], runCfg->local.mac.address[5]);

	/* offset between media and RTP */
	CMN_SN_PRINTF(dataBuf, size, index, "a=mediaclk:direct=0"EXT_NEW_LINE);
	
	CMN_SN_PRINTF(dataBuf, size, index, "a=a=mid:AUD"EXT_NEW_LINE);

	ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	return index;
}


uint16_t  extHttpSimpleRestApi(ExtHttpConn  *ehc, void *pageHandle)
{
	int index = 0;
	EXT_RUNTIME_CFG	*runCfg = ehc->runCfg;
	struct netif *_netif = (struct netif *)runCfg->netif;
	
	char *dataBuf = (char *)ehc->data+ehc->responseHeaderLength;
	uint16_t size = sizeof(ehc->data) - ehc->responseHeaderLength;

	EXT_RUNTIME_CFG *rxCfg = &tmpRuntime;
#ifdef	ARM
	extFpgaReadParams(runCfg);
#endif


	if (HTTP_IS_POST(ehc) )
	{
		EXT_DEBUGF(EXT_DBG_OFF, ("Data %d bytes: '%.*s'", ehc->leftData,   ehc->leftData, ehc->headers+ehc->headerLength+__HTTP_CRLF_SIZE)  );
		if(cmnHttpParseRestJson(rxCfg, ehc->headers+ehc->headerLength+__HTTP_CRLF_SIZE, ehc->leftData) == EXIT_FAILURE)
		{
			cmnHttpRestError(ehc, WEB_RES_ERROR, "Parameter wrong");
			return ehc->contentLength;
		}

		extSysCompareParams(ehc->runCfg, rxCfg);
		extSysConfigCtrl(ehc->runCfg, rxCfg);

	}


	CMN_SN_PRINTF(dataBuf, size, index, "{"EXT_NEW_LINE);

	/* resource core fields */
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_MODEL"\":\"%s\","EXT_NEW_LINE, runCfg->model);
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_PRODUCT"\":\"%s\","EXT_NEW_LINE, runCfg->name);
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_VERSION"\":\"%02d.%02d-%02d\","EXT_NEW_LINE, runCfg->version.major, runCfg->version.minor, runCfg->version.revision);

	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_ADDRESS"\":\"%s\","EXT_NEW_LINE, inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)) );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_GATEWAY"\":\"%s\","EXT_NEW_LINE, EXT_LWIP_IPADD_TO_STR(&(runCfg->ipGateway)) );
	
//	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_IS_DHCP"\":\"%s\",", ((_netif->ip_addr.u_addr.ip4.addr) ==runCfg->local.ip)?"YES":"NO");
//	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_IS_DHCP"\":\"%s\","EXT_NEW_LINE, ((_netif->ip_addr.addr) ==runCfg->local.ip)?"YES":"NO");
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_IS_DHCP"\":%d,"EXT_NEW_LINE, (!((_netif->ip_addr.addr) ==runCfg->local.ip)) );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_MAC"\":\"%02x:%02x:%02x:%02x:%02x:%02x\","EXT_NEW_LINE, 
		runCfg->local.mac.address[0], runCfg->local.mac.address[1], runCfg->local.mac.address[2], runCfg->local.mac.address[3], runCfg->local.mac.address[4], runCfg->local.mac.address[5]);

	if(EXT_IS_TX(runCfg))
	{
		CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_SDP_VEDIO_IP"\":\"%s\","EXT_NEW_LINE,  inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)) );
		CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_SDP_VEDIO_PORT"\":%d,"EXT_NEW_LINE, EXT_SDP_SVR_PORT);

		CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_SDP_AUDIO_IP"\":\"%s\","EXT_NEW_LINE,  inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)) );
		CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_SDP_AUDIO_PORT"\":%d,"EXT_NEW_LINE,  EXT_SDP_SVR_PORT);
	}
	else
	{
		CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_SDP_VEDIO_IP"\":\"%s\","EXT_NEW_LINE,  inet_ntoa(*(struct in_addr *)&(runCfg->sdpUriVideo.ip)) );
		CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_SDP_VEDIO_PORT"\":%d,"EXT_NEW_LINE, runCfg->sdpUriVideo.port );

		CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_SDP_AUDIO_IP"\":\"%s\","EXT_NEW_LINE,  inet_ntoa(*(struct in_addr *)&(runCfg->sdpUriAudio.ip)) );
		CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_SDP_AUDIO_PORT"\":%d,"EXT_NEW_LINE,  runCfg->sdpUriAudio.port );
	}
		
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_SDP_VEDIO_URI"\":\"%s\","EXT_NEW_LINE, runCfg->sdpUriVideo.uri);
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_SDP_AUDIO_URI"\":\"%s\","EXT_NEW_LINE, runCfg->sdpUriVideo.uri);

	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_IP_VEDIO"\":\"%s\","EXT_NEW_LINE, EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.ip)) );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_IP_AUDIO"\":\"%s\","EXT_NEW_LINE, EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.audioIp)) );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_IP_ANC"\":\"%s\","EXT_NEW_LINE, EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.ancIp)) );

	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_PORT_VEDIO"\":%d,"EXT_NEW_LINE, runCfg->dest.vport );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_PORT_AUDIO"\":%d,"EXT_NEW_LINE, runCfg->dest.aport );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_PORT_DATA"\":%d,"EXT_NEW_LINE, runCfg->dest.dport );
#if EXT_FPGA_AUX_ON	
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_PORT_STREM"\":%d,"EXT_NEW_LINE, runCfg->dest.sport );
#endif

	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_FPGA_AUTO"\":%d,"EXT_NEW_LINE, runCfg->fpgaAuto );

	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_VIDEO_HEIGHT"\":%d,"EXT_NEW_LINE, runCfg->runtime.vHeight);
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_VIDEO_WIDTH"\":%d,"EXT_NEW_LINE, runCfg->runtime.vWidth);

	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_FRAME_RATE"\":%d,"EXT_NEW_LINE, CMN_INT_FIND_NAME_V_FPS(runCfg->runtime.vFrameRate) );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_COLOR_SPACE"\":\"%s\","EXT_NEW_LINE, CMN_FIND_V_COLORSPACE(runCfg->runtime.vColorSpace) );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_COLOR_DEPTH"\":%d,"EXT_NEW_LINE, CMN_INT_FIND_NAME_V_DEPTH(runCfg->runtime.vDepth) );

#if 0
	index += snprintf(dataBuf+index, size-index, "\""EXT_IPCMD_DATA_VIDEO_INTERLACED"\":%d,", (parser->runCfg->runtime.vIsInterlaced)?1:0);
	index += snprintf(dataBuf+index, size-index, "\""EXT_IPCMD_DATA_VIDEO_SEGMENTED"\":%d,", (parser->runCfg->runtime.vIsSegmented)?1:0);
#endif	
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_VIDEO_INTERLACE"\":%d,"EXT_NEW_LINE, runCfg->runtime.vIsInterlaced);
//	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_VIDEO_SEGMENTED"\":%d,"EXT_NEW_LINE, runCfg->runtime.vIsSegmented);

//	EXT_DEBUGF(EXT_DBG_OFF, ("aSampleRate: %d", runCfg->runtime.aSampleRate));

	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_AUDIO_SAMP_RATE"\":\"%s\","EXT_NEW_LINE, CMN_FIND_A_RATE(runCfg->runtime.aSampleRate) );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_AUDIO_CHANNEL"\":%d,"EXT_NEW_LINE, runCfg->runtime.aChannels );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_AUDIO_DEPTH"\":%d,"EXT_NEW_LINE, runCfg->runtime.aDepth );

	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_AUDIO_PKT_SIZE"\":\"%s\""EXT_NEW_LINE,CMN_FIND_A_PKTSIZE(runCfg->runtime.aPktSize) );
	
//	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_IS_CONNECT"\":%d"EXT_NEW_LINE, runCfg->runtime.isConnect);
	
	CMN_SN_PRINTF(dataBuf, size, index, "}"EXT_NEW_LINE);

	ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	
	return index;
}

/* first field for this media stream */
static uint16_t _sdpParsePort(char *data, uint16_t size, const char *mediaKey, uint16_t *port)
{
	char *p;
	
	p= lwip_strnstr(data, mediaKey, size);
	if(p == NULL)
	{
		return 0;
	}
	p += strlen(mediaKey);
	while(*p == ' ')
	{
		p++;
	}

	if(cmnUtilsParseInt16(p, port) == EXIT_FAILURE)
	{
		EXT_ERRORF(("No port number for SDP %s stream", mediaKey));
		*port = -1;
		return p-data;
	}

	return p-data;
}

static err_t _sdpParseIp(char *data, uint16_t size, uint32_t *ip)
{
	int index = 0;
	char *p, *pnext;
	
	p = lwip_strnstr(data+index, SDP_ADDR_TYPE, size - index);
	if(p== NULL)
	{
		*ip = IPADDR_NONE;	
		EXT_ERRORF(("Invalidate format '"SDP_ADDR_TYPE"' for SDP stream"));
		return ERR_ARG;
	}
	
	p += strlen(SDP_ADDR_TYPE);
	while(*p == ' ')
	{
		p++;
	}
	index= p -data;
	
	pnext = lwip_strnstr(data+index, "/", size - index);
	if(pnext== NULL)
	{
		*ip = IPADDR_NONE;	
		EXT_ERRORF(("Invalidate format '"SDP_ADDR_TYPE"' for SDP stream"));
		return ERR_ARG;
	}
	*pnext = '\0';

	if(cmnUtilsParseIp(p, ip) == EXIT_FAILURE)
	{
		*ip = IPADDR_NONE;	
		EXT_ERRORF(("No IP address for SDP stream"));
//		return ERR_ARG;
	}
	p = pnext+1;

	return p - data;
}



static uint16_t _sdpParseAudioStream(HttpClient *hc, EXT_RUNTIME_CFG	*rxCfg, char *data, uint16_t size)
{
	uint16_t  index = 0, _shVal;
	char *p, *pnext;
	
	p = lwip_strnstr(data+index, SDP_MEDIA_RTP_MAP, size - index);
	if(p== NULL)
	{
		EXT_ERRORF(("Invalidate format '"SDP_ADDR_TYPE"' for SDP stream"));
		return 0;
	}
	
	index= p -data;

	p = lwip_strnstr(data+index, "L", size - index);
	if(p== NULL)
	{
		rxCfg->runtime.aDepth = -1;
		EXT_ERRORF(("Invalidate format '"SDP_ADDR_TYPE"' for SDP stream"));
		return 0;
	}
	p++;
	
	index = p -data;
	pnext = lwip_strnstr(data+index, "/", size - index);
	if(pnext== NULL)
	{
		rxCfg->runtime.aDepth = -1;
		EXT_ERRORF(("Invalidate format '"SDP_ADDR_TYPE"' for SDP stream"));
		return 0;
	}
	*pnext = '\0';

	if(cmnUtilsParseInt8(p, &rxCfg->runtime.aDepth) == EXIT_FAILURE)
	{
		EXT_ERRORF(("No depth for SDP audio stream"));
		rxCfg->runtime.aDepth = -1;
		return 0;
	}

	/* sample freq */
	p = pnext+1;
	index = p -data;
	pnext = lwip_strnstr(data+index, "/", size - index);
	if(pnext== NULL)
	{
		rxCfg->runtime.aDepth = -1;
		EXT_ERRORF(("Invalidate format for SDP audio stream"));
		return 0;
	}
	*pnext = '\0';

	if(cmnUtilsParseInt16(p, &_shVal) == EXIT_FAILURE)
	{
		EXT_ERRORF(("No Sample Freq for SDP audio stream") );
		rxCfg->runtime.aSampleRate = EXT_A_RATE_48K;
		return p-data;
	}
	else
	{
		if(_shVal == 48000)
		{
			rxCfg->runtime.aSampleRate = EXT_A_RATE_48K;
		}
		else if(_shVal == 44100)
		{
			rxCfg->runtime.aSampleRate = EXT_A_RATE_44K;
		}
		else if(_shVal == 96000)
		{
			rxCfg->runtime.aSampleRate = EXT_A_RATE_96K;
		}
		else 
		{
			EXT_ERRORF(("Not support Sample Freq for SDP audio stream '%d'", _shVal) );
			rxCfg->runtime.aSampleRate = EXT_A_RATE_48K;
		}
	}
	
	p = pnext+1;
	if(cmnUtilsParseInt8(p, &rxCfg->runtime.aChannels) == EXIT_FAILURE)
	{
		EXT_ERRORF(("No channel number for SDP audio stream"));
		rxCfg->runtime.aChannels = -1;
		return p-data;
	}


	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME"Parsed SDP Audio params:IP:%s; Port:%d; Depth:%d; Sample Freq:%d; Channels:%d",
			EXT_LWIP_IPADD_TO_STR(&rxCfg->dest.audioIp), rxCfg->dest.aport, rxCfg->runtime.aDepth, rxCfg->runtime.aSampleRate, rxCfg->runtime.aChannels ));
	
	return ERR_OK;
}


/* data from HTTP form for SDP client */
static char __sdpVideoParams(EXT_RUNTIME_CFG *rxCfg, char *key, char *value)
{
//	uint8_t colorspace;
	/* string change into integer */
	if( lwip_strnstr(key, SDP_2110_VKEY_SAMPLING, strlen(key)))
	{
		rxCfg->runtime.vColorSpace = CMN_FIND_STR_V_COLORSPACE(value);
		if(rxCfg->runtime.vColorSpace == -1)
		{
			EXT_ERRORF(("Not support colorspace '%s' for SDP video stream", value));
			return EXIT_FAILURE;
		}
	}
	else if( lwip_strnstr(key, SDP_2110_VKEY_FRAME_RATE, strlen(key)))
	{
		rxCfg->runtime.vFrameRate = CMN_FIND_STR_V_FRAME_RATE(value);
		if(rxCfg->runtime.vFrameRate == -1)
		{
			EXT_ERRORF(("Not support frame rate '%s' for SDP video stream", value));
			return EXIT_FAILURE;
		}
	}

	/* integer directly */
	else if( lwip_strnstr(key, SDP_2110_VKEY_WIDTH, strlen(key)))
	{
		if(cmnUtilsParseInt16(value, &rxCfg->runtime.vWidth) == EXIT_FAILURE)
		{
			EXT_ERRORF(("No validate width for SDP video stream"));
			return EXIT_FAILURE;
		}
	}
	else if( lwip_strnstr(key, SDP_2110_VKEY_HEIGHT, strlen(key)))
	{
		if(cmnUtilsParseInt16(value, &rxCfg->runtime.vHeight) == EXIT_FAILURE)
		{
			EXT_ERRORF(("No validate height for SDP video stream"));
			return EXIT_FAILURE;
		}
	}

	else if( lwip_strnstr(key, SDP_2110_VKEY_DEPTH, strlen(key)))
	{
		if(cmnUtilsParseInt8(value, &rxCfg->runtime.vDepth) == EXIT_FAILURE)
		{
			EXT_ERRORF(("No validate depth for SDP video stream"));
			return EXIT_FAILURE;
		}
	}
	else
	{
		EXT_INFOF(("Key '%s' and value '%s' is not support now", key,  value));
	}

	return EXIT_SUCCESS;
}


static uint16_t __parseSdpKeyValue(EXT_RUNTIME_CFG *rxCfg, char *data, uint16_t size)
{
#define	_SDP_CHAR_SEPERATE			"; "

	char *key, *value, *nextKey;
	int i = 0;
	uint16_t left = size;

	data[left] = 0;
	key = data;
	while(key)
	{
		value = lwip_strnstr(key, _CHAR_EQUAL, left );
		if(value == NULL)
		{
			EXT_INFOF(("Invalidate key/value (%s) for SDP stream, on position %d", key, size-left));
			return size - left;
		}
		key[value-key] = 0;
		value++;
		
		nextKey = lwip_strnstr(value, _SDP_CHAR_SEPERATE,  left - (value-key)) ;
		if(nextKey)
		{
			value[nextKey-value] = 0;
		}
		else
		{
			value[left-(value-key)] = 0;
		}
		printf("No#%d: '%s' = '%s'" EXT_NEW_LINE, ++i, key, value );

		if(__sdpVideoParams(rxCfg, key, value) == EXIT_FAILURE)
		{
			EXT_ERRORF(("Invalidate key/value '%s/%s' for SDP video stream", key, value ));
			return size - left;
		}
			
		if(nextKey)
		{
			nextKey++;
			left = left - (nextKey-key);
		}
		
		key = nextKey;
	}

	return size - left;
}

static err_t _sdpParseVideoStream(HttpClient *hc, EXT_RUNTIME_CFG	*rxCfg, char *data, uint16_t size)
{
	uint16_t  index = 0;
	uint16_t length = 0, left;
	char *p, *pnext;
	
	p = lwip_strnstr(data+index, SDP_MEDIA_FORMAP_PARAMS, size - index);
	if(p== NULL)
	{
		EXT_ERRORF(("Invalidate format for SDP stream"));
		return ERR_ARG;
	}
	p += strlen(SDP_MEDIA_FORMAP_PARAMS);
	while(*p != ' ')/* for rtpmap constants */
	{
		p++;
	}
	while(*p == ' ')/* spaces before first key */
	{
		p++;
	}
	
	index= p -data;
	
	pnext = lwip_strnstr(data+index, "\r", size - index);
	length = pnext - p + 1;

	left = __parseSdpKeyValue(rxCfg, data+index, length);
//	printf("left:%d\r\n", left);
	if(left > 0)
	{
		p = lwip_strnstr(data+index+left, SDP_2110_VIDEO_INTERLACE, (length-left) );
		if( p )
		{
			rxCfg->runtime.vIsInterlaced = 1;
			p = lwip_strnstr(data+index+left, SDP_2110_VIDEO_SEGMENTED, (length-left) );
			if(p )
			{
				rxCfg->runtime.vIsSegmented = 1;
			}
		}
	}

	EXT_DEBUGF(HTTP_CLIENT_DEBUG, (HTTP_CLIENT_NAME"Parsed SDP Video params:IP:%s; Port:%d; ColorSpace:%s; width:%d; height:%d; framerate:%s; depth:%d; isInterlace:%s; isSegmented:%s",
			EXT_LWIP_IPADD_TO_STR(&rxCfg->dest.ip), rxCfg->dest.vport, CMN_FIND_V_COLORSPACE(rxCfg->runtime.vColorSpace),  
			rxCfg->runtime.vWidth, rxCfg->runtime.vHeight, CMN_FIND_V_FRAME_RATE(rxCfg->runtime.vFrameRate), rxCfg->runtime.vDepth,
			(rxCfg->runtime.vIsInterlaced)?"YES":"NO", (rxCfg->runtime.vIsSegmented)?"YES":"NO"));
	
	return ERR_OK;
}


err_t extHttpSdpParse(HttpClient *hc, EXT_RUNTIME_CFG	*rxCfg, char *data, uint16_t size)
{
#define	_SDP_TYPE_UNKNOWN		0
#define	_SDP_TYPE_VIDEO			1
#define	_SDP_TYPE_AUDIO			2
	int index = 0;
	uint16_t 	port;
	uint32_t 	ip;
	char type = _SDP_TYPE_UNKNOWN;
	
	index = _sdpParsePort(data, size, SDP_MEDIA_VIDEO, &port);
	if(index == 0 || port == -1)
	{
		index = _sdpParsePort(data, size, SDP_MEDIA_AUDIO, &port);
		if(index != 0 && port != -1)
		{
			type = _SDP_TYPE_AUDIO;
		}
	}
	else
	{
		type = _SDP_TYPE_VIDEO;
	}
	
	if(type == _SDP_TYPE_UNKNOWN )
	{
		snprintf(hc->msg, sizeof(hc->msg), "No media stream is found" );
		EXT_ERRORF(("No media stream is found"));
		return ERR_ARG;
	}

	index += _sdpParseIp(data+index, size-index, &ip);
	if(ip == IPADDR_NONE)
	{
		snprintf(hc->msg, sizeof(hc->msg), "No address for %s stream", (type==_SDP_TYPE_AUDIO)?"audio":"video" );
		EXT_ERRORF(("No IP address for SDP %s stream", (type==_SDP_TYPE_AUDIO)?"audio":"video"));
		return ERR_ARG;
	}

	if(type == _SDP_TYPE_AUDIO)
	{
		rxCfg->dest.audioIp = ip;
		rxCfg->dest.aport = port;
		_sdpParseAudioStream(hc, rxCfg, data+index, size-index);
	}
	else
	{
		rxCfg->dest.ip = ip;
		rxCfg->dest.vport = port;
		_sdpParseVideoStream(hc,rxCfg, data+index, size-index);
	}

	return ERR_OK;
}

