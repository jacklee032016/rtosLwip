/*
* Web pages service in CGI
*/


#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"



static int __extHttpWebPagePrintHeader(char *data, unsigned int size, ExtHttpConn *mhc)
{
	int index = 0;
	
	index += snprintf(data+index, size-index, "HTTP/1.0 200 OK"EXT_NEW_LINE );
	index += snprintf(data+index, size-index, "Server: "MHTTPD_SERVER_AGENT"" EXT_NEW_LINE);
//	index += snprintf(data+index, size-index, "Cache-Control: no-cache" EXT_NEW_LINE );
	index += snprintf(data+index, size-index, "Content-type: text/html" EXT_NEW_LINE );
	index += snprintf(data+index, size-index, "Content-Length: 955 " EXT_NEW_LINE EXT_NEW_LINE );
	
	return index;
}


static int __extHttpWebPageReboot(char *data, unsigned int size, ExtHttpConn *mhc, int seconds)
{
	int index = 0;
//	EXT_RUNTIME_CFG	*runCfg = mhc->runCfg;

#if 1
	index += snprintf(data+index, size-index, "<DIV class=\"title\"><H2>Reboot</H2></DIV>"); 
	index += snprintf(data+index, size-index, "<DIV class=\"fields-info\"><DIV class=\"field\"><DIV>Waiting.....</DIV></DIV></DIV>");
#else
	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL class=\"label\" >Waiting.....</LABEL></DIV>\n<script type=\"text/javascript\">\n");
	index += snprintf(data+index, size-index,"\nsetTimeout(function(){alert(\"reload\") }, %d);\n", 
		       seconds*1000);
	index += snprintf(data+index, size-index, "\n</script>\n");
#endif

	return index;
}

static char _extHttpWebPageReboot(ExtHttpConn  *mhc, void *data)
{
	int index = 0;
	int headerLength = 0;
	int contentLength = 0;
	
	index += __extHttpWebPagePrintHeader((char *)mhc->data+index, sizeof(mhc->data)-index, mhc);
	headerLength = index;

	contentLength = __extHttpWebPageReboot((char *)mhc->data+index, sizeof(mhc->data)-index, mhc, 3);

	index += snprintf((char *)mhc->data+headerLength-8, 5, "%d", contentLength);

	mhc->contentLength = (unsigned short)(index+contentLength);
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;

#ifdef	ARM
//	EXT_DELAY_MS(1500);
//	EXT_REBOOT();
	extDelayReboot(1000);
#endif

	return EXIT_SUCCESS;
}


#define	FORM_ID		"formSettings2"

//#define	FORM_ID		"formFirmware"
static int __extHttpWebPageMedia(char *data, unsigned int size, ExtHttpConn *mhc)
{
	int index = 0;
	EXT_RUNTIME_CFG	*runCfg = mhc->runCfg;
	struct netif *_netif = (struct netif *)runCfg->netif;
	const EXT_CONST_STR *_str;
	const char *chVal;
	const short *shVal;
	
	/* device */
//	index += snprintf(data+index, size-index, "<DIV id=\"forms\"><FORM method=\"post\" id=\""FORM_ID"\" name=\""FORM_ID"\"  enctype=\"text/plain\" action=\"/setting\">" );
	index += snprintf(data+index, size-index, "<DIV id=\"forms\"><FORM method=\"post\" id=\""FORM_ID"\" >" );

//	<FORM method="post" id="formSettings" enctype=" action="action_page.php">    
	index += snprintf(data+index, size-index, "<DIV class=\"title\"><H2>Video/Audio Settings</H2></DIV>"EXT_NEW_LINE);
	
	index += snprintf(data+index, size-index, "<DIV class=\"fields\">"EXT_NEW_LINE"\t<DIV class=\"field\"><LABEL >SDP Video:</LABEL>");
	if(EXT_IS_TX(runCfg) )
	{
		index += snprintf(data+index, size-index, "http://%s%s", inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)), EXT_WEBPAGE_SDP_VIDEO);
	}
	else
	{
		index += snprintf(data+index, size-index, "<INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_SDP_VEDIO"\" value=\"http://%s%s\"/>", 
			inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)), EXT_WEBPAGE_SDP_VIDEO);
	}
	
	index += snprintf(data+index, size-index, "</DIV>"EXT_NEW_LINE"\t<DIV class=\"field\"><LABEL >SDP Audio:</LABEL>");
	if(EXT_IS_TX(runCfg) )
	{
		index += snprintf(data+index, size-index, "http://%s%s", inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)), EXT_WEBPAGE_SDP_AUDIO);
	}
	else
	{
		index += snprintf(data+index, size-index, "<INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_SDP_AUDIO"\" value=\"http://%s%s\"/>", 
			inet_ntoa(*(struct in_addr *)&(_netif->ip_addr)), EXT_WEBPAGE_SDP_AUDIO);
	}

