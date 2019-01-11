/*
* Web pages service in CGI
*/


#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"

#define	_JAVA_SCRIPT_OLD		0



static uint16_t _httpWebPageResult(ExtHttpConn  *ehc, const char *title, char *msg)
{
	int index = 0;
	int contentLength = 0;

	contentLength += snprintf((char *)ehc->data+index, sizeof(ehc->data)-index, 
		"<DIV class=\"title\"><H2>%s</H2></DIV>"EXT_NEW_LINE"<DIV class=\"fields-info\">"EXT_NEW_LINE"\t<DIV class=\"field\">Result:%s</DIV>"EXT_NEW_LINE"</DIV>"EXT_NEW_LINE EXT_NEW_LINE,
		title, msg);

	ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	return contentLength;
}


static uint16_t _extHttpWebPageReboot(ExtHttpConn  *ehc, void *data)
{
	int index = 0;
	char *dataBuf = (char *)ehc->data+ehc->responseHeaderLength;
	uint16_t size = sizeof(ehc->data) - ehc->responseHeaderLength;

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

#define	FORM_ID_CFG_NETWORK		"formNet"
#define	FORM_ID_CFG_RS232			"formRs232"


//#define	FORM_ID		"formFirmware"
static uint16_t _extHttpWebPageMediaHander(ExtHttpConn  *ehc, void *pageHandle)
{
	int index = 0;
	EXT_RUNTIME_CFG	*runCfg = ehc->runCfg;
	struct netif *_netif = (struct netif *)runCfg->netif;
	const EXT_CONST_STR *_str;
	const EXT_CONST_INT *_int;

	const MediaParam *_mParam = NULL;

	const char *chVal;
	const short *shVal;
//	unsigned char _regValue;

	char *dataBuf = (char *)ehc->data+ehc->responseHeaderLength;
	uint16_t size = sizeof(ehc->data) - ehc->responseHeaderLength;

#ifdef	ARM
	extFpgaReadParams(runCfg);
#endif
	/* device */
//	CMN_SN_PRINTF(dataBuf, size, index, "<DIV id=\"forms\"><FORM method=\"post\" id=\""FORM_ID"\" name=\""FORM_ID"\"  enctype=\"text/plain\" action=\"/setting\">" );

//	<FORM method="post" id="formSettings" enctype=" action="action_page.php">    
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"title\"><H2>Video/Audio Settings</H2></DIV>"EXT_NEW_LINE);

	if(!EXT_IS_TX(runCfg) )
	{
		/* input type */
		CMN_SN_PRINTF(dataBuf, size, index, EXT_NEW_LINE "<DIV class=\"fields\">"EXT_NEW_LINE);
		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\">"EXT_NEW_LINE"\t\t<DIV class=\"field\"><LABEL >Input Type:</LABEL>"EXT_NEW_LINE);

		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<select id=\"inputtype\" onchange=\"javascript_:showdiv(this.options[this.selectedIndex].value)\">"EXT_NEW_LINE);
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t\t<option value=\"SDP\">SDP</option>"EXT_NEW_LINE);
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t\t<option value=\"Data\" selected>Manual</option>"EXT_NEW_LINE);
		CMN_SN_PRINTF(dataBuf, size, index, "</select>"EXT_NEW_LINE "</DIV></DIV></DIV>"EXT_NEW_LINE);
		CMN_SN_PRINTF(dataBuf, size, index, "<br />"EXT_NEW_LINE "<div id=\"divSettingsSdp\" style=\"display:none;\">"EXT_NEW_LINE);
	}

	/* SDP */
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"fields\">"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >SDP Video:</LABEL>");


	if(EXT_IS_TX(runCfg) )
	{
		CMN_SN_PRINTF(dataBuf, size, index, "http://%s/%s</DIV>", inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)), EXT_WEBPAGE_SDP_VIDEO);
	}
	else
	{
		CMN_SN_PRINTF(dataBuf, size, index, "<DIV id=\"forms\"><FORM method=\"post\" id=\""FORM_ID_CFG_SDP"\" >" EXT_NEW_LINE );
		CMN_SN_PRINTF(dataBuf, size, index, "http://<INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_SDP_VEDIO_IP"\" value=\"%s\" style=\"width: 115px;\"/>:" EXT_NEW_LINE
			"<INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_SDP_VEDIO_PORT"\" value=\"%d\" style=\"width: 30px;\"/>"EXT_NEW_LINE
			"<INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_SDP_VEDIO_URI"\" value=\"%s\"/>" EXT_NEW_LINE,
			inet_ntoa(*(struct in_addr *)&(runCfg->sdpUriVideo.ip)), runCfg->sdpUriVideo.port, runCfg->sdpUriVideo.uri );
	}
	
	CMN_SN_PRINTF(dataBuf, size, index, EXT_NEW_LINE"\t<DIV class=\"field\"><LABEL >SDP Audio:</LABEL>");
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
#if _JAVA_SCRIPT_OLD
		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"buttons\">"EXT_NEW_LINE"\t<INPUT type=\"submit\" value=\"Apply\" class=\"btnSubmit\" onClick=\"submit_firmware('"FORM_ID_CFG_SDP"','"EXT_WEBPAGE_SDP_CLIENT"')\"/>"EXT_NEW_LINE
