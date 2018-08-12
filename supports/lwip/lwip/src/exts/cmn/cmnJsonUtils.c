
#include "lwipExt.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

#include "jsmn.h"
#include <stdlib.h> /* rand() and srand() */

#define		MUX_JSON_DEBUG						MUX_DBG_ON

#if 0
const char *jsonErrorStatus = 
		"{ \""MUX_JSON_KEY_STATUS"\":\"getParams\", "
		   "\""MUX_JSON_KEY_ERROR"\":\"readable message\", "
		   "\""MUX_JSON_KEY_DEBUG"\":\"detailed debug info\"}";

const char *jsonReplyParams = "{ \""MUX_JSON_KEY_STATUS"\":200, \""MUX_JSON_KEY_NAME"\":\"SdiOverIP-767\", "
		   "\""MUX_JSON_KEY_MODEL"\":\"500767-TX\", \""MUX_JSON_KEY_VERSION"\":\"01.01.01\", "
		   "\""MUX_JSON_KEY_MAC"\":\"1a:25:00:00:01:02\", \""MUX_JSON_KEY_IP"\":\"192.168.168.55\", "
		   "\""MUX_JSON_KEY_MASK"\":24, \""MUX_JSON_KEY_GATEWAY"\":\"192.168.168.1\", "
		   "\""MUX_JSON_KEY_DHCP"\":0, "
		   "\""MUX_JSON_KEY_TX"\":1, \""MUX_JSON_KEY_ID"\":\"5da68790-2198-43cb-9321-2951eeb5ee90\", \""MUX_JSON_KEY_DEST_IP"\":\"192.168.168.56\", "
		   "\""MUX_JSON_KEY_VIDEO_MAC_LOCAL"\":\"00:A0:E5:00:00:10\", \""MUX_JSON_KEY_VIDEO_MAC_DEST"\":\"00:A0:E5:00:00:11\", "
		   "\""MUX_JSON_KEY_VIDEO_IP_LOCAL"\":\"10.0.4.211\", \""MUX_JSON_KEY_VIDEO_IP_DEST"\":\"10.0.4.210\", "
		   "\""MUX_JSON_KEY_VIDEO_PORT_LOCAL"\":49712, \""MUX_JSON_KEY_VIDEO_PORT_DEST"\":49696, "
		   "\""MUX_JSON_KEY_AUDIO_PORT_LOCAL"\":49716, \""MUX_JSON_KEY_AUDIO_PORT_DEST"\":49700, "
		   "\""MUX_JSON_KEY_MC_IP"\":\"239.0.0.1\", \""MUX_JSON_KEY_MC_PORT"\":3700, "
		   "\""MUX_JSON_KEY_IS_CONNECT"\":1, \""MUX_JSON_KEY_IS_MC"\":1 }";
#endif


typedef struct
{
	short			statusCode;
	const char		*msg;	
}_MUX_STATUS_MSG;


const _MUX_STATUS_MSG _msgs[] =
{	
	{
		statusCode: JSON_STATUS_PARSE_ERROR,
		msg: 	"Error in parsing JSON string",
	},
	{
		statusCode: JSON_STATUS_PARSE_PARAM_ERROR,
		msg: 	"Parameters in JSON string is error",
	},

	{
		statusCode: JSON_STATUS_TAG_ERROR,
		msg: 	"Tag of IP Command is error",
	},
	{
		statusCode: JSON_STATUS_CRC_ERROR,
		msg: 	"CRC of IP Command is error",
	},
	{
		statusCode: JSON_STATUS_UNKNOWN,
		msg: 	NULL
	}
};



const char *muxJsonErrorMsg(short code)
{
	const _MUX_STATUS_MSG *msg = _msgs;

	while(msg->msg != NULL)
	{
		if(msg->statusCode == code)
		{
			return msg->msg;
		}
		
		msg++;
	}

	return "NULL";
}

void muxJsonInit(MUX_JSON_PARSER  *parser, char *jsonStr, unsigned short size)
{
	/* Prepare parser */
	jsmn_init(&parser->parser);
	
	parser->currentJSonString = jsonStr;
	if(jsonStr)
	{
		parser->jsonLength = size;
	}
	
	parser->tokenCount = MUX_JSON_TOKEN_SIZE;

	parser->outSize = MUX_JSON_OUT_BUF_SIZE;
	parser->outIndex = 0;

//	TRACE();	
}



int	muxJsonParse(MUX_JSON_PARSER  *parser, char *jStr , unsigned short size)
{
	int ret;

	muxJsonInit(parser, jStr, size);

	parser->status = JSON_STATUS_OK;
	
	ret = jsmn_parse(&parser->parser, jStr, parser->jsonLength, parser->tokens, parser->tokenCount);
	if (ret < 0)
	{
		if (ret == JSMN_ERROR_NOMEM)
		{
			MUX_ERRORF(("ERROR: JSMN_ERROR_NOMEM"));
		}
		else if(ret == JSMN_ERROR_INVAL)
		{
			MUX_ERRORF(("ERROR: JSMN_ERROR_INVAL"));
		}
		else
		{
			MUX_ERRORF(("ERROR: JSMN_ERROR_PART"));
		}

		parser->status = JSON_STATUS_PARSE_ERROR;
		return ret;
	}

	MUX_DEBUGF(MUX_JSON_DEBUG, ("JSON parsing OK"));

	return 0;
}


int muxJsonEqual(MUX_JSON_PARSER  *parser, int index, jsmntok_t *token, void *data)
{
	char	*keyStr = (char *)data;
	return jsoneq(parser->currentJSonString, token, keyStr);
}

