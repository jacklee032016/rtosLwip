/*
* Web pages service in CGI
*/


#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"


static uint16_t _extHttpWebPageReboot(ExtHttpConn  *ehc, void *data)
{
	int index = 0;
	char *dataBuf = (char *)ehc->data+ehc->headerLength;
	uint16_t size = sizeof(ehc->data) - ehc->headerLength;

#if 1
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"title\"><H2>Reboot</H2></DIV>"); 
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"fields-info\"><DIV class=\"field\"><DIV>Waiting.....</DIV></DIV></DIV>");
#else
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL class=\"label\" >Waiting.....</LABEL></DIV>\n<script type=\"text/javascript\">\n");
	CMN_SN_PRINTF(dataBuf, size, index, "\nsetTimeout(function(){alert(\"reload\") }, %d);\n", 
		       seconds*1000);
	CMN_SN_PRINTF(dataBuf, size, index, "\n</script>\n");
#endif


#ifdef	ARM
//	EXT_DELAY_MS(1500);
//	EXT_REBOOT();
	extDelayReboot(1000);
#endif

	ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	return index;
}


#define	FORM_ID_CFG_DATA		"formSettingsData"
#define	FORM_ID_CFG_SDP		"formSettingsSdp"

//#define	FORM_ID		"formFirmware"
static uint16_t _extHttpWebPageMediaHander(ExtHttpConn  *ehc, void *pageHandle)
{
	int index = 0;
	EXT_RUNTIME_CFG	*runCfg = ehc->runCfg;
	struct netif *_netif = (struct netif *)runCfg->netif;
	const EXT_CONST_STR *_str;
	const char *chVal;
	const short *shVal;

	char *dataBuf = (char *)ehc->data+ehc->headerLength;
	uint16_t size = sizeof(ehc->data) - ehc->headerLength;
	
	/* device */
//	CMN_SN_PRINTF(dataBuf, size, index, "<DIV id=\"forms\"><FORM method=\"post\" id=\""FORM_ID"\" name=\""FORM_ID"\"  enctype=\"text/plain\" action=\"/setting\">" );

//	<FORM method="post" id="formSettings" enctype=" action="action_page.php">    
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"title\"><H2>Video/Audio Settings</H2></DIV>"EXT_NEW_LINE "<DIV class=\"fields\">"EXT_NEW_LINE);
	
	
	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >SDP Video:</LABEL>");
	if(EXT_IS_TX(runCfg) )
	{
		CMN_SN_PRINTF(dataBuf, size, index, "http://%s/%s", inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)), EXT_WEBPAGE_SDP_VIDEO);
	}
	else
	{
		CMN_SN_PRINTF(dataBuf, size, index, "<DIV id=\"forms\"><FORM method=\"post\" id=\""FORM_ID_CFG_SDP"\" >" EXT_NEW_LINE );
		CMN_SN_PRINTF(dataBuf, size, index, "http://<INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_SDP_VEDIO_IP"\" value=\"%s\" style=\"width: 115px;\"/>:" EXT_NEW_LINE
			"<INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_SDP_VEDIO_PORT"\" value=\"%d\" style=\"width: 30px;\"/>"EXT_NEW_LINE
			"<INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_SDP_VEDIO_URI"\" value=\"%s\"/>" EXT_NEW_LINE,
			inet_ntoa(*(struct in_addr *)&(runCfg->sdpUriVideo.ip)), runCfg->sdpUriVideo.port, runCfg->sdpUriVideo.uri );
	}
	
	CMN_SN_PRINTF(dataBuf, size, index, "</DIV>"EXT_NEW_LINE"\t<DIV class=\"field\"><LABEL >SDP Audio:</LABEL>");
	if(EXT_IS_TX(runCfg) )
	{
		CMN_SN_PRINTF(dataBuf, size, index, "http://%s/%s"EXT_NEW_LINE"</DIV>", inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)), EXT_WEBPAGE_SDP_AUDIO);
	}
	else
	{
		CMN_SN_PRINTF(dataBuf, size, index, "http://<INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_SDP_AUDIO_IP"\" value=\"%s\" style=\"width: 115px;\"/>:"EXT_NEW_LINE
			"<INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_SDP_AUDIO_PORT"\" value=\"%d\" style=\"width: 30px;\"/>"EXT_NEW_LINE
			"<INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_SDP_AUDIO_URI"\" value=\"%s\"/>" EXT_NEW_LINE,
			inet_ntoa(*(struct in_addr *)&(runCfg->sdpUriAudio.ip)), runCfg->sdpUriAudio.port, runCfg->sdpUriAudio.uri );
		
	//<INPUT type="button" value="Submit" class="btnSubmit" id="btnSubmit" />
#if 1
		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"buttons\">"EXT_NEW_LINE"\t<INPUT type=\"button\" value=\"Apply\" class=\"btnSubmit\" onClick=\"submit_firmware('"FORM_ID_CFG_SDP"','"EXT_WEBPAGE_SDP_CLIENT"')\"/>"EXT_NEW_LINE
#else
		CMN_SN_PRINTF(data, size, index,  "\t<DIV class=\"buttons\">"EXT_NEW_LINE"\t<INPUT type=\"submit\" value=\"Submit\" class=\"btnSubmit\" id=\"btnSubmit\" />"EXT_NEW_LINE
#endif
				"\t<INPUT name=\"ResetButton\" type=\"reset\" class=\"btnReset\" value=\"Cancel\" id=\"ResetButton\"/></DIV>"EXT_NEW_LINE );

		CMN_SN_PRINTF(dataBuf, size, index, "\t</DIV>"EXT_NEW_LINE"</DIV>"EXT_NEW_LINE"</FORM>"EXT_NEW_LINE );
	}


	CMN_SN_PRINTF(dataBuf, size, index, EXT_NEW_LINE EXT_NEW_LINE"<DIV id=\"forms\"><FORM method=\"post\" id=\""FORM_ID_CFG_DATA"\" >" );

	CMN_SN_PRINTF(dataBuf, size, index, ""EXT_NEW_LINE"\t<DIV class=\"field\"><LABEL >Name:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_PRODUCT"\" value=\"%s\"/></DIV>"EXT_NEW_LINE, 
		runCfg->name);//