#else
		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"buttons\">"EXT_NEW_LINE"\t<INPUT type=\"button\" value=\"Apply\" class=\"btnSubmit\" onClick=\"submit_firmware('"FORM_ID_CFG_SDP"','"EXT_WEBPAGE_SDP_CLIENT"')\"/>"EXT_NEW_LINE
#endif
				"\t<INPUT name=\"ResetButton\" type=\"reset\" class=\"btnReset\" value=\"Cancel\" id=\"ResetButton\"/></DIV>"EXT_NEW_LINE );

		CMN_SN_PRINTF(dataBuf, size, index, "\t</DIV>"EXT_NEW_LINE"</DIV>"EXT_NEW_LINE"</FORM> </div></div>"EXT_NEW_LINE );
	}
	CMN_SN_PRINTF(dataBuf, size, index, "\t</DIV>"EXT_NEW_LINE ); /* DIV of fields */



	/* DATA */
	CMN_SN_PRINTF(dataBuf, size, index, "<br />"EXT_NEW_LINE "<div id=\"divSettingsData\"><DIV class=\"fields\">"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, EXT_NEW_LINE EXT_NEW_LINE"<DIV id=\"forms\"><FORM method=\"post\" id=\""FORM_ID_CFG_DATA"\" >" );

	CMN_SN_PRINTF(dataBuf, size, index, ""EXT_NEW_LINE"\t<DIV class=\"field\"><LABEL >Name:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_PRODUCT"\" value=\"%s\"/></DIV>"EXT_NEW_LINE, 
		runCfg->name);//

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Video Address:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_IP_VEDIO"\" value=\"%s\"/></DIV>"EXT_NEW_LINE, 
		EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.ip)) );

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Audio Address:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_IP_AUDIO"\" value=\"%s\"/></DIV>"EXT_NEW_LINE, 
		EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.audioIp)) );

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >ANC Address:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_IP_ANC"\" value=\"%s\"/></DIV>"EXT_NEW_LINE, 
		EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.ancIp)) );

#if EXT_FPGA_AUX_ON	
	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >AUX Address:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_IP_AUX"\" value=\"%s\"/></DIV>"EXT_NEW_LINE, 
		EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.auxIp)) );
#endif

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Video Port:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_PORT_VEDIO"\" value=\"%d\"/></DIV>"EXT_NEW_LINE, 
		runCfg->dest.vport );
	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Audio Port:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_PORT_AUDIO"\" value=\"%d\"/></DIV>"EXT_NEW_LINE, 
		runCfg->dest.aport );

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >ANC Port:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_PORT_DATA"\" value=\"%d\"/></DIV>"EXT_NEW_LINE, 
		runCfg->dest.dport );
#if EXT_FPGA_AUX_ON	
	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >AUX Port:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_PORT_STREM"\" value=\"%d\"/></DIV>"EXT_NEW_LINE, 
		runCfg->dest.sport );
#endif

	/* auto configuration*/
	if(EXT_IS_TX(runCfg))
	{
		/* media parameters */
#if 0		
		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Resolution:</LABEL>"EXT_NEW_LINE);

		CMN_SN_PRINTF(dataBuf, size, index, "\t\t%dx%d</DIV>"EXT_NEW_LINE, runCfg->runtime.vWidth, runCfg->runtime.vHeight);

		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Interlaced:</LABEL>"EXT_NEW_LINE);
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t%s</DIV>" EXT_NEW_LINE, CMN_FIND_V_FORMAT(runCfg->runtime.vIsInterlaced) );
		
		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Frame Rate:</LABEL>"EXT_NEW_LINE"\t\t%d</DIV>" EXT_NEW_LINE, 
			CMN_INT_FIND_NAME_V_FPS(runCfg->runtime.vFrameRate));
#else
		_mParam = constMediaParams;
		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Media:</LABEL>"EXT_NEW_LINE"\t\t%s</DIV>" EXT_NEW_LINE, 
			_mParam->desc);
