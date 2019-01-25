/*
* Common HTTP utilities in HTTP Service, NMOS and HTTP Client
*/

#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"
#include "extUdpCmd.h"


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
err_t cmnHttpParseRestJson(EXT_RUNTIME_CFG *rxCfg, char *jsonData, uint16_t size)
{
	uint8_t _chVal;
	unsigned int _intValue;
	short	shValue;
	
	EXT_JSON_PARSER  *parser = &extParser;

//	EXT_DEBUGF(EXT_DBG_OFF, ("Parsing JSON Data %d bytes: '%.*s'", size,  size, jsonData)  );

	extSysClearConfig(rxCfg);

	extJsonParse(parser, jsonData, size);

	if(parser->status != JSON_STATUS_OK)
	{
		return ERR_ARG;
	}

	/* system parsms */
	extJsonParseString(parser, EXT_WEB_CFG_FIELD_PRODUCT, rxCfg->name, sizeof(rxCfg->name) );

#if 0
	/* MAC is read only for REST API */
	extJsonParseMacAddress(parser, EXT_WEB_CFG_FIELD_MAC, &rxCfg->local.mac);
#endif

	extJsonParseUnsignedChar(parser, EXT_WEB_CFG_FIELD_IS_DHCP, &rxCfg->netMode);
	extJsonParseIpAddress(parser, EXT_WEB_CFG_FIELD_ADDRESS, &rxCfg->local.ip);
	extJsonParseIpAddress(parser, EXT_WEB_CFG_FIELD_GATEWAY, &rxCfg->ipGateway);
	extJsonParseIpAddress(parser, EXT_WEB_CFG_FIELD_NETMASK, &rxCfg->ipMask );

	/* SDP */
	extJsonParseIpAddress(parser, EXT_WEB_CFG_FIELD_SDP_VEDIO_IP, &rxCfg->sdpUriVideo.ip);
	extJsonParseUnsignedShort(parser, EXT_WEB_CFG_FIELD_SDP_VEDIO_PORT, &rxCfg->sdpUriVideo.port );
	if(extJsonParseString(parser, EXT_WEB_CFG_FIELD_SDP_VEDIO_URI, rxCfg->sdpUriVideo.uri, sizeof(rxCfg->sdpUriVideo.uri) ) == EXIT_SUCCESS)
	{
		unescape_uri(rxCfg->sdpUriVideo.uri);
	}

	extJsonParseIpAddress(parser, EXT_WEB_CFG_FIELD_SDP_AUDIO_IP, &rxCfg->sdpUriAudio.ip);
	extJsonParseUnsignedShort(parser, EXT_WEB_CFG_FIELD_SDP_AUDIO_PORT, &rxCfg->sdpUriAudio.port );
	if(extJsonParseString(parser, EXT_WEB_CFG_FIELD_SDP_AUDIO_URI, rxCfg->sdpUriAudio.uri, sizeof(rxCfg->sdpUriAudio.uri) ) == EXIT_SUCCESS)
	{
		unescape_uri(rxCfg->sdpUriAudio.uri);
	}


	/* dest address of RTP */
	extJsonParseIpAddress(parser, EXT_WEB_CFG_FIELD_IP_VEDIO, &rxCfg->dest.ip);
	extJsonParseIpAddress(parser, EXT_WEB_CFG_FIELD_IP_AUDIO, &rxCfg->dest.audioIp);
	extJsonParseIpAddress(parser, EXT_WEB_CFG_FIELD_IP_ANC, &rxCfg->dest.ancIp);
#if EXT_FPGA_AUX_ON	
	extJsonParseIpAddress(parser, EXT_WEB_CFG_FIELD_IP_AUX, &rxCfg->dest.auxIp);
#endif

	/* ports of RTP */			
	extJsonParseUnsignedShort(parser, EXT_WEB_CFG_FIELD_PORT_VEDIO, &rxCfg->dest.vport);
	extJsonParseUnsignedShort(parser, EXT_WEB_CFG_FIELD_PORT_AUDIO, &rxCfg->dest.aport);
	extJsonParseUnsignedShort(parser, EXT_WEB_CFG_FIELD_PORT_DATA, &rxCfg->dest.dport);
#if EXT_FPGA_AUX_ON	
	extJsonParseUnsignedShort(parser, EXT_WEB_CFG_FIELD_PORT_STREM, &rxCfg->dest.sport);
#endif


	extJsonParseUnsignedChar(parser, EXT_WEB_CFG_FIELD_FPGA_AUTO, &rxCfg->fpgaAuto);

	/* params of video stream */
	extJsonParseUnsignedShort(parser, EXT_WEB_CFG_FIELD_VIDEO_WIDTH, &rxCfg->runtime.vWidth);
	extJsonParseUnsignedShort(parser, EXT_WEB_CFG_FIELD_VIDEO_HEIGHT, &rxCfg->runtime.vHeight);

#if 0
	if(extJsonParseUnsignedChar(parser, EXT_WEB_CFG_FIELD_FRAME_RATE, &_chVal) == EXIT_SUCCESS)
	{
		rxCfg->runtime.vFrameRate = CMN_INT_FIND_TYPE_V_FPS(_chVal);
	}
#else
	extJsonParseString(parser, EXT_WEB_CFG_FIELD_FRAME_RATE, rxCfg->model, sizeof(rxCfg->model));
	if(!IS_STRING_NULL(rxCfg->model))
	{
		rxCfg->runtime.vFrameRate = CMN_FIND_STR_V_FPS_4_REST(rxCfg->model );
		memset(rxCfg->model, 0, sizeof(rxCfg->model));
	}
#endif

	if(extJsonParseUnsignedChar(parser, EXT_WEB_CFG_FIELD_COLOR_DEPTH, &_chVal) == EXIT_SUCCESS)
	{
		rxCfg->runtime.vDepth = CMN_INT_FIND_TYPE_V_DEPTH(_chVal);
	}

	extJsonParseUnsignedChar(parser, EXT_WEB_CFG_FIELD_VIDEO_INTERLACE, &rxCfg->runtime.vIsInterlaced);
	extJsonParseString(parser, EXT_WEB_CFG_FIELD_COLOR_SPACE, rxCfg->model, sizeof(rxCfg->model));
	if(!IS_STRING_NULL(rxCfg->model))
	{
		rxCfg->runtime.vColorSpace = CMN_FIND_STR_V_COLORSPACE(rxCfg->model);
		memset(rxCfg->model, 0, sizeof(rxCfg->model));
	}


	/*prams of audio stream */			
	extJsonParseString(parser, EXT_WEB_CFG_FIELD_AUDIO_SAMP_RATE, rxCfg->model, sizeof(rxCfg->model));
	if(!IS_STRING_NULL(rxCfg->model))
	{
		rxCfg->runtime.aSampleRate = CMN_FIND_STR_A_RATE(rxCfg->model );
		memset(rxCfg->model, 0, sizeof(rxCfg->model));
	}

	extJsonParseString(parser, EXT_WEB_CFG_FIELD_AUDIO_PKT_SIZE, rxCfg->model, sizeof(rxCfg->model));
	if(!IS_STRING_NULL(rxCfg->model))
	{
		rxCfg->runtime.aPktSize = CMN_FIND_STR_A_PKTSIZE(rxCfg->model );
		memset(rxCfg->model, 0, sizeof(rxCfg->model));
	}

	extJsonParseUnsignedChar(parser, EXT_WEB_CFG_FIELD_AUDIO_DEPTH, &rxCfg->runtime.aDepth);
	extJsonParseUnsignedChar(parser, EXT_WEB_CFG_FIELD_AUDIO_CHANNEL, &rxCfg->runtime.aChannels);

	extJsonParseUnsignedChar(parser, EXT_IPCMD_DATA_IS_RESET, &rxCfg->runtime.reset );
	extJsonParseUnsignedChar(parser, EXT_IPCMD_DATA_IS_REBOOT, &rxCfg->runtime.reboot );

	extJsonParseUnsignedChar(parser, EXT_IPCMD_DATA_BLINK, &rxCfg->runtime.blink );


	if(extJsonParseUnsignedInteger(parser, EXT_WEB_CFG_FIELD_RS232_BAUDRATE, &_intValue) == EXIT_SUCCESS)
	{
		if( CHECK_BAUDRATE(_intValue) )
		{
//			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate baudrate for '%s'", value, key);
//			return EXIT_FAILURE;
		}
		else
		{
			rxCfg->rs232Cfg.baudRate = _intValue;
		}
	}
	
	if( extJsonParseUnsignedChar(parser, EXT_WEB_CFG_FIELD_RS232_DATABITS, &_chVal) == EXIT_SUCCESS)
	{
		if( CHECK_DATABITS(_chVal) )
		{
//			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' is not validate databit for '%s'", value, key);
//			return EXIT_FAILURE;
		}
		else
		{
			rxCfg->rs232Cfg.charLength = _chVal;
		}
	}

	extJsonParseString(parser, EXT_WEB_CFG_FIELD_RS232_PARITY, rxCfg->model, sizeof(rxCfg->model));
	if(!IS_STRING_NULL(rxCfg->model))
	{
		shValue = CMN_FIND_STR_RS_PARITY(rxCfg->model );
		if(shValue != INVALIDATE_VALUE_U16)
		{
			rxCfg->rs232Cfg.parityType = (unsigned char )shValue;
		}
		memset(rxCfg->model, 0, sizeof(rxCfg->model));
	}
	
	if( extJsonParseUnsignedChar(parser, EXT_WEB_CFG_FIELD_RS232_STOPBITS, &_chVal )== EXIT_SUCCESS )
	{
		if(_chVal == 1 || _chVal == 2 )
		{
			rxCfg->rs232Cfg.stopbits = _chVal;
		}
	}



	return ERR_OK;
	
}


