#ifndef	__EXT_NMOS_H__
#define	__EXT_NMOS_H__

#define	NMOS_MDNS_TYPE_NODE				"_nmos-node._tcp"
#define	NMOS_MDNS_TYPE_REGISTRATION		"_nmos-registration._tcp.local"
#define	NMOS_MDNS_TYPE_QUERY			"_nmos-query._tcp.local"

#define	NMOS_MDNS_NODE_SERVICE			"_nmos-node"

#define	NMOS_API_URI_ROOT					"/x-nmos/"

#define	NMOS_API_URI_NODE					NMOS_API_URI_ROOT"node"
#define	NMOS_API_URI_REGISTRATION		NMOS_API_URI_ROOT"registration"
#define	NMOS_API_URI_QUERY				NMOS_API_URI_ROOT"query"
#define	NMOS_API_URI_CONNECTION			NMOS_API_URI_ROOT"connection"



#define	NMOS_API_NAME_PROTOCOL			"api_proto"
#define	NMOS_API_NAME_VERSION			"api_ver"
#define	NMOS_API_NAME_PRIORITY			"pri"

#define	NMOS_API_PROTOCOL_HTTP			"http"
#define	NMOS_API_PROTOCOL_HTTPS			"https"

#define	NMOS_API_VERSION_10				"v1.0"
#define	NMOS_API_VERSION_11				"v1.1"
#define	NMOS_API_VERSION_12				"v1.2"
//#define	NMOS_API_VERSION_20				"v2.0"

#define	NMOS_X_MANU						"urn:x-manufacturer"	/* types for manufacturer extensions */
#define	NMOS_URN							"urn:x-nmos"

#define	NMOS_URN_FORMAT					NMOS_URN":format"
#define	NMOS_URN_FORMAT_VIDEO			NMOS_URN_FORMAT":video"
#define	NMOS_URN_FORMAT_AUDIO			NMOS_URN_FORMAT":audio"
#define	NMOS_URN_FORMAT_DATA			NMOS_URN_FORMAT":data"
#define	NMOS_URN_FORMAT_MUX				NMOS_URN_FORMAT":mux"

/* used in RECEIVER, */
#define	NMOS_MEDIA_TYPE_VIDEO_RAW			"video/raw"
#define	NMOS_MEDIA_TYPE_VIDEO_H264			"video/H264"
#define	NMOS_MEDIA_TYPE_VIDEO_VC2			"video/vc2"


#define	NMOS_MEDIA_TYPE_AUDIO_L24			"audio/L24"
#define	NMOS_MEDIA_TYPE_AUDIO_L20			"audio/L20"
#define	NMOS_MEDIA_TYPE_AUDIO_L16			"audio/L16"
#define	NMOS_MEDIA_TYPE_AUDIO_L8			"audio/L8"

#define	NMOS_MEDIA_TYPE_DATA_291			"video/smpte291"

#define	NMOS_MEDIA_TYPE_MUX_2022			"video/SMPTE2022-6"


/* Flow Video */
/* interlace mode */
#define	NMOS_VIDEO_ILM_S_PROGRESSIVE		"progressive"
#define	NMOS_VIDEO_ILM_S_TFF					"interlaced_tff"
#define	NMOS_VIDEO_ILM_S_BFF					"interlaced_bff"
#define	NMOS_VIDEO_ILM_S_PSF					"interlaced_psf"	/* pregressive segmented frame */


typedef	enum
{
	NMOS_VIDEO_ILM_T_PROGRESSIVE = 0,
	NMOS_VIDEO_ILM_T_TFF,
	NMOS_VIDEO_ILM_T_BFF,
	NMOS_VIDEO_ILM_T_PSF,
}NMOS_VIDEO_ILM_T;

/* colorspace */
#define	NMOS_VIDEO_CS_S_BT601				"BT601"
#define	NMOS_VIDEO_CS_S_BT709				"BT709"
#define	NMOS_VIDEO_CS_S_BT2020				"BT2020"
#define	NMOS_VIDEO_CS_S_BT2100				"BT2100"