#endif
		/* color depth */
		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Color Depth:</LABEL>"EXT_NEW_LINE"\t\t%d</DIV>"EXT_NEW_LINE,
			CMN_INT_FIND_NAME_V_DEPTH(runCfg->runtime.vDepth) );

		/* color space*/
		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Color Space:</LABEL>"EXT_NEW_LINE);
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t%s</DIV>"EXT_NEW_LINE, CMN_FIND_V_COLORSPACE( runCfg->runtime.vColorSpace));


		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Audio Channels:</LABEL>"EXT_NEW_LINE"\t\t%d</DIV>"EXT_NEW_LINE, 
			runCfg->runtime.aChannels);

		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Audio Rate:</LABEL>"EXT_NEW_LINE"\t\t%s</DIV>"EXT_NEW_LINE,
			CMN_FIND_A_RATE(runCfg->runtime.aSampleRate));

		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Audio PKT Size:</LABEL>"EXT_NEW_LINE"\t\t%s</DIV>"EXT_NEW_LINE,
			CMN_FIND_A_PKTSIZE(runCfg->runtime.aPktSize));

//		CMN_SN_PRINTF(dataBuf, size, index, "\t\t</DIV>"EXT_NEW_LINE);/* for auto configuration */
	}	
	else
	{
		CMN_SN_PRINTF(dataBuf, size, index, "\t<br /><div class=\"field\">"EXT_NEW_LINE);
		CMN_SN_PRINTF(dataBuf, size, index, "\t<div class=\"field\"><label>Media Settings:</label>"EXT_NEW_LINE);

		CMN_SN_PRINTF(dataBuf, size, index, "\t\t<SELECT id=\""EXT_WEB_CFG_FIELD_FPGA_AUTO"\"  onchange=\"javascript_:showdiv(this.options[this.selectedIndex].value)\">"EXT_NEW_LINE );
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\""EXT_WEB_CFG_FIELD_FPGA_AUTO_V_AUTO"\" %s>ANC Data</OPTION>"EXT_NEW_LINE, 
			(runCfg->fpgaAuto)?"selected":"" );
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\""EXT_WEB_CFG_FIELD_FPGA_AUTO_V_MANUAL"\" %s>"EXT_WEB_CFG_FIELD_FPGA_AUTO_V_MANUAL"</OPTION>"EXT_NEW_LINE, 
			(runCfg->fpgaAuto)?"":"selected" );
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT>"EXT_NEW_LINE);
		
		CMN_SN_PRINTF(dataBuf, size, index, "\t</div></div>"EXT_NEW_LINE);
		CMN_SN_PRINTF(dataBuf, size, index, "\t<div id=\"divVideoSettings\" style=\"display:%s;\">"EXT_NEW_LINE, (runCfg->fpgaAuto)?"none":"inline-block");


		/* media parameters */
#if 0
		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Resolution:</LABEL>"EXT_NEW_LINE);

		CMN_SN_PRINTF(dataBuf, size, index, "\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_VIDEO_WIDTH"\">"EXT_NEW_LINE );
		shVal = videoWidthList;
		while(*shVal != 0)
		{
			CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, *shVal, (*shVal== runCfg->runtime.vWidth)?"selected":"", *shVal);
			shVal++;
		}
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT>x"EXT_NEW_LINE);

		CMN_SN_PRINTF(dataBuf, size, index, "\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_VIDEO_HEIGHT"\">" EXT_NEW_LINE);
		shVal = videoHeightList;
		while(*shVal != 0)
		{
			CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, *shVal, (*shVal== runCfg->runtime.vHeight)?"selected":"", *shVal);
			shVal++;
		}
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);

		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Interlaced:</LABEL>"EXT_NEW_LINE);
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_VIDEO_INTERLACE"\">" EXT_NEW_LINE);
		_str = _videoFormats;
		while(_str->type!= EXT_INVALIDATE_STRING_TYPE)
		{
			CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%s</OPTION>"EXT_NEW_LINE, _str->type, (_str->type== runCfg->runtime.vIsInterlaced)?"selected":"", _str->name);
			_str++;
		}
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);