jsmntok_t *muxJsonFindKeyToken(MUX_JSON_PARSER  *parser, const char *key)
{
	unsigned int i;
	int ret;
	jsmntok_t *keyToken = NULL;
	
	for(i=0; i< JSON_TOKEN_COUNT(parser); i++)
	{
		keyToken = &parser->tokens[i+1];
#if 0//def	JSON_DEBUG
		printf("No. %d item: '%.*s'\n\r", i, JSON_TOKEN_LENGTH(keyToken), parser->currentJSonString+ keyToken->start);
#endif
		ret = muxJsonEqual(parser, i, keyToken, (void *)key);
		if( ret == 0)
		{
			return keyToken+1;
		}
//		jsmntok_t *value = &muxParser.tokens[2*i+2];
	}

	return NULL;
}

#if 0
/*
https://stackoverflow.com/questions/4768180/rand-implementation
The glibc one (used by gcc) is the simple formula:
x = 1103515245 * x + 12345
https://en.wikipedia.org/wiki/Linear_congruential_generator
*/

static unsigned long random_seed;

static unsigned long  cfgRandom(void)
{
	random_seed = 1103515245 * random_seed + 12345;
	return( random_seed );
}
#endif 

void	muxUuidGenerate(MUX_UUID_T *uuid, MUX_RUNTIME_CFG *runCfg)
{
//	srand( runCfg->currentTimestamp);

	unsigned long random = rand();
	memcpy(&uuid->uuid[0], &random, sizeof(unsigned long));
	
	random = rand();
	memcpy(&uuid->uuid[sizeof(unsigned long)], &random, sizeof(unsigned long));

	random = rand();
	memcpy(&uuid->uuid[8], &random, 2);

	memcpy(&uuid->uuid[10], &runCfg->local.mac, 6);

//	printf("%d bytes random long\n\r", sizeof(unsigned long));
}

/* UUID string  8-4-4-4-12 */
char muxUuidParse(MUX_UUID_T *uuid, char *strUuid)
{
	char *tmp = strUuid;
	muxSysAtoInt8(tmp, &uuid->uuid[0]);
	tmp += 2;
	muxSysAtoInt8(tmp, &uuid->uuid[1]);
	tmp += 2;
	muxSysAtoInt8(tmp, &uuid->uuid[2]);
	tmp += 2;
	muxSysAtoInt8(tmp, &uuid->uuid[3]);
	if( *(tmp+2) != '-')
	{
		MUX_INFOF(("#9 char is '%c', not '-'", *(tmp+2)));
		goto errParsing;
	}
	
	tmp += 3;

	muxSysAtoInt8(tmp, &uuid->uuid[4]);
	tmp += 2;
	muxSysAtoInt8(tmp, &uuid->uuid[5]);
	if( *(tmp+2) != '-')
	{
		MUX_INFOF(("#14 char is '%c', not '-'", *(tmp+2)));
		goto errParsing;
	}
	tmp += 3;

	muxSysAtoInt8(tmp, &uuid->uuid[6]);
	tmp += 2;
	muxSysAtoInt8(tmp, &uuid->uuid[7]);
	if( *(tmp+2) != '-')
	{
		MUX_INFOF(("#19 char is '%c', not '-'", *(tmp+2)));
		goto errParsing;
	}
	tmp += 3;

	muxSysAtoInt8(tmp, &uuid->uuid[8]);
	tmp += 2;
	muxSysAtoInt8(tmp, &uuid->uuid[9]);
	if( *(tmp+2) != '-')
	{
		MUX_INFOF(("#24 char is '%c', not '-'", *(tmp+2)));
		goto errParsing;
	}
	tmp += 3;

	muxSysAtoInt8(tmp, &uuid->uuid[10]);
	tmp += 2;
	muxSysAtoInt8(tmp, &uuid->uuid[11]);
	tmp += 2;
	muxSysAtoInt8(tmp, &uuid->uuid[12]);
	tmp += 2;
	muxSysAtoInt8(tmp, &uuid->uuid[13]);
	tmp += 2;
	muxSysAtoInt8(tmp, &uuid->uuid[14]);
	tmp += 2;
	muxSysAtoInt8(tmp, &uuid->uuid[15]);
	tmp += 2;

	return EXIT_SUCCESS;

errParsing:
	memset(uuid, 0, sizeof(MUX_UUID_T));
	return EXIT_FAILURE;
	
}

char muxUuidEqual(MUX_UUID_T *dest, MUX_UUID_T *src)
{
	if(UUID_IS_NULL(dest) || UUID_IS_NULL(src))
		return MUX_FALSE;

	return IS_STRING_EQUAL((char *)dest->uuid, (char *)src->uuid);
}

static char _uuidString[MUX_JSON_MESSAGE_SIZE];

/* output 8-4-4-4-12 format */
char *muxUuidToString(MUX_UUID_T *guid)
{
	int index = 0;
	
	index += snprintf(_uuidString+index, sizeof(_uuidString)-index, "%02x%02x%02x%02x-",  guid->uuid[0] , guid->uuid[1] , guid->uuid[2] , guid->uuid[3]);

	index += snprintf(_uuidString+index, sizeof(_uuidString)-index, "%02x%02x-%02x%02x-%02x%02x-",  
		guid->uuid[4], guid->uuid[5], guid->uuid[6], guid->uuid[7], guid->uuid[8], guid->uuid[9] );

	index += snprintf(_uuidString+index, sizeof(_uuidString)-index, "%02x%02x%02x%02x%02x%02x",  
		guid->uuid[10], guid->uuid[11], guid->uuid[12], guid->uuid[13], guid->uuid[14], guid->uuid[15] );

	return _uuidString;
}

