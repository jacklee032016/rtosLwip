

#include "lwipMux.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

#include "jsmn.h"
#include "muxIpCmd.h"


static jsmntok_t *_muxJsonGetArrayObject(MUX_JSON_PARSER *parser, const char *key)
{
	jsmntok_t *valueObj;
	
	valueObj = muxJsonFindKeyToken(parser, key);
	if(valueObj == NULL)
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR can't find '%s' object", key);
		parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
		return NULL;
	}

	if (valueObj->type != JSMN_ARRAY)
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "'%s' was not an integer", key);
		parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
		return NULL;
	}
	return valueObj;
}


static jsmntok_t *_muxJsonGetPrimitiveValueObject(MUX_JSON_PARSER *parser, const char *key)
{
	jsmntok_t *valueObj;
	
	valueObj = muxJsonFindKeyToken(parser, key);
	if(valueObj == NULL)
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR can't find '%s' object", key);
		parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
		return NULL;
	}

	if (valueObj->type != JSMN_PRIMITIVE)
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "'%s' was not an integer", key);
		parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
		return NULL;
	}
	return valueObj;
}

int muxJsonParseUnsignedChar(MUX_JSON_PARSER *parser, const char *key, unsigned char *value)
{
	jsmntok_t *valueObj;
	
	valueObj = _muxJsonGetPrimitiveValueObject(parser, key);
	if(valueObj == NULL)
	{
		return EXIT_FAILURE;
	}

#ifdef	ARM
	if (1 != sscanf(parser->currentJSonString+ valueObj->start, "%"PRIu8, value))
#else
	if (1 != sscanf(parser->currentJSonString+ valueObj->start, "%hhu", value))
#endif		
	{
		printf("Token was not an char.");
//		return JSON_PARSE_ERROR;
		return EXIT_FAILURE;
	}

	return 0;
}

int muxJsonParseUnsignedShort(MUX_JSON_PARSER *parser, const char *key, unsigned short *value)
{
	jsmntok_t *valueObj;
	
	valueObj = _muxJsonGetPrimitiveValueObject(parser, key);
	if(valueObj == NULL)
	{
		return EXIT_FAILURE;
	}

#ifdef	ARM
	if (1 != sscanf(parser->currentJSonString+ valueObj->start, "%"PRIu16, value))
#else
	if (1 != sscanf(parser->currentJSonString+ valueObj->start, "%hu", value))
#endif		
	{
		printf("Token was not an short int.");
		return EXIT_FAILURE;
//		return JSON_PARSE_ERROR;
	}

	return EXIT_SUCCESS;
}


int muxJsonParseUnsignedInteger(MUX_JSON_PARSER *parser, const char *key, unsigned int *value)
{
	jsmntok_t *valueObj;
	
	valueObj = _muxJsonGetPrimitiveValueObject(parser, key);
	if(valueObj == NULL)
	{
		return EXIT_FAILURE;
	}

//	if (1 != sscanf(parser->currentJSonString+ valueObj->start, "%"PRIu32, value))
	if (1 != sscanf(parser->currentJSonString+ valueObj->start, "%u", value))
	{
		printf("Token was not an integer.");
		return EXIT_FAILURE;
//		return JSON_PARSE_ERROR;
	}

	return EXIT_SUCCESS;
}

/* save in parser->msg */
int muxJsonParseString(MUX_JSON_PARSER *parser, const char *key, char *retVal, int size)
{
	jsmntok_t *valueObj;
	
	valueObj = muxJsonFindKeyToken(parser, key);
	if(valueObj == NULL)
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR can't find '%s' object", key);
		parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
		return EXIT_FAILURE;
	}

	if (valueObj->type != JSMN_STRING)
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "'%s' was not an string", key);
		parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
		return EXIT_FAILURE;
	}

	snprintf(retVal, size, "%.*s", JSON_TOKEN_LENGTH(valueObj), parser->currentJSonString + valueObj->start);

	return EXIT_SUCCESS;
}

int muxJsonFindCommand(MUX_JSON_PARSER  *parser)
{
#if 0
	jsmntok_t *token = muxJsonFindKeyToken(parser, MUX_JSON_KEY_COMMAND);
	if(token == NULL)
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR can't find '%s' object", MUX_JSON_KEY_COMMAND);
		return EXIT_FAILURE;
	}

	parseStringValue(parser->cmd, parser->currentJSonString, token);
