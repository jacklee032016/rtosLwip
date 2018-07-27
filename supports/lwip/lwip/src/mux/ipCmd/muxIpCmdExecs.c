
#include "lwipMux.h"
//#include "lwip/ip4_addr.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "jsmn.h"
#include "muxIpCmd.h"


char muxIpCmdIsLocal(MUX_JSON_PARSER  *parser)
{
	char ret;
	ret = muxJsonUserValidate(parser);
	if(ret == EXIT_FAILURE)
	{
		return ret;
	}

	if(!MAC_ADDR_IS_EQUAL(&parser->target, &parser->runCfg->local.mac) )
	{
		snprintf(parser->msg, MUX_JSON_MESSAGE_SIZE, "'targ' is not my address" );
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


static unsigned char _rs232SendHexStr(char *str )
{
	int len = strlen(str);
	int i;
	unsigned char value;
	
#if MUX_RS232_DEBUG
	printf("Sent to RS232:"MUX_NEW_LINE);
#endif
	for(i=0; i< len/2; i++)
	{
		if(muxSysAtoInt8(str+i*2, &value)== EXIT_FAILURE)
		{
			MUX_ERRORF(("'%.*s' is not an integer", 2, str+i*2));
			return EXIT_FAILURE;
		}
		
#if MUX_RS232_DEBUG
		printf("%2X ", value);
#endif

#ifdef	ARM
		muxRs232Write(&value, 1);
#endif
		
	}
#if MUX_RS232_DEBUG
	printf(""MUX_NEW_LINE);
#endif


	return EXIT_SUCCESS;
}


char muxIpCmdSendRsData(MUX_JSON_PARSER  *parser)
{
	char ret;

	ret = muxIpCmdIsLocal(parser);
	if(ret == EXIT_FAILURE)
		return ret;

	if(muxJsonParseDataArray(parser) == EXIT_FAILURE)
	{
		snprintf(parser->msg, sizeof(parser->msg), "No 'Data' array is found");
		goto parseFailed;
	}

	if(muxJsonParseString(parser, MUX_IPCMD_RS232_DATA_HEX, parser->setupData.hexData, sizeof(parser->setupData.hexData)) == EXIT_FAILURE)
	{
		snprintf(parser->msg, sizeof(parser->msg), "No '%s' is found or it is invalidate value", MUX_IPCMD_RS232_DATA_HEX);
		goto parseFailed;
	}

	if(muxJsonParseUnsignedShort(parser, MUX_IPCMD_RS232_WAIT_TIME, &parser->setupData.waitMs) == EXIT_FAILURE )
	{
		snprintf(parser->msg, sizeof(parser->msg), "No '%s' is found or it is invalidate value", MUX_IPCMD_RS232_WAIT_TIME);
		goto parseFailed;
	}

	if(muxJsonParseUnsignedChar(parser, MUX_IPCMD_RS232_FEEDBACK, &parser->setupData.isFeedBack)  == EXIT_FAILURE )
	{
		snprintf(parser->msg, sizeof(parser->msg), "No '%s' is found or it is invalidate value", MUX_IPCMD_RS232_FEEDBACK);
		goto parseFailed;
	}

	MUX_DEBUGF(MUX_IPCMD_DEBUG, ("RS Data: hexdata:'%s'; feedback:%d; waitMs:%d",parser->setupData.hexData, parser->setupData.isFeedBack, parser->setupData.waitMs));

	if(_rs232SendHexStr(parser->setupData.hexData) == EXIT_FAILURE)
	{
		snprintf(parser->msg, sizeof(parser->msg), "'%s' is not validate when sent to RS232", parser->setupData.hexData);
		goto parseFailed;
	}
	
	parser->status = JSON_STATUS_OK;
	return muxIpCmdResponseReply(parser);
	
parseFailed:
	parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
	return EXIT_FAILURE;
}


static char _securityCheckId(MUX_JSON_PARSER  *parser)
{
	int index = 0;
	char *data = NULL;
	int size = 0;

	/* get ID here */
	if(0)
	{
		snprintf(parser->msg, sizeof(parser->msg), "sub-command '%s' EXEC failed", MUX_IPCMD_SC_GET_ID);
		return EXIT_FAILURE;
	}
	else
	{
		snprintf(parser->setupData.scID, sizeof(parser->setupData.scID), "%s", "1234567890");
	}

	parser->status = JSON_STATUS_OK;

	muxIpCmdResponseHeaderPrint( parser);
	data = parser->outBuffer+IPCMD_HEADER_LENGTH + parser->outIndex;
	size = parser->outSize - IPCMD_HEADER_LENGTH - parser->outIndex;
	
	index += snprintf(data+index, size-index, ",\""MUX_IPCMD_DATA_ARRAY"\":[{");
	index += snprintf(data+index, size-index, "\""MUX_IPCMD_SC_GET_ID"\":\"%02X%02X%02X%02X%02X%02X%02X%02X\"", 
		parser->setupData.scID[0], parser->setupData.scID[1], parser->setupData.scID[2], parser->setupData.scID[3], 
		parser->setupData.scID[4], parser->setupData.scID[5], parser->setupData.scID[6], parser->setupData.scID[7] );
	index += snprintf(data+index, size-index, "}]" );

	parser->outIndex += index;

	muxIpCmdResponseTailCalculate(parser);

	return EXIT_SUCCESS;
}


char muxIpCmdSecurityCheck(MUX_JSON_PARSER  *parser)
{
	char ret;

#if 1
	muxIpCmdSendMediaData(parser);
#endif

	ret = muxIpCmdIsLocal(parser);
	if(ret == EXIT_FAILURE)
		return ret;

TRACE();	
	if(muxJsonParseDataArray(parser) == EXIT_FAILURE)
	{
		snprintf(parser->msg, sizeof(parser->msg), "No 'Data' array is found");
		goto parseFailed;
	}

TRACE();	
	if(muxJsonParseString(parser, MUX_IPCMD_SC_SET_KEY, parser->setupData.scKey, sizeof(parser->setupData.scKey)) == EXIT_SUCCESS)
	{ /* for set_key */
		if( 1)
		{
			parser->status = JSON_STATUS_OK;
		}
		else
		{
			parser->status = JSON_STATUS_CMD_EXEC_ERROR;
			snprintf(parser->msg, sizeof(parser->msg), "'%s' error on hardware", MUX_IPCMD_SC_GET_STATUS);
		}
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("'%s' command OK!",MUX_IPCMD_SC_SET_KEY ));
		return muxIpCmdResponseReply(parser);
	}

	if(muxJsonParseString(parser, MUX_IPCMD_SC_GET_STATUS,  parser->setupData.scKey, sizeof(parser->setupData.scKey) )  == EXIT_SUCCESS )
	{/* for get_status */
		if( 1)
		{
			parser->status = JSON_STATUS_OK;
		}
		else
		{
			parser->status = JSON_STATUS_CMD_EXEC_ERROR;
			snprintf(parser->msg, sizeof(parser->msg), "'%s' error on hardware", MUX_IPCMD_SC_GET_STATUS);
		}
		MUX_DEBUGF(MUX_IPCMD_DEBUG, ("'%s' command OK!",MUX_IPCMD_SC_GET_STATUS ));
		return muxIpCmdResponseReply(parser);
	}

TRACE();	
	if(muxJsonParseString(parser, MUX_IPCMD_SC_GET_ID, parser->setupData.scKey, sizeof(parser->setupData.scKey) ) == EXIT_FAILURE)
	{
		snprintf(parser->msg, sizeof(parser->msg), "No validate sub-command is found for '%s'", MUX_IPCMD_CMD_SECURITY_CHECK);
		goto parseFailed;
	}

TRACE();	
	if( _securityCheckId(parser) == EXIT_FAILURE)
	{
		goto parseFailed;
	}

	MUX_DEBUGF(MUX_IPCMD_DEBUG, ("'%s' command OK!",MUX_IPCMD_SC_GET_ID ));
	return EXIT_SUCCESS;
	
parseFailed:
	parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
	return EXIT_FAILURE;
}


char	muxIpCmdSendMediaData(MUX_JSON_PARSER  *parser)
{
	int index = 0;
	char *data = NULL;
	int size = 0;

	/* get ID here */
	if(0)
	{
		snprintf(parser->msg, sizeof(parser->msg), "sub-command '%s' EXEC failed", MUX_IPCMD_SC_GET_ID);
		return EXIT_FAILURE;
	}
	else
	{
		snprintf(parser->setupData.scID, sizeof(parser->setupData.scID), "%s", "1234567890");
	}

	parser->status = JSON_STATUS_OK;

	muxIpCmdRequestHeaderPrint( parser, MUX_IPCMD_CMD_SET_PARAMS );
	data = parser->outBuffer+IPCMD_HEADER_LENGTH + parser->outIndex;
	size = parser->outSize - IPCMD_HEADER_LENGTH - parser->outIndex;
	
	index += snprintf(data+index, size-index, ",\""MUX_IPCMD_DATA_ARRAY"\":[{");
	
	index += muxIpCmdPrintMediaCfg(parser, data+index, size-index);

	index += snprintf(data+index, size-index, "}]" );

	parser->outIndex += index;

	muxIpCmdResponseTailCalculate(parser);

	return muxIpCmdSendout(parser, &parser->runCfg->dest.ip, MUX_CTRL_PORT);
	
}

