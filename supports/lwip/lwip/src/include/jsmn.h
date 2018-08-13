#ifndef __JSMN_H__
#define __JSMN_H__

#include <stddef.h>


#pragma		pack(1)

/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Other primitive: number, boolean (true/false) or null
 */
typedef enum
{
	JSMN_UNDEFINED	= 0,
	JSMN_OBJECT,
	JSMN_ARRAY,
	JSMN_STRING,
	JSMN_PRIMITIVE
}jsmntype_t;

enum jsmnerr
{
	/* Not enough tokens were provided */
	JSMN_ERROR_NOMEM = -1,
	/* Invalid character inside JSON string */
	JSMN_ERROR_INVAL = -2,
	/* The string is not a full JSON packet, more bytes expected */
	JSMN_ERROR_PART = -3
};

/**
 * JSON token description.
 * type		type (object, array, string etc.)
 * start	start position in JSON data string
 * end		end position in JSON data string
 */
typedef struct
{
	jsmntype_t	type;
	
	int			start;
	int			end;
	int			size;
#ifdef JSMN_PARENT_LINKS
	int			parent;
#endif
} jsmntok_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct
{
	unsigned int	pos;		/* offset in the JSON string */
	unsigned int	toknext;		/* next token to allocate */
	int			toksuper;	/* superior token node, e.g parent object or array */
} jsmn_parser;

/* Create JSON parser over an array of tokens */
void jsmn_init(jsmn_parser *parser);

/**
 * Run JSON parser. It parses a JSON data string into and array of tokens, each describing a single JSON object.
 */
int jsmn_parse(jsmn_parser *parser, const char *js, size_t len, jsmntok_t *tokens, unsigned int num_tokens);


/*! \public
 * @brief IoT Error enum
 *
 * Enumeration of return values from the IoT_* functions within the SDK.
 */
typedef enum {
	/** Return value of yield function to indicate auto-reconnect was successful */
	RECONNECT_SUCCESSFUL = 1,
	/** Success return value - no error occurred. */
	NONE_ERROR = 0,
	/** A generic error.  A placeholder for a more specific error. */
	GENERIC_ERROR = -1,
	/** A required parameter was passed as null. */
	NULL_VALUE_ERROR = -2,
	/** A connection could not be established. */
	CONNECTION_ERROR = -3,
	/** The subscribe failed.  A SUBACK was not returned from the service. */
	SUBSCRIBE_ERROR = -4,
	/** The publish failed.  In the case of a QoS 1 message a PUBACK was not received. */
	PUBLISH_ERROR = -5,
	/** The disconnect failed.  The disconnect control packet could not be sent. */
	DISCONNECT_ERROR = -6,
	/** An error occurred when yielding to the IoT MQTT client.  A possible cause is an unexpected TCP socket disconnect. */
	YIELD_ERROR = -7,
	/** The TCP socket could not be established. */
	TCP_CONNECT_ERROR = -8,
	/** The TLS handshake failed. */
	SSL_CONNECT_ERROR = -9,
	/** Error associated with setting up the parameters of a Socket */
	TCP_SETUP_ERROR =-10,
	/** A timeout occurred while waiting for the TLS handshake to complete. */
	SSL_CONNECT_TIMEOUT_ERROR = -11,
	/** A Generic write error based on the platform used */
	SSL_WRITE_ERROR = -12,
	/** SSL initialization error at the TLS layer */
	SSL_INIT_ERROR = -13,
	/** An error occurred when loading the certificates.  The certificates could not be located or are incorrectly formatted. */
	SSL_CERT_ERROR= -14,
	/** The unsubscribe failed.  The unsubscribe control packet could not be sent. */
	UNSUBSCRIBE_ERROR = -15,
	/** An error occurred while parsing the JSON string.  Usually malformed JSON. */
	JSON_PARSE_ERROR = -16,
	/** Shadow: The response Ack table is currently full waiting for previously published updates */
	WAIT_FOR_PUBLISH = -17,
	/** SSL Write times out */
	SSL_WRITE_TIMEOUT_ERROR = -18,
	/** SSL Read times out */
	SSL_READ_TIMEOUT_ERROR = -19,
	/** A Generic error based on the platform used */
	SSL_READ_ERROR = -20,
	/** Any time an snprintf writes more than size value, this error will be returned */
	SHADOW_JSON_BUFFER_TRUNCATED = -21,
	/** Any time an snprintf encounters an encoding error or not enough space in the given buffer */
	SHADOW_JSON_ERROR = -22,
	/** Returned when the Network is disconnected and reconnect is either disabled or physical layer is disconnected */
	NETWORK_DISCONNECTED = -23,
	/** Returned when the Network is disconnected and the reconnect attempt has timed out */
	NETWORK_RECONNECT_TIMED_OUT = -24,
	/** Returned when the Network is disconnected and the reconnect attempt is in progress */
	NETWORK_ATTEMPTING_RECONNECT = -25,
	/** Returned when the Network is already connected and a connection attempt is made */
	NETWORK_ALREADY_CONNECTED = -26,
	/** The MQTT RX buffer received corrupt message  */
	RX_MESSAGE_INVALID = -27,
	/** The MQTT RX buffer received a bigger message. The message will be dropped  */
	RX_MESSAGE_BIGGER_THAN_MQTT_RX_BUF = -28
}IoT_Error_t;