typedef	enum
{
	NMOS_VIDEO_CS_T_BT601 = 0,
	NMOS_VIDEO_CS_T_BT709,
	NMOS_VIDEO_CS_T_BT2022,
	NMOS_VIDEO_CS_T_BT2100,
}NMOS_VIDEO_CS_T;


/* transfer_characteristic */
#define	NMOS_VIDEO_TC_S_SDR				"SDR"
#define	NMOS_VIDEO_TC_S_HLG				"HLG"
#define	NMOS_VIDEO_TC_S_PQ				"PQ"


typedef	enum
{
	NMOS_VIDEO_TC_T_SDR = 0,
	NMOS_VIDEO_TC_T_HLG,
	NMOS_VIDEO_TC_T_PQ
}NMOS_VIDEO_TC_T;


/* component type */
#define	NMOS_VIDEO_CMP_TYPE_S_Y					"Y"
#define	NMOS_VIDEO_CMP_TYPE_S_CB				"Cb"
#define	NMOS_VIDEO_CMP_TYPE_S_CR				"Cr"
#define	NMOS_VIDEO_CMP_TYPE_S_I					"I"
#define	NMOS_VIDEO_CMP_TYPE_S_CT				"Ct"
#define	NMOS_VIDEO_CMP_TYPE_S_CP				"Cp"
#define	NMOS_VIDEO_CMP_TYPE_S_A					"A"
#define	NMOS_VIDEO_CMP_TYPE_S_R					"R"
#define	NMOS_VIDEO_CMP_TYPE_S_G					"G"
#define	NMOS_VIDEO_CMP_TYPE_S_B					"B"
#define	NMOS_VIDEO_CMP_TYPE_S_DEPTH_MAP		"DepthMap"


typedef	enum
{
	NMOS_VIDEO_CMP_TYPE_T_Y = 0,
	NMOS_VIDEO_CMP_TYPE_T_CB,
	NMOS_VIDEO_CMP_TYPE_T_CR,
	NMOS_VIDEO_CMP_TYPE_T_I,
	NMOS_VIDEO_CMP_TYPE_T_CT,
	NMOS_VIDEO_CMP_TYPE_T_CP,
	NMOS_VIDEO_CMP_TYPE_T_A,
	NMOS_VIDEO_CMP_TYPE_T_R,
	NMOS_VIDEO_CMP_TYPE_T_G,
	NMOS_VIDEO_CMP_TYPE_T_B,
	NMOS_VIDEO_CMP_TYPE_T_DEPTH_MAP
}NMOS_VIDEO_CMP_TYPE_T;

#define	NMOS_CONN_ACTIVATE_S_IMMEDIATE			"activate_immediate"
#define	NMOS_CONN_ACTIVATE_S_ABSOLUTE			"activate_scheduled_absolute"
#define	NMOS_CONN_ACTIVATE_S_RELATIVE			"activate_scheduled_relative"

typedef	enum
{
	NMOS_CONN_ACTIVATE_T_IMMEDIATE = 0,
	NMOS_CONN_ACTIVATE_T_ABSOLUTE,	
	NMOS_CONN_ACTIVATE_T_RELATIVE,	
}NMOS_CONN_ACTIVATE_T;


typedef	enum
{
	NMOS_SOURCE_FORMAT_VIDEO = 0,
	NMOS_SOURCE_FORMAT_AUDIO,
	NMOS_SOURCE_FORMAT_DATA,
	NMOS_SOURCE_FORMAT_MUX,
}NMOS_SOURCE_FORMAT;

typedef	enum
{
	NMOS_TRANS_T_RTP = 0,
	NMOS_TRANS_T_UCAST,
	NMOS_TRANS_T_MCAST,
	NMOS_TRANS_T_DASH,
}NMOS_TRANSPORT_TYPE;


typedef	enum
{
	NMOS_CLOCK_INTERNAL	= 0,
	NMOS_CLOCK_PTP,
	NMOS_CLOCK_NO_CLOCK
}NMOS_CLOCK_TYPE;

