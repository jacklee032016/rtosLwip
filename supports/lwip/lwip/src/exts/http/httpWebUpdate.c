/*
* Update operations of web pages
*/


#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"


uint16_t extHttpWebUpdateAvParams(ExtHttpConn *ehc, void *pageHandle)
{
//	int index = 0;

	EXT_RUNTIME_CFG *rxCfg = &tmpRuntime;
	MuxHttpHandle *page = (MuxHttpHandle *)pageHandle;

	short left = ehc->leftData;
	char *data = ehc->headers + ehc->headerLength+__HTTP_CRLF_SIZE;
	char *key, *value, *nextKey;
	int i = 0;
	uint16_t headerLength;

	extSysClearConfig(rxCfg);

//	EXT_DEBUGF(EXT_DBG_ON, (EXT_NEW_LINE"Before configured, Runtime Configuration") );extSysCfgDebugData(&tmpRuntime);
	
	data[left] = 0;
	key = data;
	while(key)
	{
		value = lwip_strnstr(key, _CHAR_EQUAL, left );
		if(value == NULL)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' has no value defined", key);
			snprintf(ehc->filename, sizeof(ehc->filename), "ERROR");
			goto error;
		}
		key[value-key] = 0;
		value++;
		
//	EXT_DEBUGF(EXT_DBG_ON, ("RX vFrameRate=%d; vDepth=%d", rxCfg->runtime.vFrameRate, rxCfg->runtime.vDepth));
		nextKey = lwip_strnstr(value, _CHAR_SEPERATE,  left - (value-key)) ;
		if(nextKey)
		{
			value[nextKey-value] = 0;
		}
		else
		{
			value[left-(value-key)] = 0;
		}
		
		if(EXT_DEBUG_HTTP_IS_ENABLE())
		{
			printf("\tNo#%d: '%s' = '%s'"EXT_NEW_LINE, ++i, key, value);
		}


#if 0		
		if( lwip_strnstr(key+1, EXT_WEB_CFG_FIELD_SDP_VEDIO, (value-key-1)))
		{
			snprintf(retVal, size, "%.*s", JSON_TOKEN_LENGTH(valueObj), parser->currentJSonString + valueObj->start);
		}
#endif
		if(extHttpParseData(ehc, rxCfg, key, value) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "key '%s' : value '%s' wrong", key, value);
			snprintf(ehc->filename, sizeof(ehc->filename), "ERROR");
			goto error;
		}
//	EXT_DEBUGF(EXT_DBG_ON, ("RX vFrameRate=%d; vDepth=%d", rxCfg->runtime.vFrameRate, rxCfg->runtime.vDepth));
			
		if(nextKey)
		{
			nextKey++;
			left = left - (nextKey-key);
		}
		
		key = nextKey;
	}

//	EXT_DEBUGF(EXT_DBG_ON, ("CFG vFrameRate=%d; vDepth=%d", ehc->runCfg->runtime.vFrameRate, ehc->runCfg->runtime.vDepth));

	extSysCompareParams(ehc->runCfg, rxCfg);
	extSysConfigCtrl(ehc->runCfg, rxCfg);

//	EXT_DEBUGF(EXT_DBG_ON, ("Data:%"U32_F":%d'%.*s", ehc->contentLength, ehc->leftData, ehc->leftData, ehc->headers+ehc->headerLength+__HTTP_CRLF_SIZE));

	headerLength = cmnHttpPrintResponseHeader(ehc, page->respType);
	ehc->headerLength = headerLength;
	headerLength = 0;

	return httpWebPageResult(ehc, "OK",(char *) "New configuration has been active now");
	
error:
	ehc->httpStatusCode = WEB_RES_ERROR;
	return 0;
}



uint16_t extHttpWebUpdateNetParams(ExtHttpConn *ehc, void *pageHandle)
{
//	int index = 0;

	EXT_RUNTIME_CFG *rxCfg = &tmpRuntime;
	MuxHttpHandle *page = (MuxHttpHandle *)pageHandle;

	short left = ehc->leftData;
	char *data = ehc->headers + ehc->headerLength+__HTTP_CRLF_SIZE;
	char *key, *value, *nextKey;
	int i = 0;
	uint16_t headerLength;

	extSysClearConfig(rxCfg);

//	EXT_DEBUGF(EXT_DBG_ON, (EXT_NEW_LINE"Before configured, Runtime Configuration") );extSysCfgDebugData(&tmpRuntime);
	
	data[left] = 0;
	key = data;
	while(key)
	{
		value = lwip_strnstr(key, _CHAR_EQUAL, left );
		if(value == NULL)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' has no value defined", key);
			snprintf(ehc->filename, sizeof(ehc->filename), "ERROR");
			goto error;
		}
		key[value-key] = 0;
		value++;
		
//	EXT_DEBUGF(EXT_DBG_ON, ("RX vFrameRate=%d; vDepth=%d", rxCfg->runtime.vFrameRate, rxCfg->runtime.vDepth));
		nextKey = lwip_strnstr(value, _CHAR_SEPERATE,  left - (value-key)) ;
		if(nextKey)
		{
			value[nextKey-value] = 0;
		}
		else
		{
			value[left-(value-key)] = 0;
		}
		
		if(EXT_DEBUG_HTTP_IS_ENABLE())
		{
			printf("\tNo#%d: '%s' = '%s'"EXT_NEW_LINE, ++i, key, value);
		}


#if 0		
		if( lwip_strnstr(key+1, EXT_WEB_CFG_FIELD_SDP_VEDIO, (value-key-1)))
		{
			snprintf(retVal, size, "%.*s", JSON_TOKEN_LENGTH(valueObj), parser->currentJSonString + valueObj->start);
		}
#endif
		if(extHttpParseData(ehc, rxCfg, key, value) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "key '%s' : value '%s' wrong", key, value);
			snprintf(ehc->filename, sizeof(ehc->filename), "ERROR");
			goto error;
		}
