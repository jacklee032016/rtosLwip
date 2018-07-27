
#include "lwipMux.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "jsmn.h"
#include "muxIpCmd.h"


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



static const	MUX_CONST_STR	_ipcmdStringRsParities[] =
{
	{
		type	: MUX_RS232_PARITY_NONE,
		name	: MUX_RS232_PAR_STR_NONE
	},
	{
		type	: MUX_RS232_PARITY_ODD,
		name	: MUX_RS232_PAR_STR_ODD
	},
	{
		type	: MUX_RS232_PARITY_EVEN,
		name	: MUX_RS232_PAR_STR_EVEN
	},
	{
		type	: MUX_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};

static const	MUX_CONST_STR	_videoColorSpaces[] =
{
	{
		type	: MUX_V_COLORSPACE_YCBCR_422,
		name	: "YCbCr-422"
	},
	{
		type	: MUX_V_COLORSPACE_YCBCR_444,
		name	: "YCbCr-444"
	},
	
	{
		type	: MUX_V_COLORSPACE_RGB,
		name	: "RGB"
	},

	{
		type	: MUX_V_COLORSPACE_YCBCR_420,
		name	: "YCbCr-420"
	},
	{
		type	: MUX_V_COLORSPACE_XYZ,
		name	: "XYZ"
	},
	{
		type	: MUX_V_COLORSPACE_KEY,
		name	: "KEY"
	},
	{
		type	: MUX_V_COLORSPACE_CL_YCBCR_422,
		name	: "CLYCbCr-422"
	},
	{
		type	: MUX_V_COLORSPACE_CL_YCBCR_444,
		name	: "CLYCbCr-444"
	},

	{
		type	: MUX_V_COLORSPACE_CL_YCBCR_420,
		name	: "CLYCbCr-420"
	},
	{
		type	: MUX_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};


const char *muxCmnStringFind(CMN_STR_TYPE  strType, unsigned short type)
{
	const MUX_CONST_STR *_str;

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

	while(_str->type!= MUX_INVALIDATE_STRING_TYPE)
	{
		if(_str->type == type)
		{
			return _str->name;
		}

		_str++;
	}
	
	return "Unknown String";
}


const short muxCmnTypeFind(CMN_STR_TYPE  strType, char *str)
{
	const MUX_CONST_STR *_str;

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

	while(_str->type!= MUX_INVALIDATE_STRING_TYPE)
	{
		if(IS_STRING_EQUAL(_str->name, str) )
		{
			return _str->type;
		}

		_str++;
	}
	
	return -1;
}


