
#include "lwipExt.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "jsmn.h"
#include "extUdpCmd.h"


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

static const	EXT_CONST_STR	_videoColorSpaces[] =
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
	
	return -1;
}

int	cmnParseGetHexIntValue(char *hexString)
{
	int value =0;
	
//	sscanf(hexString, "%x", &value);
	value = (int)strtol(hexString, NULL, 16);

	return value;
}


