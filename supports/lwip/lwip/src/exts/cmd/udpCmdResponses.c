
#include "lwipExt.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "jsmn.h"
#include "extUdpCmd.h"



int	extIpCmdPrintMediaCfg(EXT_JSON_PARSER  *parser, char *data, int size)
{
	int index = 0;

	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_IS_MCAST"\":%d,", (parser->runCfg->isMCast)?1:0);
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_MCAST_IP"\":\"%s\",", EXT_LWIP_IPADD_TO_STR(&(parser->runCfg->dest.ip)) );

	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_VIDEO_PORT"\":%d,", parser->runCfg->dest.vport );
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_AUDIO_PORT"\":%d,", parser->runCfg->dest.aport );
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_AD_PORT"\":%d,", parser->runCfg->dest.dport );
#if EXT_FPGA_AUX_ON	
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_ST_PORT"\":%d,", parser->runCfg->dest.sport );
#endif

	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_VIDEO_WIDTH"\":%d,", parser->runCfg->runtime.vWidth );
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_VIDEO_HEIGHT"\":%d,", parser->runCfg->runtime.vHeight);
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_VIDEO_FRAMERATE"\":%d,", CMN_INT_FIND_NAME_V_FPS(parser->runCfg->runtime.vFrameRate) );
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_VIDEO_DEPTH"\":%d,", CMN_INT_FIND_NAME_V_DEPTH(parser->runCfg->runtime.vDepth) );
#if 0
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_VIDEO_INTERLACED"\":%d,", (parser->runCfg->runtime.vIsInterlaced)?1:0);
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_VIDEO_SEGMENTED"\":%d,", (parser->runCfg->runtime.vIsSegmented)?1:0);
#endif	
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_VIDEO_SEGMENTED"\":%d,", parser->runCfg->runtime.vIsSegmented);

	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_VIDEO_COLORSPACE"\":\"%s\",", CMN_FIND_V_COLORSPACE(parser->runCfg->runtime.vColorSpace) );

	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_AUDIO_SAMPE_RATE"\":\"%s\",", CMN_FIND_A_RATE(parser->runCfg->runtime.aSampleRate) );
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_AUDIO_DEPTH"\":%d,", parser->runCfg->runtime.aDepth );
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_AUDIO_CHANNELS"\":%d,", parser->runCfg->runtime.aChannels );
	
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_IS_CONNECT"\":%d", parser->runCfg->runtime.isConnect);

	return index;
}

/* TX --> RX */
char extIpCmdRequestHeaderPrint(EXT_JSON_PARSER  *parser, const char *cmd)
{
	int index = 0;
	char *data = parser->outBuffer + IPCMD_HEADER_LENGTH;
	int size = parser->outSize - IPCMD_HEADER_LENGTH;
		
	index += snprintf(data+index, size-index, "{\""EXT_IPCMD_KEY_TARGET"\":" );
	MAC_ADDRESS_PRINT(data, size, index, &(parser->runCfg->dest.mac));
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_KEY_COMMAND"\":\"%s\",", cmd);

	index += snprintf(data+index, size-index, "\""EXT_IPCMD_LOGIN_ACK"\":\"%s\",", parser->runCfg->user );
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_PWD_MSG"\":\"%s\"", parser->runCfg->password );

	parser->outIndex = index;
	
	if(!EXT_DEBUG_UDP_CMD_IS_ENABLE() )
	{
		printf("output IP CMD response header %d bytes: '%s'"LWIP_NEW_LINE, parser->outIndex, data);
	}
	return EXIT_SUCCESS;

}



char extIpCmdResponseHeaderPrint(EXT_JSON_PARSER  *parser )
{
	int index = 0;
	char *data = parser->outBuffer + IPCMD_HEADER_LENGTH;
	int size = parser->outSize - IPCMD_HEADER_LENGTH;
		
	index += snprintf(data+index, size-index, "{\""EXT_IPCMD_KEY_TARGET"\":" );
	MAC_ADDRESS_PRINT(data, size, index, &(parser->runCfg->local.mac));
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_KEY_COMMAND"\":\"%s\",", parser->cmd);

	index += snprintf(data+index, size-index, "\""EXT_IPCMD_LOGIN_ACK"\":\"%s\",", PARSE_IS_OK(parser)?EXT_IPCMD_STATUS_OK:EXT_IPCMD_STATUS_FAIL );
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_PWD_MSG"\":\"%s\"", PARSE_IS_OK(parser)?EXT_IPCMD_STATUS_OK: parser->msg );

