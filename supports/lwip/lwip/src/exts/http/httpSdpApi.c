
#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"

/* section 5 of RFC4566*/
#define	SDP_VERSION		"0"

#define	SDP_USER_NAME		"-"	/*host which not support user ID */

#define	SDP_NET_TYPE		"IN"
#define	SDP_ADDR_TYPE		"IP4"

/* start/stop are 0, session is permanent */
#define	SDP_TIME_START		"0"
#define	SDP_TIME_STOP		"0"

#define	SDP_MEDIA_VIDEO							"video"
#define	SDP_MEDIA_AUDIO							"audio"
#define	SDP_MEDIA_TEXT							"text"
#define	SDP_MEDIA_APPLICATION						"application"
#define	SDP_MEDIA_MESSAGE						"message"

#define	SDP_MEDIA_PROTOCOL_AVP					"RTP/AVP"

/* 2110 definitions */
#define	SDP_2110_VIDEO_FORMAT					"raw/90000"
#define	SDP_2110_PM_GENERAL						"2110GPM"	/* general packing mode */
#define	SDP_2110_PM_BLOCK						"2110BPM"	/* general packing mode */


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


uint16_t extSdpSessionDescription(ExtHttpConn  *ehc, char *dataBuf, uint16_t size, char isVideo)
{
	int index = 0;
	EXT_RUNTIME_CFG	*runCfg = ehc->runCfg;
	struct netif *_netif = (struct netif *)runCfg->netif;

	/* version */
	CMN_SN_PRINTF(dataBuf, size, index, "v="SDP_VERSION  EXT_NEW_LINE ); 
	/* originator */
	/* session ID: 1|0(TX|RX)+1|0(VIDEO|AUDIO)+current_time() */
	CMN_SN_PRINTF(dataBuf, size, index, "o="SDP_USER_NAME" %d%d%d %d "SDP_NET_TYPE" "SDP_ADDR_TYPE" %s" EXT_NEW_LINE,
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

	index = extSdpSessionDescription(ehc, dataBuf, size, EXT_TRUE);

	/**** media stream description ******/
	/* media description*/
	CMN_SN_PRINTF(dataBuf, size, index, "m="SDP_MEDIA_VIDEO" %d "SDP_MEDIA_PROTOCOL_AVP" %d" EXT_NEW_LINE, runCfg->dest.vport, SDP_P_MEDIA_FORMAT_VIDEO);

	/*connection */
	CMN_SN_PRINTF(dataBuf, size, index, "c="SDP_NET_TYPE" "SDP_ADDR_TYPE" %s/%d" EXT_NEW_LINE, EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.ip)), SDP_P_TTL );

	/* RTP map attribute */
	CMN_SN_PRINTF(dataBuf, size, index, "a=rtpmap:%d "SDP_2110_VIDEO_FORMAT EXT_NEW_LINE, SDP_P_MEDIA_FORMAT_VIDEO);

	/* format parameters 'fmtp', page 15~20, specs 2110-20:2017 */
	CMN_SN_PRINTF(dataBuf, size, index, "a=fmtp:%d sampling=%s; width=%d; height=%d; exactframerate=%s; ",
		SDP_P_MEDIA_FORMAT_VIDEO, CMN_FIND_V_COLORSPACE(runCfg->runtime.vColorSpace), runCfg->runtime.vWidth, runCfg->runtime.vHeight, 
		CMN_FIND_V_FRAME_RATE(runCfg->runtime.vFrameRate) );

	/* PM: Packing Mode; TCP: Transfer Characteritic System; SSN: SMPTE Standard Number */
	/*  TP=2110TPN; is not defined in specs */
	CMN_SN_PRINTF(dataBuf, size, index, "depth=%d; TCS=SDR; colorimetry=BT709; PM=%s; SSN=ST2110-20:2017;",
		runCfg->runtime.vDepth, SDP_2110_PM_BLOCK );
	if(runCfg->runtime.vIsInterlaced )
	{/* p.17 in specs */
		CMN_SN_PRINTF(dataBuf, size, index, " interlace;" );
		if(runCfg->runtime.vIsSegmented )
		{
			CMN_SN_PRINTF(dataBuf, size, index, " segmented;" );
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

	index = extSdpSessionDescription(ehc, dataBuf, size, EXT_FALSE);

	/* media stream */
	CMN_SN_PRINTF(dataBuf, size, index, "m="SDP_MEDIA_AUDIO" %d "SDP_MEDIA_PROTOCOL_AVP" %d" EXT_NEW_LINE, 
		runCfg->dest.aport, SDP_P_MEDIA_FORMAT_AUDIO);

	/* connection */
	CMN_SN_PRINTF(dataBuf, size, index, "c="SDP_NET_TYPE" "SDP_ADDR_TYPE" %s/%d" EXT_NEW_LINE , EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.audioIp)), SDP_P_TTL);

	/* attribute of RTP map */
	CMN_SN_PRINTF(dataBuf, size, index, "a=rtpmap:%d %s/%d/%d"EXT_NEW_LINE, 
		SDP_P_MEDIA_FORMAT_AUDIO, SDP_P_AUDIO_DEPTH, SDP_P_AUDIO_SAMPLE_RATE, SDP_P_AUDIO_CHANNELS);

	/* channel order: see specs 2110-30 */
	CMN_SN_PRINTF(dataBuf, size, index, "a=fmtp:%d channel-order=SMPTE2110.(SGRP,SGRP,SGRP,SGRP)" EXT_NEW_LINE, SDP_P_MEDIA_FORMAT_AUDIO);
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

