/*
* common logics used in booloader, RTOS and simhost. So only ANSI is used in this file
*/

#ifndef	__EXT_SYS_PARAMS_H__
#define	__EXT_SYS_PARAMS_H__

/* defined in stdlib.h */
#ifndef	EXIT_FAILURE
#define	EXIT_FAILURE 			1
#endif

#ifndef	EXIT_SUCCESS
#define	EXIT_SUCCESS 			0
#endif

#ifndef EXT_FALSE
#define EXT_FALSE				0
#endif

#ifndef EXT_TRUE
#define EXT_TRUE				(!EXT_FALSE)
#endif

#include <stdint.h>


/***************** system options ******************/

#define		LWIP_EXT_TFTP					1

#define		LWIP_EXT_HTTP					1

#define		LWIP_EXT_NMOS					0

#define		EXT_POLL_TASK					0

/* ---------- MDNS options ---------- */
#define		LWIP_MDNS_RESPONDER        			0
#define		LWIP_NUM_NETIF_CLIENT_DATA		(LWIP_MDNS_RESPONDER)

#define		LWIP_EXT_UDP_RX_PERF			0
#define		LWIP_EXT_UDP_TX_PERF			0	/* extend for UDP TX Perf testing */

#define		LWIP_EXT_TCP_PERF				0	/* extend for TCP Perf testing */

#define		LWIP_EXT_MQTT_CLIENT			0

#define		LWIP_EXT_PTP					0	/* precision time protocol */

#define		LWIP_EXT_HTTP_CLIENT			1	/* HTTP Client */


#define		EXT_DIP_SWITCH_ON				0

#define		EXT_FPGA_AUX_ON				0

#define		SYS_DEBUG_STARTUP				1


/* button configuration */
#define		_RESET_BTN_RESTORE_FACTORY 				1
#define		_RESET_BTN_STAY_IN_BOOTLOADER			2

#define		RESET_BTN_DELAY_SECONDS					5

#define		RESET_BTN_MODE		_RESET_BTN_STAY_IN_BOOTLOADER




#define	EXT_EVENT_NONE 					0
	/**/
#define	EXT_EVENT_FACTORY_RESET			1 			/* factory reset event from ISR */

#define	EXT_SYS_EVENT_MAX				EXT_EVENT_FACTORY_RESET


#define	EXT_STATE_CONTINUE				0


#define	BTN_FACTORY_DURATION				6000 /* requirement */


#define		EXT_NET_IF_TASK_STACK_SIZE				256*4

#define		EXT_NET_IF_TASK_PRIORITY					(4)		/* highest priority level in Free RTOS */

#define		EXT_MCTRL_MBOX_SIZE						4*2

#define	EXT_RW_BUFFER_SIZE				1024


#define	EXT_COMMAND_BUFFER_SIZE			(1024+1024)	/* for output buffer */

#define	EXT_SC_KEY_MAX_LENGTH			32*2
#define	EXT_SC_ID_MAX_LENGTH				8*2

#define	EXT_MAGIC_SIZE						2

#define	EXT_USER_SIZE						16
#define	EXT_PASSWORD_SIZE					16

#define	HEX_DATA_MAX_LENGTH				128


#define	EXT_MAGIC_VALUE_A				0xA5
#define	EXT_MAGIC_VALUE_B				0x5A

#define	EXT_MAC_ADDRESS_LENGTH			6

#define	EXT_IP_CFG_DHCP_ENABLE			(1<<0)
#define	EXT_IP_CFG_MC_ENABLE				(1<<1)	/* multicast */
#define	EXT_IP_CFG_ARP_ENABLE			(1<<2)		

/* configuration options for 767 board */
/** MAC address definition.  The MAC address must be unique on the network. */
#define ETHERNET_CONF_ETHADDR0                        0x00
#define ETHERNET_CONF_ETHADDR1                        0x04
#define ETHERNET_CONF_ETHADDR2                        0x25
#define ETHERNET_CONF_ETHADDR3                        0x1C
#if EXTLAB_BOARD
#define ETHERNET_CONF_ETHADDR5                        0x01
#else
#define ETHERNET_CONF_ETHADDR5                        0x02
#endif

#define ETHERNET_CONF_ETHADDR4_TX			0x10
#define ETHERNET_CONF_ETHADDR4_RX			0xA0


#define	MCAST_DEFAULT_IPADDR0				239
#define	MCAST_DEFAULT_IPADDR1				100
#define	MCAST_DEFAULT_IPADDR2				1
#define	MCAST_DEFAULT_IPADDR3				1


/** The IP address being used. */
#define ETHERNET_CONF_IPADDR0					192
#define ETHERNET_CONF_IPADDR1					168
#if ARCH_ARM
	#if EXTLAB_BOARD
	#define ETHERNET_CONF_IPADDR2_TX			168
	#define ETHERNET_CONF_IPADDR2_RX			168
	#define ETHERNET_CONF_IPADDR3_TX			64
	#else
	#define ETHERNET_CONF_IPADDR2_TX			169
	#define ETHERNET_CONF_IPADDR2_RX			169
	#define ETHERNET_CONF_IPADDR3_TX			121
	#endif
#else
	/* X86 test environment */
	#define ETHERNET_CONF_IPADDR2_TX			166
	#define ETHERNET_CONF_IPADDR2_RX			167
	#define ETHERNET_CONF_IPADDR3_TX			2
#endif

#if ARCH_ARM
#define ETHERNET_CONF_IPADDR3_RX				65
#else
#define ETHERNET_CONF_IPADDR3_RX				3
#endif

#ifdef	ARM
#define	CONF_SVR811_IPADDR2						168 		/* X86-->board */
//#define	CONF_SVR811_IPADDR3						102 		/* test with my computer */
#define	CONF_SVR811_IPADDR3						50
#else
#define	CONF_SVR811_IPADDR2						166 		/* simhost */
#define	CONF_SVR811_IPADDR3						1 		/* simhost */
#endif
#define	CONF_SVR811_PORT							3840


/** The gateway address being used. */
#define ETHERNET_CONF_GATEWAY_ADDR0                   192
#define ETHERNET_CONF_GATEWAY_ADDR1                   168
#if ARCH_ARM
	#if EXTLAB_BOARD
	#define ETHERNET_CONF_GATEWAY_ADDR2_TX			168
	#define ETHERNET_CONF_GATEWAY_ADDR2_RX			168
	#else
	#define ETHERNET_CONF_GATEWAY_ADDR2_TX			169
	#define ETHERNET_CONF_GATEWAY_ADDR2_RX			169
	#endif
#else
	#define ETHERNET_CONF_GATEWAY_ADDR2_TX			166
	#define ETHERNET_CONF_GATEWAY_ADDR2_RX			167
#endif

#define ETHERNET_CONF_GATEWAY_ADDR3                   1

/** The network mask being used. */
#define ETHERNET_CONF_NET_MASK0                       255
#define ETHERNET_CONF_NET_MASK1                       255
#define ETHERNET_CONF_NET_MASK2                       255
#define ETHERNET_CONF_NET_MASK3                       0

