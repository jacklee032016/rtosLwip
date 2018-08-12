
#include "lwipExt.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "jsmn.h"
#include "extUdpCmd.h"



int	muxIpCmdPrintMediaCfg(MUX_JSON_PARSER  *parser, char *data, int size)
{
	int index = 0;

	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_IS_MCAST"\":%d,", (parser->runCfg->isMCast)?1:0);
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_MCAST_IP"\":\"%s\",", MUX_LWIP_IPADD_TO_STR(&(parser->runCfg->dest.ip)) );

	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_VIDEO_PORT"\":%d,", parser->runCfg->dest.vport );
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_AUDIO_PORT"\":%d,", parser->runCfg->dest.aport );
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_AD_PORT"\":%d,", parser->runCfg->dest.dport );
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_ST_PORT"\":%d", parser->runCfg->dest.sport );

	index += snprintf(data+index, size-index, ",\""MUX_IPCMD_DATA_VIDEO_WIDTH"\":%d,", parser->runCfg->runtime.vWidth );
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_VIDEO_HEIGHT"\":%d,", parser->runCfg->runtime.vHeight);
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_VIDEO_FRAMERATE"\":%d,", parser->runCfg->runtime.vFrameRate);
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_VIDEO_DEPTH"\":%d,", parser->runCfg->runtime.vDepth);

	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_VIDEO_INTERLACED"\":%d,", (parser->runCfg->runtime.vIsInterlaced)?1:0);
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_VIDEO_SEGMENTED"\":%d,", (parser->runCfg->runtime.vIsSegmented)?1:0);

	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_VIDEO_COLORSPACE"\":\"%s\",", CMN_FIND_V_COLORSPACE(parser->runCfg->runtime.vColorSpace) );

	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_AUDIO_SAMPE_RATE"\":%d,", parser->runCfg->runtime.aSampleRate );
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_AUDIO_DEPTH"\":%d,", parser->runCfg->runtime.aDepth );
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_AUDIO_CHANNELS"\":%d,", parser->runCfg->runtime.aChannels );
	
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_IS_CONNECT"\":%d", parser->runCfg->runtime.isConnect);

	return index;
}

/* TX --> RX */
char muxIpCmdRequestHeaderPrint(MUX_JSON_PARSER  *parser, const char *cmd)
{
	int index = 0;
	char *data = parser->outBuffer + IPCMD_HEADER_LENGTH;
	int size = parser->outSize - IPCMD_HEADER_LENGTH;
		
	index += snprintf(data+index, size-index, "{\""MUX_IPCMD_KEY_TARGET"\":" );
	MAC_ADDRESS_PRINT(data, size, index, &(parser->runCfg->dest.mac));
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_KEY_COMMAND"\":\"%s\",", cmd);

	index += snprintf(data+index, size-index, "\""MUX_IPCMD_LOGIN_ACK"\":%s,", parser->runCfg->user );
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_PWD_MSG"\":\"%s\"", parser->runCfg->password );

	parser->outIndex = index;
	
	if(MUX_DEBUG_IS_ENABLE(MUX_DEBUG_FLAG_CMD))
	{
		printf("output IP CMD response header %d bytes: '%s'"LWIP_NEW_LINE, parser->outIndex, data);
	}
	return EXIT_SUCCESS;

}



char muxIpCmdResponseHeaderPrint(MUX_JSON_PARSER  *parser )
{
	int index = 0;
	char *data = parser->outBuffer + IPCMD_HEADER_LENGTH;
	int size = parser->outSize - IPCMD_HEADER_LENGTH;
		
	index += snprintf(data+index, size-index, "{\""MUX_IPCMD_KEY_TARGET"\":" );
	MAC_ADDRESS_PRINT(data, size, index, &(parser->runCfg->local.mac));
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_KEY_COMMAND"\":\"%s\",", parser->cmd);

	index += snprintf(data+index, size-index, "\""MUX_IPCMD_LOGIN_ACK"\":\"%s\",", PARSE_IS_OK(parser)?"OK":"NOK");
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_PWD_MSG"\":\"%s\"", PARSE_IS_OK(parser)?"OK": parser->msg );

//	index += snprintf(data+index, size-index, "\""MUX_JSON_KEY_DEBUG"\":\"%s\"}", parser->msg);

	parser->outIndex = index;
	
	if(MUX_DEBUG_IS_ENABLE(MUX_DEBUG_FLAG_CMD))
	{
		printf("output IP CMD response header %d bytes: '%s'"LWIP_NEW_LINE, parser->outIndex, data);
	}
	return EXIT_SUCCESS;

}


