
#include "lwipExt.h"
//#include "lwip/ip4_addr.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "jsmn.h"
#include "extUdpCmd.h"


char extIpCmdIsLocal(EXT_JSON_PARSER  *parser)
{
	char ret;
	ret = extJsonUserValidate(parser);
	if(ret == EXIT_FAILURE)
	{
		return ret;
	}

	if(!MAC_ADDR_IS_EQUAL(&parser->target, &parser->runCfg->local.mac) )
	{
		snprintf(parser->msg, EXT_JSON_MESSAGE_SIZE, "'targ' is not my address" );
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


static unsigned char _rs232SendHexStr(char *str )
{
	int len = strlen(str);
	int i;
	unsigned char value;
	
#if EXT_RS232_DEBUG
	printf("Sent to RS232:"EXT_NEW_LINE);
#endif
	for(i=0; i< len/2; i++)
	{
		if(extSysAtoInt8(str+i*2, &value)== EXIT_FAILURE)
		{
			EXT_ERRORF(("'%.*s' is not an integer", 2, str+i*2));
			return EXIT_FAILURE;
		}
		
#if EXT_RS232_DEBUG
		printf("%2X ", value);
#endif

#ifdef	ARM
		extRs232Write(&value, 1);
#endif
		
	}
#if EXT_RS232_DEBUG
	printf(""EXT_NEW_LINE);
#endif


	return EXIT_SUCCESS;
}


char extIpCmdSendRsData(EXT_JSON_PARSER  *parser)
{
	char ret;

	ret = extIpCmdIsLocal(parser);
	if(ret == EXIT_FAILURE)
		return ret;

	if(extJsonParseDataArray(parser) == EXIT_FAILURE)
	{
		snprintf(parser->msg, sizeof(parser->msg), "No 'Data' array is found");
		goto parseFailed;
	}

	if(extJsonParseString(parser, EXT_IPCMD_RS232_DATA_HEX, parser->setupData.hexData, sizeof(parser->setupData.hexData)) == EXIT_FAILURE)
	{
		snprintf(parser->msg, sizeof(parser->msg), "No '%s' is found or it is invalidate value", EXT_IPCMD_RS232_DATA_HEX);
		goto parseFailed;
	}

	if(extJsonParseUnsignedShort(parser, EXT_IPCMD_RS232_WAIT_TIME, &parser->setupData.waitMs) == EXIT_FAILURE )
	{
		snprintf(parser->msg, sizeof(parser->msg), "No '%s' is found or it is invalidate value", EXT_IPCMD_RS232_WAIT_TIME);
		goto parseFailed;
	}

	if(extJsonParseUnsignedChar(parser, EXT_IPCMD_RS232_FEEDBACK, &parser->setupData.isFeedBack)  == EXIT_FAILURE )
	{
		snprintf(parser->msg, sizeof(parser->msg), "No '%s' is found or it is invalidate value", EXT_IPCMD_RS232_FEEDBACK);
		goto parseFailed;
	}

	EXT_DEBUGF(EXT_IPCMD_DEBUG, ("RS Data: hexdata:'%s'; feedback:%d; waitMs:%d",parser->setupData.hexData, parser->setupData.isFeedBack, parser->setupData.waitMs));

	if(_rs232SendHexStr(parser->setupData.hexData) == EXIT_FAILURE)
	{
		snprintf(parser->msg, sizeof(parser->msg), "'%s' is not validate when sent to RS232", parser->setupData.hexData);
		goto parseFailed;
	}
	
	parser->status = JSON_STATUS_OK;
	return extIpCmdResponseReply(parser);
	
parseFailed:
	parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
	return EXIT_FAILURE;
}


static char _securityCheckId(EXT_JSON_PARSER  *parser)
{
	int index = 0;
	char *data = NULL;
	int size = 0;

	/* get ID here */
	if(0)
	{
		snprintf(parser->msg, sizeof(parser->msg), "sub-command '%s' EXEC failed", EXT_IPCMD_SC_GET_ID);
		return EXIT_FAILURE;
	}
	else
	{
		snprintf(parser->setupData.scID, sizeof(parser->setupData.scID), "%s", "1234567890");
	}

	parser->status = JSON_STATUS_OK;

	extIpCmdResponseHeaderPrint( parser);
	data = parser->outBuffer+IPCMD_HEADER_LENGTH + parser->outIndex;
	size = parser->outSize - IPCMD_HEADER_LENGTH - parser->outIndex;
	
	index += snprintf(data+index, size-index, ",\""EXT_IPCMD_DATA_ARRAY"\":[{");
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_SC_GET_ID"\":\"%02X%02X%02X%02X%02X%02X%02X%02X\"", 
		parser->setupData.scID[0], parser->setupData.scID[1], parser->setupData.scID[2], parser->setupData.scID[3], 
		parser->setupData.scID[4], parser->setupData.scID[5], parser->setupData.scID[6], parser->setupData.scID[7] );
	index += snprintf(data+index, size-index, "}]" );

	parser->outIndex += index;

	extIpCmdResponseTailCalculate(parser);

	return EXIT_SUCCESS;
}


char extIpCmdSecurityCheck(EXT_JSON_PARSER  *parser)
{
	char ret;

#if 1
	extIpCmdSendMediaData(parser);
#endif

	ret = extIpCmdIsLocal(parser);
	if(ret == EXIT_FAILURE)
		return ret;

TRACE();	
	if(extJsonParseDataArray(parser) == EXIT_FAILURE)
	{
		snprintf(parser->msg, sizeof(parser->msg), "No 'Data' array is found");
		goto parseFailed;
	}

TRACE();	
	if(extJsonParseString(parser, EXT_IPCMD_SC_SET_KEY, parser->setupData.scKey, sizeof(parser->setupData.scKey)) == EXIT_SUCCESS)
	{ /* for set_key */
		if( 1)
		{
			parser->status = JSON_STATUS_OK;
		}
		else
		{
			parser->status = JSON_STATUS_CMD_EXEC_ERROR;
			snprintf(parser->msg, sizeof(parser->msg), "'%s' error on hardware", EXT_IPCMD_SC_GET_STATUS);
		}
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("'%s' command OK!",EXT_IPCMD_SC_SET_KEY ));
		return extIpCmdResponseReply(parser);
	}

	if(extJsonParseString(parser, EXT_IPCMD_SC_GET_STATUS,  parser->setupData.scKey, sizeof(parser->setupData.scKey) )  == EXIT_SUCCESS )
	{/* for get_status */
		if( 1)
		{
			parser->status = JSON_STATUS_OK;
		}
		else
		{
			parser->status = JSON_STATUS_CMD_EXEC_ERROR;
			snprintf(parser->msg, sizeof(parser->msg), "'%s' error on hardware", EXT_IPCMD_SC_GET_STATUS);
		}
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("'%s' command OK!",EXT_IPCMD_SC_GET_STATUS ));
		return extIpCmdResponseReply(parser);
	}

