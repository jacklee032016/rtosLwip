
#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"

uint16_t extHttpSdpVideo(ExtHttpConn  *ehc, void *pageHandle)
{
	int index = 0;
	EXT_RUNTIME_CFG	*runCfg = ehc->runCfg;
	struct netif *_netif = (struct netif *)runCfg->netif;

	char *dataBuf = (char *)ehc->data+ehc->headerLength;
	uint16_t size = sizeof(ehc->data) - ehc->headerLength;

	CMN_SN_PRINTF(dataBuf, size, index, "v=0"EXT_NEW_LINE"o=- 833113865 0 IN IP4 %s" EXT_NEW_LINE, inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)) ); 
	CMN_SN_PRINTF(dataBuf, size, index, "s=%s 2110"EXT_NEW_LINE "t=0 0"EXT_NEW_LINE"m=video 1000 RTP/AVP 96" EXT_NEW_LINE, runCfg->model);
	CMN_SN_PRINTF(dataBuf, size, index, "c=IN IP4 %s/64" EXT_NEW_LINE "a=rtpmap:96 raw/90000"EXT_NEW_LINE, EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.ip)));
	CMN_SN_PRINTF(dataBuf, size, index, "a=fmtp:96 sampling=%s; width=%d; height=%d; exactframerate=%d; depth=%d; TCS=SDR; colorimetry=BT709; PM=2110GPM; SSN=ST2110-20:2017; TP=2110TPN;" EXT_NEW_LINE,
		CMN_FIND_V_COLORSPACE(runCfg->runtime.vColorSpace), runCfg->runtime.vWidth, runCfg->runtime.vHeight, runCfg->runtime.vFrameRate, runCfg->runtime.vDepth);
	CMN_SN_PRINTF(dataBuf, size, index, "a=ts-refclk:ptp=IEEE1588-2008:"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, "a=mediaclk:direct=0"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, "a=mid:VID"EXT_NEW_LINE);

	ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	return index;
}


uint16_t extHttpSdpAudio(ExtHttpConn  *ehc, void *pageHandle)
{
	int index = 0;
	EXT_RUNTIME_CFG	*runCfg = ehc->runCfg;
	struct netif *_netif = (struct netif *)runCfg->netif;
	char *dataBuf = (char *)ehc->data+ehc->headerLength;
	uint16_t size = sizeof(ehc->data) - ehc->headerLength;

	CMN_SN_PRINTF(dataBuf, size, index, "v=0"EXT_NEW_LINE"o=- 977740550 0 IN IP4 %s" EXT_NEW_LINE, inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)) ); 
	CMN_SN_PRINTF(dataBuf, size, index, "s=%s 2110"EXT_NEW_LINE "t=0 0"EXT_NEW_LINE"m=audio 1000 RTP/AVP 100" EXT_NEW_LINE, runCfg->model);
	
	CMN_SN_PRINTF(dataBuf, size, index, "c=IN IP4 %s/64" EXT_NEW_LINE "a=rtpmap:100 L24/48000/16"EXT_NEW_LINE, EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.audioIp)));
	CMN_SN_PRINTF(dataBuf, size, index, "a=fmtp:100 channel-order=SMPTE2110.(SGRP,SGRP,SGRP,SGRP)" EXT_NEW_LINE "a=ptime:0.125"EXT_NEW_LINE );
	CMN_SN_PRINTF(dataBuf, size, index, "a=ts-refclk:ptp=IEEE1588-2008:"EXT_NEW_LINE);
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
	char *dataBuf = (char *)ehc->data+ehc->headerLength;
	uint16_t size = sizeof(ehc->data) - ehc->headerLength;

	CMN_SN_PRINTF(dataBuf, size, index, "{"EXT_NEW_LINE);

	/* resource core fields */
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_MODEL"\":\"%s\","EXT_NEW_LINE, runCfg->model);
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_PRODUCT"\":\"%s\","EXT_NEW_LINE, runCfg->name);
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_VERSION"\":\"%02d.%02d-%02d\","EXT_NEW_LINE, runCfg->version.major, runCfg->version.minor, runCfg->version.revision);

	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_ADDRESS"\":\"%s\","EXT_NEW_LINE, inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)) );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_GATEWAY"\":\"%s\","EXT_NEW_LINE, EXT_LWIP_IPADD_TO_STR(&(runCfg->ipGateway)) );