#if 0
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>Interlaced</OPTION>"EXT_NEW_LINE, 
			EXT_VIDEO_INTLC_INTERLACED, (EXT_VIDEO_INTLC_INTERLACED== runCfg->runtime.vIsInterlaced)?"selected":"");
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>Progressive</OPTION>"EXT_NEW_LINE, 
			EXT_VIDEO_INTLC_PROGRESSIVE, (EXT_VIDEO_INTLC_PROGRESSIVE== runCfg->runtime.vIsInterlaced)?"selected":"");
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);
#endif
		
		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Frame Rate:</LABEL>"EXT_NEW_LINE"\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_FRAME_RATE"\">" EXT_NEW_LINE);
		_int =  intVideoFpsList;
		while(_int->type != 0xFF )
		{
			CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, _int->type, (_int->type == runCfg->runtime.vFrameRate)?"selected":"", _int->name);
			_int++;
		}
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);
#else
		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Media:</LABEL>"EXT_NEW_LINE);

		CMN_SN_PRINTF(dataBuf, size, index, "\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_VIDEO_WIDTH"\">"EXT_NEW_LINE );
		_mParam = constMediaParams;
		while(_mParam->desc != NULL)
		{
			CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%s</OPTION>"EXT_NEW_LINE, _mParam->index, (1== runCfg->runtime.vWidth)?"selected":"", _mParam->desc );
			_mParam++;
		}
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT>"EXT_NEW_LINE);
#endif

		/* color depth */
		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Color Depth:</LABEL>"EXT_NEW_LINE"\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_COLOR_DEPTH"\">"EXT_NEW_LINE);
		_int =  intVideoColorDepthList;
		while(_int->type!= 0xFF)
		{
			CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, _int->type, (_int->type== runCfg->runtime.vDepth)?"selected":"", _int->name);
			_int++;
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

		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Audio Rate:</LABEL>"EXT_NEW_LINE"\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_AUDIO_SAMP_RATE"\">"EXT_NEW_LINE );
		_str = _audioRates;
		while(_str->type!= EXT_INVALIDATE_STRING_TYPE)
		{
			CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%s</OPTION>"EXT_NEW_LINE, _str->type, ((unsigned char)_str->type== runCfg->runtime.aSampleRate)?"selected":"", _str->name);
			_str++;
		}
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);

		CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Audio PKT Size:</LABEL>"EXT_NEW_LINE"\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_AUDIO_PKT_SIZE"\">"EXT_NEW_LINE );
		_str = _audioPktSizes;
		while(_str->type!= EXT_INVALIDATE_STRING_TYPE)
		{
			CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%s</OPTION>"EXT_NEW_LINE, _str->type, (_str->type== runCfg->runtime.aPktSize)?"selected":"", _str->name);
			_str++;
		}
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);

		CMN_SN_PRINTF(dataBuf, size, index, "\t\t</DIV>"EXT_NEW_LINE);/* for auto configuration */
		}


//<INPUT type="button" value="Submit" class="btnSubmit" id="btnSubmit" />
#if _JAVA_SCRIPT_OLD
	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"buttons\">"EXT_NEW_LINE"\t<INPUT type=\"submit\" value=\"Submit\" class=\"btnSubmit\" onClick=\"submit_firmware('"FORM_ID_CFG_DATA"','"EXT_WEBPAGE_SETTING"')\"/>"EXT_NEW_LINE
#else
	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"buttons\">"EXT_NEW_LINE"\t<INPUT type=\"button\" value=\"Submit\" class=\"btnSubmit\" onClick=\"submit_firmware('"FORM_ID_CFG_DATA"','"EXT_WEBPAGE_SETTING"')\"/>"EXT_NEW_LINE
//	CMN_SN_PRINTF(data, size, index,  "\t<DIV class=\"buttons\">"EXT_NEW_LINE"\t<INPUT type=\"submit\" value=\"Submit\" class=\"btnSubmit\" id=\"btnSubmit\" />"EXT_NEW_LINE
#endif
			"\t<INPUT name=\"ResetButton\" type=\"reset\" class=\"btnReset\" value=\"Cancel\" id=\"ResetButton\"/></DIV>"EXT_NEW_LINE );

	CMN_SN_PRINTF(dataBuf, size, index, "</FORM></DIV></DIV>"EXT_NEW_LINE );

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

#if EXT_HTTPD_DEBUG
	printf("Data:'%s'\r\n", dataBuf);
#endif
	
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
	char *dataBuf = (char *)ehc->data+ehc->responseHeaderLength;
	uint16_t size = sizeof(ehc->data) - ehc->responseHeaderLength;
	struct netif *_netif = (struct netif *)runCfg->netif;
	
	/* device */
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"title\"><H2>Device</H2></DIV><DIV class=\"fields-info\"><DIV class=\"field\"><LABEL >Product Name:</LABEL><DIV class=\"label\">%s</DIV></DIV>",
		EXT_767_PRODUCT_NAME);
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >Model:</LABEL><DIV class=\"label\">%s-%s</DIV></DIV>"EXT_NEW_LINE, EXT_767_MODEL, EXT_IS_TX(runCfg)?"TX":"RX" );
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >Custom Name:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", runCfg->name);
#ifdef	ARM
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >Firmware Version:</LABEL><DIV class=\"label\">%s(Build %s)</DIV></DIV>",  sysVersion(), sysBuildTime() );
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >FPGA Version:</LABEL><DIV class=\"label\">%s</DIV></DIV>",  extFgpaReadVersion() );
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >Security Check:</LABEL><DIV class=\"label\">%s</DIV></DIV>",  (bspScCheckMAC(runCfg->sc)==EXIT_FAILURE)?"FAILED":"PASS" );
#else
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >Firmware Version:</LABEL><DIV class=\"label\">%02d.%02d.%02d</DIV></DIV>",
		runCfg->version.major, runCfg->version.minor, runCfg->version.revision);