//	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >Video IP Address:</LABEL><DIV class=\"label\" ><INPUT type=\"text\" id=\""EXT_WEB_CFG_FIELD_IP_VEDIO"\" value=\"%s\"/></DIV></DIV>", 
//		EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.ip)) );
	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Video IP Address:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_IP_VEDIO"\" value=\"%s\"/></DIV>"EXT_NEW_LINE, 
		EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.ip)) );

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Audio IP Address:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_IP_AUDIO"\" value=\"%s\"/></DIV>"EXT_NEW_LINE, 
		EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.audioIp)) );

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Video Port:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_PORT_VEDIO"\" value=\"%d\"/></DIV>"EXT_NEW_LINE, 
		runCfg->dest.vport );
	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Audio Port:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_PORT_AUDIO"\" value=\"%d\"/></DIV>"EXT_NEW_LINE, 
		runCfg->dest.aport );

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Aux Data Port:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_PORT_DATA"\" value=\"%d\"/></DIV>"EXT_NEW_LINE, 
		runCfg->dest.dport );
	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Steam Port:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_PORT_STREM"\" value=\"%d\"/></DIV>"EXT_NEW_LINE, 
		runCfg->dest.sport );

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Resolution:</LABEL>"EXT_NEW_LINE);

	CMN_SN_PRINTF(dataBuf, size, index, "\t\t<SELECT id=\""EXT_WEB_CFG_FIELD_VIDEO_WIDTH"\">"EXT_NEW_LINE );
	shVal = videoWidthList;
	while(*shVal != 0)
	{
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, *shVal, (*shVal== runCfg->runtime.vWidth)?"selected":"", *shVal);
		shVal++;
	}
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT>x"EXT_NEW_LINE);

	CMN_SN_PRINTF(dataBuf, size, index, "\t\t<SELECT id=\""EXT_WEB_CFG_FIELD_VIDEO_HEIGHT"\">" EXT_NEW_LINE);
	shVal = videoHeightList;
	while(*shVal != 0)
	{
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, *shVal, (*shVal== runCfg->runtime.vHeight)?"selected":"", *shVal);
		shVal++;
	}
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);

	
	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Frame Rate:</LABEL>"EXT_NEW_LINE"\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_FRAME_RATE"\">" EXT_NEW_LINE);
	chVal = videoFpsList;
	while(*chVal != 0)
	{
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, *chVal, (*chVal== runCfg->runtime.vFrameRate)?"selected":"", *chVal);
		chVal++;
	}
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);

	/* color depth */
	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Color Depth:</LABEL>"EXT_NEW_LINE"\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_COLOR_DEPTH"\">"EXT_NEW_LINE);
	chVal = videoColorDepthList;
	while(*chVal != 0)
	{
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, *chVal, (*chVal== runCfg->runtime.vDepth)?"selected":"", *chVal);
		chVal++;
	}
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);

	/* color space*/
	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Color Space:</LABEL>"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_COLOR_SPACE"\">"EXT_NEW_LINE);
	_str = _videoColorSpaces;
	while(_str->type!= EXT_INVALIDATE_STRING_TYPE)
	{
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%s</OPTION>"EXT_NEW_LINE, _str->type, (_str->type== runCfg->runtime.vColorSpace)?"selected":"", _str->name);
		_str++;
	}
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);