#define HEX_TO_DECIMAL(char1, char2)	\
    (((char1 >= 'A') ? (((char1 & 0xdf) - 'A') + 10) : (char1 - '0')) * 16) + \
    (((char2 >= 'A') ? (((char2 & 0xdf) - 'A') + 10) : (char2 - '0')))


/* Decodes a uri, changing %xx encodings with the actual character.  The query_string should already be gone.
 * Return values: *  1: success;  0: illegal string
 */
int unescape_uri(char *uri)
{
	char c, d;
	char *uri_old;

	uri_old = uri;

	while ((c = *uri_old))
	{
		if (c == '%')
		{
			uri_old++;
			if ((c = *uri_old++) && (d = *uri_old++))
			{
				*uri = HEX_TO_DECIMAL(c, d);
				if (*uri < 32 || *uri > 126)
				{/* control chars in URI */
					*uri = '\0';
					return 0;
				}
			}
			else
			{
				*uri = '\0';
				return 0;
			}
			++uri;
		}
		else if (c == '?')
		{  /* query string */
			*uri = '\0';
			return (1);
		}
		else if (c == '#')
		{  /* fragment */
			/* legal part of URL, but we do *not* care.
			* However, we still have to look for the query string */
			break;
		}
		else
		{
			*uri++ = c;
			uri_old++;
		}
	}

	*uri = '\0';
	return 1;
}