#endif
	CMN_SN_PRINTF(dataBuf, size, index, "</DIV>");

	/* settings */
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"title\"><H2>Settings</H2></DIV><DIV class=\"fields-info\"><DIV class=\"field\"><LABEL >MAC Address:</LABEL><DIV class=\"label\" >");
	MAC_ADDRESS_PRINT(dataBuf, size, index, &(runCfg->local.mac));
	CMN_SN_PRINTF(dataBuf, size, index, "</DIV></DIV>");

	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >IP Address:</LABEL><DIV class=\"label\" >%s</DIV></DIV>",
		 inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)) );
//	EXT_LWIP_IPADD_TO_STR(&(runCfg->local.ip)) );

	CMN_SN_PRINTF(dataBuf, size, index, " <DIV class=\"field\"><LABEL >Subnet Mask:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", EXT_LWIP_IPADD_TO_STR(&(runCfg->ipMask)) );

	CMN_SN_PRINTF(dataBuf, size, index, " <DIV class=\"field\"><LABEL >Gateway:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", EXT_LWIP_IPADD_TO_STR(&(runCfg->ipGateway)) );
	CMN_SN_PRINTF(dataBuf, size, index, " <DIV class=\"field\"><LABEL >DHCP:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", STR_BOOL_VALUE(runCfg->netMode) );
#if EXT_DIP_SWITCH_ON
	CMN_SN_PRINTF(dataBuf, size, index, " <DIV class=\"field\"><LABEL >Dipswitch:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", STR_BOOL_VALUE(runCfg->isDipOn) );
#endif
	CMN_SN_PRINTF(dataBuf, size, index, "</DIV>");

	/* RS232 */
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"title\"><H2>RS232</H2></DIV><DIV class=\"fields-info\"><DIV class=\"field\"><LABEL >Baudrate:</LABEL><DIV class=\"label\">%"U32_F"</DIV></DIV>",
		runCfg->rs232Cfg.baudRate);

	CMN_SN_PRINTF(dataBuf, size, index,  "<DIV class=\"field\"><LABEL >Databits:</LABEL><DIV class=\"label\">%d</DIV></DIV>",
		runCfg->rs232Cfg.charLength);
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >Parity:</LABEL><DIV class=\"label\">%s</DIV></DIV>",
		CMN_FIND_RS_PARITY((unsigned short)runCfg->rs232Cfg.parityType) );
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"field\"><LABEL >Stopbits:</LABEL><DIV class=\"label\">%d</DIV></DIV>",
		runCfg->rs232Cfg.stopbits);

	CMN_SN_PRINTF(dataBuf, size, index, "</DIV>" );

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
//	int i = 0;
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