// utility functions
/**
 * @brief          JSON Equality Check
 *
 * Given a token pointing to a particular JSON node and an
 * input string, check to see if the key is equal to the string.
 *
 * @param json      json string
 * @param tok     	json token - pointer to key to test for equality
 * @param s			input string for key to test equality
 *
 * @return         	0 if equal, 1 otherwise
 */
char jsoneq(const char *json, jsmntok_t *tok, const char *s);

/**
 * @brief          Parse a signed 32-bit integer value from a JSON node.
 *
 * Given a JSON node parse the integer value from the value.
 *
 * @param jsonString	json string
 * @param tok     		json token - pointer to JSON node
 * @param i				address of int32_t to be updated
 *
 * @return         		NONE_ERROR - success
 * @return				JSON_PARSE_ERROR - error parsing value
 */
IoT_Error_t parseInteger32Value(int *i, const char *jsonString, jsmntok_t *token);

/**
 * @brief          Parse a signed 16-bit integer value from a JSON node.
 *
 * Given a JSON node parse the integer value from the value.
 *
 * @param jsonString	json string
 * @param tok     		json token - pointer to JSON node
 * @param i				address of int16_t to be updated
 *
 * @return         		NONE_ERROR - success
 * @return				JSON_PARSE_ERROR - error parsing value
 */
IoT_Error_t parseInteger16Value(short *i, const char *jsonString, jsmntok_t *token);

/**
 * @brief          Parse a signed 8-bit integer value from a JSON node.
 *
 * Given a JSON node parse the integer value from the value.
 *
 * @param jsonString	json string
 * @param tok     		json token - pointer to JSON node
 * @param i				address of int8_t to be updated
 *
 * @return         		NONE_ERROR - success
 * @return				JSON_PARSE_ERROR - error parsing value
 */
IoT_Error_t parseInteger8Value(char *i, const char *jsonString, jsmntok_t *token);

/**
 * @brief          Parse an unsigned 32-bit integer value from a JSON node.
 *
 * Given a JSON node parse the integer value from the value.
 *
 * @param jsonString	json string
 * @param tok     		json token - pointer to JSON node
 * @param i				address of uint32_t to be updated
 *
 * @return         		NONE_ERROR - success
 * @return				JSON_PARSE_ERROR - error parsing value
 */
IoT_Error_t parseUnsignedInteger32Value(unsigned int *i, const char *jsonString, jsmntok_t *token);

/**
 * @brief          Parse an unsigned 16-bit integer value from a JSON node.
 *
 * Given a JSON node parse the integer value from the value.
 *
 * @param jsonString	json string
 * @param tok     		json token - pointer to JSON node
 * @param i				address of uint16_t to be updated
 *
 * @return         		NONE_ERROR - success
 * @return				JSON_PARSE_ERROR - error parsing value
 */