TRACE();	
	if(extJsonParseString(parser, EXT_IPCMD_SC_GET_ID, parser->setupData.scKey, sizeof(parser->setupData.scKey) ) == EXIT_FAILURE)
	{
		snprintf(parser->msg, sizeof(parser->msg), "No validate sub-command is found for '%s'", EXT_IPCMD_CMD_SECURITY_CHECK);
		goto parseFailed;
	}

TRACE();	
	if( _securityCheckId(parser) == EXIT_FAILURE)
	{
		goto parseFailed;
	}

	EXT_DEBUGF(EXT_IPCMD_DEBUG, ("'%s' command OK!",EXT_IPCMD_SC_GET_ID ));
	return EXIT_SUCCESS;
	
parseFailed:
	parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
	return EXIT_FAILURE;
}


char	extIpCmdSendMediaData(EXT_JSON_PARSER  *parser)
{
	int index = 0;
	char *data = NULL;
	int size = 0;

	/* get ID here */
	if(0)
	{
		snprintf(parser->msg, sizeof(parser->msg), "sub-command '%s' EXEC failed", EXT_IPCMD_SC_GET_ID);
		return EXIT_FAILURE;
	}
	else
	{
		snprintf(parser->setupData.scID, sizeof(parser->setupData.scID), "%s", "1234567890");
	}

	parser->status = JSON_STATUS_OK;

	extIpCmdRequestHeaderPrint( parser, EXT_IPCMD_CMD_SET_PARAMS );
	data = parser->outBuffer+IPCMD_HEADER_LENGTH + parser->outIndex;
	size = parser->outSize - IPCMD_HEADER_LENGTH - parser->outIndex;
	
	index += snprintf(data+index, size-index, ",\""EXT_IPCMD_DATA_ARRAY"\":[{");
	
	index += extIpCmdPrintMediaCfg(parser, data+index, size-index);

	index += snprintf(data+index, size-index, "}]" );

	parser->outIndex += index;

	extIpCmdResponseTailCalculate(parser);

	return extIpCmdSendout(parser, &parser->runCfg->dest.ip, EXT_CTRL_PORT);
	
}