#if 0		
		if(EXT_DEBUG_HC_IS_ENABLE())
		{
			printf("\tNo#%d: '%s' = '%s'" EXT_NEW_LINE, ++i, key, value );
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

	ehc->runCfg->sdpUriVideo.next = NULL;
	ret = extHttpClientNewRequest(&ehc->runCfg->sdpUriVideo) ;
	ehc->runCfg->sdpUriAudio.next = NULL;
	ret2 = extHttpClientNewRequest(&ehc->runCfg->sdpUriAudio);

//	EXT_DEBUGF(EXT_DBG_OFF, ("Data:%"U32_F":%d'%.*s", ehc->contentLength, ehc->leftData, ehc->leftData, ehc->headers+ehc->headerLength+__HTTP_CRLF_SIZE));

	headerLength = cmnHttpPrintResponseHeader(ehc, page->respType);
	ehc->responseHeaderLength = headerLength;
	headerLength = 0;

	if(ret == ERR_OK && ret2 == ERR_ALREADY )
	{
		CMN_SN_PRINTF((char *)ehc->data+ehc->responseHeaderLength, sizeof(ehc->data)-ehc->responseHeaderLength, headerLength, 
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

	return _httpWebPageResult(ehc, "OK",(char *) "New configuration has been active now");
	
error:
	ehc->httpStatusCode = WEB_RES_ERROR;
	return 0;
}


//#define	FORM_ID		"formFirmware"
static uint16_t _extHttpWebPageSysCfgsHander(ExtHttpConn  *ehc, void *pageHandle)
{
	int index = 0;
	EXT_RUNTIME_CFG	*runCfg = ehc->runCfg;
	struct netif *_netif = (struct netif *)runCfg->netif;
	const EXT_CONST_STR *_str;
	const EXT_CONST_INT *_int;

	const char *chVal;
	const short *shVal;
//	unsigned char _regValue;

	char *dataBuf = (char *)ehc->data+ehc->responseHeaderLength;
	uint16_t size = sizeof(ehc->data) - ehc->responseHeaderLength;


	/* IP address */
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"title\"><H2>NETWORK</H2></DIV>"EXT_NEW_LINE);

	CMN_SN_PRINTF(dataBuf, size, index, EXT_NEW_LINE EXT_NEW_LINE"<DIV class=\"fields\"><DIV id=\"forms\"><FORM method=\"post\" id=\""FORM_ID_CFG_NETWORK"\" >" );

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >DHCP:</LABEL>"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_IS_DHCP"\">"EXT_NEW_LINE );
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"1\" %s>Enabled</OPTION>"EXT_NEW_LINE,  EXT_DHCP_IS_ENABLE(runCfg)?"selected":"");
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"0\" %s>Disabled</OPTION>"EXT_NEW_LINE,  EXT_DHCP_IS_ENABLE(runCfg)?"":"selected");
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT>"EXT_NEW_LINE);

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >IP Address:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_ADDRESS"\" value=\"%s\"/></DIV>"EXT_NEW_LINE, 
		inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)) );

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Netmask:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_NETMASK"\" value=\"%s\"/></DIV>"EXT_NEW_LINE, 
		EXT_LWIP_IPADD_TO_STR(&(runCfg->ipMask))  );

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Gateway:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_GATEWAY"\" value=\"%s\"/></DIV>"EXT_NEW_LINE, 
		EXT_LWIP_IPADD_TO_STR(&(runCfg->ipGateway))  );

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >MAC Address:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_MAC"\" value=\"%02x:%02x:%02x:%02x:%02x:%02x\"/></DIV>"EXT_NEW_LINE, 
		runCfg->local.mac.address[0], runCfg->local.mac.address[1], runCfg->local.mac.address[2], runCfg->local.mac.address[3], runCfg->local.mac.address[4], runCfg->local.mac.address[5] );

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"buttons\">"EXT_NEW_LINE"\t<INPUT type=\"button\" value=\"Submit\" class=\"btnSubmit\" onClick=\"submit_firmware('"FORM_ID_CFG_NETWORK"','"EXT_WEBPAGE_SYS_UPDATE"')\"/>"EXT_NEW_LINE
			"\t<INPUT name=\"ResetButton\" type=\"reset\" class=\"btnReset\" value=\"Cancel\" id=\"ResetButton\"/></DIV>"EXT_NEW_LINE );
	CMN_SN_PRINTF(dataBuf, size, index, "</FORM></DIV></DIV></DIV>"EXT_NEW_LINE );
	


	/* RS232 */
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"title\"><H2>RS232</H2></DIV>"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, EXT_NEW_LINE EXT_NEW_LINE"<DIV class=\"fields\"><DIV id=\"forms\"><FORM method=\"post\" id=\""FORM_ID_CFG_RS232"\" >" );

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Baudrate:</LABEL>"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_RS232_BAUDRATE"\">"EXT_NEW_LINE );
	shVal = constRs232Baudrates;
	while(*shVal != 0)
	{
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, *shVal, (*shVal== runCfg->rs232Cfg.baudRate)?"selected":"", *shVal);
		shVal++;
	}
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT>"EXT_NEW_LINE);


	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Databits:</LABEL>"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_RS232_DATABITS"\">"EXT_NEW_LINE );
	shVal = constRs232Databits;
	while(*shVal != 0)
	{
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, *shVal, (*shVal== runCfg->rs232Cfg.charLength)?"selected":"", *shVal);
		shVal++;
	}
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT>"EXT_NEW_LINE);

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Paritiy:</LABEL>"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_RS232_PARITY"\">" EXT_NEW_LINE);
	_str = _ipcmdStringRsParities;
	while(_str->type!= EXT_INVALIDATE_STRING_TYPE)
	{
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%s</OPTION>"EXT_NEW_LINE, _str->type, (_str->type== runCfg->rs232Cfg.parityType)?"selected":"", _str->name);
		_str++;
	}
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);

	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"field\"><LABEL >Stopbits:</LABEL>"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_RS232_STOPBITS"\">" EXT_NEW_LINE);
	_str = _ipcmdStringRsStopbits;
	while(_str->type!= EXT_INVALIDATE_STRING_TYPE)
	{
		CMN_SN_PRINTF(dataBuf, size, index, "\t\t\t<OPTION value=\"%d\" %s>%s</OPTION>"EXT_NEW_LINE, _str->type, (_str->type== runCfg->rs232Cfg.stopbits)?"selected":"", _str->name);
		_str++;
	}
	CMN_SN_PRINTF(dataBuf, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);


	CMN_SN_PRINTF(dataBuf, size, index, "\t<DIV class=\"buttons\">"EXT_NEW_LINE"\t<INPUT type=\"button\" value=\"Submit\" class=\"btnSubmit\" onClick=\"submit_firmware('"FORM_ID_CFG_NETWORK"','"EXT_WEBPAGE_SYS_UPDATE"')\"/>"EXT_NEW_LINE
			"\t<INPUT name=\"ResetButton\" type=\"reset\" class=\"btnReset\" value=\"Cancel\" id=\"ResetButton\"/></DIV>"EXT_NEW_LINE );
	CMN_SN_PRINTF(dataBuf, size, index, "</FORM></DIV></DIV></DIV></DIV>"EXT_NEW_LINE );
	

	/* Reset */
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"title\"><H2>Reset</H2></DIV>"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, EXT_NEW_LINE EXT_NEW_LINE"<DIV class=\"fields\"><DIV class=\"field\"><DIV style=\"margin-left: 5px;\">Restore the device to factory settings.</DIV></DIV>" );
	CMN_SN_PRINTF(dataBuf, size, index, EXT_NEW_LINE EXT_NEW_LINE"<DIV class=\"buttons\"><input type=\"button\" value=\"Reset\" class=\"btnSubmit\" onclick=\"reset_device()\"></DIV></DIV>" );


	/* Reboot */
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV class=\"title\"><H2>Reboot</H2></DIV>"EXT_NEW_LINE);
	CMN_SN_PRINTF(dataBuf, size, index, EXT_NEW_LINE EXT_NEW_LINE"<DIV class=\"fields\"><DIV class=\"field\"><DIV style=\"margin-left: 5px;\">Please wait one minute after rebooting.</DIV></DIV>" );
	CMN_SN_PRINTF(dataBuf, size, index, EXT_NEW_LINE EXT_NEW_LINE"<DIV class=\"buttons\"><input type=\"button\" value=\"Reboot\" class=\"btnSubmit\" onclick=\"reboot_device()\"></DIV></DIV>" );