#define	EXT_HTTP_SVR_PORT						80

#define	EXT_SDP_SVR_PORT						80

#define	EXT_MEDIA_PORT_RANGE					10		/* range of RTP port */

#define	EXT_MEDIA_PORT_TX_VIDEO				40000
#define	EXT_MEDIA_PORT_TX_AUDIO				40010
#define	EXT_MEDIA_PORT_TX_DATA				40020
#define	EXT_MEDIA_PORT_TX_STREA				40030

#define	EXT_MEDIA_PORT_RX_VIDEO				48000
#define	EXT_MEDIA_PORT_RX_AUDIO				48010
#define	EXT_MEDIA_PORT_RX_DATA				48020
#define	EXT_MEDIA_PORT_RX_STREA				48030


#define	EXT_767_PRODUCT_NAME				"SdiOverIP-767"

#define	EXT_767_MODEL							"500767"

#define	EXT_USER								"admin"
#define	EXT_PASSWORD							"admin"

#define	EXT_SUPER_USER						"muxlab"
#define	EXT_SUPER_PASSWORD					"muxlab"

#define	EXT_MDNS_HOST_NAME					"an767tx"
#define	EXT_MDNS_SERVICE_NAME				"nmos"


#define	EXT_MQTT_CLIENT_ID					"extMqtt"
#define	EXT_MQTT_USER_NAME					"admin"
#define	EXT_MQTT_PASSWORD					"admin"

#define	EXT_NEW_LINE							"\r\n"
//#define	EXT_NEW_LINE							"\r"

#define	EXT_PROMPT							">"


#define	ASCII_KEY_TAB				( 0x09 )	/* horizontal TAB */
#define	ASCII_KEY_LF				( 0x0A )	/* Line Feed, \n */
#define	ASCII_KEY_CR				( 0x0D )	/* Carier Return, \r */
/* DEL acts as a backspace. */
#define	ASCII_KEY_DEL				( 0x7F )

#define	EXT_CMD_MAX_ARGUMENTS		10
#define	EXT_CMD_MAX_LENGTH			32

/* when server send uri without root, client always append root in their requesting uri */

#define	EXT_WEBPAGE_ROOT						"/"

#define	EXT_WEBPAGE_INFO						"info"
#define	EXT_WEBPAGE_MEDIA					"media"
#define	EXT_WEBPAGE_SETTING					"setting"
#define	EXT_WEBPAGE_SDP_CLIENT				"sdpClient"

#define	EXT_WEBPAGE_UPDATE_MCU				"mcuUpdate"
#define	EXT_WEBPAGE_UPDATE_FPGA				"fpgaUpdate"

#define	EXT_WEBPAGE_SYS_CFGS					"sysCfgs"
#define	EXT_WEBPAGE_SYS_UPDATE				"sysUpdate"
#define	EXT_WEBPAGE_RS232_UPDATE			"rs232Update"


#define	EXT_WEBPAGE_REBOOT					"reboot"
#define	EXT_WEBPAGE_RESET					"reset"


#define	EXT_WEBPAGE_UPDATE_HTML				"upgrade.html"
#define	EXT_WEBPAGE_SETTINGS_HTML			"settings.html"

#define	EXT_WEBPAGE_SDP_VIDEO				"video.sdp"
#define	EXT_WEBPAGE_SDP_AUDIO				"audio.sdp"
#define	EXT_WEBPAGE_SDP_ANC					"anc.sdp"

#define	EXT_WEBPAGE_API_SERVICE				"service"



/* macros for version */
#define	EXT_VERSION_DOT(a, b, c)				a ##.## b ##.## c

#define	EXT_VERSION(a, b, c)					EXT_VERSION_DOT(a, b, c)

#define	EXT_STRINGIFY(s)         					EXT_TOSTRING(s)
#define	EXT_TOSTRING(s)						#s


#define	EXT_GLUE(a, b)							a ## b
#define	EXT_JOIN(a, b)							EXT_GLUE(a, b)

#define	EXT_VERSION_MAJOR						1
#define	EXT_VERSION_MINOR						1
#define	EXT_VERSION_REVISION					1

#define	BL_VERSION_MAJOR						0
#define	BL_VERSION_MINOR						1
#define	BL_VERSION_REVISION					1


#define	EXT_VERSION_INFO()					((EXT_VERSION_MAJOR<<16)|(EXT_VERSION_MINOR<<8)|(EXT_VERSION_REVISION))


#define	BL_VERSION_INFO()						((BL_VERSION_MAJOR<<16)|(BL_VERSION_MINOR<<8)|(BL_VERSION_REVISION))


#define	EXT_VERSION_INTEGER()					((EXT_VERSION_MAJOR)<<16 | (EXT_VERSION_MINOR)<<8 | (EXT_VERSION_REVISION))


#define	BL_VERSION_TOKEN						EXT_VERSION(BL_VERSION_MAJOR, BL_VERSION_MINOR, BL_VERSION_REVISION)
#define	EXT_VERSION_TOKEN					EXT_VERSION(EXT_VERSION_MAJOR, EXT_VERSION_MINOR, EXT_VERSION_REVISION)


/* only call EXT_STRINGFY, can't call EXT_TOSTRING; otherwise return string of 'EXT_VERSION_TOKEN' */
#define	EXT_VERSION_STRING		\
			EXT_STRINGIFY(EXT_VERSION_TOKEN)	
#define	BL_VERSION_STRING		\
			EXT_STRINGIFY(BL_VERSION_TOKEN)	


#define	BL_SYSTEM_NAME			"500767 Bootloader"
#define	EXT_SYSTEM_NAME			"MuxLab 500767 SDI over IP"

//EXT_NEW_LINE EXT_NEW_LINE 

#define	BUILD_DATE_TIME			__DATE__" "__TIME__

#define	EXT_SYSTEM_STRING(sysName, verInfo) 		\
		EXT_NEW_LINE"" sysName" (" \
		"Version: " verInfo "; " \
		"Built: " BUILD_DATE_TIME ")"EXT_NEW_LINE


#define	EXT_OS_NAME		EXT_SYSTEM_STRING(EXT_SYSTEM_NAME, EXT_VERSION_STRING)

#define	EXT_BL_NAME		EXT_SYSTEM_STRING(BL_SYSTEM_NAME, BL_VERSION_STRING)


#define	EXT_TASK_CONSOLE				"console"
#define	EXT_TASK_MAC					"mac"	/* GMAC controller */

#define	EXT_TASK_HTTP					"httpd"
#define	EXT_TASK_HTTP_CLIENT			"hcd"
#define	EXT_TASK_SCHEDULE				"sched"	/* schedule task before HTTP Client */

#define	EXT_TASK_NAME					"poll"

#define	EXT_TASK_UDP_CMD_NAME		"udpd"

#define	EXT_TASK_RS232					"rs232"
#define	EXT_TASK_RESET					"reset"

/* following are not used */
#define	EXT_TASK_TELNET				"telnetd"
#define	EXT_TASK_SYS_CTRL				"sysd"

