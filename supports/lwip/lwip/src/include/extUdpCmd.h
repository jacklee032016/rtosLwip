
#ifndef	__EXT_UDP_CMD_H__
#define	__EXT_UDP_CMD_H__

#define		MUX_CTRL_PORT							3600

#define		CMD_TAG_REQUEST						(0xa0fa)		/* request */
#define		CMD_TAG_RESPONSE						(0xa0fb)		/* response */


#define		MUX_IPCMD_DEBUG						MUX_DBG_ON


#define	MUX_IP_CMD_MAX_LENGTH					1020

#define	IPCMD_HEADER_LENGTH						4

struct	_cmn_mux_ip_cmd
{
	unsigned short		tag;
	unsigned short		length;
	
	unsigned char			data[MUX_IP_CMD_MAX_LENGTH];	/* it should be 65531 to optimize memory allocation */
}__attribute__ ((packed));


typedef struct _cmn_mux_ip_cmd CMN_IP_COMMAND;

/* IP command lists */
#define	MUX_IPCMD_CMD_GET_PARAMS				"get_param"
#define	MUX_IPCMD_CMD_SET_PARAMS				"set_param"

#define	MUX_IPCMD_CMD_SEND_RS232				"send_data_rs232"
#define	MUX_IPCMD_CMD_SECURITY_CHECK			"security_check"


/* fields in IP command */
#define	MUX_IPCMD_KEY_TARGET						"targ"
#define	MUX_IPCMD_KEY_COMMAND					"cmd"

#define	MUX_IPCMD_LOGIN_ACK						"login-ack" /* login user name or status */
#define	MUX_IPCMD_PWD_MSG						"pwd-msg" /* login password or status msg */

#define	MUX_IPCMD_DATA_ARRAY					"data" /* login password or status msg */


/* fields in Data array */
#define	MUX_IPCMD_DATA_P_NAME					"pName"
#define	MUX_IPCMD_DATA_C_NAME					"cName"		/* customer name */
#define	MUX_IPCMD_DATA_MODEL					"model"
#define	MUX_IPCMD_DATA_FW_VER					"fwVer"

#define	MUX_IPCMD_DATA_MAC						"mac"
#define	MUX_IPCMD_DATA_IP							"ip"
#define	MUX_IPCMD_DATA_MASK						"mask"
#define	MUX_IPCMD_DATA_GATEWAY					"gateway"
#define	MUX_IPCMD_DATA_DHCP						"isDhcp"

#define	MUX_IPCMD_DATA_IS_MCAST					"isMCAST"
#define	MUX_IPCMD_DATA_MCAST_IP					"MCASTip"

#define	MUX_IPCMD_DATA_IS_DIP					"isDipOn"
#define	MUX_IPCMD_DATA_IS_RESET					"isReset"
#define	MUX_IPCMD_DATA_IS_REBOOT				"isReboot"

#define	MUX_IPCMD_DATA_RS_BAUDRATE				"RS232Baudrate"
#define	MUX_IPCMD_DATA_RS_DATABITS				"RS232Databits"
#define	MUX_IPCMD_DATA_RS_PARITY					"RS232Parity"
#define	MUX_IPCMD_DATA_RS_STOPBITS				"RS232Stopbits"
#define	MUX_IPCMD_DATA_RS_FEEDBACKIP			"RS232FeedbackIp"


#define	MUX_IPCMD_DATA_VIDEO_PORT				"vidPort"
#define	MUX_IPCMD_DATA_AUDIO_PORT				"audPort"
#define	MUX_IPCMD_DATA_AD_PORT					"datPort"
#define	MUX_IPCMD_DATA_ST_PORT					"strPort"


#define	MUX_IPCMD_DATA_VIDEO_WIDTH				"vidW"
#define	MUX_IPCMD_DATA_VIDEO_HEIGHT				"vidH"
#define	MUX_IPCMD_DATA_VIDEO_FRAMERATE			"vidFps"
#define	MUX_IPCMD_DATA_VIDEO_COLORSPACE		"vidClrSpace"
#define	MUX_IPCMD_DATA_VIDEO_DEPTH				"vidDepth"

#define	MUX_IPCMD_DATA_VIDEO_INTERLACED		"vidIsIntlc"
#define	MUX_IPCMD_DATA_VIDEO_SEGMENTED			"vidIsSgmt"


#define	MUX_IPCMD_DATA_AUDIO_SAMPE_RATE		"audFrq"
#define	MUX_IPCMD_DATA_AUDIO_DEPTH				"audBit"
#define	MUX_IPCMD_DATA_AUDIO_CHANNELS			"audChan"

#define	MUX_IPCMD_DATA_IS_CONNECT				"IsConnect"	/* 811 send to TX(stop/start) or RX(connect/disconnect) */


#define	MUX_IPCMD_RS232_DATA_HEX				"hexdata"
#define	MUX_IPCMD_RS232_FEEDBACK				"isFeedback"
#define	MUX_IPCMD_RS232_WAIT_TIME				"waitTime"

/* Security Check */
#define	MUX_IPCMD_SC_GET_ID						"get_id"
#define	MUX_IPCMD_SC_SET_KEY						"set_key"
#define	MUX_IPCMD_SC_GET_STATUS					"get_status"


void muxIpCmdAgentInit(MUX_JSON_PARSER  *parser);


char muxJsonRequestParse(MUX_JSON_PARSER *parserr, MUX_RUNTIME_CFG	*runCfg);


char muxIpCmdResponseHeaderPrint(MUX_JSON_PARSER  *parser);
char muxIpCmdResponseTailCalculate(MUX_JSON_PARSER  *parser);

char	muxIpCmdResponseReply(MUX_JSON_PARSER  *parser);
char	muxJsonResponsePrintConfig(MUX_JSON_PARSER  *parser);


char muxIpCmdSecurityCheck(MUX_JSON_PARSER  *parser);
char muxIpCmdSendRsData(MUX_JSON_PARSER  *parser);

char muxIpCmdSetupParams(MUX_JSON_PARSER  *parser);

char muxJsonUserValidate(MUX_JSON_PARSER  *parser);

int	muxIpCmdPrintMediaCfg(MUX_JSON_PARSER  *parser, char *data, int size);
/* send IP command by TX */

char muxIpCmdRequestHeaderPrint(MUX_JSON_PARSER  *parser, const char *cmd);

char	muxIpCmdSendMediaData(MUX_JSON_PARSER  *parser);

char muxIpCmdSendout(MUX_JSON_PARSER  *parser, unsigned int *ip, unsigned short port);


char muxIpCmdIsLocal(MUX_JSON_PARSER  *parser);

#endif