#if EXT_HTTPD_DEBUG
	printf("Data:'%s'\r\n", dataBuf);
#endif
	
	ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	return index;
}

uint16_t extHttpWebPageRootHander(ExtHttpConn  *ehc, void *pageHandle)
{
	int index = 0;
	char *dataBuf = (char *)ehc->data+ehc->responseHeaderLength;
	uint16_t size = sizeof(ehc->data) - ehc->responseHeaderLength;

	
	CMN_SN_PRINTF(dataBuf, size, index, "<HTML><HEAD><TITLE>Muxlab %s-500767</TITLE>", EXT_IS_TX(&extRun)?"TX":"RX" );
	CMN_SN_PRINTF(dataBuf, size, index, "<LINK href=\"/styles.css\" type=\"text/css\" rel=\"stylesheet\"><SCRIPT type=\"text/javascript\" src=\"/load_html.js\"></SCRIPT></HEAD>"EXT_NEW_LINE );
#if 0
	CMN_SN_PRINTF(dataBuf, size, index, "<BODY onload=\"JavaScript:load_http_doc('%s', 'content','')\"><DIV id=\"body\"><DIV id=\"header\"><a id=\"logo\" href=\"/\"><img alt=\"Muxlab Control Panel\" src=\"/logo.jpg\"></a><br />",
		EXT_WEBPAGE_MEDIA);
#else
	CMN_SN_PRINTF(dataBuf, size, index, "<BODY onload=\"JavaScript:load_http_doc('%s', 'content','')\"><DIV id=\"body\"><DIV id=\"header\"><br />",
		EXT_WEBPAGE_MEDIA);
#endif

	CMN_SN_PRINTF(dataBuf, size, index, "<div data-text=\"dt_productName\" id=\"id_mainProductName\">500767-%s 3G-SDI/ST2110 over IP Uncompressed Gateway %s</div></DIV>" , 
		EXT_IS_TX(&extRun)?"TX":"RX", EXT_IS_TX(&extRun)?"TX":"RX"   );
	
	CMN_SN_PRINTF(dataBuf, size, index, "<DIV id=\"nav\"><a id=\"nav_media\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">Media</a>", 
		EXT_WEBPAGE_MEDIA);

	CMN_SN_PRINTF(dataBuf, size, index, "<a data-text=\"Info\" id=\"nav_info\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">System Info</a>", 
		EXT_WEBPAGE_INFO);
	
	CMN_SN_PRINTF(dataBuf, size, index, "<a id=\"nav_upgrade_mcu\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">Upgrade</a>", 
		EXT_WEBPAGE_UPDATE_HTML);

	CMN_SN_PRINTF(dataBuf, size, index, "<a id=\"nav_upgrade_fpga\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">Settings</a></DIV>",
		EXT_WEBPAGE_SYS_CFGS);

//	CMN_SN_PRINTF(dataBuf, size, index, "<a id=\"nav_upgrade_fpga\" class=\"\" href=\"JavaScript:reboot_device()\">Reboot</a></DIV>");

	CMN_SN_PRINTF(dataBuf, size, index, "<DIV id=\"message\"></DIV><DIV id=\"content\"></DIV><DIV id=\"footer\">&copy; MuxLab Inc. %s</DIV></DIV>", EXT_OS_NAME );

	CMN_SN_PRINTF(dataBuf, size, index,  
		"<iframe width=\"0\" height=\"0\" name=\"dummyframe\" id=\"dummyframe\"></iframe>"EXT_NEW_LINE);

#if 0
	CMN_SN_PRINTF(dataBuf, size, index,  
		"<SCRIPT type=\"text/javascript\">"EXT_NEW_LINE);

#if 0
#endif


	CMN_SN_PRINTF(dataBuf, size, index,  
		"\tfunction showdiv(aval){"EXT_NEW_LINE
		"\tif (aval == \"SDP\"){"EXT_NEW_LINE
		"\t\tdivSettingsSdp.style.display= 'inline-block';"EXT_NEW_LINE
		"\t\tdivSettingsData.style.display='none';}"EXT_NEW_LINE
		"\telse{"EXT_NEW_LINE
		"\t\tdivSettingsSdp.style.display='none';"EXT_NEW_LINE
		"\t\tdivSettingsData.style.display= 'inline-block';}}"EXT_NEW_LINE);


#if _JAVA_SCRIPT_OLD
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
#if 0
		           "\t\tdocument.getElementById(form).submit();"EXT_NEW_LINE
		           "\t\tdocument.getElementById('content').innerHTML = '<img src=\"loading.gif\" />';"EXT_NEW_LINE"\t} "EXT_NEW_LINE);