//	index += snprintf(data+index, size-index, "\""EXT_JSON_KEY_DEBUG"\":\"%s\"}", parser->msg);

	parser->outIndex = index;

#if 0	
	if(!EXT_DEBUG_UDP_CMD_IS_ENABLE() )
	{
		printf("output IP CMD response header %d bytes: '%s'"LWIP_NEW_LINE, parser->outIndex, data);
	}
#endif	
	return EXIT_SUCCESS;

}


/* when TX--> RX directly, it send request; otherwise all reply response */
char extIpCmdResponseTailCalculate(EXT_JSON_PARSER  *parser, char isRequest)
{
	unsigned int crc32 =0;
	int index = 0;
	CMN_IP_COMMAND *ipCmd = NULL;

	ipCmd = (CMN_IP_COMMAND *)parser->outBuffer;

	index += snprintf(parser->outBuffer+IPCMD_HEADER_LENGTH+parser->outIndex+index, parser->outSize-IPCMD_HEADER_LENGTH-parser->outIndex-index, "}");
	parser->outIndex += index;

	ipCmd->tag = (isRequest==EXT_FALSE)? CMD_TAG_RESPONSE:CMD_TAG_REQUEST;
	ipCmd->length =  htons(parser->outIndex + IPCMD_HEADER_LENGTH);

	crc32 =  htonl(cmnMuxCRC32b(ipCmd->data, parser->outIndex) );
	*(unsigned int *)(ipCmd->data+parser->outIndex) = crc32;

	parser->outIndex += +2*IPCMD_HEADER_LENGTH;

//	EXT_DEBUGF(EXT_IPCMD_DEBUG, ("Reply %d bytes packet", parser->outIndex) );
//	CONSOLE_DEBUG_MEM((uint8_t *)ipCmd, parser->outIndex, 0, "Reply IP Cmd");

	return EXIT_SUCCESS;
	
}

char	extIpCmdResponseReply(EXT_JSON_PARSER  *parser)
{
	extIpCmdResponseHeaderPrint( parser);
	extIpCmdResponseTailCalculate(parser, EXT_FALSE);
	
	return EXIT_SUCCESS;
}


char	extJsonResponsePrintConfig(EXT_JSON_PARSER  *parser)
{
	int index = 0;
	char *data = NULL;
	int size = 0;

	extIpCmdResponseHeaderPrint( parser);
	data = parser->outBuffer+IPCMD_HEADER_LENGTH + parser->outIndex;
	size = parser->outSize - IPCMD_HEADER_LENGTH - parser->outIndex;
	
	index += snprintf(data+index, size-index, ",\""EXT_IPCMD_DATA_ARRAY"\":[{");

	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_P_NAME"\":\"%s\",", EXT_767_PRODUCT_NAME );
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_MODEL"\":\"%s-%s\",", EXT_767_MODEL, EXT_IS_TX(parser->runCfg)?"TX":"RX" );
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_FW_VER"\":\"%02d.%02d.%02d\",", parser->runCfg->version.major, parser->runCfg->version.minor, parser->runCfg->version.revision);

	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_C_NAME"\":\"%s\",", parser->runCfg->name );
	
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_MAC"\":" );
	MAC_ADDRESS_PRINT(data, size, index, &(parser->runCfg->local.mac));
	
//	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_IP"\":\"%s\",", EXT_LWIP_IPADD_TO_STR(&(parser->runCfg->local.ip)) );
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_IP"\":\"%s\",", extLwipIpAddress(parser->runCfg));
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_MASK"\":\"%s\",", EXT_LWIP_IPADD_TO_STR(&(parser->runCfg->ipMask)) );
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_GATEWAY"\":\"%s\",", EXT_LWIP_IPADD_TO_STR(&(parser->runCfg->ipGateway)) );

	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_DHCP"\":%d,", EXT_DHCP_IS_ENABLE(parser->runCfg)?1:0);