IoT_Error_t parseUnsignedInteger16Value(unsigned short *i, const char *jsonString, jsmntok_t *token);

/**
 * @brief          Parse an unsigned 8-bit integer value from a JSON node.
 *
 * Given a JSON node parse the integer value from the value.
 *
 * @param jsonString	json string
 * @param tok     		json token - pointer to JSON node
 * @param i				address of uint8_t to be updated
 *
 * @return         		NONE_ERROR - success
 * @return				JSON_PARSE_ERROR - error parsing value
 */
IoT_Error_t parseUnsignedInteger8Value(unsigned char *i, const char *jsonString, jsmntok_t *token);

/**
 * @brief          Parse a float value from a JSON node.
 *
 * Given a JSON node parse the float value from the value.
 *
 * @param jsonString	json string
 * @param tok     		json token - pointer to JSON node
 * @param f				address of float to be updated
 *
 * @return         		NONE_ERROR - success
 * @return				JSON_PARSE_ERROR - error parsing value
 */
//IoT_Error_t parseFloatValue(float *f, const char *jsonString, jsmntok_t *token);

/**
 * @brief          Parse a double value from a JSON node.
 *
 * Given a JSON node parse the double value from the value.
 *
 * @param jsonString	json string
 * @param tok     		json token - pointer to JSON node
 * @param d				address of double to be updated
 *
 * @return         		NONE_ERROR - success
 * @return				JSON_PARSE_ERROR - error parsing value
 */
//IoT_Error_t parseDoubleValue(double *d, const char *jsonString, jsmntok_t *token);

/**
 * @brief          Parse a boolean value from a JSON node.
 *
 * Given a JSON node parse the boolean value from the value.
 *
 * @param jsonString	json string
 * @param tok     		json token - pointer to JSON node
 * @param b				address of boolean to be updated
 *
 * @return         		NONE_ERROR - success
 * @return				JSON_PARSE_ERROR - error parsing value
 */
IoT_Error_t parseBooleanValue(char *b, const char *jsonString, jsmntok_t *token);

/**
 * @brief          Parse a string value from a JSON node.
 *
 * Given a JSON node parse the string value from the value.
 *
 * @param jsonString	json string
 * @param tok     		json token - pointer to JSON node
 * @param s				address of string to be updated
 *
 * @return         		NONE_ERROR - success
 * @return				JSON_PARSE_ERROR - error parsing value
 */
IoT_Error_t parseStringValue(char *buf, const char *jsonString, jsmntok_t *token);


#define	JSON_TOKEN_LENGTH(token)	\
				((token)->end - (token)->start)



#define	JSON_DEBUG




#define	EXT_JSON_KEY_STATUS					"code" //"status"

#define	EXT_JSON_KEY_ERROR					"error"
#define	EXT_JSON_KEY_DEBUG					"debug"

/* only used in response */
#define	EXT_JSON_KEY_NAME					"name"
#define	EXT_JSON_KEY_MODEL					"model"
#define	EXT_JSON_KEY_VERSION					"version"

/* used both in request and response */
#define	EXT_JSON_KEY_COMMAND				"cmd"

#define	EXT_JSON_KEY_USER					"user"
#define	EXT_JSON_KEY_PASSWORD				"pwd"

#define	EXT_JSON_KEY_ID						"uuid"
#define	EXT_JSON_KEY_MAC						"mac"
#define	EXT_JSON_KEY_IP						"ip"
#define	EXT_JSON_KEY_MASK					"mask"
#define	EXT_JSON_KEY_GATEWAY				"gw"
#define	EXT_JSON_KEY_DHCP					"dhcp"
#define	EXT_JSON_KEY_TX						"tx"
#define	EXT_JSON_KEY_DEST_IP					"destIp"
#define	EXT_JSON_KEY_VIDEO_MAC_LOCAL		"videoMacLocal"
#define	EXT_JSON_KEY_VIDEO_MAC_DEST		"videoMacDest"
#define	EXT_JSON_KEY_VIDEO_IP_LOCAL			"videoIpLocal"
#define	EXT_JSON_KEY_VIDEO_IP_DEST			"videoIpDest"
#define	EXT_JSON_KEY_VIDEO_PORT_LOCAL		"videoPortLocal"
#define	EXT_JSON_KEY_VIDEO_PORT_DEST		"videoPortDest"