#if 0	
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >Audio Frequency:</LABEL><DIV class=\"label\" >%d</DIV></DIV>", runCfg->runtime.aSampleRate);
	CMN_SN_PRINTF(dataBuf, size, index,  "<DIV class=\"field\"><LABEL >Audio Bitrate:</LABEL><DIV class=\"label\" >%d</DIV></DIV>", runCfg->runtime.aDepth );
#endif

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Audio Channels:</LABEL>"EXT_NEW_LINE"\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_AUDIO_CHANNEL"\">"EXT_NEW_LINE );
	chVal = audioChannelsList;
	while(*chVal != 0)
	{
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, *chVal, (*chVal== runCfg->runtime.aChannels)?"selected":"", *chVal);
		chVal++;
	}
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);


//<INPUT type="button" value="Submit" class="btnSubmit" id="btnSubmit" />
#if 1
	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"buttons\">"EXT_NEW_LINE"\t<INPUT type=\"button\" value=\"Submit\" class=\"btnSubmit\" onClick=\"submit_firmware('"FORM_ID_CFG_DATA"','"EXT_WEBPAGE_SETTING"')\"/>"EXT_NEW_LINE
#else
	CMN_SN_PRINTF(data, size, index,  "\t<DIV class=\"buttons\">"EXT_NEW_LINE"\t<INPUT type=\"submit\" value=\"Submit\" class=\"btnSubmit\" id=\"btnSubmit\" />"EXT_NEW_LINE
#endif
			"\t<INPUT name=\"ResetButton\" type=\"reset\" class=\"btnReset\" value=\"Cancel\" id=\"ResetButton\"/></DIV>"EXT_NEW_LINE );

	CMN_SN_PRINTF(dataBuf, size, index, "\t</DIV>"EXT_NEW_LINE"</DIV>"EXT_NEW_LINE"</FORM></DIV>"EXT_NEW_LINE );

#if 0
#if 1
	CMN_SN_PRINTF(dataBuf, size, index,  "<SCRIPT type=\"text/javascript\">function submit_setting(chip){ var form = document.getElementById('"FORM_ID"');");
	CMN_SN_PRINTF(dataBuf, size, index,  "var formData = new FormData(form);xhr = new XMLHttpRequest();xhr.open('POST', chip);xhr.send(formData);");
#else
	CMN_SN_PRINTF(dataBuf, size, index,  "<SCRIPT type=\"text/javascript\">function submit_setting(chip){ alert('Submit POST'); document."FORM_ID".submit();");
#endif
	CMN_SN_PRINTF(dataBuf, size, index,  "document.getElementById('content').innerHTML= '<img src=\"/loading.gif\" />';");
	CMN_SN_PRINTF(dataBuf, size, index,  "xhr.onload = function()" 
		"{if (xhr.status !== 200) {alert('Request failed.  Returned status of ' + xhr.status);} "
		"else{document.getElementById('content').innerHTML=xhr.responseText;}};}" 	);
	CMN_SN_PRINTF(dataBuf, size, index,  "</SCRIPT>");
#endif

	printf("Data:'%s'\r\n", dataBuf);
	
	ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	return index;
}



#ifdef	ARM
//_CODE char *versionString;
#endif

static uint16_t _extHttpWebPageInfoHander(ExtHttpConn  *ehc, void *pageHandle)
{
	int index = 0;
	EXT_RUNTIME_CFG	*runCfg = ehc->runCfg;
	char *dataBuf = (char *)ehc->data+ehc->headerLength;
	uint16_t size = sizeof(ehc->data) - ehc->headerLength;
	
	/* device */
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"title\"><H2>Device</H2></DIV><DIV class=\"fields-info\"><DIV class=\"field\"><LABEL >Product Name:</LABEL><DIV class=\"label\">%s</DIV></DIV>",
		EXT_767_PRODUCT_NAME);
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >Model:</LABEL><DIV class=\"label\">%s-%s</DIV></DIV>"EXT_NEW_LINE, EXT_767_MODEL, EXT_IS_TX(runCfg)?"TX":"RX" );
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >Custom Name:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", runCfg->name);
#ifdef	ARM
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >Firmware Version:</LABEL><DIV class=\"label\">%s(Build %s)</DIV></DIV>",  EXT_VERSION_STRING, BUILD_DATE_TIME );
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >FPGA Version:</LABEL><DIV class=\"label\">%s</DIV></DIV>",  extFgpaReadVersion() );
#else
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >Firmware Version:</LABEL><DIV class=\"label\">%02d.%02d.%02d</DIV></DIV>",
		runCfg->version.major, runCfg->version.minor, runCfg->version.revision);