//	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Video IP Address:</LABEL><DIV class=\"label\" ><INPUT type=\"text\" id=\""EXT_WEB_CFG_FIELD_IP_VEDIO"\" value=\"%s\"/></DIV></DIV>", 
//		EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.ip)) );
	CMN_SN_PRINTF(data, size, index, "</DIV>"EXT_NEW_LINE"\t<DIV class=\"field\"><LABEL >Video IP Address:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_IP_VEDIO"\" value=\"%s\"/></DIV>"EXT_NEW_LINE, 
		EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.ip)) );

	CMN_SN_PRINTF(data, size, index, "\t<DIV class=\"field\"><LABEL >Audio IP Address:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_IP_AUDIO"\" value=\"%s\"/></DIV>"EXT_NEW_LINE, 
		EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.audioIp)) );

	CMN_SN_PRINTF(data, size, index, "\t<DIV class=\"field\"><LABEL >Video Port:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_PORT_VEDIO"\" value=\"%d\"/></DIV>"EXT_NEW_LINE, 
		runCfg->dest.vport );
	CMN_SN_PRINTF(data, size, index, "\t<DIV class=\"field\"><LABEL >Audio Port:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_PORT_AUDIO"\" value=\"%d\"/></DIV>"EXT_NEW_LINE, 
		runCfg->dest.aport );

	CMN_SN_PRINTF(data, size, index, "\t<DIV class=\"field\"><LABEL >Data Port:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_PORT_DATA"\" value=\"%d\"/></DIV>"EXT_NEW_LINE, 
		runCfg->dest.dport );
	CMN_SN_PRINTF(data, size, index, "\t<DIV class=\"field\"><LABEL >Stream Port:</LABEL><INPUT type=\"text\" name=\""EXT_WEB_CFG_FIELD_PORT_STREM"\" value=\"%d\"/></DIV>"EXT_NEW_LINE, 
		runCfg->dest.sport );

	CMN_SN_PRINTF(data, size, index, "\t<DIV class=\"field\"><LABEL >Resolution:</LABEL>"EXT_NEW_LINE);

	CMN_SN_PRINTF(data, size, index, "\t\t<SELECT id=\""EXT_WEB_CFG_FIELD_VIDEO_WIDTH"\">"EXT_NEW_LINE );
	shVal = videoWidthList;
	while(*shVal != 0)
	{
		CMN_SN_PRINTF(data, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, *shVal, (*shVal== runCfg->runtime.vWidth)?"selected":"", *shVal);
		shVal++;
	}
	CMN_SN_PRINTF(data, size, index, "\t\t</SELECT>x"EXT_NEW_LINE);

	CMN_SN_PRINTF(data, size, index, "\t\t<SELECT id=\""EXT_WEB_CFG_FIELD_VIDEO_HEIGHT"\">" EXT_NEW_LINE);
	shVal = videoHeightList;
	while(*shVal != 0)
	{
		CMN_SN_PRINTF(data, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, *shVal, (*shVal== runCfg->runtime.vHeight)?"selected":"", *shVal);
		shVal++;
	}
	CMN_SN_PRINTF(data, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);

	
	CMN_SN_PRINTF(data, size, index, "\t<DIV class=\"field\"><LABEL >Frame Rate:</LABEL>"EXT_NEW_LINE"\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_FRAME_RATE"\">" EXT_NEW_LINE);
	chVal = videoFpsList;
	while(*chVal != 0)
	{
		CMN_SN_PRINTF(data, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, *chVal, (*chVal== runCfg->runtime.vFrameRate)?"selected":"", *chVal);
		chVal++;
	}
	CMN_SN_PRINTF(data, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);

	/* color depth */
	CMN_SN_PRINTF(data, size, index, "\t<DIV class=\"field\"><LABEL >Color Depth:</LABEL>"EXT_NEW_LINE"\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_COLOR_DEPTH"\">"EXT_NEW_LINE);
	chVal = videoColorDepthList;
	while(*chVal != 0)
	{
		CMN_SN_PRINTF(data, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, *chVal, (*chVal== runCfg->runtime.vDepth)?"selected":"", *chVal);
		chVal++;
	}
	CMN_SN_PRINTF(data, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);

	/* color space*/
	CMN_SN_PRINTF(data, size, index, "\t<DIV class=\"field\"><LABEL >Color Space:</LABEL>"EXT_NEW_LINE);
	CMN_SN_PRINTF(data, size, index, "\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_COLOR_SPACE"\">"EXT_NEW_LINE);
	_str = _videoColorSpaces;
	while(_str->type!= EXT_INVALIDATE_STRING_TYPE)
	{
		CMN_SN_PRINTF(data, size, index, "\t\t\t<OPTION value=\"%d\" %s>%s</OPTION>"EXT_NEW_LINE, _str->type, (_str->type== runCfg->runtime.vColorSpace)?"selected":"", _str->name);
		_str++;
	}
	CMN_SN_PRINTF(data, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);