char muxIpCmdResponseTailCalculate(MUX_JSON_PARSER  *parser)
{
	unsigned int crc32 =0;
	int index = 0;
	CMN_IP_COMMAND *ipCmd = NULL;

	ipCmd = (CMN_IP_COMMAND *)parser->outBuffer;

	index += snprintf(parser->outBuffer+IPCMD_HEADER_LENGTH+parser->outIndex+index, parser->outSize-IPCMD_HEADER_LENGTH-parser->outIndex-index, "}");
	parser->outIndex += index;
	
	ipCmd->tag = CMD_TAG_RESPONSE;
	ipCmd->length =  htons(parser->outIndex + IPCMD_HEADER_LENGTH);

	crc32 =  htonl(cmnMuxCRC32b(ipCmd->data, parser->outIndex) );
	*(unsigned int *)(ipCmd->data+parser->outIndex) = crc32;

	parser->outIndex += +2*IPCMD_HEADER_LENGTH;

	MUX_DEBUGF(MUX_IPCMD_DEBUG, ("Reply %d bytes packet", parser->outIndex) );
//	CONSOLE_DEBUG_MEM((uint8_t *)ipCmd, parser->outIndex, 0, "Reply IP Cmd");

	return EXIT_SUCCESS;
	
}

char	muxIpCmdResponseReply(MUX_JSON_PARSER  *parser)
{
	muxIpCmdResponseHeaderPrint( parser);
	muxIpCmdResponseTailCalculate(parser);
	
	return EXIT_SUCCESS;
}


char	muxJsonResponsePrintConfig(MUX_JSON_PARSER  *parser)
{
	int index = 0;
	char *data = NULL;
	int size = 0;

	muxIpCmdResponseHeaderPrint( parser);
	data = parser->outBuffer+IPCMD_HEADER_LENGTH + parser->outIndex;
	size = parser->outSize - IPCMD_HEADER_LENGTH - parser->outIndex;
	
	index += snprintf(data+index, size-index, ",\""MUX_IPCMD_DATA_ARRAY"\":[{");

	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_P_NAME"\":\"%s\",", MUX_767_PRODUCT_NAME );
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_MODEL"\":\"%s-%s\",", MUX_767_MODEL, MUX_IS_TX(parser->runCfg)?"TX":"RX" );
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_FW_VER"\":\"%02d.%02d.%02d\",", parser->runCfg->version.major, parser->runCfg->version.minor, parser->runCfg->version.revision);

	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_C_NAME"\":\"%s\",", parser->runCfg->name );
	
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_MAC"\":" );
	MAC_ADDRESS_PRINT(data, size, index, &(parser->runCfg->local.mac));
	
//	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_IP"\":\"%s\",", MUX_LWIP_IPADD_TO_STR(&(parser->runCfg->local.ip)) );
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_IP"\":\"%s\",", muxLwipIpAddress());
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_MASK"\":\"%s\",", MUX_LWIP_IPADD_TO_STR(&(parser->runCfg->ipMask)) );
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_GATEWAY"\":\"%s\",", MUX_LWIP_IPADD_TO_STR(&(parser->runCfg->ipGateway)) );

	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_DHCP"\":%d,", MUX_DHCP_IS_ENABLE(parser->runCfg)?1:0);

	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_IS_DIP"\":%d,", (parser->runCfg->isDipOn)?1:0);


	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_RS_BAUDRATE"\":%d,", parser->runCfg->rs232Cfg.baudRate );
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_RS_DATABITS"\":%d,", parser->runCfg->rs232Cfg.charLength );
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_RS_PARITY"\":\"%s\",",  CMN_FIND_RS_PARITY((unsigned short)parser->runCfg->rs232Cfg.parityType) );
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_DATA_RS_STOPBITS"\":%d,", parser->runCfg->rs232Cfg.stopbits );

	index += muxIpCmdPrintMediaCfg(parser, data+index, size-index);

	index += snprintf(data+index, size-index, "}]" );

	parser->outIndex += index;

	if(MUX_DEBUG_IS_ENABLE(MUX_DEBUG_FLAG_CMD))
	{
		printf("output RES %d bytes: '%s'"LWIP_NEW_LINE, parser->outIndex, parser->outBuffer);
	}
	
	muxIpCmdResponseTailCalculate(parser);
	
	return EXIT_SUCCESS;
}



typedef char (*MuxCmdCallback)(MUX_JSON_PARSER  *parser);


typedef struct
{
	const char				*cmd;	/* when command is NULL, this is end of handlers table */
//	void						*pStruct;
	MuxCmdCallback			handler;	/* when handler is NULL, this command is not handled now */
//	char						isFree;
}MuxCmdHandle;

char muxJsonUserValidate(MUX_JSON_PARSER  *parser)
{
	char name[MUX_USER_SIZE];
	
	if(muxJsonParseString(parser, MUX_IPCMD_LOGIN_ACK, name, MUX_USER_SIZE) == EXIT_FAILURE )
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "'"MUX_IPCMD_LOGIN_ACK"' is provided in JSON data");
		return EXIT_FAILURE;
	}
	if(strncmp(name, parser->runCfg->user, strlen(parser->runCfg->user)) )
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "'%s' is not validate user", name);
//		TRACE();
		return EXIT_FAILURE;
	}

	if(muxJsonParseString(parser, MUX_IPCMD_PWD_MSG, name, MUX_USER_SIZE) == EXIT_FAILURE )
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "'"MUX_IPCMD_PWD_MSG"' is provided in JSON data");
		return EXIT_FAILURE;
	}
	if(strncmp(name, parser->runCfg->password, strlen(parser->runCfg->password)) )
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "'%s' authentication failed", name);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