//	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_IS_DHCP"\":\"%s\",", ((_netif->ip_addr.u_addr.ip4.addr) ==runCfg->local.ip)?"YES":"NO");
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_IS_DHCP"\":\"%s\","EXT_NEW_LINE, ((_netif->ip_addr.addr) ==runCfg->local.ip)?"YES":"NO");
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_MAC"\":\"%02x:%02x:%02x:%02x:%02x:%02x\","EXT_NEW_LINE, 
		runCfg->local.mac.address[0], runCfg->local.mac.address[1], runCfg->local.mac.address[2], runCfg->local.mac.address[3], runCfg->local.mac.address[4], runCfg->local.mac.address[5]);

	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_SDP_VEDIO"\":\"http://%s%s\","EXT_NEW_LINE, 
		inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)), EXT_WEBPAGE_SDP_VIDEO );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_SDP_AUDIO"\":\"http://%s%s\","EXT_NEW_LINE, 
		inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)), EXT_WEBPAGE_SDP_AUDIO);

	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_IP_VEDIO"\":\"%s\","EXT_NEW_LINE, EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.ip)) );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_IP_AUDIO"\":\"%s\","EXT_NEW_LINE, EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.audioIp)) );

	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_PORT_VEDIO"\":%d,"EXT_NEW_LINE, runCfg->dest.vport );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_PORT_AUDIO"\":%d,"EXT_NEW_LINE, runCfg->dest.aport );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_PORT_DATA"\":%d,"EXT_NEW_LINE, runCfg->dest.dport );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_PORT_STREM"\":%d,"EXT_NEW_LINE, runCfg->dest.sport );

	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_VIDEO_HEIGHT"\":%d,"EXT_NEW_LINE, runCfg->runtime.vHeight);
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_VIDEO_WIDTH"\":%d,"EXT_NEW_LINE, runCfg->runtime.vWidth);

	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_FRAME_RATE"\":%d,"EXT_NEW_LINE, runCfg->runtime.vFrameRate);
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_COLOR_SPACE"\":\"%s\","EXT_NEW_LINE, CMN_FIND_V_COLORSPACE(runCfg->runtime.vColorSpace) );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_COLOR_DEPTH"\":%d,"EXT_NEW_LINE, runCfg->runtime.vDepth);

#if 0
	index += snprintf(dataBuf+index, size-index, "\""EXT_IPCMD_DATA_VIDEO_INTERLACED"\":%d,", (parser->runCfg->runtime.vIsInterlaced)?1:0);
	index += snprintf(dataBuf+index, size-index, "\""EXT_IPCMD_DATA_VIDEO_SEGMENTED"\":%d,", (parser->runCfg->runtime.vIsSegmented)?1:0);
#endif	
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_VIDEO_SEGMENTED"\":%d,"EXT_NEW_LINE, runCfg->runtime.vIsSegmented);


	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_AUDIO_SAMP_RATE"\":%d,"EXT_NEW_LINE, runCfg->runtime.aSampleRate );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_AUDIO_CHANNEL"\":%d,"EXT_NEW_LINE, runCfg->runtime.aChannels );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_AUDIO_DEPTH"\":%d,"EXT_NEW_LINE, runCfg->runtime.aDepth );
	
	CMN_SN_PRINTF(dataBuf, size, index, "\t\""EXT_WEB_CFG_FIELD_IS_CONNECT"\":%d"EXT_NEW_LINE, runCfg->runtime.isConnect);
	
	CMN_SN_PRINTF(dataBuf, size, index, "}"EXT_NEW_LINE);

	ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	return index;
}

