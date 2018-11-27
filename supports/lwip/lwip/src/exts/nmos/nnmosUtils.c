/*
* utils for NMOS operations
*/


#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"

#if LWIP_EXT_NMOS

static const	EXT_CONST_STR	_nmosStringVideoComponents[] =
{
	{
		type	: NMOS_VIDEO_CMP_TYPE_T_Y,
		name	: NMOS_VIDEO_CMP_TYPE_S_Y
	},
	{
		type	: NMOS_VIDEO_CMP_TYPE_T_CB,
		name	: NMOS_VIDEO_CMP_TYPE_S_CB
	},
	{
		type	: NMOS_VIDEO_CMP_TYPE_T_CR,
		name	: NMOS_VIDEO_CMP_TYPE_S_CR
	},
	{
		type	: NMOS_VIDEO_CMP_TYPE_T_I,
		name	: NMOS_VIDEO_CMP_TYPE_S_I
	},
	{
		type	: NMOS_VIDEO_CMP_TYPE_T_CT,
		name	: NMOS_VIDEO_CMP_TYPE_S_CT
	},
	{
		type	: NMOS_VIDEO_CMP_TYPE_T_CP,
		name	: NMOS_VIDEO_CMP_TYPE_S_CP
	},
	{
		type	: NMOS_VIDEO_CMP_TYPE_T_A,
		name	: NMOS_VIDEO_CMP_TYPE_S_A
	},
	{
		type	: NMOS_VIDEO_CMP_TYPE_T_R,
		name	: NMOS_VIDEO_CMP_TYPE_S_R
	},
	{
		type	: NMOS_VIDEO_CMP_TYPE_T_G,
		name	: NMOS_VIDEO_CMP_TYPE_S_G
	},
	{
		type	: NMOS_VIDEO_CMP_TYPE_T_B,
		name	: NMOS_VIDEO_CMP_TYPE_S_B
	},
	{
		type	: NMOS_VIDEO_CMP_TYPE_T_DEPTH_MAP,
		name	: NMOS_VIDEO_CMP_TYPE_S_DEPTH_MAP
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};


static const	EXT_CONST_STR	_nmosStringColorSpaces[] =
{
	{
		type	: NMOS_VIDEO_CS_T_BT601,
		name	: NMOS_VIDEO_CS_S_BT601
	},
	{
		type	: NMOS_VIDEO_CS_T_BT709,
		name	: NMOS_VIDEO_CS_S_BT709
	},
	{
		type	: NMOS_VIDEO_CS_T_BT2022,
		name	: NMOS_VIDEO_CS_S_BT2020
	},
	{
		type	: NMOS_VIDEO_CS_T_BT2100,
		name	: NMOS_VIDEO_CS_S_BT2100
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};



static const	EXT_CONST_STR	_nmosStringTransferCharacters[] =
{
	{
		type	: NMOS_VIDEO_TC_T_SDR,
		name	: NMOS_VIDEO_TC_S_SDR
	},
	{
		type	: NMOS_VIDEO_TC_T_HLG,
		name	: NMOS_VIDEO_TC_S_HLG
	},
	{
		type	: NMOS_VIDEO_TC_T_PQ,
		name	: NMOS_VIDEO_TC_S_PQ
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};



static const	EXT_CONST_STR	_nmosStringInterlaceModes[] =
{
	{
		type	: NMOS_VIDEO_ILM_T_PROGRESSIVE,
		name	: NMOS_VIDEO_ILM_S_PROGRESSIVE
	},
	{
		type	: NMOS_VIDEO_ILM_T_TFF,
		name	: NMOS_VIDEO_ILM_S_TFF
	},
	{
		type	: NMOS_VIDEO_ILM_T_BFF,
		name	: NMOS_VIDEO_ILM_S_BFF
	},
	{
		type	: NMOS_VIDEO_ILM_T_PSF,
		name	: NMOS_VIDEO_ILM_S_PSF
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};


static const	EXT_CONST_STR	_nmosStringTransports[] =
{
	{
		type	: NMOS_TRANS_T_RTP,
		name	: NMOS_URN_TRANSPORT_RTP
	},
	{
		type	: NMOS_TRANS_T_UCAST,
		name	: NMOS_URN_TRANSPORT_RTP_UCAST
	},
	{
		type	: NMOS_TRANS_T_MCAST,
		name	: NMOS_URN_TRANSPORT_RTP_MCAST
	},
	{
		type	: NMOS_TRANS_T_DASH,
		name	: NMOS_URN_TRANSPORT_DASH
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};


static const	EXT_CONST_STR	_nmosStringSourceFormats[] =
{
	{
		type	: NMOS_SOURCE_FORMAT_VIDEO,
		name	: NMOS_URN_FORMAT_VIDEO
	},
	{
		type	: NMOS_SOURCE_FORMAT_AUDIO,
		name	: NMOS_URN_FORMAT_AUDIO
	},
	{
		type	: NMOS_SOURCE_FORMAT_DATA,
		name	: NMOS_URN_FORMAT_DATA
	},
	{
		type	: NMOS_SOURCE_FORMAT_MUX,
		name	: NMOS_URN_FORMAT_MUX
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};

static const	EXT_CONST_STR	_nmosStringAudioChannelSymbols[] =
{
	{
		type	: NMOS_AUDIO_CHAN_T_L,
		name	: "L"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_R,
		name	: "R"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_C,
		name	: "C"
	},

	{
		type	: NMOS_AUDIO_CHAN_T_LFE,
		name	: "LFE"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_LS,
		name	: "Ls"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_RS,
		name	: "Rs"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_LSS,
		name	: "Lss"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_RSS,
		name	: "Rss"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_LRS,
		name	: "Lrs"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_RRS,
		name	: "Rrs"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_LC,
		name	: "Lc"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_RC,
		name	: "Rc"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_CS,
		name	: "Cs"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_HI,
		name	: "HI"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_VIN,
		name	: "VIN"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_M1,
		name	: "M1"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_M2,
		name	: "M2"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_LT,
		name	: "Lt"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_RT,
		name	: "Rt"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_LST,
		name	: "Lst"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_RST,
		name	: "Rst"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_S,
		name	: "S"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_NSC,
		name	: "NSC"
	},
	{
		type	: NMOS_AUDIO_CHAN_T_UNDEFINE,
		name	: "U"
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};



static const	EXT_CONST_STR	_nmosStringConnActivates[] =
{
	{
		type	: NMOS_CONN_ACTIVATE_T_IMMEDIATE,
		name	: NMOS_CONN_ACTIVATE_S_IMMEDIATE
	},
	{
		type	: NMOS_CONN_ACTIVATE_T_ABSOLUTE,
		name	: NMOS_CONN_ACTIVATE_S_ABSOLUTE
	},
	{
		type	: NMOS_CONN_ACTIVATE_T_RELATIVE,
		name	: NMOS_CONN_ACTIVATE_S_RELATIVE
	},
	{
		type	: EXT_INVALIDATE_STRING_TYPE,
		name	: NULL
	}
};


const char *extNmosStringFindFormat(NMOS_STR_TYPE strType, unsigned short type)
{
	const EXT_CONST_STR *_str;

	switch(strType)
	{
		case NMOS_STR_T_SOURCE_FORMAT:
			_str = _nmosStringSourceFormats;
			break;
		case NMOS_STR_T_AUDIO_CHANNEL:
			_str = _nmosStringAudioChannelSymbols;
			break;
		case NMOS_STR_T_TRANSPORT:
			_str = _nmosStringTransports;
			break;

		case NMOS_STR_T_INTERLACE_MODE:
			_str = _nmosStringInterlaceModes;
			break;
		case NMOS_STR_T_TRANSFER_CHARACTER:
			_str = _nmosStringTransferCharacters;
			break;
		case NMOS_STR_T_COLOR_SPACE:
			_str = _nmosStringColorSpaces;
			break;
		case NMOS_STR_T_VIDEO_COMPONENT:
			_str = _nmosStringVideoComponents;
			break;
		case NMOS_STR_T_CONN_ACTIVATE:
			_str = _nmosStringConnActivates;
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

#endif