typedef	enum
{
	NMOS_AUDIO_CHAN_T_L = 0,		/* left */
	NMOS_AUDIO_CHAN_T_R,			/* right */
	NMOS_AUDIO_CHAN_T_C,			/* center */
	NMOS_AUDIO_CHAN_T_LFE,		/* low frequenct effects */
	NMOS_AUDIO_CHAN_T_LS,		/* left suround */
	NMOS_AUDIO_CHAN_T_RS,		/* right surround */
	NMOS_AUDIO_CHAN_T_LSS,		/* left side surround */
	NMOS_AUDIO_CHAN_T_RSS,		/* right side srround  */
	NMOS_AUDIO_CHAN_T_LRS,		/* left Rear surround */
	NMOS_AUDIO_CHAN_T_RRS,		/* right rear surround */

	NMOS_AUDIO_CHAN_T_LC,		/* left center */
	NMOS_AUDIO_CHAN_T_RC,		/* right center */
	NMOS_AUDIO_CHAN_T_CS,		/* center surround */
	NMOS_AUDIO_CHAN_T_HI,		/* Hearing impaired  */
	NMOS_AUDIO_CHAN_T_VIN,		/* visually impaired  */
	
	NMOS_AUDIO_CHAN_T_M1,		/* Mono one */
	NMOS_AUDIO_CHAN_T_M2,		/* Mono two */
	NMOS_AUDIO_CHAN_T_LT,		/* left total */
	NMOS_AUDIO_CHAN_T_RT,		/* right total */
	NMOS_AUDIO_CHAN_T_LST,		/* left surround total */
	NMOS_AUDIO_CHAN_T_RST,		/* right surround total  */
	NMOS_AUDIO_CHAN_T_S,			/* surround */
	NMOS_AUDIO_CHAN_T_NSC,			/* Numbered Source Channel */
	NMOS_AUDIO_CHAN_T_UNDEFINE,		/* undefined */
}NMOS_AUDIO_CHAN_T;



typedef	enum
{
	NMOS_STR_T_SOURCE_FORMAT = 0,
	NMOS_STR_T_AUDIO_CHANNEL,
	NMOS_STR_T_TRANSPORT,
	NMOS_STR_T_INTERLACE_MODE,
	NMOS_STR_T_TRANSFER_CHARACTER,
	NMOS_STR_T_COLOR_SPACE,
	NMOS_STR_T_VIDEO_COMPONENT,
	NMOS_STR_T_CONN_ACTIVATE,
	
}NMOS_STR_TYPE;

#define	NMOS_FIND_SOURCE_FORMAT(type)		\
	extNmosStringFindFormat(NMOS_STR_T_SOURCE_FORMAT, (type) )

	
#define	NMOS_FIND_CHANNEL_SYMBOL(type)		\
	extNmosStringFindFormat(NMOS_STR_T_AUDIO_CHANNEL, (type) )


#define	NMOS_FIND_TRANSPORT(type)		\
	extNmosStringFindFormat(NMOS_STR_T_TRANSPORT, (type) )


#define	NMOS_FIND_INTERLACE_MODE(type)		\
	extNmosStringFindFormat(NMOS_STR_T_INTERLACE_MODE, (type) )

#define	NMOS_FIND_TRANSFER_CHARC(type)		\
	extNmosStringFindFormat(NMOS_STR_T_TRANSFER_CHARACTER, (type) )


#define	NMOS_FIND_COLOR_SPACE(type)		\
	extNmosStringFindFormat(NMOS_STR_T_COLOR_SPACE, (type) )


#define	NMOS_FIND_VIDEO_COMPONENT(type)		\
	extNmosStringFindFormat(NMOS_STR_T_VIDEO_COMPONENT, (type) )

#define	NMOS_FIND_CONN_ACTIVATE(type)		\
	extNmosStringFindFormat(NMOS_STR_T_CONN_ACTIVATE, (type) )


#define	NMOS_URN_TRANSPORT				NMOS_URN":transport"
#define	NMOS_URN_TRANSPORT_RTP			NMOS_URN_TRANSPORT":rtp"
#define	NMOS_URN_TRANSPORT_RTP_MCAST	NMOS_URN_TRANSPORT_RTP":mcast"
#define	NMOS_URN_TRANSPORT_RTP_UCAST	NMOS_URN_TRANSPORT_RTP":ucast"
#define	NMOS_URN_TRANSPORT_DASH			NMOS_URN_TRANSPORT":dash"