#endif
	CMN_SN_PRINTF(dataBuf, size, index, "</DIV>");

	/* settings */
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"title\"><H2>Settings</H2></DIV><DIV class=\"fields-info\"><DIV class=\"field\"><LABEL >MAC Address:</LABEL><DIV class=\"label\" >");
	MAC_ADDRESS_PRINT(dataBuf, size, index, &(runCfg->local.mac));
	CMN_SN_PRINTF(dataBuf, size, index, "</DIV></DIV>");

	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >IP Address:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", EXT_LWIP_IPADD_TO_STR(&(runCfg->local.ip)) );

	CMN_SN_PRINTF(dataBuf, size, index, " <DIV class=\"field\"><LABEL >Subnet Mask:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", EXT_LWIP_IPADD_TO_STR(&(runCfg->ipMask)) );

	CMN_SN_PRINTF(dataBuf, size, index, " <DIV class=\"field\"><LABEL >Gateway:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", EXT_LWIP_IPADD_TO_STR(&(runCfg->ipGateway)) );
	CMN_SN_PRINTF(dataBuf, size, index, " <DIV class=\"field\"><LABEL >DHCP:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", STR_BOOL_VALUE(runCfg->netMode) );
	CMN_SN_PRINTF(dataBuf, size, index, " <DIV class=\"field\"><LABEL >Dipswitch:</LABEL><DIV class=\"label\" >%s</DIV></DIV></DIV>", STR_BOOL_VALUE(runCfg->isDipOn) );

	/* RS232 */
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"title\"><H2>RS232</H2></DIV><DIV class=\"fields-info\"><DIV class=\"field\"><LABEL >Baudrate:</LABEL><DIV class=\"label\">%d</DIV></DIV>",
		runCfg->rs232Cfg.baudRate);

	CMN_SN_PRINTF(dataBuf, size, index,  "<DIV class=\"field\"><LABEL >Databits:</LABEL><DIV class=\"label\">%d</DIV></DIV>",
		runCfg->rs232Cfg.charLength);
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >Parity:</LABEL><DIV class=\"label\">%s</DIV></DIV>",
		CMN_FIND_RS_PARITY((unsigned short)runCfg->rs232Cfg.parityType) );
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >Stopbits:</LABEL><DIV class=\"label\">%d</DIV></DIV>",
		runCfg->rs232Cfg.stopbits);

	CMN_SN_PRINTF(dataBuf, size, index, "</DIV></DIV>" );

	ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	return index;
}


static uint16_t _extHttpWebSdpClientHander(ExtHttpConn *ehc, void *pageHandle)
{
//	int index = 0;
#define	_CHAR_SEPERATE			"&"
#define	_CHAR_EQUAL				"="

	EXT_RUNTIME_CFG *rxCfg = &tmpRuntime;
	MuxHttpHandle *page = (MuxHttpHandle *)pageHandle;

	short left = ehc->leftData;
	char *data = ehc->headers + ehc->headerLength+__HTTP_CRLF_SIZE;
	char *key, *value, *nextKey;
	err_t ret, ret2 ;
	int i = 0;
	uint16_t headerLength;

	extSysClearConfig(rxCfg);
	
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
		
		nextKey = lwip_strnstr(value, _CHAR_SEPERATE,  left - (value-key)) ;
		if(nextKey)
		{
			value[nextKey-value] = 0;
		}
		else
		{
			value[left-(value-key)] = 0;
		}
		printf("No#%d: '%s' = '%s'" EXT_NEW_LINE, ++i, key, value );


#if 0		
		if( lwip_strnstr(key+1, EXT_WEB_CFG_FIELD_SDP_VEDIO, (value-key-1)))
		{
			snprintf(retVal, size, "%.*s", JSON_TOKEN_LENGTH(valueObj), parser->currentJSonString + valueObj->start);
		}
#endif
		if(extHttpParseSdpClientData(ehc, rxCfg, key, value) == EXIT_FAILURE)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "key '%s' : value '%s' wrong", key, value);
			snprintf(ehc->filename, sizeof(ehc->filename), "ERROR");
			goto error;
		}
			
		if(nextKey)
		{
			nextKey++;
			left = left - (nextKey-key);
		}
		
		key = nextKey;
	}

	extSysConfigSdpClient(ehc->runCfg, rxCfg);

	ret = extHttpClientNewRequest(&ehc->runCfg->sdpUriVideo) ;
	ret2 = extHttpClientNewRequest(&ehc->runCfg->sdpUriAudio);

	EXT_DEBUGF(EXT_DBG_ON, ("Data:%d:%d'%.*s", ehc->contentLength, ehc->leftData, ehc->leftData, ehc->headers+ehc->headerLength+__HTTP_CRLF_SIZE));

	headerLength = cmnHttpPrintResponseHeader(ehc, page->respType);
	ehc->headerLength = headerLength;
	headerLength = 0;

	if(ret == ERR_OK && ret2 == ERR_ALREADY )
	{
		CMN_SN_PRINTF((char *)ehc->data+ehc->headerLength, sizeof(ehc->data)-ehc->headerLength, headerLength, 
			"<DIV class=\"title\"><H2>%s</H2></DIV>"EXT_NEW_LINE"<DIV class=\"fields-info\">"EXT_NEW_LINE"\t<DIV class=\"field\"><LABEL >Result:%s</LABEL></DIV>"EXT_NEW_LINE"</DIV>"EXT_NEW_LINE EXT_NEW_LINE,
			"OK, Waiting", "Requesting, parsing SDP, and configuring hardware now...");
		
		ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	}
	else
	{
		snprintf(ehc->boundary, sizeof(ehc->boundary), "%s", "HTTP Client is busy now");
		ehc->httpStatusCode = WEB_RES_ERROR;
	}
	
	return headerLength;