//	EXT_DEBUGF(EXT_DBG_ON, ("RX vFrameRate=%d; vDepth=%d", rxCfg->runtime.vFrameRate, rxCfg->runtime.vDepth));
			
		if(nextKey)
		{
			nextKey++;
			left = left - (nextKey-key);
		}
		
		key = nextKey;
	}

//	EXT_DEBUGF(EXT_DBG_ON, ("CFG vFrameRate=%d; vDepth=%d", ehc->runCfg->runtime.vFrameRate, ehc->runCfg->runtime.vDepth));

	extSysCompareParams(ehc->runCfg, rxCfg);
	extSysConfigCtrl(ehc->runCfg, rxCfg);

//	EXT_DEBUGF(EXT_DBG_ON, ("Data:%"U32_F":%d'%.*s", ehc->contentLength, ehc->leftData, ehc->leftData, ehc->headers+ehc->headerLength+__HTTP_CRLF_SIZE));

	headerLength = cmnHttpPrintResponseHeader(ehc, page->respType);
	ehc->headerLength = headerLength;
	headerLength = 0;

	return httpWebPageResult(ehc, "OK",(char *) "New configuration can been active after reboot");
	
error:
	ehc->httpStatusCode = WEB_RES_ERROR;
	return 0;
}



uint16_t extHttpWebUpdateRs232Params(ExtHttpConn *ehc, void *pageHandle)
{
//	int index = 0;

	EXT_RUNTIME_CFG *rxCfg = &tmpRuntime;
	MuxHttpHandle *page = (MuxHttpHandle *)pageHandle;

	short left = ehc->leftData;
	char *data = ehc->headers + ehc->headerLength+__HTTP_CRLF_SIZE;
	char *key, *value, *nextKey;
	int i = 0;
	uint16_t headerLength;

	extSysClearConfig(rxCfg);

//	EXT_DEBUGF(EXT_DBG_ON, (EXT_NEW_LINE"Before configured, Runtime Configuration") );extSysCfgDebugData(&tmpRuntime);
	
	data[left] = 0;
	key = data;
	while(key)
	{
		value = lwip_strnstr(key, _CHAR_EQUAL, left );
		if(value == NULL)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' has no value defined", key);
			snprintf(ehc->filename, sizeof(ehc->filename), "ERROR");
			goto error;
		}
		key[value-key] = 0;
		value++;
		
//	EXT_DEBUGF(EXT_DBG_ON, ("RX vFrameRate=%d; vDepth=%d", rxCfg->runtime.vFrameRate, rxCfg->runtime.vDepth));
		nextKey = lwip_strnstr(value, _CHAR_SEPERATE,  left - (value-key)) ;
		if(nextKey)
		{
			value[nextKey-value] = 0;
		}
		else
		{
			value[left-(value-key)] = 0;
		}
		
		if(EXT_DEBUG_HTTP_IS_ENABLE())
		{
			printf("\tNo#%d: '%s' = '%s'"EXT_NEW_LINE, ++i, key, value);
		}


#if 0		
		if( lwip_strnstr(key+1, EXT_WEB_CFG_FIELD_SDP_VEDIO, (value-key-1)))
		{
			snprintf(retVal, size, "%.*s", JSON_TOKEN_LENGTH(valueObj), parser->currentJSonString + valueObj->start);
		}
#endif
		if(extHttpParseData(ehc, rxCfg, key, value) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "key '%s' : value '%s' wrong", key, value);
			snprintf(ehc->filename, sizeof(ehc->filename), "ERROR");
			goto error;
		}
//	EXT_DEBUGF(EXT_DBG_ON, ("RX vFrameRate=%d; vDepth=%d", rxCfg->runtime.vFrameRate, rxCfg->runtime.vDepth));
			
		if(nextKey)
		{
			nextKey++;
			left = left - (nextKey-key);
		}
		
		key = nextKey;
	}

//	EXT_DEBUGF(EXT_DBG_ON, ("CFG vFrameRate=%d; vDepth=%d", ehc->runCfg->runtime.vFrameRate, ehc->runCfg->runtime.vDepth));

	extSysCompareParams(ehc->runCfg, rxCfg);
	extSysConfigCtrl(ehc->runCfg, rxCfg);

//	EXT_DEBUGF(EXT_DBG_ON, ("Data:%"U32_F":%d'%.*s", ehc->contentLength, ehc->leftData, ehc->leftData, ehc->headers+ehc->headerLength+__HTTP_CRLF_SIZE));

	headerLength = cmnHttpPrintResponseHeader(ehc, page->respType);
	ehc->headerLength = headerLength;
	headerLength = 0;

	return httpWebPageResult(ehc, "OK",(char *) "New configuration has been active now");
	
error:
	ehc->httpStatusCode = WEB_RES_ERROR;
	return 0;
}