#if 0	
	CMN_SN_PRINTF(data, size, index, "<DIV class=\"field\"><LABEL >Audio Frequency:</LABEL><DIV class=\"label\" >%d</DIV></DIV>", runCfg->runtime.aSampleRate);
	CMN_SN_PRINTF(data, size, index,  "<DIV class=\"field\"><LABEL >Audio Bitrate:</LABEL><DIV class=\"label\" >%d</DIV></DIV>", runCfg->runtime.aDepth );
#endif

	CMN_SN_PRINTF(data, size, index, "\t<DIV class=\"field\"><LABEL >Audio Channels:</LABEL>"EXT_NEW_LINE"\t\t<SELECT name=\""EXT_WEB_CFG_FIELD_AUDIO_CHANNEL"\">"EXT_NEW_LINE );
	chVal = audioChannelsList;
	while(*chVal != 0)
	{
		CMN_SN_PRINTF(data, size, index, "\t\t\t<OPTION value=\"%d\" %s>%d</OPTION>"EXT_NEW_LINE, *chVal, (*chVal== runCfg->runtime.aChannels)?"selected":"", *chVal);
		chVal++;
	}
	CMN_SN_PRINTF(data, size, index, "\t\t</SELECT></DIV>"EXT_NEW_LINE);

//<INPUT type="button" value="Submit" class="btnSubmit" id="btnSubmit" />
#if 1
	CMN_SN_PRINTF(data, size, index,  "\t<DIV class=\"buttons\">"EXT_NEW_LINE"\t<INPUT type=\"submit\" value=\"Submit\" class=\"btnSubmit\" onClick=\"submit_firmware('"FORM_ID"','\"settings2\"')\"/>"EXT_NEW_LINE
#else
	CMN_SN_PRINTF(data, size, index,  "\t<DIV class=\"buttons\">"EXT_NEW_LINE"\t<INPUT type=\"submit\" value=\"Submit\" class=\"btnSubmit\" id=\"btnSubmit\" />"EXT_NEW_LINE
#endif
			"\t<INPUT name=\"ResetButton\" type=\"reset\" class=\"btnReset\" value=\"Cancel\" id=\"ResetButton\"/></DIV>"EXT_NEW_LINE );

	CMN_SN_PRINTF(data, size, index, "\t</DIV>"EXT_NEW_LINE"</DIV>"EXT_NEW_LINE"</FORM></DIV>"EXT_NEW_LINE );

#if 0
#if 1
	CMN_SN_PRINTF(data, size, index,  "<SCRIPT type=\"text/javascript\">function submit_setting(chip){ var form = document.getElementById('"FORM_ID"');");
	CMN_SN_PRINTF(data, size, index,  "var formData = new FormData(form);xhr = new XMLHttpRequest();xhr.open('POST', chip);xhr.send(formData);");
#else
	CMN_SN_PRINTF(data, size, index,  "<SCRIPT type=\"text/javascript\">function submit_setting(chip){ alert('Submit POST'); document."FORM_ID".submit();");
#endif
	CMN_SN_PRINTF(data, size, index,  "document.getElementById('content').innerHTML= '<img src=\"/loading.gif\" />';");
	CMN_SN_PRINTF(data, size, index,  "xhr.onload = function()" 
		"{if (xhr.status !== 200) {alert('Request failed.  Returned status of ' + xhr.status);} "
		"else{document.getElementById('content').innerHTML=xhr.responseText;}};}" 	);
	CMN_SN_PRINTF(data, size, index,  "</SCRIPT>");
#endif

	printf("Data:'%s'\r\n", data);
	
	return index;
}