#define	NMOS_URN_DEVICE_TYPE				NMOS_URN":device"
#define	NMOS_URN_DEVICE_TYPE_GENERIC	NMOS_URN_DEVICE_TYPE":generic"
#define	NMOS_URN_DEVICE_TYPE_PIPELINE	NMOS_URN_DEVICE_TYPE":pipeline"


#define	NMOS_URN_CONTROL					NMOS_URN":control"
#define	NMOS_URN_CONTROL_SRCTRL			NMOS_URN_CONTROL":sr-ctrl/v1.0"

#define	NMOS_X_MANU_CONTROL				NMOS_X_MANU":control"
#define	NMOS_X_MANU_CONTROL_GENERIC	NMOS_X_MANU_CONTROL":generic"
#define	NMOS_X_MANU_CONTROL_LEGACY		NMOS_X_MANU_CONTROL":legacy"


/* label string for output */
/* resource ID for all NMOS object */
#define	NMOS_LABEL_DESCRIPTION			"description"
#define	NMOS_LABEL_ID						"id"
#define	NMOS_LABEL_LABEL					"label"
#define	NMOS_LABEL_TAGS					"tags"
#define	NMOS_LABEL_VERSION				"version"


#define	NMOS_LABEL_ACTIVE					"active"
#define	NMOS_LABEL_ACTIVATION			"activation"
#define	NMOS_LABEL_ACTIVATION_TIME		"activation_time"

#define	NMOS_LABEL_BIT_DEPTH				"bit_depth"
#define	NMOS_LABEL_CLOCK_NAME			"clock_name"
#define	NMOS_LABEL_COLOR_SPACE			"colorspace"
#define	NMOS_LABEL_COMPONENTS			"components"
#define	NMOS_LABEL_DENOMINATOR			"denominator"
#define	NMOS_LABEL_DEST_IP				"destination_ip"
#define	NMOS_LABEL_DEST_PORT				"destination_port"
#define	NMOS_LABEL_FORMAT				"format"

#define	NMOS_LABEL_FRAME_HEIGHT			"frame_height"
#define	NMOS_LABEL_FRAME_WIDTH			"frame_width"

#define	NMOS_LABEL_HEIGHT				"height"
#define	NMOS_LABEL_HREF					"href"
#define	NMOS_LABEL_GRAIN_RATE			"grain_rate"
#define	NMOS_LABEL_NAME					"name"
#define	NMOS_LABEL_NUMERATOR			"numerator"

#define	NMOS_LABEL_DEVICE_ID				"device_id"

#define	NMOS_LABEL_FALSE					"false"

/* Forward Error Correction */
#define	NMOS_LABEL_FEC_ENABLED				"fec_enabled"
#define	NMOS_LABEL_FEC_DEST_IP				"fec_destination_ip"
#define	NMOS_LABEL_FEC_TYPE					"fec_type"
#define	NMOS_LABEL_FEC_MODE					"fec_mode"
#define	NMOS_LABEL_FEC_BLOCK_WIDTH			"fec_block_width"
#define	NMOS_LABEL_FEC_BLOCK_HEIGHT			"fec_block_height"

#define	NMOS_LABEL_FEC_1D_DEST_PORT			"fec1D_destination_port"
#define	NMOS_LABEL_FEC_1D_SRC_PORT			"fec1D_source_port"

#define	NMOS_LABEL_FEC_2D_DEST_PORT			"fec2D_destination_port"
#define	NMOS_LABEL_FEC_2D_SRC_PORT			"fec2D_source_port"

#define	NMOS_LABEL_FLOW_ID				"flow_id"
#define	NMOS_LABEL_RECEIVER_ID			"receiver_id"
#define	NMOS_LABEL_SOURCE_ID				"source_id"
#define	NMOS_LABEL_SENDER_ID				"sender_id"
#define	NMOS_LABEL_INTERFACE_BINDS		"interface_bindings"
#define	NMOS_LABEL_INTERFACE_IP			"interface_ip"