error:

	ehc->httpStatusCode = WEB_RES_ERROR;
	return 0;
}

static uint16_t _extHttpWebSettingHander(ExtHttpConn *ehc, void *pageHandle)
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
		
		nextKey = lwip_strnstr(value, _CHAR_SEPERATE,  left - (value-key)) ;
		if(nextKey)
		{
			value[nextKey-value] = 0;
		}
		else
		{
			value[left-(value-key)] = 0;
		}
		printf("No#%d: '%s' = '%s'" EXT_NEW_LINE, ++i, key, value );


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
			
		if(nextKey)
		{
			nextKey++;
			left = left - (nextKey-key);
		}
		
		key = nextKey;
	}

	extSysCompareParams(ehc->runCfg, rxCfg);
	extSysConfigCtrl(ehc->runCfg, rxCfg);

	EXT_DEBUGF(EXT_DBG_ON, ("Data:%d:%d'%.*s", ehc->contentLength, ehc->leftData, ehc->leftData, ehc->headers+ehc->headerLength+__HTTP_CRLF_SIZE));

	headerLength = cmnHttpPrintResponseHeader(ehc, page->respType);
	ehc->headerLength = headerLength;
	headerLength = 0;

	CMN_SN_PRINTF((char *)ehc->data+ehc->headerLength, sizeof(ehc->data)-ehc->headerLength, headerLength, 
		"<DIV class=\"title\"><H2>%s</H2></DIV>"EXT_NEW_LINE"<DIV class=\"fields-info\">"EXT_NEW_LINE"\t<DIV class=\"field\"><LABEL >Result:%s</LABEL></DIV>"EXT_NEW_LINE"</DIV>"EXT_NEW_LINE EXT_NEW_LINE,
		"OK", "New Configuration has been active now");
	
	ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	return headerLength;
error:

	ehc->httpStatusCode = WEB_RES_ERROR;
	return 0;
}


uint16_t extHttpWebPageRootHander(ExtHttpConn  *ehc, void *pageHandle)
{
	int index = 0;
	char *dataBuf = (char *)ehc->data+ehc->headerLength;
	uint16_t size = sizeof(ehc->data) - ehc->headerLength;
	
	CMN_SN_PRINTF(dataBuf, size, index, "<HTML><HEAD><TITLE>Muxlab %s-500767</TITLE>", EXT_IS_TX(&extRun)?"TX":"RX" );
	CMN_SN_PRINTF(dataBuf, size, index, "<LINK href=\"/styles.css\" type=\"text/css\" rel=\"stylesheet\"><SCRIPT type=\"text/javascript\" src=\"/load_html.js\"></SCRIPT></HEAD>"EXT_NEW_LINE );
	CMN_SN_PRINTF(dataBuf, size, index, "<BODY onload=\"JavaScript:load_http_doc('%s', 'content','')\"><DIV id=\"body\"><DIV id=\"header\"><a id=\"logo\" href=\"/\"><img alt=\"Muxlab Control Panel\" src=\"/logo.jpg\"></a><br />",
		EXT_WEBPAGE_MEDIA);
	CMN_SN_PRINTF(dataBuf, size, index, "<div data-text=\"dt_productName\" id=\"id_mainProductName\">500767-%s-UTP 3G-SDI/ST2110 over IP Uncompressed Extender %s</div></DIV>" , 
		EXT_IS_TX(&extRun)?"TX":"RX", EXT_IS_TX(&extRun)?"TX":"RX"   );
	
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV id=\"nav\"><a id=\"nav_media\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">Media</a>", 
		EXT_WEBPAGE_MEDIA);

	CMN_SN_PRINTF(dataBuf, size, index, "<a data-text=\"Info\" id=\"nav_info\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">System Info</a>", 
		EXT_WEBPAGE_INFO);
	
	CMN_SN_PRINTF(dataBuf, size, index, "<a id=\"nav_upgrade_mcu\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">Upgrade MCU</a>", 
		EXT_WEBPAGE_UPDATE_MCU_HTML);

	CMN_SN_PRINTF(dataBuf, size, index, "<a id=\"nav_upgrade_fpga\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">Upgrade FPGA</a>",
		EXT_WEBPAGE_UPDATE_FPGA_HTML);

	CMN_SN_PRINTF(dataBuf, size, index, "<a id=\"nav_upgrade_fpga\" class=\"\" href=\"JavaScript:reboot_device()\">Reboot</a></DIV>");

	CMN_SN_PRINTF(dataBuf, size, index, "<DIV id=\"message\"></DIV><DIV id=\"content\"></DIV><DIV id=\"footer\">&copy; MuxLab Inc. %s</DIV></DIV>", EXT_OS_NAME );


	CMN_SN_PRINTF(dataBuf, size, index,  
		"<SCRIPT type=\"text/javascript\">"EXT_NEW_LINE);