#else
	if(muxJsonParseString(parser, MUX_IPCMD_KEY_COMMAND, parser->cmd, sizeof(parser->cmd)) != EXIT_SUCCESS )
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR can't find '%s' object", MUX_IPCMD_KEY_COMMAND);
		return EXIT_FAILURE;
	}
#endif
	return 0;
}

/* in LwIP, IP address is return in network byte order */
int muxJsonParseIpAddress(MUX_JSON_PARSER *parser, const char *key, unsigned int *ipAddress)
{
	if(muxJsonParseString(parser, key, parser->msg, MUX_JSON_MESSAGE_SIZE) != EXIT_SUCCESS)
	{
		*ipAddress =  IPADDR_NONE;
		return EXIT_FAILURE;
	}

#ifdef	JSON_DEBUG
	printf("%s Address:%s\n\r", key, parser->msg );
#endif

	*ipAddress = ipaddr_addr(parser->msg);
#ifdef	JSON_DEBUG
	printf("Address:%d\n\r", *ipAddress );
#endif
	return EXIT_SUCCESS;
}

int muxJsonParseMacAddress(MUX_JSON_PARSER *parser, const char *key, MUX_MAC_ADDRESS *macAddress)
{
	if(muxJsonParseString(parser, key, parser->msg, MUX_JSON_MESSAGE_SIZE) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	return muxMacAddressParse(macAddress, (const char *)parser->msg );
}

int muxJsonParseUuid(MUX_JSON_PARSER *parser, const char *key, MUX_UUID_T *guid)
{
	if(muxJsonParseString(parser, key, parser->msg, MUX_JSON_MESSAGE_SIZE) != EXIT_SUCCESS )
	{
		return EXIT_FAILURE;
	}

//	printf("Parsed UUID string:'%s'\n\r", parser->msg);
	muxUuidParse(guid, parser->msg );
	return EXIT_SUCCESS;
}


/* after command has been parsed, parse other fields */
char muxJsonRequestParse(MUX_JSON_PARSER *parser, MUX_RUNTIME_CFG	*tmpCfg)
{
	unsigned int intValue;
	short type;
	int count = 0;

#if 0
	if(muxJsonParseString(parser, MUX_JSON_KEY_USER, runCfg->user, MUX_USER_SIZE) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}
	if(muxJsonParseString(parser, MUX_JSON_KEY_PASSWORD, runCfg->password, MUX_PASSWORD_SIZE) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}
#endif


	if( muxJsonParseString(parser, MUX_IPCMD_DATA_C_NAME, tmpCfg->name, sizeof(tmpCfg->name) ) == EXIT_SUCCESS )
	{
		count++;
	}

	if(muxJsonParseMacAddress(parser, MUX_IPCMD_DATA_MAC, &tmpCfg->local.mac) == EXIT_SUCCESS)
	{
		count++;
	}

	if(muxJsonParseIpAddress(parser, MUX_IPCMD_DATA_IP, &tmpCfg->local.ip ) == EXIT_SUCCESS)
		{
		count++;
		}
	
	if(muxJsonParseIpAddress(parser, MUX_IPCMD_DATA_MASK, &tmpCfg->ipMask) == EXIT_SUCCESS)
		{
		count ++;
		}
	if(muxJsonParseIpAddress(parser, MUX_IPCMD_DATA_GATEWAY, &tmpCfg->ipGateway) == EXIT_SUCCESS)
	{
		count++;
	}


	if(muxJsonParseUnsignedInteger(parser, MUX_IPCMD_DATA_DHCP, &intValue)== EXIT_SUCCESS)
	{
		tmpCfg->netMode = intValue+1;
		count++;
	}
	
	if(muxJsonParseUnsignedInteger(parser, MUX_IPCMD_DATA_IS_DIP, &intValue)== EXIT_SUCCESS)
	{
		tmpCfg->isDipOn = intValue+1;
		count++;
	}

	/* RS232 */
	if(muxJsonParseUnsignedInteger(parser, MUX_IPCMD_DATA_RS_BAUDRATE, &tmpCfg->rs232Cfg.baudRate ) == EXIT_SUCCESS)
	{
		count++;
		
		if(tmpCfg->rs232Cfg.baudRate != MUX_BAUDRATE_9600 && tmpCfg->rs232Cfg.baudRate != MUX_BAUDRATE_19200 &&
			tmpCfg->rs232Cfg.baudRate != MUX_BAUDRATE_38400 && tmpCfg->rs232Cfg.baudRate != MUX_BAUDRATE_57600 &&
			tmpCfg->rs232Cfg.baudRate != MUX_BAUDRATE_115200 && tmpCfg->rs232Cfg.baudRate != 0 )
		{
			snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR invalidate value '%d' for '%s' object", tmpCfg->rs232Cfg.baudRate, MUX_IPCMD_DATA_RS_BAUDRATE);
			parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
			return EXIT_FAILURE;
		}
	}
	
	

	if(muxJsonParseUnsignedChar(parser, MUX_IPCMD_DATA_RS_DATABITS, &tmpCfg->rs232Cfg.charLength ) == EXIT_SUCCESS)
	{
		count++;
		if(tmpCfg->rs232Cfg.charLength != MUX_RS232_CHAR_LENGTH_5 && tmpCfg->rs232Cfg.charLength != MUX_RS232_CHAR_LENGTH_6 &&
			tmpCfg->rs232Cfg.charLength != MUX_RS232_CHAR_LENGTH_7 && tmpCfg->rs232Cfg.charLength != MUX_RS232_CHAR_LENGTH_8 && 
			tmpCfg->rs232Cfg.charLength != 0 )
		{
			snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR invalidate value '%d' for '%s' object", tmpCfg->rs232Cfg.charLength, MUX_IPCMD_DATA_RS_DATABITS);
			parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
			return EXIT_FAILURE;
		}
	}
	
	if(muxJsonParseUnsignedChar(parser, MUX_IPCMD_DATA_RS_STOPBITS, &tmpCfg->rs232Cfg.stopbits ) == EXIT_SUCCESS)
	{
		count++;
		if(tmpCfg->rs232Cfg.stopbits != MUX_RS232_STOP_BITS_1 && tmpCfg->rs232Cfg.stopbits != MUX_RS232_STOP_BITS_2 &&
			tmpCfg->rs232Cfg.stopbits != 0 )
		{
			snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR invalidate value '%d' for '%s' object", tmpCfg->rs232Cfg.stopbits, MUX_IPCMD_DATA_RS_STOPBITS);
			parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
			return EXIT_FAILURE;
		}
	}

	memset(tmpCfg->model, 0, sizeof(tmpCfg->model));	
	if(muxJsonParseString(parser, MUX_IPCMD_DATA_RS_PARITY, tmpCfg->model, sizeof(tmpCfg->model) ) == EXIT_SUCCESS)
	{
		count++;
		if(strlen(tmpCfg->model) )
		{
			type = CMN_FIND_STR_RS_PARITY(tmpCfg->model);
			if(type< 0)
			{
				snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR invalidate value '%s' for '%s' object", tmpCfg->model, MUX_IPCMD_DATA_RS_PARITY);
				parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
				return EXIT_FAILURE;
			}
			tmpCfg->rs232Cfg.parityType = type +1;
		}
	}
	

	/* media ip and ports */
	if(muxJsonParseUnsignedInteger(parser, MUX_IPCMD_DATA_IS_MCAST, &intValue)== EXIT_SUCCESS)
	{
		count++;
		tmpCfg->isMCast = intValue+1;
	}
	
	if(muxJsonParseIpAddress(parser, MUX_IPCMD_DATA_MCAST_IP, &tmpCfg->dest.ip) == EXIT_SUCCESS)
	{
		count++;
		if(tmpCfg->isMCast > 1 && !IP_ADDR_IS_MULTICAST(tmpCfg->dest.ip) && tmpCfg->dest.ip != 0 )
		{
			snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR: ''%s' of '%s' object is not multicast address", MUX_LWIP_IPADD_TO_STR(&(tmpCfg->dest.ip)) , MUX_IPCMD_DATA_MCAST_IP);
			parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
			return EXIT_FAILURE;
		}
	}

	if(muxJsonParseUnsignedShort(parser, MUX_IPCMD_DATA_VIDEO_PORT, &tmpCfg->dest.vport) == EXIT_SUCCESS)
		count++;
	if(muxJsonParseUnsignedShort(parser, MUX_IPCMD_DATA_AUDIO_PORT, &tmpCfg->dest.aport) == EXIT_SUCCESS)
		count++;
	
	
	if(muxJsonParseUnsignedShort(parser, MUX_IPCMD_DATA_AD_PORT, &tmpCfg->dest.dport) == EXIT_SUCCESS)
		count++;
	
	if(muxJsonParseUnsignedShort(parser, MUX_IPCMD_DATA_ST_PORT, &tmpCfg->dest.sport) == EXIT_SUCCESS)
		count++;

	/* params for AV */
	if(muxJsonParseUnsignedShort(parser, MUX_IPCMD_DATA_VIDEO_WIDTH, &tmpCfg->runtime.vWidth ) == EXIT_SUCCESS)
		count++;
	if(muxJsonParseUnsignedShort(parser, MUX_IPCMD_DATA_VIDEO_HEIGHT, &tmpCfg->runtime.vHeight) == EXIT_SUCCESS)
		count++;
	
	if(muxJsonParseUnsignedChar(parser, MUX_IPCMD_DATA_VIDEO_FRAMERATE, &tmpCfg->runtime.vFrameRate) == EXIT_SUCCESS)
		count++;
	
	if(muxJsonParseUnsignedChar(parser, MUX_IPCMD_DATA_VIDEO_DEPTH, &tmpCfg->runtime.vDepth) == EXIT_SUCCESS)
		count++;

	if(muxJsonParseUnsignedInteger(parser, MUX_IPCMD_DATA_VIDEO_INTERLACED, &intValue)== EXIT_SUCCESS)
	{
		count++;
		tmpCfg->runtime.vIsInterlaced = intValue+1;
	}
	if(muxJsonParseUnsignedInteger(parser, MUX_IPCMD_DATA_VIDEO_SEGMENTED, &intValue)== EXIT_SUCCESS)
	{
		count++;
		tmpCfg->runtime.vIsSegmented = intValue+1;
	}

//	muxJsonParseUnsignedShort(parser, MUX_IPCMD_DATA_VIDEO_COLORSPACE, CMN_FIND_V_COLORSPACE(tmpCfg->runtime.vColorSpace) );

	memset(tmpCfg->model, 0, sizeof(tmpCfg->model));	

	if(muxJsonParseString(parser, MUX_IPCMD_DATA_VIDEO_COLORSPACE, tmpCfg->model, sizeof(tmpCfg->model) ) == EXIT_SUCCESS)
	{
		count++;
		if(strlen(tmpCfg->model) )
		{
			type = CMN_FIND_STR_V_COLORSPACE(tmpCfg->model);
			if(type< 0)
			{
				snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR invalidate value '%s' for '%s' object", tmpCfg->model, MUX_IPCMD_DATA_VIDEO_COLORSPACE);
				parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
				return EXIT_FAILURE;
			}
			tmpCfg->runtime.vColorSpace = type +1;
		}
	}

	if(muxJsonParseUnsignedShort(parser, MUX_IPCMD_DATA_AUDIO_SAMPE_RATE, &tmpCfg->runtime.aSampleRate ) == EXIT_SUCCESS)
	{
		count++;
		if(tmpCfg->runtime.aSampleRate != 44100 && tmpCfg->runtime.aSampleRate != 48000 &&
			tmpCfg->runtime.aSampleRate != 0 )
		{
			snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR invalidate value '%d' for '%s' object", tmpCfg->runtime.aSampleRate, MUX_IPCMD_DATA_AUDIO_SAMPE_RATE);
			parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
			return EXIT_FAILURE;
		}
	}
	
	if(muxJsonParseUnsignedChar(parser, MUX_IPCMD_DATA_AUDIO_DEPTH, &tmpCfg->runtime.aDepth ) == EXIT_SUCCESS)
	{
		count++;
		if(tmpCfg->runtime.aDepth != 16 && tmpCfg->runtime.aDepth != 24 &&
			tmpCfg->runtime.aDepth != 0 )
		{
			snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR invalidate value '%d' for '%s' object", tmpCfg->runtime.aDepth, MUX_IPCMD_DATA_AUDIO_DEPTH);
			parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
			return EXIT_FAILURE;
		}
	}
	
	if(muxJsonParseUnsignedChar(parser, MUX_IPCMD_DATA_AUDIO_CHANNELS, &tmpCfg->runtime.aChannels ) == EXIT_SUCCESS)
	{
		count++;
		if(tmpCfg->runtime.aChannels != 1 && tmpCfg->runtime.aChannels != 2 &&
			tmpCfg->runtime.aChannels != 0 )
		{
			snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR invalidate value '%d' for '%s' object", tmpCfg->runtime.aChannels, MUX_IPCMD_DATA_AUDIO_CHANNELS);
			parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
			return EXIT_FAILURE;
		}
	}

	if(muxJsonParseUnsignedInteger(parser, MUX_IPCMD_DATA_IS_CONNECT, &intValue)== EXIT_SUCCESS)
	{
		count++;
		tmpCfg->runtime.isConnect = intValue+1;
	}

	
#if 0//def	X86
	printf("tx:%d;\tMcIpPort:%d\n\r", runCfg->isTx, runCfg->mcPort);

	muxDebugCfg(runCfg, LWIP_NEW_LINE"PARSED Cfg:");
#endif
	if(count == 0)
	{
		snprintf(parser->msg, sizeof(parser->msg), "No validate parameter in this command");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}



unsigned char muxJsonParseDataArray(MUX_JSON_PARSER *parser)
{
	jsmntok_t *jkey = _muxJsonGetArrayObject(parser, MUX_IPCMD_DATA_ARRAY);

	if(jkey == NULL)
	{
		MUX_ERRORF(("Can found :'%s' array",MUX_IPCMD_DATA_ARRAY));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


char muxJsonRequestParseCommand(char *jsonRequest, unsigned short size, MUX_JSON_PARSER *parser)
{
//	int intValue;

	muxJsonParse(parser, (char *)jsonRequest, size);

TRACE();
	if(parser->status == JSON_STATUS_OK)
	{
#if 0	
		if(JSON_TOKEN_COUNT(parser)%2 != 1)
		{
			snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "token number %d is not even number", JSON_TOKEN_COUNT(parser)  );
			goto parseFailed;
		}
#endif

//		muxJsonIterate(parser, tokenPrintHandle, NULL);
	}
	else
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR: JSON library parsing failed" );
		goto parseFailed;
	}

#if 0//def	X86
	if(MUX_DEBUG_IS_ENABLE(MUX_DEBUG_FLAG_CMD) )
	{
		muxJsonPrint(parser, parser->tokens, JSON_TOKEN_COUNT(parser), 2, LWIP_NEW_LINE"Passed");
	}
#endif

TRACE();
	if(muxJsonParseMacAddress(parser, MUX_IPCMD_KEY_TARGET, &parser->target) == EXIT_FAILURE)
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "ERROR: can't find or invalidate '%s' object", MUX_IPCMD_KEY_TARGET);
		goto parseFailed;
	}

TRACE();
	if(muxJsonFindCommand(parser) == EXIT_FAILURE)
	{
		goto parseFailed;
	}

TRACE();
	return EXIT_SUCCESS;

parseFailed:
TRACE();
	parser->status = JSON_STATUS_PARSE_ERROR;
	
	return EXIT_FAILURE;
	
}