#define	NMOS_LABEL_INTERLACE_MODE		"interlace_mode"

#define	NMOS_LABEL_MASTER_EANBLE		"master_enable"
#define	NMOS_LABEL_MAXIMUM				"maximum"
#define	NMOS_LABEL_MANIFEST_HREF		"manifest_href"

#define	NMOS_LABEL_MEDIA_TYPE			"media_type"
#define	NMOS_LABEL_MEDIA_TYPES			"media_types"
#define	NMOS_LABEL_MINIMUM				"minimum"

#define	NMOS_LABEL_MODE					"mode"

#define	NMOS_LABEL_MULTICAST_IP			"multicast_ip"

#define	NMOS_LABEL_NODE_ID				"node_id"
#define	NMOS_LABEL_PARENTS				"parents"
#define	NMOS_LABEL_REQUESTED_TIME		"requested_time"

#define	NMOS_LABEL_RTP_ENABLED			"rtp_enabled"
#define	NMOS_LABEL_RTCP_ENABLED			"rtcp_enabled"
#define	NMOS_LABEL_RTCP_DEST_IP			"rtcp_destination_ip"
#define	NMOS_LABEL_RTCP_DEST_PORT		"rtcp_destination_port"

#define	NMOS_LABEL_SAMPLE_RATE			"sample_rate"
#define	NMOS_LABEL_SOURCE_IP				"source_ip"
#define	NMOS_LABEL_SOURCE_PORT			"source_port"
#define	NMOS_LABEL_SUBSCRIPTION			"subscription"
#define	NMOS_LABEL_SYMBOL				"symbol"
#define	NMOS_LABEL_TRANSFER_CHARC		"transfer_characteristic"
#define	NMOS_LABEL_TRANSPORT				"transport"
#define	NMOS_LABEL_TRANSPORT_PARAMS	"transport_params"


#define	NMOS_LABEL_TRUE					"true"

#define	NMOS_LABEL_TYPE					"type"

#define	NMOS_LABEL_WIDTH					"width"

/* plural labels */
#define	NMOS_LABEL_VERSIONS				"versions"
#define	NMOS_LABEL_CAPS					"caps"
#define	NMOS_LABEL_CHANNELS				"channels"
#define	NMOS_LABEL_CLOCKS				"clocks"

#define	NMOS_LABEL_CONTROLS				"controls"
#define	NMOS_LABEL_RECEIVERS				"receivers"
#define	NMOS_LABEL_SENDERS				"senders"
#define	NMOS_LABEL_FLOWS					"flows"


#define	NMOS_LABEL_CLOCK_INDEX			"clk"


typedef	enum
{
	NMOS_API_T_REGISTRATION = 0,
	NMOS_API_T_QUERY,
	NMOS_API_T_NODE,
	NMOS_API_T_CONNECTION,

	NMOS_API_T_WEB_SERVICE,	/* web page service */
	NMOS_API_T_UNKNOWN
}NMOS_API_TYPE;


typedef	enum
{
	NMOS_API_VER_F_10 = 0,
	NMOS_API_VER_F_11,
	NMOS_API_VER_F_12,
	NMOS_API_VER_F_20,
	NMOS_API_VER_F_UNKNOWN
}NMOS_API_VER_F;	/* version flag */


typedef	enum
{
	NMOS_API_PROTO_F_HTTP = 0,
	NMOS_API_PROTO_F_HTTPS,
	NMOS_API_PROTO_F_WEB_SOCKET,
	NMOS_API_PROTO_F_UNKNOWN
}NMOS_API_PROTO_F;	/* protocol flag */



#define	_NMOS_VERSION_SET(apiIf, ver)	\
	(CFG_SET_FLAGS((apiIf)->versionFlags, (ver)))

/* version */
#define	NMOS_VERSION_SET10(apiIf)	\
	(_NMOS_VERSION_SET( (apiIf), (1<<NMOS_API_VER_F_10)))


#define	NMOS_VERSION_SET11(apiIf)	\
	(_NMOS_VERSION_SET( (apiIf), (1<<NMOS_API_VER_F_11)))