/** Debug level: ALL messages*/
#define	EXT_DBG_LEVEL_ALL				0x00
#define	EXT_DBG_LEVEL_INFO			0x01
#define	EXT_DBG_LEVEL_WARN			0x02
#define	EXT_DBG_LEVEL_ERROR			0x03
#define	EXT_DBG_LEVEL_FATAL			0x04


#define	EXT_DBG_MASK_LEVEL			0x04
#define	EXT_DBG_LEVEL_OFF			LWIP_DBG_LEVEL_ALL


#define	EXT_DBG_ON					0x80U
#define	EXT_DBG_OFF					0x00U


#define	EXT_DBG_TRACE					0x40U
#define	EXT_DBG_STATE					0x20U
#define	EXT_DBG_FRESH					0x10U
#define	EXT_DBG_HALT					0x08U


/* level used */
#define	EXT_DBG_TYPES_ON				EXT_DBG_ON
#define	EXT_DBG_MIN_LEVEL			EXT_DBG_LEVEL_ALL



#define	EXT_MDNS_CLIENT_DEBUG				EXT_DBG_OFF
#define	EXT_NMOS_DEBUG						EXT_DBG_OFF

#ifdef	ARM
#define	EXT_HTTPD_DEBUG						EXT_DBG_OFF
#define	EXT_HTTPD_DATA_DEBUG					EXT_DBG_OFF
#define	EXT_HTTPC_DEBUG						EXT_DBG_OFF
#else
#define	EXT_HTTPD_DEBUG						EXT_DBG_OFF
#define	EXT_HTTPD_DATA_DEBUG					EXT_DBG_OFF
#define	EXT_HTTPC_DEBUG						EXT_DBG_OFF
#endif



/*
* ESC (27, 0x1b) charactor is '\e' or '\x1b'
*/

#define ANSI_COLOR_RED				"\x1b[31m"	/* ESC[31m : red */
#define ANSI_COLOR_GREEN			"\x1b[32m"
#define ANSI_COLOR_YELLOW			"\x1b[33m"
#define ANSI_COLOR_BLUE			"\x1b[34m"
#define ANSI_COLOR_MAGENTA		"\x1b[35m"
#define ANSI_COLOR_CYAN			"\x1b[36m"
#define ANSI_COLOR_RESET			"\x1b[0m"	/* for all colors, other than red, this must be used. April,15,2018. JL*/


#define	ERROR_TEXT_BEGIN			"\t\e[31m ERR:"
#define	ERROR_TEXT_END			"\e[0m"


#define	WARN_TEXT_BEGIN			""ANSI_COLOR_MAGENTA"WARN:"

#define	INFO_TEXT_BEGIN			""ANSI_COLOR_BLUE"INFO:"


#ifndef __EXT_RELEASE__
	#define	EXT_PRINTF(x)						{printf x ;}
	
//	#define	EXT_DEBUGF(fmt, args...)	{printf("[%s-%u] DEBUG: " fmt EXT_NEW_LINE, __FILE__, __LINE__, ## args);}
	#define	EXT_DEBUGF(debug, message)		do { \
                               if ( \
                                   ((debug) & EXT_DBG_ON) && \
                                   ((debug) & EXT_DBG_TYPES_ON) && \
                                   ((int16_t)((debug) & EXT_DBG_MASK_LEVEL) >= EXT_DBG_MIN_LEVEL)) { \
                                 _TRACE_OUT(message);printf(EXT_NEW_LINE); \
                                 if ((debug) & EXT_DBG_HALT) { \
                                   while(1); \
                                 } \
                               } \
                             } while(0)

                             
	#define	EXT_INFOF(message)		{printf(ANSI_COLOR_CYAN "%s:[%s-%u]:", sysTaskName(), __FILE__, __LINE__);EXT_PRINTF(message);printf((ANSI_COLOR_RESET EXT_NEW_LINE));}
	
	#define	EXT_ERRORF(message)		{printf(ERROR_TEXT_BEGIN "%s: ERROR:[%s-%u]:", sysTaskName(), __FILE__, __LINE__);EXT_PRINTF(message); printf((ERROR_TEXT_END  EXT_NEW_LINE));}

//	#define	EXT_ASSERT(x)				{printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); while(1);}
	#define	EXT_ASSERT(msg, x)			{if((x)==0) {printf(ERROR_TEXT_BEGIN"%s: ASSERT: [%s-%u]:",  sysTaskName(), __FILE__, __LINE__ );printf msg ;printf((ERROR_TEXT_END EXT_NEW_LINE)); while(0){};}}
	#define	EXT_ABORT(fmt, args... )		printf("%s: ABORT in [" __FILE__ "-%u]:" fmt EXT_NEW_LINE, sysTaskName(), __LINE__, ##args );while(1){}
#else
	#define	EXT_PRINTF(x)						{;}

	#define	EXT_DEBUGF(debug, message)		{}

	#define	EXT_INFOF(message)				{printf message ;printf(EXT_NEW_LINE);}

	#define	EXT_ERRORF(message)				{printf message ; printf(EXT_NEW_LINE);}
	
//	#define	EXT_ASSERT(x)				{while (1);}
	#define	EXT_ASSERT(msg, x)				{}
	#define	EXT_ABORT(fmt, args... )		{}
#endif

#define	_TRACE_OUT(message)	\
			{EXT_PRINTF(("%s: [%s-%u.%s()]: ",  sysTaskName(), __FILE__, __LINE__, __FUNCTION__) );EXT_PRINTF(message); }

#define	TRACE()						_TRACE_OUT((EXT_NEW_LINE) )




/* when RELEASE build, only omit the output  */
#define	EXT_DBG_ERRORF(message, expression, handler) do { if (!(expression)) { \
		_TRACE_OUT(message); handler;}} while(0)


#define EXT_ARRAYSIZE(x)		(sizeof(x)/sizeof((x)[0]))


#define CFG_MAKEU32(a,b,c,d) (((int)((a) & 0xff) << 24) | \
                               ((int)((b) & 0xff) << 16) | \
                               ((int)((c) & 0xff) << 8)  | \
                                (int)((d) & 0xff))


#define	CFG_SET_FLAGS(flags, value)	\
		flags |= (value) 

#define	CFG_CLEAR_FLAGS(flags, value)	\
		flags &= ~((value) ) 


#define	EXT_DHCP_IS_ENABLE(runCfg)		\
				( (runCfg)->netMode != EXT_FALSE )

#define	EXT_CFG_SET_DHCP(runCfg, value)	\
				{(runCfg)->netMode = value;}

/*
				{ if(value==0) {CFG_CLEAR_FLAGS((runCfg)->netMode,(EXT_IP_CFG_DHCP_ENABLE));} \
				else{CFG_SET_FLAGS((runCfg)->netMode, (EXT_IP_CFG_DHCP_ENABLE));} }
*/