#if 0
	CMN_SN_PRINTF(dataBuf, size, index,  
			"function submit_firmware(formId, chip){" EXT_NEW_LINE \

/*			
		       "\tvar data = new FormData(); data.append('user', 'person');"
		       "\tvar params = 'orem=ipsum&name=binny';"
*/		       
		       "var form = document.getElementById(formId); "EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, 
		       "\tvar formData = new FormData(form);"EXT_NEW_LINE \
/*		       "\txhr.setRequestHeader(\"Content-type\", \"text/plain;charset=UTF-8\");" EXT_NEW_LINE */
/*		       "\tif (formId == '"FORM_ID"') xhr.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");" EXT_NEW_LINE  */
		       "\txhr = new XMLHttpRequest();"EXT_NEW_LINE 
/*		       "\txhr.setRequestHeader(\"Cache-Control\", \"no-cache\");"EXT_NEW_LINE*/
/*			 "alert('Submit');" EXT_NEW_LINE
			 "console.log('Submit' );" EXT_NEW_LINE*/
		       "\txhr.open('POST', chip);"EXT_NEW_LINE
			"\tif (formId == '"FORM_ID_CFG_SDP"' || formId =='"FORM_ID_CFG_DATA"' ){ xhr.send(urlencodeFormData(formData));}"EXT_NEW_LINE
			"\telse{ xhr.send(formData);}"EXT_NEW_LINE); 
/*		       "\txhr.send(urlencodeFormData(formData));"EXT_NEW_LINE);*/
	CMN_SN_PRINTF(dataBuf, size, index, 
			"\tdocument.getElementById('content').innerHTML = '<img src=\"/loading.gif\" />';"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, 
			"\txhr.onload = function(){" EXT_NEW_LINE
			"\t\tif (xhr.status !== 200) {alert('Request failed.  Returned status of ' + xhr.status);} "EXT_NEW_LINE
			"\t\telse{document.getElementById('content').innerHTML=xhr.responseText;}" EXT_NEW_LINE
			"\t}"EXT_NEW_LINE"}" EXT_NEW_LINE EXT_NEW_LINE);
#else

	CMN_SN_PRINTF(dataBuf, size, index,  
			"function submit_firmware(form, url){" EXT_NEW_LINE \
		     "\t if (form == 'formFirmware')	{"EXT_NEW_LINE 
		           "\t\tvar fileInput = document.querySelector('input[type=\"file\"]');"EXT_NEW_LINE
		           "\t\tvar data = new FormData();"EXT_NEW_LINE
		           "\t\tdata.append('file', fileInput.files[0]);"EXT_NEW_LINE"\t} "EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, 
		       "\telse{"EXT_NEW_LINE
		       	"\t\tvar data = new URLSearchParams();"EXT_NEW_LINE
		       	"\t\tfor (const pair of new FormData(document.getElementById(form))){"EXT_NEW_LINE
		       		"\t\t\tdata.append(pair[0], pair[1]);"EXT_NEW_LINE
		       	 "\t\t}"EXT_NEW_LINE
		       "\t}"EXT_NEW_LINE EXT_NEW_LINE); 
	
	CMN_SN_PRINTF(dataBuf, size, index, 
			"\tfetch(url,{\tmode: 'no-cors', \tmethod: 'POST',\tbody: data,	})"EXT_NEW_LINE);

	CMN_SN_PRINTF(dataBuf, size, index, 
			"\t.then(function(response){	\t return response.text();})"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, 
			"\t.then(function(text){\t document.getElementById('content').innerHTML = text;})"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, 
			"\t.catch(function(err){\t document.getElementById('content').innerHTML = err;});"EXT_NEW_LINE);

	CMN_SN_PRINTF(dataBuf, size, index, 
			"}"EXT_NEW_LINE EXT_NEW_LINE);
#endif

	
	CMN_SN_PRINTF(dataBuf, size, index, 
			"function reboot_device(){"EXT_NEW_LINE
			"\tvar response = confirm(\"Do you want to reboot?\"); " EXT_NEW_LINE
			"\tif (response){ "EXT_NEW_LINE
			"\t\tload_http_doc('%s', 'content',''); "EXT_NEW_LINE
			"\t\tsetTimeout(function(){window.location.reload(1);}, 3000); "EXT_NEW_LINE
			"\t}"EXT_NEW_LINE"}" EXT_NEW_LINE EXT_NEW_LINE, EXT_WEBPAGE_REBOOT);