#define	NMOS_VERSION_SET12(apiIf)	\
	(_NMOS_VERSION_SET( (apiIf), (1<<NMOS_API_VER_F_12)))


#define	NMOS_VERSION_SET20(apiIf)	\
	(_NMOS_VERSION_SET( (apiIf), (1<<NMOS_API_VER_F_20)))


#define	NMOS_VERSION_IS_10(apiIf)	\
			((apiIf)->versionFlags & (1<<NMOS_API_VER_F_10) )

#define	NMOS_VERSION_IS_11(apiIf)	\
			((apiIf)->versionFlags & (1<<NMOS_API_VER_F_11) )

#define	NMOS_VERSION_IS_12(apiIf)	\
			((apiIf)->versionFlags & (1<<NMOS_API_VER_F_12) )


#define	NMOS_VERSION_IS_20(apiIf)	\
			((apiIf)->versionFlags & (1<<NMOS_API_VER_F_20) )


/* protocol */

#define	_NMOS_PROTOCOL_SET(apiIf, proto)	\
	(CFG_SET_FLAGS((apiIf)->protoFlags, (proto)))


#define	NMOS_PROTOCOL_SET_HTTP(apiIf)	\
	(_NMOS_PROTOCOL_SET( (apiIf), (1<<NMOS_API_PROTO_F_HTTP)))


#define	NMOS_PROTOCOL_SET_HTTPS(apiIf)	\
	(_NMOS_PROTOCOL_SET( (apiIf), (1<<NMOS_API_PROTO_F_HTTPS)))


#define	NMOS_PROTOCOL_SET_WEB_SOCKET(apiIf)	\
	(_NMOS_PROTOCOL_SET( (apiIf), (1<<NMOS_API_PROTO_F_WEB_SOCKET)))


#define	NMOS_PROTOCOL_IS_HTTP(apiIf)	\
			((apiIf)->protoFlags & (1<<NMOS_API_PROTO_F_HTTP) )

#define	NMOS_PROTOCOL_IS_HTTPS(apiIf)	\
			((apiIf)->protoFlags & (1<<NMOS_API_PROTO_F_HTTPS) )


#define	NMOS_PROTOCOL_IS_WEB_SOCKET(apiIf)	\
			((apiIf)->protoFlags & (1<<NMOS_API_PROTO_F_WEB_SOCKET) )


#define		EXT_JSON_BOOL_STR( _bool)	\
				((_bool==0)?NMOS_LABEL_FALSE:NMOS_LABEL_TRUE)

typedef	struct
{
	NMOS_API_TYPE	type;
	
	/* from RR TXT: only HTTP is support */
	unsigned char		protoFlags;
	unsigned 	char		versionFlags;
	unsigned char		priority;

	/* parsed from RR SRV */
	unsigned short	port;
	unsigned int		ip;
	char				hostname[128];	/* use to lookup IP address */

	/* parse from in RR PTR, name of service */
	char				name[128];
	
	char				*service;	/* save constant string of service access point */
}NMOS_API_INTERFACE;

struct _MuxHttpConn;

typedef char (*MuxHttpCallback)(struct _MuxHttpConn  *mhc, void *data);


struct _ApiAcceePoint;
typedef	struct _ApiAcceePoint
{
	unsigned char						type;
	const char						*name;

	MuxHttpCallback					callback;

	const struct _ApiAcceePoint		*child;
	const struct _ApiAcceePoint		*next;
}ApiAccessPoint;


/*********************   Node definitions **************/

#define	NMOS_NODE_URL_STR_SELF				"self"
#define	NMOS_NODE_URL_STR_DEVICES			"devices"
#define	NMOS_NODE_URL_STR_SOURCES			"sources"
#define	NMOS_NODE_URL_STR_FLOWS				"flows"
#define	NMOS_NODE_URL_STR_SENDERS			"senders"
#define	NMOS_NODE_URL_STR_RECEIVERS			"receivers"
#define	NMOS_NODE_URL_STR_TARGET			"target"


#define	NMOS_CONN_URL_STR_BULK				"bulk"
#define	NMOS_CONN_URL_STR_SINGLE			"single"

