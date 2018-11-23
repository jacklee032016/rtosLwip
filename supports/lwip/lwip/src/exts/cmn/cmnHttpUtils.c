/*
* Common HTTP utilities in HTTP Service, NMOS and HTTP Client
*/

#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"


const char *httpCommonHeader = EXT_HTTP_HDR_S_SERVER EXT_HTTP_HDR_S_CONTENT_TYPE_JSON;

/* HTTP status header */
const	EXT_CONST_STR	_httpHdrs[] =
{
	{
		type	: WEB_RES_REQUEST_OK,
		name	: EXT_HTTP_HDR_S_STATUS_200
	},
	{
		type	: WEB_RES_SW_PROTOCOL,
		name	: EXT_HTTP_HDR_S_STATUS_101
	},
	{
		type	: WEB_RES_TEMP_REDIRECT,
		name	: EXT_HTTP_HDR_S_STATUS_307
	},
	{
		type	: WEB_RES_BAD_REQUEST,
		name	: EXT_HTTP_HDR_S_STATUS_400
	},
	{
		type	: WEB_RES_NOT_FOUND,
		name	: EXT_HTTP_HDR_S_STATUS_404
	},
	{
		type	: WEB_RES_METHOD_NA,
		name	: EXT_HTTP_HDR_S_STATUS_405
	},
	{
		type	: WEB_RES_LOCKED,
		name	: EXT_HTTP_HDR_S_STATUS_423
	},
	{
		type	: WEB_RES_ERROR,
		name	: EXT_HTTP_HDR_S_STATUS_500
	},
	{
		type	: WEB_RES_NOT_IMP,
		name	: EXT_HTTP_HDR_S_STATUS_501
	},
	

	{
		type	: WEB_RES_ERROR_MAX,
		name	: NULL
	}
};


/* look up status header of HTTP response */
const char *extHttpFindStatusHeader(unsigned short httpStatusCode)
{
	const EXT_CONST_STR *hdr = _httpHdrs;

	while(hdr->type != WEB_RES_ERROR_MAX && hdr->name != NULL )
	{
		if(hdr->type == httpStatusCode)
		{
			return hdr->name;
		}

		hdr++;
	}

	EXT_ERRORF(("HTTP Status header can not found for code %d", httpStatusCode));

	return EXT_HTTP_HDR_S_STATUS_404;
}

char	cmnHttpRestError(ExtHttpConn *ehc, unsigned short httpErrorCode, const char *debug)
{
	int index = 0;
	const char *statusHdr;
	char *position;

	statusHdr = extHttpFindStatusHeader(httpErrorCode);
	ehc->httpStatusCode = httpErrorCode;
	ehc->reqType = EXT_HTTP_REQ_T_REST;

	statusHdr += 9 + 4;/* sizeof 'HTTP/1.1 ''*/
	position = strstr(statusHdr, MHTTP_CRLF);

	EXT_DEBUGF(EXT_NMOS_DEBUG, ("Request on '%s' is wrong '%s', because %s", ehc->uri, statusHdr, debug));
	index += snprintf((char *)ehc->data+index, sizeof(ehc->data)-index, "{\""EXT_JSON_KEY_STATUS"\":%d,", httpErrorCode );
	index += snprintf((char *)ehc->data+index, sizeof(ehc->data)-index, "\""EXT_JSON_KEY_ERROR"\":\"%.*s\",", (position-statusHdr), statusHdr );
	index += snprintf((char *)ehc->data+index, sizeof(ehc->data)-index, "\""EXT_JSON_KEY_DEBUG"\":\"%s\"}", debug );

	ehc->contentLength = (unsigned short)index;
	ehc->dataSendIndex = 0;

	return EXIT_SUCCESS;
}


int cmnHttpPrintResponseHeader(ExtHttpConn *ehc, const char contentType)
{
	int index = 0;
	
	CMN_SN_PRINTF((char *)ehc->data, sizeof(ehc->data), index, "HTTP/1.0 200 OK"EXT_NEW_LINE );
	CMN_SN_PRINTF((char *)ehc->data, sizeof(ehc->data), index, "Server: "MHTTPD_SERVER_AGENT"" EXT_NEW_LINE);
//	index += snprintf(data+index, size-index, "Cache-Control: no-cache" EXT_NEW_LINE );
	if(contentType == WEB_RESP_SDP)
	{
		CMN_SN_PRINTF((char *)ehc->data, sizeof(ehc->data), index, "Content-type: %s" EXT_NEW_LINE, HTTP_HDR_SDP);
	}
	else if(contentType == WEB_RESP_JSON)
	{
		CMN_SN_PRINTF((char *)ehc->data, sizeof(ehc->data), index, "Content-type: %s" EXT_NEW_LINE, HTTP_HDR_JSON);
	}
	else
	{
		CMN_SN_PRINTF((char *)ehc->data, sizeof(ehc->data), index, "Content-type: %s" EXT_NEW_LINE, HTTP_HDR_HTML);
	}
	
	CMN_SN_PRINTF((char *)ehc->data, sizeof(ehc->data), index, "Content-Length: 955 " EXT_NEW_LINE EXT_NEW_LINE );
	
	return index;
}