static char _muxCmdGetParams(MUX_JSON_PARSER  *parser)
{
	char ret;
	ret = muxJsonUserValidate(parser);
	if(ret == EXIT_FAILURE)
	{
		return ret;
	}

	if(!MAC_ADDR_IS_EQUAL(&parser->target, &parser->runCfg->local.mac) 
		&& !MAC_ADDR_IS_BOARDCAST(&parser->target) 
		&& !MAC_ADDR_IS_EQUAL(&parser->target, &parser->runCfg->dest.mac))
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "'targ' is not boardcast or local address" );
		return EXIT_FAILURE;
	}

	return muxJsonResponsePrintConfig(parser);
}

#if 0
static char _muxCfgParamsUpdate(MUX_RUNTIME_CFG *runCfg, MUX_RUNTIME_CFG *newCfg)
{
#if 1
//	printf("Configuration update length :%d; save length %d"LWIP_NEW_LINE, MUX_RUNTIME_CFG_UPDATE_SIZE, MUX_RUNTIME_CFG_WRITE_SIZE);
	memcpy(((char *)runCfg+MUX_MAGIC_SIZE), ((char *)newCfg+MUX_MAGIC_SIZE), MUX_RUNTIME_CFG_UPDATE_SIZE -MUX_MAGIC_SIZE);
#else
	memcpy(runCfg, newCfg, sizeof(MUX_RUNTIME_CFG));
	sprintf(runCfg->name, "%s", MUX_767_PRODUCT_NAME);
	sprintf(runCfg->model, "%s", MUX_767_MODEL);

	snprintf(runCfg->user, MUX_USER_SIZE, "%s", MUX_USER);
	snprintf(runCfg->password, MUX_PASSWORD_SIZE, "%s", MUX_PASSWORD);

	runCfg->httpPort = MUX_HTTP_SVR_PORT;
#endif
	return 0;
}
#endif


MuxCmdHandle 	_muxCmdTable[] =
{
	{
		cmd		: MUX_IPCMD_CMD_GET_PARAMS,
		handler	: _muxCmdGetParams	
	},
	{
		cmd		: MUX_IPCMD_CMD_SET_PARAMS,
		handler	: muxIpCmdSetupParams	
	},
	
	{
		cmd		: MUX_IPCMD_CMD_SEND_RS232,
		handler	: muxIpCmdSendRsData	
	},
	{
		cmd		: MUX_IPCMD_CMD_SECURITY_CHECK,
		handler	: muxIpCmdSecurityCheck	
	},

	{
		cmd		: NULL,
		handler	: NULL	
	}
};


char muxJsonHandle(MUX_JSON_PARSER  *parser )
{
	char ret = 0;
	MuxCmdHandle *handle = _muxCmdTable;

#if 0
	if(strncmp(parser->cmd, MUX_JSON_CMD_GET_PARAMS, strlen(MUX_JSON_CMD_GET_PARAMS)) )
	{/* except getParams: getParams begin a new session */
		MUX_UUID_T uuid;
		if(muxJsonParseUuid(parser, MUX_JSON_KEY_ID, &uuid)<0)
		{
			snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "No '"MUX_JSON_KEY_ID"' in JSON data" );
			goto err;
		}

		if(strncmp((const char *)uuid.uuid, (const char *)parser->uuid.uuid, sizeof(MUX_UUID_T)) )
		{
			snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "'"MUX_JSON_KEY_ID"' is invalidate" );
			goto err;
		}	
	}
#endif

	while(handle->cmd)
	{
		if(strncasecmp(handle->cmd, parser->cmd, strlen(handle->cmd)) == 0)
		{
			ret = handle->handler(parser);
//			printf("return value of IP CMD handler:%d"LWIP_NEW_LINE, ret );
			if(ret == EXIT_FAILURE)
			{
				parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
			}	
//TRACE();
			if(MUX_DEBUG_IS_ENABLE(MUX_DEBUG_FLAG_CMD))
			{
				printf("output IP CMD RES for cmd %s %p, %d bytes: '%.*s'"LWIP_NEW_LINE, 
					parser->cmd, (void *)parser, parser->outIndex, parser->outIndex-2*IPCMD_HEADER_LENGTH, parser->outBuffer+IPCMD_HEADER_LENGTH );
				MUX_DEBUGF(MUX_IPCMD_DEBUG, ("output IP CMD RES for cmd %s %p, %d bytes: '%.*s'"LWIP_NEW_LINE, 
					parser->cmd, (void *)parser, parser->outIndex, parser->outIndex-2*IPCMD_HEADER_LENGTH, parser->outBuffer+IPCMD_HEADER_LENGTH ));
			}

			return ret;
		}
		handle++;
	}

//err:
	snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "No command '%s' is found!", parser->cmd);
	parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
	
	return EXIT_FAILURE;
}