#if 0
function (fd)
        {
            var s = '';
            function encode(s){ return encodeURIComponent(s).replace(/%20/g,'+'); }
            for(var pair of fd.entries()){
                if(typeof pair[1]=='string'){
                    s += (s?'&':'') + encode(pair[0])+'='+encode(pair[1]);
                }
            }
            return s;
        }	
#endif

#if 0
	CMN_SN_PRINTF(dataBuf, size, index, 
			"function urlencodeFormData(fd){"EXT_NEW_LINE
			"\tvar s = '';" EXT_NEW_LINE
			"\t function encode(s){ return encodeURIComponent(s).replace(/%20/g,'+'); }"EXT_NEW_LINE
			"\t for(var pair of fd.entries()){"EXT_NEW_LINE
			"\t\t if(typeof pair[1]=='string'){"EXT_NEW_LINE
			"\t\t\t s += (s?'\r\n':'') + encode(pair[0])+'='+encode(pair[1]);"EXT_NEW_LINE
			"\t}"EXT_NEW_LINE"}" EXT_NEW_LINE " return s;"EXT_NEW_LINE"}"EXT_NEW_LINE EXT_NEW_LINE);

#endif


#if 0
#if 0
	CMN_SN_PRINTF(dataBuf, size, index, 
			"function submit_setting(chip){"EXT_NEW_LINE
			"\tvar form = document.getElementById('"FORM_ID"');");
	CMN_SN_PRINTF(dataBuf, size, index, 
			"\tvar formData = new FormData(form);"EXT_NEW_LINE
			"\txhr = new XMLHttpRequest();"EXT_NEW_LINE
			"\txhr.open('POST', chip);"EXT_NEW_LINE
			"\txhr.send(formData);"EXT_NEW_LINE);
#else
	CMN_SN_PRINTF(dataBuf, size, index, 
			"function submit_setting(chip){"EXT_NEW_LINE
			"\talert('Submit POST');"EXT_NEW_LINE
			"\tdocument."FORM_ID".submit();");
#endif
	CMN_SN_PRINTF(dataBuf, size, index, 
			"\tdocument.getElementById('content').innerHTML= '<img src=\"/loading.gif\" />';"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, 
			"\txhr.onload = function(){"EXT_NEW_LINE 
			"\t\tif (xhr.status !== 200){alert('Request failed.  Returned status of ' + xhr.status);} "EXT_NEW_LINE
			"\t\telse{document.getElementById('content').innerHTML=xhr.responseText;}"EXT_NEW_LINE
			"\t}" EXT_NEW_LINE 
			"}" EXT_NEW_LINE EXT_NEW_LINE	);
#endif

	CMN_SN_PRINTF(dataBuf, size, index, "</SCRIPT></BODY></HTML>");

	printf("Data:'%s'\r\n", dataBuf);
	
	ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	return index;
}


/* root access point of NODE API */
static const MuxHttpHandle	_webpages[] =
{
	/* HTML URIs */
	{
		uri 		: 	EXT_WEBPAGE_ROOT,
		method	: 	HTTP_METHOD_GET,
		handler	:	extHttpWebPageRootHander,
		respType: 	WEB_RESP_HTML
	},
	
	{
		uri 		: 	EXT_WEBPAGE_ROOT EXT_WEBPAGE_INFO,
		method	: 	HTTP_METHOD_GET,
		handler	:	_extHttpWebPageInfoHander,
		respType: 	WEB_RESP_HTML
	},
	
	{
		uri 		: 	EXT_WEBPAGE_ROOT EXT_WEBPAGE_MEDIA,
		method	: 	HTTP_METHOD_GET,
		handler	:	_extHttpWebPageMediaHander,
		respType: 	WEB_RESP_HTML
	},

	{
		uri 		: 	EXT_WEBPAGE_ROOT EXT_WEBPAGE_REBOOT,
		method	: 	HTTP_METHOD_GET,
		handler	:	_extHttpWebPageReboot,
		respType: 	WEB_RESP_HTML
	},

	/* HTML POST */
	{
		uri 		: 	EXT_WEBPAGE_ROOT EXT_WEBPAGE_SETTING,
		method	: 	HTTP_METHOD_POST,
		handler	:	_extHttpWebSettingHander,
		respType: 	WEB_RESP_HTML
	},

	{
		uri 		: 	EXT_WEBPAGE_ROOT EXT_WEBPAGE_SDP_CLIENT,
		method	: 	HTTP_METHOD_POST,
		handler	:	_extHttpWebSdpClientHander,
		respType: 	WEB_RESP_HTML
	},
	
	/* SDP URIs */
	{
		uri 		: 	EXT_WEBPAGE_ROOT EXT_WEBPAGE_SDP_VIDEO,
		method	: 	HTTP_METHOD_GET,
		handler	:	extHttpSdpVideo,
		respType: 	WEB_RESP_SDP
	},

	{
		uri 		: 	EXT_WEBPAGE_ROOT EXT_WEBPAGE_SDP_AUDIO,
		method	: 	HTTP_METHOD_GET,
		handler	:	extHttpSdpAudio,
		respType: 	WEB_RESP_SDP
	},

	/* JSON URIs */
	{
		uri 		: 	EXT_WEBPAGE_ROOT EXT_WEBPAGE_API_SERVICE,
		method	: 	HTTP_METHOD_GET,
		handler	:	extHttpSimpleRestApi,
		respType: 	WEB_RESP_JSON
	},


	{
		uri 		: 	NULL,
		method	: 	HTTP_METHOD_UNKNOWN,
		handler :		NULL,
		respType: 	WEB_RESP_UNKNOWN
	}
};