/* called by JSON client and POST service */
int cmnHttpParseRestJson(EXT_RUNTIME_CFG *rxCfg, char *jsonData, uint16_t size)
{
	EXT_JSON_PARSER  *parser = &extParser;

	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Parsing JSON Data %d bytes: '%.*s'", size,  size, jsonData)  );

	extSysClearConfig(rxCfg);

	extJsonParse(parser, jsonData, size);

	if(parser->status != JSON_STATUS_OK)
	{

		return EXIT_FAILURE;
	}
	
	extJsonParseString(parser, EXT_WEB_CFG_FIELD_PRODUCT, rxCfg->name, sizeof(rxCfg->name) );

	extJsonParseMacAddress(parser, EXT_WEB_CFG_FIELD_MAC, &rxCfg->local.mac);

	extJsonParseIpAddress(parser, EXT_WEB_CFG_FIELD_ADDRESS, &rxCfg->local.ip);
	
	extJsonParseIpAddress(parser, EXT_WEB_CFG_FIELD_GATEWAY, &rxCfg->ipGateway);

	/* dest address of RTP */
	extJsonParseIpAddress(parser, EXT_WEB_CFG_FIELD_IP_VEDIO, &rxCfg->dest.ip);
	extJsonParseIpAddress(parser, EXT_WEB_CFG_FIELD_IP_AUDIO, &rxCfg->dest.audioIp);

	/* ports of RTP */			
	extJsonParseUnsignedShort(parser, EXT_WEB_CFG_FIELD_PORT_VEDIO, &rxCfg->dest.vport);
	extJsonParseUnsignedShort(parser, EXT_WEB_CFG_FIELD_PORT_AUDIO, &rxCfg->dest.aport);
	extJsonParseUnsignedShort(parser, EXT_WEB_CFG_FIELD_PORT_DATA, &rxCfg->dest.dport);
	extJsonParseUnsignedShort(parser, EXT_WEB_CFG_FIELD_PORT_STREM, &rxCfg->dest.sport);

	/* params of video stream */
	extJsonParseUnsignedShort(parser, EXT_WEB_CFG_FIELD_VIDEO_WIDTH, &rxCfg->runtime.vWidth);
	extJsonParseUnsignedShort(parser, EXT_WEB_CFG_FIELD_VIDEO_HEIGHT, &rxCfg->runtime.vHeight);
	
	extJsonParseUnsignedChar(parser, EXT_WEB_CFG_FIELD_FRAME_RATE, &rxCfg->runtime.vFrameRate);
	extJsonParseUnsignedChar(parser, EXT_WEB_CFG_FIELD_COLOR_DEPTH, &rxCfg->runtime.vDepth);

	extJsonParseUnsignedChar(parser, EXT_WEB_CFG_FIELD_VIDEO_INTERLACE, &rxCfg->runtime.vIsInterlaced);
	extJsonParseUnsignedChar(parser, EXT_WEB_CFG_FIELD_VIDEO_SEGMENTED, &rxCfg->runtime.vIsSegmented);

	extJsonParseString(parser, EXT_WEB_CFG_FIELD_COLOR_SPACE, parser->msg, EXT_JSON_MESSAGE_SIZE);
	rxCfg->runtime.vColorSpace = CMN_FIND_STR_V_COLORSPACE(parser->msg);

	/*prams of audio stream */			
	extJsonParseUnsignedShort(parser, EXT_WEB_CFG_FIELD_AUDIO_SAMP_RATE, &rxCfg->runtime.aSampleRate);
	extJsonParseUnsignedChar(parser, EXT_WEB_CFG_FIELD_AUDIO_DEPTH, &rxCfg->runtime.aDepth);
	extJsonParseUnsignedChar(parser, EXT_WEB_CFG_FIELD_AUDIO_CHANNEL, &rxCfg->runtime.aChannels);


	EXT_DEBUGF(EXT_DBG_ON, ("Parsed params:name:%s; IP:%s", rxCfg->name, EXT_LWIP_IPADD_TO_STR(&rxCfg->local.ip)) );

	EXT_DEBUGF(EXT_DBG_ON, ("Parsed SDP Audio params:IP:%s; Port:%d; Depth:%d; Sample Freq:%d; Channels:%d",
			EXT_LWIP_IPADD_TO_STR(&rxCfg->dest.audioIp), rxCfg->dest.aport, rxCfg->runtime.aDepth, rxCfg->runtime.aSampleRate, rxCfg->runtime.aChannels ));

	EXT_DEBUGF(EXT_DBG_ON, ("Parsed SDP Video params:IP:%s; Port:%d; ColorSpace:%s; width:%d; height:%d; framerate:%s; depth:%d; isInterlace:%s; isSegmented:%s",
		EXT_LWIP_IPADD_TO_STR(&rxCfg->dest.ip), rxCfg->dest.vport, CMN_FIND_V_COLORSPACE(rxCfg->runtime.vColorSpace),  
		rxCfg->runtime.vWidth, rxCfg->runtime.vHeight, CMN_FIND_V_FRAME_RATE(rxCfg->runtime.vFrameRate), rxCfg->runtime.vDepth,
		(rxCfg->runtime.vIsInterlaced)?"YES":"NO", (rxCfg->runtime.vIsSegmented)?"YES":"NO"));


	return EXIT_SUCCESS;
	
}