static char _extHttpWebPageMediaHander(ExtHttpConn  *mhc, void *pageHandle)
{
	int index = 0;
	int headerLength = 0;
	int contentLength = 0;

	
	index += __extHttpWebPagePrintHeader((char *)mhc->data+index, sizeof(mhc->data)-index, mhc);
	headerLength = index;

	contentLength = __extHttpWebPageMedia((char *)mhc->data+index, sizeof(mhc->data)-index, mhc);

	index += snprintf((char *)mhc->data+headerLength-8, 5, "%d", contentLength);

	mhc->contentLength = (unsigned short)(index+contentLength);
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}

#ifdef	ARM
//_CODE char *versionString;
#endif

static int __extHttpWebPageInfo(char *data, unsigned int size, ExtHttpConn *mhc)
{
	int index = 0;
	EXT_RUNTIME_CFG	*runCfg = mhc->runCfg;
	
	/* device */
	index += snprintf(data+index, size-index, "<DIV class=\"title\"><H2>Device</H2></DIV><DIV class=\"fields-info\"><DIV class=\"field\"><LABEL >Product Name:</LABEL><DIV class=\"label\">%s</DIV></DIV>",
		EXT_767_PRODUCT_NAME);
	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Model:</LABEL><DIV class=\"label\">%s-%s</DIV></DIV>"EXT_NEW_LINE, EXT_767_MODEL, EXT_IS_TX(runCfg)?"TX":"RX" );
	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Custom Name:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", runCfg->name);
#ifdef	ARM
	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Firmware Version:</LABEL><DIV class=\"label\">%s(Build %s)</DIV></DIV>",  EXT_VERSION_STRING, BUILD_DATE_TIME );
	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >FPGA Version:</LABEL><DIV class=\"label\">%s</DIV></DIV>",  extFgpaReadVersion() );
#else
	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Firmware Version:</LABEL><DIV class=\"label\">%02d.%02d.%02d</DIV></DIV>",
		runCfg->version.major, runCfg->version.minor, runCfg->version.revision);
#endif
	index += snprintf(data+index, size-index, "</DIV>");

	/* settings */
	index += snprintf(data+index, size-index, "<DIV class=\"title\"><H2>Settings</H2></DIV><DIV class=\"fields-info\"><DIV class=\"field\"><LABEL >MAC Address:</LABEL><DIV class=\"label\" >");
	MAC_ADDRESS_PRINT(data, size, index, &(runCfg->local.mac));
	index += snprintf(data+index, size-index, "</DIV></DIV>");

	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >IP Address:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", EXT_LWIP_IPADD_TO_STR(&(runCfg->local.ip)) );

	index += snprintf(data+index, size-index, " <DIV class=\"field\"><LABEL >Subnet Mask:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", EXT_LWIP_IPADD_TO_STR(&(runCfg->ipMask)) );

	index += snprintf(data+index, size-index, " <DIV class=\"field\"><LABEL >Gateway:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", EXT_LWIP_IPADD_TO_STR(&(runCfg->ipGateway)) );
	index += snprintf(data+index, size-index, " <DIV class=\"field\"><LABEL >DHCP:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", STR_BOOL_VALUE(runCfg->netMode) );
	index += snprintf(data+index, size-index, " <DIV class=\"field\"><LABEL >Dipswitch:</LABEL><DIV class=\"label\" >%s</DIV></DIV></DIV>", STR_BOOL_VALUE(runCfg->isDipOn) );

	/* RS232 */
	index += snprintf(data+index, size-index, "<DIV class=\"title\"><H2>RS232</H2></DIV><DIV class=\"fields-info\"><DIV class=\"field\"><LABEL >Baudrate:</LABEL><DIV class=\"label\">%d</DIV></DIV>",
		runCfg->rs232Cfg.baudRate);

	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Databits:</LABEL><DIV class=\"label\">%d</DIV></DIV>",
		runCfg->rs232Cfg.charLength);
	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Parity:</LABEL><DIV class=\"label\">%s</DIV></DIV>",
		CMN_FIND_RS_PARITY((unsigned short)runCfg->rs232Cfg.parityType) );
	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Stopbits:</LABEL><DIV class=\"label\">%d</DIV></DIV>",
		runCfg->rs232Cfg.stopbits);

	index += snprintf(data+index, size-index, "</DIV></DIV>" );
	
	return index;
}