/* add at the header of list */
#define ADD_ELEMENT(header, element)	\
	if (header == NULL){					\
		header  = element;				\
		element->next   = NULL;			\
	}									\
	else	{								\
		element->next   = header;		\
		header = element;				\
	}


/* add at the tail of list */
#define APPEND_ELEMENT(header, element, type)	\
	if ((header) == NULL){					\
		(header)  = (element);				\
		(element)->next   = NULL;			\
	}									\
	else	{ type *cur = (header), *_next = (header)->next; \
		while(_next){cur = _next; _next = cur->next;};		\
		cur->next = (element);				\
	}



#define REMOVE_ELEMENT(header, element, type)	\
{type **cp, *c1; cp = &header; \
	while ((*cp) != NULL){  c1 = *cp; \
		if (c1 == element ) \
		{ *cp = element->next;} \
		else	{ cp = &c1->next;} \
	}; }


#define		STR_BOOL_VALUE(bValue)	\
			(((bValue)==0)?"NO":"YES")



#define	IS_STRING_NULL( str)	\
	(((str)==NULL)||(!strlen((str))) )


#define	IS_STRING_EQUAL( str1, str2)	\
	( !strcasecmp((str1), (str2)))


/* use option of -fpack-struct of GCC command options in Rules.mak. 05.07,2018 */

//#define	EXT_PACK_SET(bytes)		#pragma		pack(bytes)

//#define	EXT_PACK_RESET()			#pragma		pack()


//EXT_PACK_SET(1);
#pragma		pack(1)

typedef	enum _boot_mode
{
	BOOT_MODE_BOOTLOADER = 1,
	BOOT_MODE_RTOS,
	BOOT_MODE_TEST
}boot_mode;



typedef	enum
{
	EXT_CFG_MAIN = 0,		/* main configuration in NVRAM */
	EXT_CFG_BACKUP		/* backup configuration in NVRAM */
}EXT_CFG_TYPE;


typedef	struct
{
	unsigned char		major;
	unsigned char		minor;
	unsigned char		revision;
}EXT_FM_VERSION;


typedef	struct
{/* must as 'unsigned', otherwise it output FFFFFFFa1 when print as char */
	unsigned char		address[EXT_MAC_ADDRESS_LENGTH];
}EXT_MAC_ADDRESS;


#define	MAC_ADDR_IS_BOARDCAST(macAddr)		\
	( (macAddr)->address[0]==0xFF && (macAddr)->address[1]==0xFF && (macAddr)->address[4]==0xFF && (macAddr)->address[5]==0xFF  )

#define	MAC_ADDR_IS_EQUAL(macAdd1, macAdd2 )	\
	( (macAdd1)->address[0]==(macAdd2)->address[0] && \
		(macAdd1)->address[1]==(macAdd2)->address[1] && \
		(macAdd1)->address[2]==(macAdd2)->address[2] && \
		(macAdd1)->address[3]==(macAdd2)->address[3] && \
		(macAdd1)->address[4]==(macAdd2)->address[4] && \
		(macAdd1)->address[5]==(macAdd2)->address[5]  )

#define	MAC_ADDR_IS_NULL(macAddr)		\
	( (macAddr)->address[0]==0 && (macAddr)->address[1]==0 && (macAddr)->address[4]==0 && (macAddr)->address[5]==0  )


#if 0//BYTE_ORDER == BIG_ENDIAN
#define _PP_HTONL(x) (x)
#else /* BYTE_ORDER != BIG_ENDIAN */
#define _PP_HTONL(x) ((((x) & 0x000000ffUL) << 24) | \
                     (((x) & 0x0000ff00UL) <<  8) | \
                     (((x) & 0x00ff0000UL) >>  8) | \
                     (((x) & 0xff000000UL) >> 24))
#endif /* BYTE_ORDER == BIG_ENDIAN */

#define IP_ADDR_IS_MULTICAST( addr)  \
		((addr & _PP_HTONL(0xf0000000UL)) == _PP_HTONL(0xe0000000UL))


#define	INVALIDATE_VALUE_U8						0xFF
#define	INVALIDATE_VALUE_U16						0xFFFF
#define	INVALIDATE_VALUE_U32						0xFFFFFFFF

#define	EXT_INVALIDATE_STRING_TYPE				0xFFFF



#define	FIELD_INVALIDATE_U8( field) 					((field) = INVALIDATE_VALUE_U8)
#define	FIELD_INVALIDATE_U16(field) 					((field) = INVALIDATE_VALUE_U16)
#define	FIELD_INVALIDATE_U32(field)					((field) = INVALIDATE_VALUE_U32)

#define	FIELD_IS_CHANGED_U8(field)					((field) != INVALIDATE_VALUE_U8)
#define	FIELD_IS_CHANGED_U16(field)				((field) != INVALIDATE_VALUE_U16)
#define	FIELD_IS_CHANGED_U32(field)				((field) != INVALIDATE_VALUE_U32)




#define	EXT_UUID_STR_LENGTH		36
/* UUID string  8-4-4-4-12 */

/* 128 bit UUID */
typedef	struct
{
	unsigned char		uuid[16];
}EXT_UUID_T;

typedef	struct
{
	unsigned int		seconds;
	unsigned int		nanoSeconds;
}MuxNmosVersion; /* version of this entity */


typedef	struct
{
	EXT_UUID_T						uuid;
	MuxNmosVersion					version;

}MuxNmosID;

typedef struct
{
	EXT_MAC_ADDRESS		mac;
	uint32_t					ip;
	uint32_t					audioIp;
	uint32_t					ancIp;
#if EXT_FPGA_AUX_ON	
	uint32_t					auxIp;
#endif

	unsigned short			vport;
	unsigned short			aport;
	unsigned short			dport;	/* anccilary data port */
#if EXT_FPGA_AUX_ON	
	unsigned short			sport;	/* anccilary strea port */
#endif
}EXT_VIDEO_CONFIG;

/* firmware update */
typedef	enum
{
	EXT_FM_TYPE_NONE = 0,		/* no update */
	EXT_FM_TYPE_RTOS,
	EXT_FM_TYPE_FPGA,	
	EXT_FM_TYPE_UNKNOWN
}EXT_FM_T;

#ifndef	ARM
#include <stdio.h>		/* FILE * */
#endif

/* write to flash */
typedef struct
{
	unsigned char		type;
//	unsigned	int		startSector;
	
	unsigned char		isWrite;

	unsigned int		size;	/*total length saved in flash */


#ifndef	ARM
	FILE				*fp;
#endif
}EXT_FM_UPDATE;


typedef	enum
{
	EXT_RS232_PARITY_NONE = 0,
	EXT_RS232_PARITY_ODD,
	EXT_RS232_PARITY_EVEN,
}EXT_RS232_PARITY;

#define	EXT_RS232_PAR_STR_NONE		"none"
#define	EXT_RS232_PAR_STR_ODD		"odd"
#define	EXT_RS232_PAR_STR_EVEN		"even"