#if EXT_DIP_SWITCH_ON
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_IS_DIP"\":%d,", (parser->runCfg->isDipOn)?1:0);
#endif
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_BACKUP_MC_IP"\":\"%s\",", extCmnIp4addr_ntoa(&parser->runCfg->ipMulticast));


	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_RS_BAUDRATE"\":%"U32_F",", parser->runCfg->rs232Cfg.baudRate );
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_RS_DATABITS"\":%d,", parser->runCfg->rs232Cfg.charLength );
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_RS_PARITY"\":\"%s\",",  CMN_FIND_RS_PARITY((unsigned short)parser->runCfg->rs232Cfg.parityType) );
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_RS_STOPBITS"\":%d,", parser->runCfg->rs232Cfg.stopbits );

	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_IS_REBOOT"\":%d,", (parser->runCfg->runtime.reboot)?1:0);
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_DATA_BLINK"\":%d,", (parser->runCfg->runtime.blink)?1:0);

	index += extIpCmdPrintMediaCfg(parser, data+index, size-index);

	index += snprintf(data+index, size-index, "}]" );

	parser->outIndex += index;

	if(EXT_DEBUG_UDP_CMD_IS_ENABLE() )
	{
		printf("output RES %d bytes: '%.*s'"LWIP_NEW_LINE, parser->outIndex, parser->outIndex - IPCMD_HEADER_LENGTH, parser->outBuffer + IPCMD_HEADER_LENGTH);
	}
	
	extIpCmdResponseTailCalculate(parser, EXT_FALSE);
	
	return EXIT_SUCCESS;
}



typedef char (*MuxCmdCallback)(EXT_JSON_PARSER  *parser);


typedef struct
{
	const char				*cmd;	/* when command is NULL, this is end of handlers table */
//	void						*pStruct;
	MuxCmdCallback			handler;	/* when handler is NULL, this command is not handled now */
//	char						isFree;
}MuxCmdHandle;

