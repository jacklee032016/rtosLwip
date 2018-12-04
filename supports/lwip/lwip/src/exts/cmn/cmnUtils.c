
#include "lwipExt.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "jsmn.h"
#include "extUdpCmd.h"
#include "httpClient.h"

unsigned int cmnMuxCRC32b(void *message, int len)
{
	int i, j;
	unsigned int byte, crc, mask;
	char * tmp = (char*) message;
	i = 0;
	crc = 0xFFFFFFFF;
	
	//while (tmp[i] != 0)
	for (i=0; i< len; i++)	
	{
		byte = tmp[i];            // Get next byte.
		crc = crc ^ byte;
		for (j = 7; j >= 0; j--)
		{// Do eight times.
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
		
//		i = i + 1;
	}
	
	return ~crc;
}


static const	EXT_CONST_STR	_httpStringStates[] =
{
	{
		type	: H_STATE_INIT,
		name	: "INIT"
	},
	{
		type	: H_STATE_REQ,
		name	: "REQ"
	},
	{
		type	: H_STATE_DATA,
		name	: "DATA"
	},
	{
		type	: H_STATE_RESP,
		name	: "RESP"
	},
	{
		type	: H_STATE_CLOSE,
		name	: "CLOSE"
	},
	{
		type	: H_STATE_ERROR,
		name	: "ERROR"
	},
	{
		type	: H_STATE_FREE,
		name	: "FREE"
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};


static const	EXT_CONST_STR	_httpStringEvents[] =
{
	{
		type	: H_EVENT_NEW,
		name	: "NEW"
	},
	{
		type	: H_EVENT_RECV,
		name	: "RECV"
	},
	{
		type	: H_EVENT_POLL,
		name	: "POLL"
	},
	{
		type	: H_EVENT_SENT,
		name	: "SENT"
	},
	{
		type	: H_EVENT_CLOSE,
		name	: "CLOSE"
	},
	{
		type	: H_EVENT_ERROR,
		name	: "ERROR"
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};


static const	EXT_CONST_STR	_hcStringStates[] =
{
	{
		type	: HC_STATE_WAIT,
		name	: "WAIT"
	},
	{
		type	: HC_STATE_INIT,
		name	: "INIT"
	},
	{
		type	: HC_STATE_CONN,
		name	: "CONN"
	},
	{
		type	: HC_STATE_DATA,
		name	: "DATA"
	},
	{
		type	: HC_STATE_ERROR,
		name	: "ERROR"
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};


static const	EXT_CONST_STR	_hcStringEvents[] =
{
	{
		type	: HC_EVENT_NEW,
		name	: "NEW"
	},
	{
		type	: HC_EVENT_TIMEOUT,
		name	: "TIMEOUT"
	},
	{
		type	: HC_EVENT_CONNECTED,
		name	: "CONN"
	},
	{
		type	: HC_EVENT_RECV,
		name	: "RECV"
	},
	{
		type	: HC_EVENT_POLL,
		name	: "POLL"
	},
	{
		type	: HC_EVENT_SENT,
		name	: "SENT"
	},
	{
		type	: HC_EVENT_CLOSE,
		name	: "CLOSE"
	},
	{
		type	: HC_EVENT_ERROR,
		name	: "ERROR"
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};


static const	EXT_CONST_STR	_ipcmdStringRsParities[] =
{
	{
		type	: EXT_RS232_PARITY_NONE,
		name	: EXT_RS232_PAR_STR_NONE
	},
	{
		type	: EXT_RS232_PARITY_ODD,
		name	: EXT_RS232_PAR_STR_ODD
	},
	{
		type	: EXT_RS232_PARITY_EVEN,
		name	: EXT_RS232_PAR_STR_EVEN
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};

const	EXT_CONST_STR	_videoColorSpaces[] =
{
	{
		type	: EXT_V_COLORSPACE_YCBCR_422,
		name	: "YCbCr-422"
	},
	{
		type	: EXT_V_COLORSPACE_YCBCR_444,
		name	: "YCbCr-444"
	},
	
	{
		type	: EXT_V_COLORSPACE_RGB,
		name	: "RGB"
	},

	{
		type	: EXT_V_COLORSPACE_YCBCR_420,
		name	: "YCbCr-420"
	},
	{
		type	: EXT_V_COLORSPACE_XYZ,
		name	: "XYZ"
	},
	{
		type	: EXT_V_COLORSPACE_KEY,
		name	: "KEY"
	},
	{
		type	: EXT_V_COLORSPACE_CL_YCBCR_422,
		name	: "CLYCbCr-422"
	},
	{
		type	: EXT_V_COLORSPACE_CL_YCBCR_444,
		name	: "CLYCbCr-444"
	},

	{
		type	: EXT_V_COLORSPACE_CL_YCBCR_420,
		name	: "CLYCbCr-420"
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};


const	EXT_CONST_STR	_videoFramerates[] =
{
	{
		type	: EXT_V_FRAMERATE_T_23,
		name	: "24000/1001"
	},
	{
		type	: EXT_V_FRAMERATE_T_24,
		name	: "24"
	},

	{
		type	: EXT_V_FRAMERATE_T_25,
		name	: "25"
	},
	{
		type	: EXT_V_FRAMERATE_T_29,
		name	: "30000/1001"
	},

	{
		type	: EXT_V_FRAMERATE_T_30,
		name	: "30"
	},
	
	{
		type	: EXT_V_FRAMERATE_T_50,
		name	: "50"
	},
	
	{
		type	: EXT_V_FRAMERATE_T_59,
		name	: "60000/1001"
	},
	
	{
		type	: EXT_V_FRAMERATE_T_60,
		name	: "60"
	},
	
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};


const	EXT_CONST_STR	_audioPktSizes[] =
{
	{
		type	: EXT_A_PKT_SIZE_1MS,
		name	: "1ms"
	},
	{
		type	: EXT_A_PKT_SIZE_125MKS,
		name	: "125mks"
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};


const	EXT_CONST_STR	_audioRates[] =
{
	{
		type	: EXT_A_RATE_48K,
		name	: "48KHz"
	},
	{
		type	: EXT_A_RATE_44K,
		name	: "44.1KHz"
	},
	{
		type	: EXT_A_RATE_96K,
		name	: "96KHz"
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};

const EXT_CONST_STR _strTypes[] =
{
	{
		type	: CMN_STR_T_RS_PARITY,
		name	: "RS232Parity"
	},
	{
		type	: CMN_STR_T_V_COLORSPACE,
		name	: "ColorSpace"
	},
	{
		type	: CMN_STR_T_V_FRAME_RATE,
		name	: "VideoFps"
	},

	{
		type	: CMN_STR_T_HTTP_STATES,
		name	: "sState"
	},
	{
		type	: CMN_STR_T_HTTP_EVENTS,
		name	: "sEvent"
	},
	{
		type	: CMN_STR_T_HC_STATES,
		name	: "ClientState"
	},
	{
		type	: CMN_STR_T_HC_EVENTS,
		name	: "ClientEvent"
	},
	{
		type	: CMN_STR_T_A_PKTSIZE,
		name	: "AudioPktSize"
	},
	{
		type	: CMN_STR_T_A_RATE,
		name	: "AudoRate"
	},

	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};


const short	videoWidthList[]=
{
//	480,
//	576,	
//	640,
	720,
//	800,
	960,
//	1024,
	1280,
//	1360,
//	1440,
//	1600,
//	1680,
	1920,
//	2048,
//	2560,
//	3840,
//	4096,
	0
};

const short videoHeightList[]=
{
	480,
//	600,
	576,
	720,
//	768,
//	800,
//	1024,
//	1050,
	1080,
//	1200,
//	1440,
//	1600,
//	2160,
	0
};

const char videoFpsList[]  =
{
	EXT_V_FRAMERATE_T_23,
	EXT_V_FRAMERATE_T_24,
	EXT_V_FRAMERATE_T_25,
	EXT_V_FRAMERATE_T_29,
	EXT_V_FRAMERATE_T_30,
	EXT_V_FRAMERATE_T_50,
	EXT_V_FRAMERATE_T_59,
	EXT_V_FRAMERATE_T_60,
	0
};

const char videoColorDepthList[] =
{
	EXT_V_DEPTH_8,	
	EXT_V_DEPTH_10,	
	EXT_V_DEPTH_12,	
	EXT_V_DEPTH_16,
	0
};

const char audioChannelsList[] =
{
	4,	
	8,
	12,
	16,	
	0
};


const char *__extCmnFindTypeStr(unsigned short type)
{
	const EXT_CONST_STR *_str = _strTypes;

	while(_str->type!= EXT_INVALIDATE_STRING_TYPE)
	{
		if(_str->type == type)
		{
			return _str->name;
		}

		_str++;
	}

	return "UnknownType";
}


const char *extCmnStringFind(CMN_STR_TYPE  strType, unsigned short type)
{
	const EXT_CONST_STR *_str;

	switch(strType)
	{
		case CMN_STR_T_RS_PARITY:
			_str = _ipcmdStringRsParities;
			break;
		case CMN_STR_T_V_COLORSPACE:
			_str = _videoColorSpaces;
			break;
		case CMN_STR_T_V_FRAME_RATE:
			_str = _videoFramerates;
			break;

		case CMN_STR_T_HTTP_STATES:
			_str = _httpStringStates;
			break;
		case CMN_STR_T_HTTP_EVENTS:
			_str = _httpStringEvents;
			break;
		case CMN_STR_T_HC_STATES:
			_str = _hcStringStates;
			break;
		case CMN_STR_T_HC_EVENTS:
			_str = _hcStringEvents;
			break;

		case CMN_STR_T_A_PKTSIZE:
			_str = _audioPktSizes;
			break;

		case CMN_STR_T_A_RATE:
			_str = _audioRates;
			break;

		default:
			return "Unknown String Type";
			break;
	}

	while(_str->type!= EXT_INVALIDATE_STRING_TYPE)
	{
		if(_str->type == type)
		{
			return _str->name;
		}

		_str++;
	}

	EXT_ERRORF(("unknown type %d in constant type :%s", type, __extCmnFindTypeStr(strType) ) );
	return "Unknown String";
}


const short extCmnTypeFind(CMN_STR_TYPE  strType, char *str)
{
	const EXT_CONST_STR *_str;

	switch(strType)
	{
		case CMN_STR_T_RS_PARITY:
			_str = _ipcmdStringRsParities;
			break;
		case CMN_STR_T_V_COLORSPACE:
			_str = _videoColorSpaces;
			break;
		case CMN_STR_T_V_FRAME_RATE:
			_str = _videoFramerates;
			break;
		case CMN_STR_T_HTTP_STATES:
			_str = _httpStringStates;
			break;
		case CMN_STR_T_HTTP_EVENTS:
			_str = _httpStringEvents;
			break;
		case CMN_STR_T_HC_STATES:
			_str = _hcStringStates;
			break;
		case CMN_STR_T_HC_EVENTS:
			_str = _hcStringEvents;
			break;

		case CMN_STR_T_A_PKTSIZE:
			_str = _audioPktSizes;
			break;
		case CMN_STR_T_A_RATE:
			_str = _audioRates;
			break;

		default:
			return -1;
			break;
	}

	while(_str->type!= EXT_INVALIDATE_STRING_TYPE)
	{
		if(IS_STRING_EQUAL(_str->name, str) )
		{
			return _str->type;
		}

		_str++;
	}
	
	EXT_ERRORF(("unknown type %s in constant type :%s", str, __extCmnFindTypeStr(strType) ) );
	return -1;
}

int	cmnParseGetHexIntValue(char *hexString)
{
	int value =0;
	
//	sscanf(hexString, "%x", &value);
	value = (int)strtol(hexString, NULL, 16);

	return value;
}

char cmnUtilsParseIp(char *strIpAddress, uint32_t  *ip)
{
	*ip = ipaddr_addr(strIpAddress);
	
	if(*ip == IPADDR_NONE)
	{
		printf("Token '%s' was not IP address.", strIpAddress);
		return EXIT_FAILURE;
	}
	else
	{
		return EXIT_SUCCESS;
	}
}

char cmnUtilsParseInt32(char *strValue, uint32_t  *value)
{
//	if (1 != sscanf(strValue, "%"PRIu32, value))
	if (1 != sscanf(strValue, "%"U32_F, value))
	{
		printf("Token '%s' was not an 32-bit int.", strValue );
		return EXIT_FAILURE;
	}
	else
	{
		return EXIT_SUCCESS;
	}
}

char cmnUtilsParseInt16(char *strValue, uint16_t  *value)
{
#ifdef	ARM
	if (1 != sscanf(strValue, "%"PRIu16, value))
#else
	if (1 != sscanf(strValue, "%hu", value))
#endif		
	{
		printf("Token '%s' was not an short int.", strValue);
		return EXIT_FAILURE;
	}
	else
	{
		return EXIT_SUCCESS;
	}
}

char cmnUtilsParseInt8(char *strValue, uint8_t  *value)
{
#ifdef	ARM
	if (1 != sscanf(strValue, "%"PRIu8, value))
#else
	if (1 != sscanf(strValue, "%hhu", value))
#endif		
	{
		printf("Token '%s' was not an 8-bit int.", strValue );
		return EXIT_FAILURE;
	}
	else
	{
		return EXIT_SUCCESS;
	}
}




/* return < 0, error; 0 : on this header; others, content length */
uint32_t	cmnHttpParseHeaderContentLength(char *headers, uint32_t headerLength)
{
	char *scontent_len_end, *scontent_len;
	char *content_len_num;
	int contentLen;

		/* search for "Content-Length: " */
#define HTTP_HDR_CONTENT_LEN                "Content-Length: "
#define HTTP_HDR_CONTENT_LEN_LEN            16
#define HTTP_HDR_CONTENT_LEN_DIGIT_MAX_LEN  10

	scontent_len = lwip_strnstr(headers, HTTP_HDR_CONTENT_LEN, headerLength);
	if (scontent_len == NULL)
	{
		/* If we come here, headers are fully received (double-crlf), but Content-Length
		was not included. Since this is currently the only supported method, we have to fail in this case! */
//		EXT_ERRORF(("Error when parsing Content-Length"));
//		cmnHttpRestError(ehc, WEB_RES_BAD_REQUEST, "Content-Length is wrong");
		return ERR_OK;
	}
	
	scontent_len_end = lwip_strnstr(scontent_len + HTTP_HDR_CONTENT_LEN_LEN, MHTTP_CRLF, HTTP_HDR_CONTENT_LEN_DIGIT_MAX_LEN);
	if (scontent_len_end == NULL)
	{
		EXT_ERRORF( ("Error when parsing number in Content-Length: '%s'",scontent_len+HTTP_HDR_CONTENT_LEN_LEN ));
//		cmnHttpRestError(ehc, WEB_RES_BAD_REQUEST, "Content-Length is wrong");
		return ERR_VAL;
	}

	content_len_num = scontent_len + HTTP_HDR_CONTENT_LEN_LEN;
	contentLen = atoi(content_len_num);
			
	if (contentLen == 0)
	{
		/* if atoi returns 0 on error, fix this */
		if ((content_len_num[0] != '0') || (content_len_num[1] != '\r'))
		{
			contentLen = -1;
		}
	}
			
	if (contentLen < 0)
	{
		EXT_ERRORF( ("POST received invalid Content-Length: %s", content_len_num));
//		cmnHttpRestError(ehc, WEB_RES_BAD_REQUEST, "Content-Length is wrong");
		return ERR_VAL;
	}

	EXT_DEBUGF(EXT_DBG_OFF, ("Content length:%d\r\n", contentLen));
	return contentLen;
}