#define	NMOS_CONN_URL_STR_CONSTRAINTS		"constraints"
#define	NMOS_CONN_URL_STR_STAGED			"staged"
#define	NMOS_CONN_URL_STR_ACTIVE			"active"
#define	NMOS_CONN_URL_STR_TRANSPORT_FILE	"transportfile"

typedef enum
{
	NMOS_NODE_URL_T_SELF = 0,
	NMOS_NODE_URL_T_DEVICES,
	NMOS_NODE_URL_T_SOURCES,
	NMOS_NODE_URL_T_FLOWS,
	NMOS_NODE_URL_T_SENDERS,
	NMOS_NODE_URL_T_RECEIVERS,
	NMOS_NODE_URL_T_TARGET,
	NMOS_NODE_URL_T_UNKNOWN
}NMOS_NODE_URL_T;


typedef enum
{
	NMOS_CONN_URL_T_SELF = 0,
	NMOS_CONN_URL_T_BULK,
	NMOS_CONN_URL_T_BULK_SENDERS,
	NMOS_CONN_URL_T_BULK_RECEIVERS,
	NMOS_CONN_URL_T_SINGLE,
	NMOS_CONN_URL_T_SINGLE_SENDOERS,
	NMOS_CONN_URL_T_SINGLE_SENDOERS_CONSTRAINTS,
	NMOS_CONN_URL_T_SINGLE_SENDOERS_STAGED,
	NMOS_CONN_URL_T_SINGLE_SENDOERS_ACTIVE,
	NMOS_CONN_URL_T_SINGLE_SENDOERS_TRANSPORTFILE,
	NMOS_CONN_URL_T_SINGLE_RECEIVERS,
	NMOS_CONN_URL_T_SINGLE_RECEIVERS_CONSTRAINTS,
	NMOS_CONN_URL_T_SINGLE_RECEIVERS_STAGED,
	NMOS_CONN_URL_T_SINGLE_RECEIVERS_ACTIVE,
	NMOS_CONN_URL_T_UNKNOWN
}NMOS_CONN_URL_T;

#define	NMOS_NAME_LENGTH			32

/* resource_core.json */
/* abstract object for all NMOS object */
struct _MuxNmosResource;

struct	_NmosMediaType;

struct	_MuxNmosReceiver;
struct	_MuxNmosSender;
struct	_MuxNmosFlow;
struct	_MuxNmosSource;
struct	_MuxNmosDevice;
struct 	_MuxNmosNode;

typedef	struct _MuxNmosResource
{
	MuxNmosID						nmosId;

	char								label[NMOS_NAME_LENGTH];
	char								description[NMOS_NAME_LENGTH];

	void								*tags;

	struct _MuxNmosNode				*node;
	struct _MuxNmosResource			*next;
}MuxNmosResource;

typedef	struct
{
	unsigned	char		denominator;
	unsigned short	numberator;		/* frame rate */
}grain_rate;


/* object for connection management */
typedef	struct
{
	unsigned short				sourcePortMini;
	unsigned short				destPortMini;
	unsigned short				portRange;

	unsigned char					rtpEnabled;
	unsigned char					rtcpEnabled;
	unsigned char					fecEnabled;
	
	struct _MuxNmosResource		*parent;
}MuxNmosConstraints;



/* only used in RECEIVER */
typedef	struct	_NmosMediaType
{
	const char				*type;
	struct _NmosMediaType	*next;
}NmosMediaType;

typedef	struct	_MuxNmosSender
{
	MuxNmosResource					resourceId;

	NMOS_TRANSPORT_TYPE			transport;

	char								*manifest_href;	/* URI for sender */
	const char						*interface_binding;

	MuxNmosConstraints				constraints;

	struct _MuxNmosSender			*next;

	struct _MuxNmosDevice			*device;
	struct _MuxNmosFlow				*flow;

	struct _MuxNmosReceiver			*subscriber;		/* NMOS unicast receiver, subscribed to this sender */
	unsigned char						enabled;
}MuxNmosSender;