#else
		           "\t\tvar fileInput = document.querySelector('input[type=\"file\"]');"EXT_NEW_LINE
		           "\t\tvar data = new FormData();"EXT_NEW_LINE
		           "\t\tdata.append('file', fileInput.files[0]);"EXT_NEW_LINE"\t} "EXT_NEW_LINE);
#endif
	CMN_SN_PRINTF(dataBuf, size, index, 
		       "\telse{"EXT_NEW_LINE
		       	"\t\tvar data = new URLSearchParams();"EXT_NEW_LINE
		       	"\t\tfor (const pair of new FormData(document.getElementById(form))){"EXT_NEW_LINE
		       		"\t\t\tdata.append(pair[0], pair[1]);"EXT_NEW_LINE
		       	 "\t\t}"EXT_NEW_LINE
		       "\t}"EXT_NEW_LINE EXT_NEW_LINE); 

	CMN_SN_PRINTF(dataBuf, size, index, 
			"\tdocument.getElementById('content').innerHTML = '<img src=\"loading.gif\" />';"EXT_NEW_LINE);
	
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

#if _JAVA_SCRIPT_OLD
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
#endif

	CMN_SN_PRINTF(dataBuf, size, index, "</BODY></HTML>");

#if EXT_HTTPD_DEBUG
//	printf("Data:'%s'\r\n", dataBuf);
#endif

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
		uri 		: 	EXT_WEBPAGE_ROOT EXT_WEBPAGE_SYS_CFGS,
		method	: 	HTTP_METHOD_GET,
		handler	:	_extHttpWebPageSysCfgsHander,
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


char extHttpWebService(ExtHttpConn *ehc, void *data)
{
	const MuxHttpHandle	*page = _webpages;
//	char		ret;

	ehc->reqType = EXT_HTTP_REQ_T_CGI;
	
	while(page->uri )
	{
		EXT_DEBUGF(EXT_DBG_OFF, ("API:'%s' :: REQ:'%s'", page->uri, ehc->uri));
		if( IS_STRING_EQUAL(page->uri, ehc->uri) || IS_STRING_EQUAL(page->uri+1, ehc->uri)) /* for '/vdeo.sdp' or 'video.sdp' */
		{

			int headerLength = 0;
			int contentLength = 0;
			char  strLength[16];

//			if(page->method !=  HTTP_METHOD_POST)
			{/* POST will keep data */
				headerLength = cmnHttpPrintResponseHeader(ehc, page->respType);
				ehc->responseHeaderLength = headerLength;
			}
			contentLength = page->handler(ehc, (void *)page);

#if 0
			if(page->method ==  HTTP_METHOD_POST)
			{/* POST will keep data, so POST will output header in its handler  */
				headerLength = cmnHttpPrintResponseHeader(ehc, page->respType);
				ehc->headerLength = headerLength;
			}
#endif

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

///	contentLength = _httpWebPageResult(ehc, title, msg);
	contentLength += snprintf((char *)ehc->data+index, sizeof(ehc->data)-index, 
		"<DIV class=\"title\"><H2>%s</H2></DIV>"EXT_NEW_LINE"<DIV class=\"fields-info\">"EXT_NEW_LINE"\t<DIV class=\"field\">Result:%s</DIV>"EXT_NEW_LINE"</DIV>"EXT_NEW_LINE EXT_NEW_LINE,
		title, msg);

	index += snprintf((char *)ehc->data+headerLength-8, 5, "%d", contentLength);

	ehc->contentLength = (unsigned short)(index+contentLength);
//	ehc->dataSendIndex = 0;
	
	ehc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}