typedef	enum
{
	EXT_RS232_CHAR_LENGTH_5 = 5,
	EXT_RS232_CHAR_LENGTH_6,
	EXT_RS232_CHAR_LENGTH_7,
	EXT_RS232_CHAR_LENGTH_8
}EXT_RS232_CHAR_LENGTH;

typedef	enum
{
	EXT_RS232_STOP_BITS_1 = 1,
	EXT_RS232_STOP_BITS_2,
	EXT_RS232_STOP_BITS_15,
}EXT_RS232_STOP_BITS;

#define	EXT_BAUDRATE_9600		9600
#define	EXT_BAUDRATE_19200		19200
#define	EXT_BAUDRATE_38400		38400
#define	EXT_BAUDRATE_57600		57600
#define	EXT_BAUDRATE_115200		115200

typedef	enum
{
	EXT_V_FRAMERATE_T_23 = 2,	/* 24/1.001 */
	EXT_V_FRAMERATE_T_24 = 3,
	EXT_V_FRAMERATE_T_25 = 5,
	EXT_V_FRAMERATE_T_29 = 6,	/* 30/1.001 */
	EXT_V_FRAMERATE_T_30 = 7,
	EXT_V_FRAMERATE_T_50 = 9,
	EXT_V_FRAMERATE_T_59 = 0xA,	/* 60/1.001 */
	EXT_V_FRAMERATE_T_60 = 0xB
}EXT_V_FRAMERATE;


#define	VIDEO_WIDTH_1920				1920
#define	VIDEO_HEIGHT_1080				1080

#define	VIDEO_WIDTH_1280				1280
#define	VIDEO_HEIGHT_720				720



typedef	enum
{/* value definitions are from FPGA */
	EXT_V_COLORSPACE_YCBCR_422		= 0,
	EXT_V_COLORSPACE_YCBCR_444 		= 1,
	EXT_V_COLORSPACE_RGB			= 2,
	EXT_V_COLORSPACE_YCBCR_420		= 3,
	EXT_V_COLORSPACE_XYZ			= 4,
	EXT_V_COLORSPACE_KEY			= 5,
	EXT_V_COLORSPACE_CL_YCBCR_422	= 8,
	EXT_V_COLORSPACE_CL_YCBCR_444	= 9,
	EXT_V_COLORSPACE_CL_YCBCR_420	= 11
}EXT_V_COLORSPACE;


typedef	enum
{
	EXT_V_DEPTH_8		= 0,	
	EXT_V_DEPTH_10	= 1,	
	EXT_V_DEPTH_12	= 2,	
	EXT_V_DEPTH_16	= 3
}EXT_V_DEPTH;

typedef	enum
{
	EXT_VIDEO_INTLC_INTERLACED		= 0,		/* interlaced image + segmented transport */
	EXT_VIDEO_INTLC_B_PROGRESSIVE	= 1,		/* progressive image + segmented transport , PsF(Progressive segmented Frame), 1080P Level B*/
	EXT_VIDEO_INTLC_A_PROGRESSIVE	= 3,		/* progressive image + progressive transport */
	EXT_VIDEO_INTLC_NONE				
}EXT_VIDEO_INTLC;


typedef	enum
{
	EXT_A_RATE_48K		= 0,
	EXT_A_RATE_44K		= 1,
	EXT_A_RATE_96K		= 8,
	EXT_A_RATE_NONE
}EXT_A_RATE;


typedef	enum
{
	EXT_A_PKT_SIZE_1MS		= 0,
	EXT_A_PKT_SIZE_125US		= 1,
	EXT_A_PKT_SIZE_NONE
}EXT_A_PKT_SIZE;


typedef	enum
{
	CMN_STR_T_RS_PARITY = 0,
	CMN_STR_T_V_COLORSPACE,

	CMN_STR_T_V_FRAME_RATE,

	CMN_STR_T_V_FPS_4_REST,

	CMN_STR_T_V_DEPTH,
	CMN_STR_T_V_IMAGE_FORMAT,
	
	CMN_STR_T_HTTP_STATES,
	CMN_STR_T_HTTP_EVENTS,

	CMN_STR_T_HC_STATES,
	CMN_STR_T_HC_EVENTS,

	CMN_STR_T_A_PKTSIZE,
	CMN_STR_T_A_RATE,

}CMN_STR_TYPE;


typedef	enum
{
	CMN_INT_T_V_DEPTH = 0,
	CMN_INT_T_V_FPS,

}CMN_INT_TYPE;



const char *extCmnStringFind(CMN_STR_TYPE strType, unsigned short type);
const short extCmnTypeFind(CMN_STR_TYPE  strType, char *str);


#define	CMN_FIND_RS_PARITY(type)		\
	extCmnStringFind(CMN_STR_T_RS_PARITY, (type) )

#define	CMN_FIND_V_COLORSPACE(type)		\
	extCmnStringFind(CMN_STR_T_V_COLORSPACE, (type) )


#define	CMN_FIND_V_FRAME_RATE(type)		\
	extCmnStringFind(CMN_STR_T_V_FRAME_RATE, (type) )

#define	CMN_FIND_V_FPS_4_REST(type)		\
	extCmnStringFind(CMN_STR_T_V_FPS_4_REST, (type) )


#define	CMN_FIND_V_FORMAT(type)		\
	extCmnStringFind(CMN_STR_T_V_IMAGE_FORMAT, (type) )



#define	CMN_FIND_HTTP_STATE(type)		\
	extCmnStringFind(CMN_STR_T_HTTP_STATES, (type) )

#define	CMN_FIND_HTTP_EVENT(type)		\
	extCmnStringFind(CMN_STR_T_HTTP_EVENTS, (type) )

#define	CMN_FIND_HC_STATE(type)		\
	extCmnStringFind(CMN_STR_T_HC_STATES, (type) )

#define	CMN_FIND_HC_EVENT(type)		\
	extCmnStringFind(CMN_STR_T_HC_EVENTS, (type) )


#define	CMN_FIND_A_PKTSIZE(type)		\
	extCmnStringFind(CMN_STR_T_A_PKTSIZE, (type) )

#define	CMN_FIND_A_RATE(type)		\
	extCmnStringFind(CMN_STR_T_A_RATE, (type) )



#define	CMN_FIND_STR_RS_PARITY(str)		\
	extCmnTypeFind(CMN_STR_T_RS_PARITY, (str) )

#define	CMN_FIND_STR_V_COLORSPACE(str)		\
	extCmnTypeFind(CMN_STR_T_V_COLORSPACE, (str) )

#define	CMN_FIND_STR_V_FRAME_RATE(str)		\
	extCmnTypeFind(CMN_STR_T_V_FRAME_RATE, (str) )

#define	CMN_FIND_STR_V_FPS_4_REST(str)		\
	extCmnTypeFind(CMN_STR_T_V_FPS_4_REST, (str) )


#define	CMN_FIND_STR_V_FORMAT(str)		\
	extCmnTypeFind(CMN_STR_T_V_IMAGE_FORMAT, (str) )


#define	CMN_FIND_STR_HTTP_STATE(str)		\
	extCmnTypeFind(CMN_STR_T_HTTP_STATES, (str) )