typedef	struct	_MuxNmosReceiver
{
	MuxNmosResource					resourceId;

	NMOS_TRANSPORT_TYPE			transport;
	const char						*interface_binding;
	NMOS_SOURCE_FORMAT			format;

	const NmosMediaType				*caps;

	MuxNmosConstraints				constraints;

	struct _MuxNmosReceiver			*next;

	struct _MuxNmosDevice			*device;
	
	struct _MuxNmosSender			*subscriber;		/* NMOS unicast receiver, subscribed to this sender */
	unsigned char						enabled;
}MuxNmosReceiver;

/* used by FLOW */
struct _NmosVideoComponent;

typedef	struct _NmosVideoComponent
{
	NMOS_VIDEO_CMP_TYPE_T				name;
	
	unsigned short						width;
	unsigned short						height;
	unsigned char							bitDepth;

	const struct _NmosVideoComponent		*next;
}NmosVideoComponent;

/* used by RESOURCE */
struct _NmosAudioChannel;

typedef	struct _NmosAudioChannel
{
	char							label[NMOS_NAME_LENGTH];
	NMOS_AUDIO_CHAN_T			symbol;

	struct _NmosAudioChannel		*next;
}NmosAudioChannel;


typedef	struct	_MuxNmosFlow
{
	MuxNmosResource					resourceId;
	NMOS_SOURCE_FORMAT			format;
	char								mediaType[NMOS_NAME_LENGTH];
	void								*parents;
	grain_rate						gRate;	/* Number of Grains per second, options, from flow_core, for periodic flow */

	/* video specific */
	NMOS_VIDEO_CS_T				colorspace;
	unsigned short					frameWidth;
	unsigned short					frameHeight;

	/* optional fields */
	NMOS_VIDEO_ILM_T				interlaceMode;	
	NMOS_VIDEO_TC_T				transferCharacter;
	const NmosVideoComponent			*components;		/* raw video */

	/* audio specific */
	grain_rate						sampleRate;
	unsigned short					bitDepth;		/* raw audio */

	struct _MuxNmosSource			*source;
	struct _MuxNmosSender			*sender;
	
	struct _MuxNmosFlow				*next;
}MuxNmosFlow;


typedef	struct	_MuxNmosSource
{
	MuxNmosResource					resourceId;

	struct _MuxNmosFlow				*flow;
	
	/* 4 source core fields*/
	struct _MuxNmosDevice			*device;
	void								*parent;
	void								*caps;		/* TBD */
	unsigned char						clockIndex;
	
	/* */
	NMOS_SOURCE_FORMAT			format;
	
	grain_rate						gRate;
	NmosAudioChannel					*audioChannels;

	struct _MuxNmosSource			*next;
}MuxNmosSource;


typedef	struct _MuxNmosDevice
{
	MuxNmosResource					resourceId;

	struct	_MuxNmosSender		*senders;
	struct	_MuxNmosReceiver		*receivers;

	struct	_MuxNmosFlow		*flows;	/* not defined in model, add to refer in app */

	/* following fields are not return in API of device */
	struct	_MuxNmosSource		*sources;	
	struct	_MuxNmosNode		*node;
}MuxNmosDevice;


typedef	struct _MuxNmosNode
{
	MuxNmosResource					resourceId;
	
	unsigned char						versionFlags;	/* API version node support */

	MuxNmosDevice					*device;

	/* API interface of RDS */
	NMOS_API_INTERFACE				registrationApi;
	NMOS_API_INTERFACE				queryApi;


	EXT_RUNTIME_CFG				*runCfg;
}MuxNmosNode;



char extNmosPostDataBegin(void *conn, unsigned char *data, unsigned short len);
char extNmosPostDataRecv(void *connection, struct pbuf *p);
void extNmosPostDataFinished(void *conn);

void	extNmosNodeInit(MuxNmosNode	*node, EXT_RUNTIME_CFG *runCfg);


const char *extNmosStringFindFormat(NMOS_STR_TYPE strType, unsigned short type);

int extNmosSdpMediaHander(char *data, unsigned int size, MuxNmosSender *snd);



extern	MuxNmosNode		nmosNode;

extern	const ApiAccessPoint	apConnRoot;
extern	const ApiAccessPoint	apNodeRoot;


#endif