#define	EXT_JSON_KEY_AUDIO_PORT_LOCAL		"audioPortLocal"
#define	EXT_JSON_KEY_AUDIO_PORT_DEST		"audioPortDest"

#define	EXT_JSON_KEY_ANC_DT_PORT_LOCAL		"dataPortLocal"
#define	EXT_JSON_KEY_ANC_DT_PORT_DEST		"dataPortDest"

#define	EXT_JSON_KEY_ANC_ST_PORT_LOCAL		"streaPortLocal"
#define	EXT_JSON_KEY_ANC_ST_PORT_DEST		"streaPortDest"


#define	EXT_JSON_KEY_MC_IP					"mcIp"
#define	EXT_JSON_KEY_MC_PORT				"mcPort"

#define	EXT_JSON_KEY_IS_CONNECT				"isConnect"
#define	EXT_JSON_KEY_IS_MC					"isMc"

/* following is just configuration items, never send/receive */
#define	EXT_JSON_KEY_HTTP_PORT				"HttpPort"


#define	EXT_JSON_CMD_GET_PARAMS				"getParams"
#define	EXT_JSON_CMD_SET_PARAMS				"setParams"

#define	EXT_JSON_CMD_TRANSFER				"transfer"


#define	EXT_JSON_KEY_ACTION					"action"

#define	EXT_JSON_KEY_ACTION_START			"start"
#define	EXT_JSON_KEY_ACTION_STOP			"stop"



#define	EXT_JSON_MESSAGE_SIZE		64		/* it is also used in parsing request, the most length of request is UUID, 36 */
#define	EXT_JSON_TOKEN_SIZE			50*2
#define	EXT_JSON_OUT_BUF_SIZE		512*2


#define	JSON_TOKEN_COUNT(mParser)	\
		((mParser)->parser.toknext)


#define	MAC_ADDRESS_PRINT(buffer, size, index, mac) \
		{(index) += snprintf((buffer)+(index), (size)-(index), "\"%02x:%02x:%02x:%02x:%02x:%02x\",",  \
			(mac)->address[0] , (mac)->address[1] , (mac)->address[2] , (mac)->address[3], (mac)->address[4], (mac)->address[5] );}

#define	UUID_PRINT(buffer, size, index, guid)	\
		{(index) += snprintf((buffer)+(index), (size)-(index), "\"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\",",  \
			(guid)->uuid[0] , (guid)->uuid[1] , (guid)->uuid[2] , (guid)->uuid[3], \
			(guid)->uuid[4], (guid)->uuid[5], (guid)->uuid[6], (guid)->uuid[7], (guid)->uuid[8], (guid)->uuid[9], \
			(guid)->uuid[10], (guid)->uuid[11], (guid)->uuid[12], (guid)->uuid[13], (guid)->uuid[14], (guid)->uuid[15] );}


typedef	enum
{
	JSON_STATUS_OK = 200,

	JSON_STATUS_REDIRECT = 300,
	JSON_STATUS_NOT_FOUND = 400,

	JSON_STATUS_ERROR	= 500,
	
	JSON_STATUS_PRIVATE = 550, /* private status of MuxLab */
	JSON_STATUS_PARSE_ERROR,
	JSON_STATUS_PARSE_PARAM_ERROR,
	JSON_STATUS_TAG_ERROR,
	JSON_STATUS_CRC_ERROR,
	
	JSON_STATUS_CMD_EXEC_ERROR,

	JSON_STATUS_UNKNOWN

}JSON_STATUS_T;

#define	HEX_DATA_MAX_LENGTH		128