char extJsonUserValidate(EXT_JSON_PARSER  *parser)
{
	char name[EXT_USER_SIZE];
	
	if(extJsonParseString(parser, EXT_IPCMD_LOGIN_ACK, name, EXT_USER_SIZE) == EXIT_FAILURE )
	{
//		snprintf(parser->msg, EXT_JSON_MESSAGE_SIZE, "'"EXT_IPCMD_LOGIN_ACK"' is not provided or invalidate in JSON data");
		return EXIT_FAILURE;
	}
	if(strncmp(name, parser->runCfg->user, strlen(parser->runCfg->user)) )
	{
//		snprintf(parser->msg, EXT_JSON_MESSAGE_SIZE, "'%s' is not validate user", name);
//		TRACE();
		return EXIT_FAILURE;
	}

	if(extJsonParseString(parser, EXT_IPCMD_PWD_MSG, name, EXT_USER_SIZE) == EXIT_FAILURE )
	{
//		snprintf(parser->msg, EXT_JSON_MESSAGE_SIZE, "'"EXT_IPCMD_PWD_MSG"' is not provided or invalidate in JSON data");
		return EXIT_FAILURE;
	}
	if(strncmp(name, parser->runCfg->password, strlen(parser->runCfg->password)) )
	{
		snprintf(parser->msg, EXT_JSON_MESSAGE_SIZE, "'%s' authentication failed", name);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

static char _extCmdGetParams(EXT_JSON_PARSER  *parser)
{
	char ret;
	ret = extJsonUserValidate(parser);
	if(ret == EXIT_FAILURE)
	{
		return ret;
	}

	if(!MAC_ADDR_IS_EQUAL(&parser->target, &parser->runCfg->local.mac) 
		&& !MAC_ADDR_IS_BOARDCAST(&parser->target) 
		&& !MAC_ADDR_IS_EQUAL(&parser->target, &parser->runCfg->dest.mac))
	{
		snprintf(parser->msg, EXT_JSON_MESSAGE_SIZE, "'targ' is not boardcast or local address" );
		return EXIT_FAILURE;
	}

	if(extJsonParseIpAddress(parser, EXT_IPCMD_DATA_811_IP, &parser->runCfg->ipSvr811 ) == EXIT_SUCCESS)
	{
//		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("811 IP is '%s'", EXT_LWIP_IPADD_TO_STR(&parser->runCfg->ipSvr811)));
	}
#if EXT_IPCMD_DEBUG
	else
	{
		EXT_INFOF( ("811 IP is not defined in get_param") );
	}
#endif

	if(extJsonParseUnsignedShort(parser, EXT_IPCMD_DATA_811_PORT, &parser->runCfg->portSvr811 ) == EXIT_SUCCESS)
	{
//		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("811 port is '%d'", parser->runCfg->portSvr811) );
	}
#if EXT_IPCMD_DEBUG
	else
	{
		EXT_INFOF( ("811 port is not defined in get_param") );
	}
#endif

	return extJsonResponsePrintConfig(parser);
}

#if 0
static char _extCfgParamsUpdate(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *newCfg)
{
#if 1
//	printf("Configuration update length :%d; save length %d"LWIP_NEW_LINE, EXT_RUNTIME_CFG_UPDATE_SIZE, EXT_RUNTIME_CFG_WRITE_SIZE);
	memcpy(((char *)runCfg+EXT_MAGIC_SIZE), ((char *)newCfg+EXT_MAGIC_SIZE), EXT_RUNTIME_CFG_UPDATE_SIZE -EXT_MAGIC_SIZE);
#else
	memcpy(runCfg, newCfg, sizeof(EXT_RUNTIME_CFG));
	sprintf(runCfg->name, "%s", EXT_767_PRODUCT_NAME);
	sprintf(runCfg->model, "%s", EXT_767_MODEL);

	snprintf(runCfg->user, EXT_USER_SIZE, "%s", EXT_USER);
	snprintf(runCfg->password, EXT_PASSWORD_SIZE, "%s", EXT_PASSWORD);

	runCfg->httpPort = EXT_HTTP_SVR_PORT;
#endif
	return 0;
}
#endif


MuxCmdHandle 	_extCmdTable[] =
{
	{
		cmd		: EXT_IPCMD_CMD_GET_PARAMS,
		handler	: _extCmdGetParams	
	},
	{
		cmd		: EXT_IPCMD_CMD_SET_PARAMS,
		handler	: extIpCmdSetupParams	
	},
	
	{
		cmd		: EXT_IPCMD_CMD_SEND_RS232,
		handler	: extIpCmdSendRsData	
	},
	{
		cmd		: EXT_IPCMD_CMD_SECURITY_CHECK,
		handler	: extIpCmdSecurityCheck	
	},

	{
		cmd		: NULL,
		handler	: NULL	
	}
};


char extJsonHandle(EXT_JSON_PARSER  *parser )
{
	char ret = 0;
	MuxCmdHandle *handle = _extCmdTable;

#if 0
	if(strncmp(parser->cmd, EXT_JSON_CMD_GET_PARAMS, strlen(EXT_JSON_CMD_GET_PARAMS)) )
	{/* except getParams: getParams begin a new session */
		EXT_UUID_T uuid;
		if(extJsonParseUuid(parser, EXT_JSON_KEY_ID, &uuid)<0)
		{
			snprintf(parser->msg, EXT_JSON_MESSAGE_SIZE, "No '"EXT_JSON_KEY_ID"' in JSON data" );
			goto err;
		}

		if(strncmp((const char *)uuid.uuid, (const char *)parser->uuid.uuid, sizeof(EXT_UUID_T)) )
		{
			snprintf(parser->msg, EXT_JSON_MESSAGE_SIZE, "'"EXT_JSON_KEY_ID"' is invalidate" );
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

#if 0
			if(EXT_DEBUG_PKTS_IS_ENABLE() )
			{
				printf("output IP CMD RES for cmd %s %p, %d bytes: '%.*s'"LWIP_NEW_LINE, 
					parser->cmd, (void *)parser, parser->outIndex, parser->outIndex-2*IPCMD_HEADER_LENGTH, parser->outBuffer+IPCMD_HEADER_LENGTH );
//				EXT_DEBUGF(EXT_IPCMD_DEBUG, ("output IP CMD RES for cmd %s %p, %d bytes: '%.*s'"LWIP_NEW_LINE, 
//					parser->cmd, (void *)parser, parser->outIndex, parser->outIndex-2*IPCMD_HEADER_LENGTH, parser->outBuffer+IPCMD_HEADER_LENGTH ));
			}
#endif

			return ret;
		}
		handle++;
	}

//err:
	snprintf(parser->msg, EXT_JSON_MESSAGE_SIZE, "No command '%s' is found!", parser->cmd);
	parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
	
	return EXIT_FAILURE;
}