uint16_t _httpWebPageResult(ExtHttpConn  *ehc, char *title, char *msg)
{
	int index = 0;
	int contentLength = 0;

	contentLength += snprintf((char *)ehc->data+index, sizeof(ehc->data)-index, 
		"<DIV class=\"title\"><H2>%s</H2></DIV>"EXT_NEW_LINE"<DIV class=\"fields-info\">"EXT_NEW_LINE"\t<DIV class=\"field\"><LABEL >Result:%s</LABEL></DIV>"EXT_NEW_LINE"</DIV>"EXT_NEW_LINE EXT_NEW_LINE,
		title, msg);

	ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	return contentLength;
}


char extHttpWebService(ExtHttpConn *ehc, void *data)
{
	const MuxHttpHandle	*page = _webpages;
//	char		ret;

	ehc->reqType = EXT_HTTP_REQ_T_CGI;
	
	while(page->uri )
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("API:'%s' :: REQ:'%s'", page->uri, ehc->uri));
		if( IS_STRING_EQUAL(page->uri, ehc->uri))
		{

			int headerLength = 0;
			int contentLength = 0;
			char  strLength[16];

			if(page->method !=  HTTP_METHOD_POST)
			{/* POST will keep data */
				headerLength = cmnHttpPrintResponseHeader(ehc, page->respType);
				ehc->headerLength = headerLength;
			}
			contentLength = page->handler(ehc, (void *)page);

			if(page->method ==  HTTP_METHOD_POST)
			{/* POST will keep data */
				headerLength = cmnHttpPrintResponseHeader(ehc, page->respType);
				ehc->headerLength = headerLength;
			}

			if(ehc->httpStatusCode != WEB_RES_REQUEST_OK )
			{
				contentLength = _httpWebPageResult(ehc, "ERROR"/*ehc->filename*/, ehc->boundary );
			}

			/* update content-length header */
			snprintf(strLength, sizeof(strLength), "%d", contentLength);
			strncpy((char *)ehc->data+headerLength-8, strLength, strlen(strLength) );

			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("response content length:'%s'", strLength));
			ehc->contentLength = (unsigned short)(headerLength + contentLength);
			ehc->dataSendIndex = 0;
			
			ehc->httpStatusCode = WEB_RES_REQUEST_OK;

#if 0			
			if(ret == EXIT_FAILURE)
			{
				extHttpWebPageResult(ehc, "Data Error", "Some data is invalidate, correct and try again!");
			}
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("return value of web page '%s' handler:%d", page->uri, ret) );
#endif

			if(EXT_DEBUG_IS_ENABLE(EXT_DEBUG_FLAG_CMD))
			{
//				printf("output RES12 %p, %d bytes: '%s'"LWIP_NEW_LINE, (void *)parser, parser->outIndex, parser->outBuffer);
			}

			return EXIT_SUCCESS;
		}

		page++;
	}

	return EXIT_FAILURE;
}


char extHttpWebPageResult(ExtHttpConn  *ehc, char *title, char *msg)
{
	int index = 0;
	int headerLength = 0;
	int contentLength = 0;

	memset(ehc->data, 0, sizeof(ehc->data));
	index += cmnHttpPrintResponseHeader(ehc, WEB_RESP_HTML);
	headerLength = index;

	contentLength += snprintf((char *)ehc->data+index, sizeof(ehc->data)-index, 
		"<DIV class=\"title\"><H2>%s</H2></DIV>"EXT_NEW_LINE"<DIV class=\"fields-info\">"EXT_NEW_LINE"\t<DIV class=\"field\"><LABEL >Result:%s</LABEL></DIV>"EXT_NEW_LINE"</DIV>"EXT_NEW_LINE EXT_NEW_LINE,
		title, msg);

	index += snprintf((char *)ehc->data+headerLength-8, 5, "%d", contentLength);

	ehc->contentLength = (unsigned short)(index+contentLength);
	ehc->dataSendIndex = 0;
	
	ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}


