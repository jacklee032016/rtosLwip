/*
* Web pages service in CGI
*/


#include "lwipExt.h"

#include "http.h"
#include "jsmn.h"

static int __extHttpWebPagePrintHeader(char *data, unsigned int size, MuxHttpConn *mhc)
{
	int index = 0;
	
	index += snprintf(data+index, size-index, "HTTP/1.0 200 OK"EXT_NEW_LINE );
	index += snprintf(data+index, size-index, "Server: "MHTTPD_SERVER_AGENT"" EXT_NEW_LINE);
	index += snprintf(data+index, size-index, "Content-type: text/html" EXT_NEW_LINE );
	index += snprintf(data+index, size-index, "Content-Length: 955 " EXT_NEW_LINE EXT_NEW_LINE );
	
	return index;
}


static int __extHttpWebPageReboot(char *data, unsigned int size, MuxHttpConn *mhc, int seconds)
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

static char _extHttpWebPageReboot(MuxHttpConn  *mhc, void *data)
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


static int __extHttpWebPageMedia(char *data, unsigned int size, MuxHttpConn *mhc)
{
	int index = 0;
	EXT_RUNTIME_CFG	*runCfg = mhc->runCfg;
	
	/* device */
	index += snprintf(data+index, size-index, "<DIV class=\"title\"><H2>Video/Audio</H2></DIV><DIV class=\"fields-info\"><DIV class=\"field\"><LABEL >Multicast:</LABEL><DIV class=\"label\">%s</DIV></DIV>",
		STR_BOOL_VALUE(runCfg->isMCast) );

	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Media IP Address:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", EXT_LWIP_IPADD_TO_STR(&(runCfg->dest.ip)) );

	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Video Port:</LABEL><DIV class=\"label\">%d</DIV></DIV>", runCfg->dest.vport );
	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Audio Port:</LABEL><DIV class=\"label\">%d</DIV></DIV>", runCfg->dest.aport );

	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Data Port:</LABEL><DIV class=\"label\">%d</DIV></DIV>", runCfg->dest.dport );
	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Stream Port:</LABEL><DIV class=\"label\">%d</DIV></DIV>", runCfg->dest.sport );

	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Resolution:</LABEL><DIV class=\"label\">%d x %d</DIV></DIV>", runCfg->runtime.vWidth, runCfg->runtime.vHeight);
	
	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Frame Rate:</LABEL><DIV class=\"label\" >%d</DIV></DIV>", runCfg->runtime.vFrameRate );

	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Color Depth:</LABEL><DIV class=\"label\" >%d</DIV></DIV>", runCfg->runtime.vDepth );

	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Color Space:</LABEL><DIV class=\"label\" >%s</DIV></DIV>", CMN_FIND_V_COLORSPACE(runCfg->runtime.vColorSpace));

	
	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Audio Frequency:</LABEL><DIV class=\"label\" >%d</DIV></DIV>", runCfg->runtime.aSampleRate);
	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Audio Bitrate:</LABEL><DIV class=\"label\" >%d</DIV></DIV>", runCfg->runtime.aDepth );
	index += snprintf(data+index, size-index, "<DIV class=\"field\"><LABEL >Audio Channels:</LABEL><DIV class=\"label\" >%d</DIV></DIV>", runCfg->runtime.aChannels);

	index += snprintf(data+index, size-index, "</DIV></DIV>" );
	
	return index;
}


static char _extHttpWebPageMediaHander(MuxHttpConn  *mhc, void *data)
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

static int __extHttpWebPageInfo(char *data, unsigned int size, MuxHttpConn *mhc)
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


static char _extHttpWebPageInfoHander(MuxHttpConn  *mhc, void *data)
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