#define	CMN_FIND_STR_HTTP_EVENT(str)		\
	extCmnTypeFind(CMN_STR_T_HTTP_EVENTS, (str) )

#define	CMN_FIND_STR_HC_STATE(str)		\
	extCmnTypeFind(CMN_STR_T_HC_STATES, (str) )

#define	CMN_FIND_STR_HC_EVENT(str)		\
	extCmnTypeFind(CMN_STR_T_HC_EVENTS, (str) )


#define	CMN_FIND_STR_A_PKTSIZE(str)		\
	extCmnTypeFind(CMN_STR_T_A_PKTSIZE, (str) )

#define	CMN_FIND_STR_A_RATE(str)		\
	extCmnTypeFind(CMN_STR_T_A_RATE, (str) )


typedef struct
{
	const unsigned short		type;
	const char				*name;
}EXT_CONST_STR;


typedef struct
{
	const unsigned char		type;
	const unsigned char		name;
}EXT_CONST_INT;


typedef	struct
{
	int					index;
	const char			*desc;

	uint16_t				width;
	uint16_t				height;

	uint8_t				fps;

	uint8_t				interlaced;	/* refer to EXT_VIDEO_INTLC */
}MediaParam;


const uint8_t extCmnIntFindName(CMN_INT_TYPE  intType, uint8_t type);
const uint8_t extCmnIntFindType(CMN_INT_TYPE  intType, uint8_t name);

#define	CMN_INT_FIND_NAME_V_DEPTH(type)		\
	extCmnIntFindName(CMN_INT_T_V_DEPTH, (type) )

#define	CMN_INT_FIND_NAME_V_FPS(type)		\
	extCmnIntFindName(CMN_INT_T_V_FPS, (type) )



#define	CMN_INT_FIND_TYPE_V_DEPTH(name)		\
	extCmnIntFindType(CMN_INT_T_V_DEPTH, (name) )

#define	CMN_INT_FIND_TYPE_V_FPS(name)		\
	extCmnIntFindType(CMN_INT_T_V_FPS, (name) )



/** Input parameters when initializing RS232 and similar modes. */
typedef struct _MuxRS232
{
	uint32_t			baudRate;
	unsigned 	char		charLength;

	unsigned char		parityType;
	/** 1, 1.5 or 2 stop bits. */
	unsigned char		stopbits;
}MuxRs232Cfg;


/* runtime parameters which can't be saved in flash */
typedef	struct
{/* all data types are based FPGA registers */
	unsigned short		vWidth;
	unsigned short		vHeight;

	unsigned char			vFrameRate;
	unsigned char			vColorSpace;
	unsigned char			vDepth;

	unsigned char			vIsInterlaced;	/* used now. 01.15, 2019 */
	unsigned char			vIsSegmented;

	unsigned char			aSampleRate;
	unsigned char			aChannels;	/* 4/8/12/16 */
	unsigned char			aDepth;		/* 16, 24 bits, etc.  not configurable, 12.07, 2018 */
	unsigned char			aPktSize;	/* 1ms or 125ms. */

	unsigned char			rtpTypeVideo;
	unsigned char			rtpTypeAudio;
	unsigned char			rtpTypeAnc;

	unsigned char			vpid;		/* VPID_CODE */


	/* version */	
	unsigned char			version;
	unsigned char			revision;

	unsigned char			year;
	unsigned char			month;
	unsigned char			day;
	unsigned char			hour;
	unsigned char			minute;

	/* FPGA register */
	unsigned char			paramsState;
	unsigned char			sdiState;		/* connected or disconnect */

	/* UDP command from 811 */
	unsigned char			isConnect;

	unsigned char			reboot;
	unsigned char			reset;
	unsigned char			blink;

	/* http connections */
	uint32_t				connHttpCount;
	uint32_t				currentHttpConns;


}MuxRunTimeParam;

typedef	enum
{
	FPGA_PARAM_STATE_STABLE = 0,
	FPGA_PARAM_STATE_UPDATED,			/* updated from FPGA, and send to 811 */
	FPGA_PARAM_STATE_ACKNOWLEDGED,		/* acknowledged by 811 */
}FPGA_PARAM_STATE_T;


typedef	enum
{
	HC_REQ_SDP_VIDEO,
	HC_REQ_SDP_AUDIO,
	HC_REQ_SDP_ANC,
	HC_REQ_JSON,
	HC_REQ_HTML,
	HC_REQ_UNKNOWN
}HC_REQ_T;



/* event for scheduler */
typedef	struct _HttpClientReq
{
	char					type;
	
	uint32_t				ip;
	uint16_t				port;

	char					uri[64];
	struct _HttpClientReq 	*next;
}HttpClientReq;

/* definitions for security chip */
#define	SC_ROM_ID_SIZE				8
#define	SC_PAGE_SIZE				32
#define	SC_PERSONNAL_SIZE			4
#define	SC_CHALLENGE_SIZE			32

#define	SC_SECRET_SIZE				32


typedef struct
{
	unsigned char		romId[SC_ROM_ID_SIZE];	
	unsigned char		secret[SC_SECRET_SIZE];			
	unsigned char		challenge[SC_CHALLENGE_SIZE];
	unsigned char		pageData[SC_PAGE_SIZE];	/* memory page data, 32 byte */
	unsigned char		manId[2];
//	unsigned char 	personalityByte[SC_PERSONNAL_SIZE];
	
	unsigned char		readMac[SC_PAGE_SIZE]; /* MAC from chip*/
	unsigned char 	pageNum;/*page number */

	unsigned char		isAnon;	/*anonymous flag. 1--anonymous mode, 0-NOT anonymous mode, using ROM ID */

	unsigned char		isExist;	/* chip exist or not */
}SC_CTRL;


#define	IS_SECURITY_CHIP_EXIST(sc)		\
	( (sc)->isExist == EXT_TRUE )


#define	FPGA_CFG_AUTO				0
#define	FPGA_CFG_MANUAL			1
#define	FPGA_CFG_SDP				2

#define	FPGA_CFG_STR_NAME(fpgaAuto)	\
	((fpgaAuto)==FPGA_CFG_AUTO)?EXT_WEB_CFG_FIELD_FPGA_AUTO_V_AUTO:((fpgaAuto)==FPGA_CFG_MANUAL)?EXT_WEB_CFG_FIELD_FPGA_AUTO_V_MANUAL:((fpgaAuto)==FPGA_CFG_SDP)?EXT_WEB_CFG_FIELD_FPGA_AUTO_V_SDP:"None"


/* IP address, port number and state are all defined as unsigned type */
struct	_EXT_RUNTIME_CFG
{
	unsigned char			magic[EXT_MAGIC_SIZE];

	/* in order to guanratee the compatible between bootloader and RTOS, put updateInfo at the beginning of this structure */
	EXT_FM_UPDATE		firmUpdateInfo;

	/* following params are changed seldom to guanratee the compatible between bootloader and OS */
	unsigned char			isTx;
	MuxRs232Cfg			rs232Cfg;