static char _extHttpWebPageInfoHander(ExtHttpConn  *mhc, void *pageHandle)
{
	int index = 0;
	int headerLength = 0;
	int contentLength = 0;

	
	index += __extHttpWebPagePrintHeader((char *)mhc->data+index, sizeof(mhc->data)-index, mhc);
	headerLength = index;

	contentLength = __extHttpWebPageInfo((char *)mhc->data+index, sizeof(mhc->data)-index, mhc);

	index += snprintf((char *)mhc->data+headerLength-8, 5, "%d", contentLength);

	mhc->contentLength = (unsigned short)(index+contentLength);
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}



static int __extHttpWebPageRoot(char *data, unsigned int size, ExtHttpConn *mhc)
{
	int index = 0;
	
	index += snprintf(data+index, size-index, "<HTML><HEAD><TITLE>Muxlab %s-500767</TITLE>", EXT_IS_TX(&extRun)?"TX":"RX" );
	index += snprintf(data+index, size-index, "<LINK href=\"/styles.css\" type=\"text/css\" rel=\"stylesheet\"><SCRIPT type=\"text/javascript\" src=\"/load_html.js\"></SCRIPT></HEAD>"EXT_NEW_LINE );
	index += snprintf(data+index, size-index, "<BODY onload=\"JavaScript:load_http_doc('%s', 'content','')\"><DIV id=\"body\"><DIV id=\"header\"><a id=\"logo\" href=\"/\"><img alt=\"Muxlab Control Panel\" src=\"/logo.jpg\"></a><br />",
		EXT_WEBPAGE_MEDIA);
	index += snprintf(data+index, size-index, "<div data-text=\"dt_productName\" id=\"id_mainProductName\">500767-%s-UTP 3G-SDI/ST2110 over IP Uncompressed Extender %s, UTP</div></DIV>" , 
		EXT_IS_TX(&extRun)?"TX":"RX", EXT_IS_TX(&extRun)?"TX":"RX"   );
	
	index += snprintf(data+index, size-index, "<DIV id=\"nav\"><a id=\"nav_media\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">Media</a>", 
		EXT_WEBPAGE_MEDIA);

	index += snprintf(data+index, size-index, "<a data-text=\"Info\" id=\"nav_info\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">System Info</a>", 
		EXT_WEBPAGE_INFO);
	
	index += snprintf(data+index, size-index, "<a id=\"nav_upgrade_mcu\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">Upgrade MCU</a>", 
		EXT_WEBPAGE_UPDATE_MCU_HTML);

	index += snprintf(data+index, size-index, "<a id=\"nav_upgrade_fpga\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">Upgrade FPGA</a>",
		EXT_WEBPAGE_UPDATE_FPGA_HTML);

	index += snprintf(data+index, size-index, "<a id=\"nav_upgrade_fpga\" class=\"\" href=\"JavaScript:reboot_device()\">Reboot</a></DIV>");

	index += snprintf(data+index, size-index, "<DIV id=\"message\"></DIV><DIV id=\"content\"></DIV><DIV id=\"footer\">&copy; MuxLab Inc. %s</DIV></DIV>", EXT_OS_NAME );


	index += snprintf(data+index, size-index, 
		"<SCRIPT type=\"text/javascript\">"EXT_NEW_LINE 
			"function submit_firmware(formId, chip){" EXT_NEW_LINE \
/*			
		       "\tvar data = new FormData(); data.append('user', 'person');"
		       "\tvar params = 'orem=ipsum&name=binny';"
*/		       
		       "var form = document.getElementById(formId); "EXT_NEW_LINE);
	index += snprintf(data+index, size-index, 
		       "\tvar formData = new FormData(form);"EXT_NEW_LINE \
/*		       "\txhr.setRequestHeader(\"Content-type\", \"text/plain;charset=UTF-8\");" EXT_NEW_LINE */
/*		       "\tif (formId == '"FORM_ID"') xhr.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");" EXT_NEW_LINE  */
		       "\txhr = new XMLHttpRequest();"EXT_NEW_LINE 
/*		       "\txhr.setRequestHeader(\"Cache-Control\", \"no-cache\");"EXT_NEW_LINE*/
/*			 "alert('Submit');" EXT_NEW_LINE
			 "console.log('Submit' );" EXT_NEW_LINE*/
		       "\txhr.open('POST', chip);"EXT_NEW_LINE
			"\tif (formId == '"FORM_ID"'){ xhr.send(urlencodeFormData(formData));}"EXT_NEW_LINE
			"\telse{ xhr.send(formData);}"EXT_NEW_LINE); 
/*		       "\txhr.send(urlencodeFormData(formData));"EXT_NEW_LINE);*/
	index += snprintf(data+index, size-index, 
			"\tdocument.getElementById('content').innerHTML = '<img src=\"/loading.gif\" />';"EXT_NEW_LINE);
	index += snprintf(data+index, size-index, 
			"\txhr.onload = function(){" EXT_NEW_LINE
			"\t\tif (xhr.status !== 200) {alert('Request failed.  Returned status of ' + xhr.status);} "EXT_NEW_LINE
			"\t\telse{document.getElementById('content').innerHTML=xhr.responseText;}" EXT_NEW_LINE
			"\t}"EXT_NEW_LINE"}" EXT_NEW_LINE EXT_NEW_LINE);
	
	index += snprintf(data+index, size-index, 
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
	index += snprintf(data+index, size-index, 
			"function urlencodeFormData(fd){"EXT_NEW_LINE
			"\tvar s = '';" EXT_NEW_LINE
			"\t function encode(s){ return encodeURIComponent(s).replace(/%20/g,'+'); }"EXT_NEW_LINE
			"\t for(var pair of fd.entries()){"EXT_NEW_LINE
			"\t\t if(typeof pair[1]=='string'){"EXT_NEW_LINE
			"\t\t\t s += (s?'\r\n':'') + encode(pair[0])+'='+encode(pair[1]);"EXT_NEW_LINE
			"\t}"EXT_NEW_LINE"}" EXT_NEW_LINE " return s;"EXT_NEW_LINE"}"EXT_NEW_LINE EXT_NEW_LINE);

#if 0
#if 0
	CMN_SN_PRINTF(data, size, index,  
			"function submit_setting(chip){"EXT_NEW_LINE
			"\tvar form = document.getElementById('"FORM_ID"');");
	CMN_SN_PRINTF(data, size, index,  
			"\tvar formData = new FormData(form);"EXT_NEW_LINE
			"\txhr = new XMLHttpRequest();"EXT_NEW_LINE
			"\txhr.open('POST', chip);"EXT_NEW_LINE
			"\txhr.send(formData);"EXT_NEW_LINE);
#else
	CMN_SN_PRINTF(data, size, index,  
			"function submit_setting(chip){"EXT_NEW_LINE
			"\talert('Submit POST');"EXT_NEW_LINE
			"\tdocument."FORM_ID".submit();");
#endif
	CMN_SN_PRINTF(data, size, index,  
			"\tdocument.getElementById('content').innerHTML= '<img src=\"/loading.gif\" />';"EXT_NEW_LINE);
	CMN_SN_PRINTF(data, size, index,  
			"\txhr.onload = function(){"EXT_NEW_LINE 
			"\t\tif (xhr.status !== 200){alert('Request failed.  Returned status of ' + xhr.status);} "EXT_NEW_LINE
			"\t\telse{document.getElementById('content').innerHTML=xhr.responseText;}"EXT_NEW_LINE
			"\t}" EXT_NEW_LINE 
			"}" EXT_NEW_LINE EXT_NEW_LINE	);
#endif

	index += snprintf(data+index, size-index, "</SCRIPT></BODY></HTML>");

	printf("Data:'%s'\r\n", data);
	
	return index;
}


static char _extHttpWebSettingHander(ExtHttpConn *ehc, void *pageHandle)
{
//	int index = 0;
#define	_CHAR_SEPERATE			"&"
#define	_CHAR_EQUAL				"="

	EXT_RUNTIME_CFG *rxCfg = &tmpRuntime;

	short left = ehc->leftData;
	char *data = ehc->headers + ehc->headerLength+__HTTP_CRLF_SIZE;
	char *key, *value, *nextKey;
	int i = 0;

	extSysClearConfig(rxCfg);
	
	data[left] = 0;
	key = data;
	while(key)
	{
		value = lwip_strnstr(key, _CHAR_EQUAL, left );
		if(value == NULL)
		{
			snprintf(ehc->boundary, sizeof(ehc->boundary), "'%s' has no value defined", key);
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
	return extHttpWebPageResult(ehc, "Waiting...", "Implementing in next stage");
error:

	return extHttpWebPageResult(ehc, "ERROR", ehc->boundary);
	
}


char extHttpWebPageRootHander(ExtHttpConn  *mhc, void *pageHandle)
{
	int index = 0;
	int headerLength = 0;
	int contentLength = 0;

	TRACE();
	
	index += __extHttpWebPagePrintHeader((char *)mhc->data+index, sizeof(mhc->data)-index, mhc);
	headerLength = index;

	contentLength = __extHttpWebPageRoot((char *)mhc->data+index, sizeof(mhc->data)-index, mhc);

	index += snprintf((char *)mhc->data+headerLength-8, 5, "%d", contentLength);

	mhc->contentLength = (unsigned short)(index+contentLength);
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}



/* root access point of NODE API */
static const MuxHttpHandle	_webpages[] =
{
	{
		uri 		: 	"/",
		method	: 	HTTP_METHOD_GET,
		handler	:	extHttpWebPageRootHander
	},
	
	{
		uri 		: 	EXT_WEBPAGE_INFO,
		method	: 	HTTP_METHOD_GET,
		handler	:	_extHttpWebPageInfoHander
	},
	
	{
		uri 		: 	EXT_WEBPAGE_MEDIA,
		method	: 	HTTP_METHOD_GET,
		handler	:	_extHttpWebPageMediaHander
	},

	{
		uri 		: 	EXT_WEBPAGE_REBOOT,
		method	: 	HTTP_METHOD_GET,
		handler	:	_extHttpWebPageReboot
	},

	{
		uri 		: 	EXT_WEBPAGE_SETTING,
		method	: 	HTTP_METHOD_POST,
		handler	:	_extHttpWebSettingHander
	},

	{
		uri 		: 	EXT_WEBPAGE_SDP_VIDEO,
		method	: 	HTTP_METHOD_GET,
		handler	:	extHttpSdpVideo
	},

	{
		uri 		: 	EXT_WEBPAGE_SDP_AUDIO,
		method	: 	HTTP_METHOD_GET,
		handler	:	extHttpSdpAudio
	},

	{
		uri 		: 	EXT_WEBPAGE_API_SERVICE,
		method	: 	HTTP_METHOD_GET,
		handler	:	extHttpSimpleApi
	},


	{
		uri 		: 	NULL,
		method	: 	HTTP_METHOD_UNKNOWN,
		handler :	NULL
	}
};


char extHttpWebService(ExtHttpConn *ehc, void *data)
{
	const MuxHttpHandle	*page = _webpages;
	char		ret;

	ehc->reqType = EXT_HTTP_REQ_T_CGI;
	
	while(page->uri )
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("API:'%s' :: REQ:'%s'", page->uri, ehc->uri));
		if( IS_STRING_EQUAL(page->uri, ehc->uri))
		{
			ret = page->handler(ehc, (void *)page);
			if(ret == EXIT_FAILURE)
			{
				extHttpWebPageResult(ehc, "Data Error", "Some data is invalidate, correct and try again!");
			}	
			if(EXT_DEBUG_IS_ENABLE(EXT_DEBUG_FLAG_CMD))
			{
//				printf("output RES12 %p, %d bytes: '%s'"LWIP_NEW_LINE, (void *)parser, parser->outIndex, parser->outBuffer);
			}
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("return value of web page '%s' handler:%d", page->uri, ret) );

			return ret;
		}

		page++;
	}

	return EXIT_FAILURE;
}


char extHttpWebPageResult(ExtHttpConn  *mhc, char *title, char *msg)
{
	int index = 0;
	int headerLength = 0;
	int contentLength = 0;

	memset(mhc->data, 0, sizeof(mhc->data));
	index += __extHttpWebPagePrintHeader((char *)mhc->data+index, sizeof(mhc->data)-index, mhc);
	headerLength = index;

	contentLength += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, 
		"<DIV class=\"title\"><H2>%s</H2></DIV>"EXT_NEW_LINE"<DIV class=\"fields-info\">"EXT_NEW_LINE"\t<DIV class=\"field\"><LABEL >Result:%s</LABEL></DIV>"EXT_NEW_LINE"</DIV>"EXT_NEW_LINE EXT_NEW_LINE,
		title, msg);

	index += snprintf((char *)mhc->data+headerLength-8, 5, "%d", contentLength);

	mhc->contentLength = (unsigned short)(index+contentLength);
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}