#ifdef	X86
int muxJsonPrint(MUX_JSON_PARSER *parser, jsmntok_t *t, size_t count, int indent, const char *prompt)
{
	int i, j, k;
//	int indent = 2;

	
	if(count == 0)
	{
		return 0;
	}
	
	if (t->type == JSMN_PRIMITIVE)
	{
		printf("%.*s", t->end - t->start, parser->currentJSonString +t->start);
		return 1;
	}
	else if (t->type == JSMN_STRING)
	{
		printf("'%.*s'", t->end - t->start, parser->currentJSonString +t->start);
		return 1;
	}
	else if (t->type == JSMN_OBJECT)
	{
		printf(""MUX_NEW_LINE);
		j = 0;
		for (i = 0; i < t->size; i++)
		{
			for (k = 0; k < indent; k++)
				printf("  ");
			
			j += muxJsonPrint(parser, t+1+j, count-j, indent+1, prompt);
			printf(": ");
			j += muxJsonPrint(parser, t+1+j, count-j, indent+1, prompt);
			printf(""MUX_NEW_LINE);
		}
		return j+1;
	}
	else if (t->type == JSMN_ARRAY)
	{
		j = 0;
		printf("ARRAY\n");
		for (i = 0; i < t->size; i++)
		{
			for (k = 0; k < indent-1; k++)
				printf("  ");
			
			printf("   - ");
			j += muxJsonPrint(parser, t+1+j, count-j, indent+1, prompt);
			printf(""MUX_NEW_LINE);
		}
		
		return j+1;
	}
	return 0;
}