typedef	struct
{
	/* RS232 data */
	char				hexData[HEX_DATA_MAX_LENGTH]; /* string */
	unsigned char		isFeedBack;
	unsigned short	waitMs;

	char				scKey[EXT_SC_KEY_MAX_LENGTH];
	char				scID[EXT_SC_ID_MAX_LENGTH];
}MuxSetupData;

typedef struct
{
	jsmn_parser		parser;

	int				status;
	int				outSize;
	int				outIndex;
	
	char				outBuffer[EXT_JSON_OUT_BUF_SIZE];
	char				msg[EXT_JSON_MESSAGE_SIZE];

	EXT_UUID_T		uuid;

	EXT_MAC_ADDRESS	target;
	char					cmd[32];

	char					*currentJSonString;
	int					jsonLength;	

	jsmntok_t 			tokens[EXT_JSON_TOKEN_SIZE];
	int					tokenCount;

	EXT_RUNTIME_CFG	*runCfg;
	MuxSetupData		setupData;
}EXT_JSON_PARSER;


typedef int (* token_callback)(EXT_JSON_PARSER *parser, int index, jsmntok_t  *key, void *data);


#define	PARSE_IS_OK(parser)		\
			(parser->status == JSON_STATUS_OK)


const char *extJsonErrorMsg(short code);

void extJsonInit(EXT_JSON_PARSER  *parser, char *jsonStr, unsigned short size);

int	extJsonParse(EXT_JSON_PARSER  *parser, char *jStr, unsigned short size );

int extJsonEqual(EXT_JSON_PARSER  *parser, int index, jsmntok_t *token, void *data);
jsmntok_t *extJsonFindKeyToken(EXT_JSON_PARSER  *parser, const char *key);


int extJsonParseUnsignedChar(EXT_JSON_PARSER *parser, const char *key, unsigned char *value);
int extJsonParseUnsignedShort(EXT_JSON_PARSER *parser, const char *key, unsigned short *value);
int extJsonParseUnsignedInteger(EXT_JSON_PARSER *parser, const char *key, unsigned int *value);

int extJsonParseString(EXT_JSON_PARSER *parser, const char *key, char *retVal, int size);

int extJsonFindCommand(EXT_JSON_PARSER  *parser);

int extJsonParseIpAddress(EXT_JSON_PARSER *parser, const char *key, unsigned int *ipAddress);
int extJsonParseMacAddress(EXT_JSON_PARSER *parser, const char *key, EXT_MAC_ADDRESS *macAddress);

int extJsonParseUuid(EXT_JSON_PARSER *parser, const char *key, EXT_UUID_T *guid);




unsigned char  extJsonParseDataArray(EXT_JSON_PARSER *parser);

char extJsonRequestParseCommand(char *jsonRequest, unsigned short size, EXT_JSON_PARSER *parser);


void	extUuidGenerate(EXT_UUID_T *uuid, EXT_RUNTIME_CFG *runCfg);
char extUuidParse(EXT_UUID_T *uuid, char *strUuid);
char extUuidEqual(EXT_UUID_T *dest, EXT_UUID_T *src);
char *extUuidToString(EXT_UUID_T *uuid);


char extJsonHandle(EXT_JSON_PARSER  *parser);


#ifdef	X86

int extJsonPrint(EXT_JSON_PARSER *parser, jsmntok_t *t, size_t count, int indent, const char *prompt);
void extJsonDebug(EXT_JSON_PARSER *parser , EXT_RUNTIME_CFG *runCfg, const char *prompt);
void	extDebugCfg(EXT_RUNTIME_CFG *cfg, const char *prompt);

char	extSysParamsInit(EXT_RUNTIME_CFG *runCfg);
#endif


extern	const char *jsonGetParamsCmd;
//extern	const char *jsonErrorStatus;

extern	const char *jsonReplyParams;
extern	EXT_JSON_PARSER  extParser;

//extern	EXT_JSON_PARSER  *parser;


#endif /* __JSMN_H_ */