	char					user[EXT_USER_SIZE];
	char					password[EXT_PASSWORD_SIZE];

	char					superUser[EXT_USER_SIZE];
	char					superPassword[EXT_PASSWORD_SIZE];

	unsigned char			isStoreDefaultCfg;
	

	unsigned char			isMCast;
	unsigned char			isUpdate;	/*isUpdate: enter into bootloader and wait for further commands; otherwise, enter into OS directly */
#if EXT_DIP_SWITCH_ON
	unsigned char			isDipOn;
#endif
	unsigned char			netMode;

	EXT_VIDEO_CONFIG	local;
	uint32_t				ipMask;
	uint32_t				ipGateway;
	uint32_t				ipMulticast;	/* save the backup multicast IP which is used when DIP is off. Oct.3, 2018 */

	unsigned char			isMacConfiged;

	unsigned char			isMacConfiged2;
	
	EXT_VIDEO_CONFIG	dest;	/* only for TX */
	

	/* following fields are not modified by SetParams command */
	char					name[32];	/* can be modified */
	char					model[32];
	EXT_FM_VERSION		version;

	/* following fields are not transmitted to client */
	unsigned short		httpPort;

	unsigned char			isConfigFpga;	/* whether configure FPGA when MCU startup. Just for FPGA debugging */

	MuxNmosID			nodeID;
	MuxNmosID			deviceID;

	HttpClientReq			sdpUriVideo;
	HttpClientReq			sdpUriAudio;
	HttpClientReq			sdpUriAnc;
	
	HttpClientReq			restUrl;		/* REST API */
	HttpClientReq			htmlUrl;
		
	/* in order to make old bootloader compatible with new RTOS, all new field must be added after here */
	/* add for 811. Aug.31, 2018 */
	uint32_t				ipSvr811;			/* IP of server 811 */	
	unsigned short		portSvr811;			/* port of server 811 */

	unsigned char			fpgaAuto;			/* RX auto configured itself */
	
	MuxRunTimeParam		runtime;				/* must be saved */

	unsigned char			endMagic[EXT_MAGIC_SIZE];


	char					hexData[HEX_DATA_MAX_LENGTH]; /* string, for RS232 data */


	/* following fields are only for runtime and not save in NVRAM */
	unsigned	int			debugOptions;

	unsigned	int			debugHttpClient;
	unsigned	int			debugHttp;
	unsigned	int			debugUdpCmd;
	
	unsigned int			currentTimestamp;	/* ms */
	char					currentHttpConns;	/* number of current HTTP connection */

	boot_mode			bootMode;

	unsigned char			*bufRead;	/* read buffer for Xmodem or SPI flash read when update RTOS in EFC flash */
	unsigned char			*bufWrite;	/* write buffer for SPI flash write or EFC flash write */
	unsigned short		bufLength;

	void					*data;

	void					*netif;

	SC_CTRL				*sc;
};//__attribute__((packed));

//EXT_PACK_RESET();

// #pragma		pack()

/* update size by setParams command */
#define		EXT_RUNTIME_CFG_UPDATE_SIZE		(offsetof(struct _EXT_RUNTIME_CFG, name))

#define		EXT_RUNTIME_CFG_WRITE_SIZE		(offsetof(struct _EXT_RUNTIME_CFG, debugOptions))


//#if ( sizeof(EXT_FM_VERSION) != 3 )
//	#error Data structure is packed wrong
//#endif

typedef char assertion_on_mystruct[( sizeof(EXT_FM_VERSION)==3  )*2-1 ];

#define DUMB_STATIC_ASSERT(test)	typedef char assertion_on_mystruct[( !!(test) )*2-1 ]


//DUMB_STATIC_ASSERT( sizeof(mystruct)==1024 );
//DUMB_STATIC_ASSERT( sizeof(my_other_struct)==23 );
//DUMB_STATIC_ASSERT( sizeof(minimum_size_struct) >= 23 );

//static_assert(sizeof(EXT_FM_VERSION) == 3,   "Check your assumptions");


typedef	struct _EXT_RUNTIME_CFG		EXT_RUNTIME_CFG;


typedef	char (*MuxDelayJob)(void *data);


#define	CMN_SN_PRINTF(data, size, index, ...)   \
	do{ if((size)-(index) > 0 ){\
		(index) += snprintf((data)+(index), ((size)-(index)), __VA_ARGS__);}else{EXT_DEBUGF(EXT_DBG_ON, ("index %d is out of range of size %d", (index), (size)) );}			\
		}while(0)

/* ##__VA_ARGS__*/
	

void extBcd2Ascii(unsigned char src, char *dest);
char extSysAtoInt8(const char *str, unsigned char *value);
char	extMacAddressParse(EXT_MAC_ADDRESS *macAddress, const char *macStr);

void extCfgFromFactory( EXT_RUNTIME_CFG *cfg );
void extCfgFactoryKeepMac( EXT_RUNTIME_CFG *cfg );

void extCfgInitAfterReadFromFlash(EXT_RUNTIME_CFG *runCfg);

char	*sysTaskName(void);


void	extNmosIdGenerate(MuxNmosID *nmosId, EXT_RUNTIME_CFG *runCfg);

unsigned int cmnMuxCRC32b(void *message, int len);


char extTxMulticastIP2Mac(EXT_RUNTIME_CFG *runCfg);


extern	EXT_RUNTIME_CFG			extRun;


#define	EXT_DEBUG_FLAG_IP_IN					(1<<0)
#define	EXT_DEBUG_FLAG_IP_OUT				(1<<1)
#define	EXT_DEBUG_FLAG_UDP_IN				(1<<2)
#define	EXT_DEBUG_FLAG_UDP_OUT				(1<<3)
#define	EXT_DEBUG_FLAG_TCP_IN				(1<<4)
#define	EXT_DEBUG_FLAG_TCP_OUT				(1<<5)
#define	EXT_DEBUG_FLAG_DHCP					(1<<6)


#define	EXT_DEBUG_FLAG_IGMP					(1<<8)		

/* bit31~16 for software debugging */
#define	EXT_DEBUG_FLAG_CMD					(1<<16)		/* debug IP CMD */	



#define	EXT_DEBUG_IS_ENABLE(flag)		\
				((extRun.debugOptions&(flag)) )


#define	EXT_DEBUG_PKTS_ENABLR()		\
				((extRun.debugOptions != 0) )


#define	EXT_DEBUG_SET_DISABLE(_optFlag)	\
				CFG_CLEAR_FLAGS(extRun.debugOptions, (_optFlag) )

#define	EXT_DEBUG_SET_ENABLE(_optFlag)	\
				CFG_SET_FLAGS(extRun.debugOptions, (_optFlag) )


#define	EXT_DEBUG_PKTS_IS_ENABLE()		\
				((extRun.debugOptions != 0) )

#define	EXT_DEBUG_PKTS_SET_DISABLE()	\
				(extRun.debugOptions = 0 )

#define	EXT_DEBUG_PKTS_SET_ENABLE()	\
				(extRun.debugOptions = 1 )