#define	MAC_ADDRESS_DEBUG(mac) \
		{printf( "\"%02x:%02x:%02x:%02x:%02x:%02x\""MUX_NEW_LINE,  \
			(mac)->address[0] , (mac)->address[1] , (mac)->address[2] , (mac)->address[3], (mac)->address[4], (mac)->address[5] );}

#define	UUID_DEBUG(guid)	\
		{printf( "\"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\""MUX_NEW_LINE,  \
			(guid)->uuid[0] , (guid)->uuid[1] , (guid)->uuid[2] , (guid)->uuid[3], \
			(guid)->uuid[4], (guid)->uuid[5], (guid)->uuid[6], (guid)->uuid[7], (guid)->uuid[8], (guid)->uuid[9], \
			(guid)->uuid[10], (guid)->uuid[11], (guid)->uuid[12], (guid)->uuid[13], (guid)->uuid[14], (guid)->uuid[15] );}


void	muxDebugCfg(MUX_RUNTIME_CFG *cfg, const char *prompt)
{
	printf("%s: "LWIP_NEW_LINE, prompt);

	printf(MUX_JSON_KEY_TX"\t: %s"LWIP_NEW_LINE, MUX_IS_TX(cfg)?"TX":"RX");
	printf(MUX_JSON_KEY_MAC"\t: ");
	MAC_ADDRESS_DEBUG(&cfg->local.mac);
	printf(MUX_JSON_KEY_IP"\t: %s"LWIP_NEW_LINE,  MUX_LWIP_IPADD_TO_STR(&cfg->local.ip));
	printf(MUX_JSON_KEY_MASK"\t: %s"LWIP_NEW_LINE,  MUX_LWIP_IPADD_TO_STR(&cfg->ipMask));
	printf(MUX_JSON_KEY_GATEWAY"\t: %s"LWIP_NEW_LINE,  MUX_LWIP_IPADD_TO_STR(&cfg->ipGateway));
	printf(MUX_JSON_KEY_DHCP"\t: %s"LWIP_NEW_LINE LWIP_NEW_LINE, MUX_DHCP_IS_ENABLE(cfg)?"Yes":"No");
	
	printf(MUX_JSON_KEY_VIDEO_MAC_LOCAL"\t: ");
	MAC_ADDRESS_DEBUG(&cfg->local.mac);
	printf(MUX_JSON_KEY_VIDEO_IP_LOCAL"\t: %s"LWIP_NEW_LINE,  MUX_LWIP_IPADD_TO_STR(&cfg->local.ip));
	printf(MUX_JSON_KEY_VIDEO_PORT_LOCAL"\t: %d"LWIP_NEW_LINE, cfg->local.vport);
	printf(MUX_JSON_KEY_AUDIO_PORT_LOCAL"\t: %d"LWIP_NEW_LINE LWIP_NEW_LINE, cfg->local.aport);

	
	printf(MUX_JSON_KEY_VIDEO_MAC_DEST"\t: ");
	MAC_ADDRESS_DEBUG(&cfg->dest.mac);
	printf(MUX_JSON_KEY_VIDEO_IP_DEST"\t: %s"LWIP_NEW_LINE,  MUX_LWIP_IPADD_TO_STR(&cfg->dest.ip) );
	printf(MUX_JSON_KEY_VIDEO_PORT_DEST"\t: %d"LWIP_NEW_LINE, cfg->dest.vport);
	printf(MUX_JSON_KEY_AUDIO_PORT_DEST"\t: %d"LWIP_NEW_LINE, cfg->dest.aport);

}

void muxJsonDebug(MUX_JSON_PARSER *parser , MUX_RUNTIME_CFG *cfg, const char *prompt)
{
	/*raw json string */
	printf("Raw JSON data: %d tokens"MUX_NEW_LINE, JSON_TOKEN_COUNT(parser) );
	muxJsonPrint(parser, parser->tokens, JSON_TOKEN_COUNT(parser), 2, prompt);

	printf("%s Debug output:\n\r", prompt);
	
	printf(MUX_JSON_KEY_COMMAND"\t: %s\n\r", parser->cmd);
	printf(MUX_JSON_KEY_ID"\t: ");
	UUID_DEBUG(&parser->uuid);

	muxDebugCfg(cfg, prompt);
}
#endif