static int __extHttpWebPageRoot(char *data, unsigned int size, MuxHttpConn *mhc)
{
	int index = 0;
	
	index += snprintf(data+index, size-index, "<HTML><HEAD><TITLE>Muxlab %s-500767</TITLE>", EXT_IS_TX(&extRun)?"TX":"RX" );
	index += snprintf(data+index, size-index, "<LINK href=\"/styles.css\" type=\"text/css\" rel=\"stylesheet\"><SCRIPT type=\"text/javascript\" src=\"/load_html.js\"></SCRIPT></HEAD>"EXT_NEW_LINE );
	index += snprintf(data+index, size-index, "<BODY onload=\"JavaScript:load_http_doc('%s', 'content','')\"><DIV id=\"body\"><DIV id=\"header\"><a id=\"logo\" href=\"/\"><img alt=\"Muxlab Control Panel\" src=\"/logo.jpg\"></a><br />",
		EXT_WEBPAGE_INFO);
	index += snprintf(data+index, size-index, "<div data-text=\"dt_productName\" id=\"id_mainProductName\">500767-%s-UTP 3G-SDI/ST2110 over IP Uncompressed Extender %s, UTP</div></DIV>" , 
		EXT_IS_TX(&extRun)?"TX":"RX", EXT_IS_TX(&extRun)?"TX":"RX"   );
	index += snprintf(data+index, size-index, "<DIV id=\"nav\"><a data-text=\"Info\" id=\"nav_info\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">Info</a>", 
		EXT_WEBPAGE_INFO);
	index += snprintf(data+index, size-index, "<a id=\"nav_media\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">Media</a>", 
		EXT_WEBPAGE_MEDIA);

	index += snprintf(data+index, size-index, "<a id=\"nav_upgrade_mcu\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">Upgrade MCU</a>", 
		EXT_WEBPAGE_UPDATE_MCU_HTML);

	index += snprintf(data+index, size-index, "<a id=\"nav_upgrade_fpga\" class=\"\" href=\"JavaScript:load_http_doc('%s', 'content','')\">Upgrade FPGA</a>",
		EXT_WEBPAGE_UPDATE_FPGA_HTML);

	index += snprintf(data+index, size-index, "<a id=\"nav_upgrade_fpga\" class=\"\" href=\"JavaScript:reboot_device()\">Reboot</a></DIV>");

	index += snprintf(data+index, size-index, "<DIV id=\"message\"></DIV><DIV id=\"content\"></DIV><DIV id=\"footer\">&copy; MuxLab Inc. %s</DIV></DIV>", EXT_OS_NAME );


	index += snprintf(data+index, size-index, "<SCRIPT type=\"text/javascript\">function submit_firmware(chip)	{ var form = document.getElementById('formFirmware'); ");
	index += snprintf(data+index, size-index, "var formData = new FormData(form);xhr = new XMLHttpRequest();xhr.open('POST', chip);xhr.send(formData);");
	index += snprintf(data+index, size-index, "document.getElementById('content').innerHTML = '<img src=\"/loading.gif\" />';");
	index += snprintf(data+index, size-index, "xhr.onload = function()" 
		"{if (xhr.status !== 200) {alert('Request failed.  Returned status of ' + xhr.status);} "
		"else{document.getElementById('content').innerHTML=xhr.responseText;}};}" 	);
	index += snprintf(data+index, size-index, " function reboot_device(){var response = confirm(\"Do you want to reboot?\"); "
		"if (response){ "
		"load_http_doc('%s', 'content',''); "
		"setTimeout(function(){window.location.reload(1);}, 3000); } }", EXT_WEBPAGE_REBOOT);

	index += snprintf(data+index, size-index, "</SCRIPT></BODY></HTML>");
	
	return index;
}


char extHttpWebPageRootHander(MuxHttpConn  *mhc, void *data)
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
		uri 		: 	NULL,
		method	: 	HTTP_METHOD_UNKNOWN,
		handler :	NULL
	}
};


char extHttpWebService(MuxHttpConn *mhc, void *data)
{
	const MuxHttpHandle	*page = _webpages;
	char		ret;

	mhc->reqType = EXT_HTTP_REQ_T_CGI;
	
	while(page->uri )
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("API:'%s' :: REQ:'%s'", page->uri, mhc->uri));
		if( IS_STRING_EQUAL(page->uri, mhc->uri))
		{
			ret = page->handler(mhc, (void *)page);
			if(ret == EXIT_FAILURE)
			{
			}	
			if(EXT_DEBUG_IS_ENABLE(EXT_DEBUG_FLAG_CMD))
			{
//				printf("output RES12 %p, %d bytes: '%s'"LWIP_NEW_LINE, (void *)parser, parser->outIndex, parser->outBuffer);
			}
			EXT_DEBUGF(EXT_HTTPD_DEBUG, ("return value of web page %s handler:%d", page->uri, ret) );

			return ret;
		}

		page++;
	}

	return EXIT_FAILURE;
}


char extHttpWebPageResult(MuxHttpConn  *mhc, char *title, char *msg)
{
	int index = 0;
	int headerLength = 0;
	int contentLength = 0;

	memset(mhc->data, 0, sizeof(mhc->data));
	index += __extHttpWebPagePrintHeader((char *)mhc->data+index, sizeof(mhc->data)-index, mhc);
	headerLength = index;

	contentLength += snprintf((char *)mhc->data+index, sizeof(mhc->data)-index, "<DIV class=\"title\"><H2>%s</H2></DIV><DIV class=\"fields-info\"><DIV class=\"field\"><LABEL >Result:%s</LABEL></DIV></DIV>",
		title, msg);

	index += snprintf((char *)mhc->data+headerLength-8, 5, "%d", contentLength);

	mhc->contentLength = (unsigned short)(index+contentLength);
	mhc->dataSendIndex = 0;
	
	mhc->httpStatusCode = WEB_RES_REQUEST_OK;
	return EXIT_SUCCESS;
}