/* debug for UDP Command  */
#define	EXT_DEBUG_UDP_CMD_IS_ENABLE()		\
				((extRun.debugUdpCmd != 0) )

#define	EXT_DEBUG_UDP_CMD_SET_DISABLE()	\
				(extRun.debugUdpCmd = 0 )

#define	EXT_DEBUG_UDP_CMD_SET_ENABLE()	\
				(extRun.debugUdpCmd = 1 )



/* debug for HTTP server */
#define	EXT_DEBUG_HTTP_IS_ENABLE()		\
				((extRun.debugHttp != 0) )

#define	EXT_DEBUG_HTTP_SET_DISABLE()	\
				(extRun.debugHttp = 0 )

#define	EXT_DEBUG_HTTP_SET_ENABLE()	\
				(extRun.debugHttp = 1 )


/* debug for HTTP client */
#define	EXT_DEBUG_HC_IS_ENABLE()		\
				((extRun.debugHttpClient != 0) )

#define	EXT_DEBUG_HC_SET_DISABLE()	\
				(extRun.debugHttpClient = 0 )

#define	EXT_DEBUG_HC_SET_ENABLE()	\
				(extRun.debugHttpClient= 1 )





#define	EXT_IS_TX(runCfg)	\
			((runCfg)->isTx != 0 )


#define	EXT_IS_DIP_ON(runCfg) \
			((runCfg)->isDipOn != 0 )


#define	EXT_IS_MULTICAST(runCfg) \
			((runCfg)->isMCast != 0 )


#define	UUID_IS_NULL(_uuid)	\
		((_uuid)->uuid[0]==0 && (_uuid)->uuid[1]==0 && (_uuid)->uuid[2]==0 && (_uuid)->uuid[4]==0 && (_uuid)->uuid[15]==0 )


struct _EXT_CLI_CMD;

/* return TRUE, more data needed; FALSE: no mor data */
typedef char (*EXT_CMD_CALLBACK)(const struct _EXT_CLI_CMD *cmd, char *outBuffer, unsigned int bufferSzie );

/* The structure that defines command line commands.  A command line command
should be defined by declaring a const structure of this type. */
struct _EXT_CLI_CMD
{
	const char * const			name;
	const char * const			helpString;/* String that describes how to use the command.  Should start with the command itself, and end with "\r\n".  For example "help: Returns a list of all the commands\r\n". */

	/* return EXT_TRUE, continue this command again */
	const EXT_CMD_CALLBACK	callback;
	
//	int8_t					numberOfParams;
};

typedef struct _EXT_CLI_CMD		EXT_CLI_CMD_T;


/* 3 instances in bootloader, RTOS, and simulator */
extern const  EXT_CLI_CMD_T extCmds[];
extern const	char *versionString;

char cmnCmdLineProcess( const char * const cmdInput, char *outBuffer, unsigned int bufferSize );

char cmnCmdHelp(const struct _EXT_CLI_CMD *cmd,  char *outBuffer,  unsigned int bufferLen);
char cmnCmdVersion(const struct _EXT_CLI_CMD *cmd,  char *outBuffer,  unsigned int bufferLen);



char bspCfgSave( EXT_RUNTIME_CFG *cfg, EXT_CFG_TYPE cfgType );

#ifdef	ARM 
#define	FOR_U32 	"lu"
#else
#define	FOR_U32	"u"
#endif


extern	const	EXT_CONST_STR	_videoColorSpaces[];
extern	const short	videoWidthList[];
extern	const short 	videoHeightList[];

extern	const int32_t constRs232Baudrates[];
extern	const short constRs232Databits[];

extern	const	EXT_CONST_STR	_videoFormats[];


extern	const	EXT_CONST_INT	intVideoColorDepthList[];
extern	const	EXT_CONST_INT	intVideoFpsList[];


extern	const char 	audioChannelsList[];
extern	const	EXT_CONST_STR	_audioPktSizes[];
extern	const	EXT_CONST_STR	_audioRates[];


extern	const	EXT_CONST_STR	_ipcmdStringRsParities[];
extern	const	EXT_CONST_STR	_ipcmdStringRsStopbits[];


extern	const MediaParam constMediaParams[];


extern	EXT_RUNTIME_CFG tmpRuntime;


char cmnUtilsParseIp(char *strIpAddress, uint32_t  *ip);
char cmnUtilsParseInt32(char *strValue, uint32_t  *value);
char cmnUtilsParseInt16(char *strValue, uint16_t  *value);
char cmnUtilsParseInt8(char *strValue, uint8_t  *value);


void extSysClearConfig(EXT_RUNTIME_CFG *rxCfg);

char extSysCompareParams(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg);
char extSysConfigCtrl(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg);
char extSysConfigSdpClient(EXT_RUNTIME_CFG *runCfg, EXT_RUNTIME_CFG *rxCfg);


void extHttpClientMain(void *data);

#define	__HTTP_CRLF_SIZE		4

#if 1
#define		HTTP_HDR_SDP					"application/octet-stream"
#define		HTTP_HDR_SDP_EXT				"application/sdp"
#else
#define		HTTP_HDR_SDP					"application/sdp"
#endif
#define		HTTP_HDR_JSON					"application/json"
#define		HTTP_HDR_HTML					"text/html"

uint32_t	cmnHttpParseHeaderContentLength(char *headers, uint32_t headerLength);


char *getTaskName(void);


#define	CHECK_BAUDRATE(baudRate)	\
	( (baudRate) != EXT_BAUDRATE_9600 && (baudRate) != EXT_BAUDRATE_19200 && \
			(baudRate) != EXT_BAUDRATE_38400 && (baudRate) != EXT_BAUDRATE_57600 &&	\
			(baudRate) != EXT_BAUDRATE_115200 && (baudRate) != 0 )

#if 0
#define	CHECK_DATABITS(dataBits)	\
		( (dataBits) != EXT_RS232_CHAR_LENGTH_5 && (dataBits) != EXT_RS232_CHAR_LENGTH_6 &&	\
			(dataBits) != EXT_RS232_CHAR_LENGTH_7 && (dataBits) != EXT_RS232_CHAR_LENGTH_8 && 	\
			(dataBits) != 0 )
#else
#define	CHECK_DATABITS(dataBits)	\
		( (dataBits) != EXT_RS232_CHAR_LENGTH_7 && (dataBits) != EXT_RS232_CHAR_LENGTH_8 && 	\
			(dataBits) != 0 )
#endif


const MediaParam *extCmnVideoParamFind(EXT_RUNTIME_CFG *runCfg);
char extCmnVideoParamPopulate(EXT_RUNTIME_CFG *runCfg, uint8_t index);


#define	SDP_P_MEDIA_FORMAT_VIDEO				96
#define	SDP_P_MEDIA_FORMAT_AUDIO				97
#define	SDP_P_MEDIA_FORMAT_ANC					100


unsigned char rs232SendHexStr(char *str );

uint32_t sys_get_ms(void);

char *sysTimestamp(void);


#endif

