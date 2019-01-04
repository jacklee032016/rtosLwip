
#ifndef	__LWIP_EXT_H__
#define	__LWIP_EXT_H__


#include "lwip/opt.h"  /* lwip/arch.h --> arch/cc.h --> #include "extSysParams.h" */

#include <string.h>

#include "lwip/def.h"
#include "lwip/ip_addr.h"
#include "lwip/ip6_addr.h"
#include "lwip/netif.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/udp.h"
#include "lwip/raw.h"
#include "lwip/snmp.h"
#include "lwip/igmp.h"
#include "lwip/etharp.h"
#include "lwip/stats.h"
#include "lwip/sys.h"
#include "lwip/ip.h"
#if ENABLE_LOOPBACK
#if LWIP_NETIF_LOOPBACK_MULTITHREADING
#include "lwip/tcpip.h"
#endif /* LWIP_NETIF_LOOPBACK_MULTITHREADING */
#endif /* ENABLE_LOOPBACK */

#include "netif/ethernet.h"

#if LWIP_AUTOIP
#include "lwip/autoip.h"
#endif /* LWIP_AUTOIP */
#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif /* LWIP_DHCP */
#if LWIP_IPV6_DHCP6
#include "lwip/dhcp6.h"
#endif /* LWIP_IPV6_DHCP6 */
#if LWIP_IPV6_MLD
#include "lwip/mld6.h"
#endif /* LWIP_IPV6_MLD */
#if LWIP_IPV6
#include "lwip/nd6.h"
#endif

#if LWIP_EXT_MQTT_CLIENT
#include "lwip/apps/mqtt.h"
#endif

#include "lwip/ip_addr.h"
#include "lwip/inet.h"

#include "compact.h"
#include "extMems.h"

#include "extNmos.h"
#include "http.h"


//#include "lwip/apps/httpApp.h"

#ifndef	TRACE
	#define	TRACE()						printf(__FILE__", line %u"EXT_NEW_LINE, __LINE__)
#endif


#define	_CHAR_SEPERATE			"&"
#define	_CHAR_EQUAL				"="



void extLwipHttpSvrInit(void *data);

void extHttpSvrMain(void *data);

void extLwipNetStatusCallback(struct netif *netif);

void extLwipStartNic(EXT_RUNTIME_CFG *runCfg);
char extLwipStartup(EXT_RUNTIME_CFG *runCfg);

void extNetRawTelnetInit(EXT_RUNTIME_CFG *runCfg);

#if LWIP_MDNS_RESPONDER
void extLwipMdsnDestroy(struct netif *netif);
#endif

char	 extLwipGroupMgr(EXT_RUNTIME_CFG *runCfg, unsigned int gAddress, unsigned char isAdd);

char	 extIgmpGroupMgr(EXT_RUNTIME_CFG *runCfg, unsigned char isAdd);

void extLwipDhcpDebug(	ip4_addr_t *ip, ip4_addr_t *mask, ip4_addr_t *gw);
void extLwipEthHdrDebugPrint(void *ethHdr, const char *prompt);

void extLwipArpDebugPrint(struct pbuf *p, const char *prompt);
void extLwipIp4DebugPrint(struct pbuf *p, const char *prompt);
void extLwipIgmpDebugPrint(const ip4_addr_t *groupaddr, const char isJoin);

char extNetMulticastIP4Mac(uint32_t	*ipAddress, EXT_MAC_ADDRESS *macAddress);


void extVideoConfigCopy(EXT_VIDEO_CONFIG *dest, EXT_VIDEO_CONFIG *src);


char	extNetIsGroupAddress(uint32_t	*ipAddress);




#define	NETIF_HWADDR_OFFSET()		\
			(offsetof(struct netif, hwaddr_len))


#define	EXT_LWIP_INT_TO_IP(ipAddr,  intAddr)	\
				((ipAddr)->addr = intAddr)

#define	EXT_LWIP_IPADD_TO_STR(ipAddr)		\
			inet_ntoa((*(struct in_addr *)(ipAddr)) )


#define	EXT_LWIP_DEBUG	0


#define	EXT_LWIP_DEBUG_NETIF(_netif)	\
			EXT_DEBUGF(EXT_DBG_ON, ("netif :%p; hwaddr_len:%d, offset:%d:%d:%d"EXT_NEW_LINE,  \
				(_netif), (_netif)->hwaddr_len, (offsetof(struct netif, rs_count)), (offsetof(struct netif, mtu)), NETIF_HWADDR_OFFSET() ) )


#define	EXT_LWIP_DEBUG_PBUF(_pbuf)	\
			EXT_DEBUGF(EXT_DBG_ON, ("pbuf :%p, size:%d, next:%p; payload:%p, total:%d, len:%d, type:%d, ref:%d"EXT_NEW_LINE,  \
				(_pbuf), sizeof(struct pbuf), _pbuf->next, _pbuf->payload, _pbuf->tot_len, _pbuf->len, _pbuf->type, _pbuf->ref ) )



#define	MDNS_SERVICE_NAME_SIZE		128

typedef	enum
{
	MDNS_CLIENT_S_INIT = 0,
	MDNS_CLIENT_S_PARSE_PTR,
	MDNS_CLIENT_S_PARSE_SRV,
	MDNS_CLIENT_S_PARSE_TXT,

	MDNS_CLIENT_S_REQUEST_API,
		
	MDNS_CLIENT_S_UNKNOWN,

}MDNS_CLIENT_STATE;

#include "lwip/apps/mdns.h"
#include "lwip/apps/mdns_priv.h"


typedef	struct 
{
	u16_t				txId;

	char					domainName[MDNS_DOMAIN_MAXLEN];

	char					service[MDNS_SERVICE_NAME_SIZE];
	u16_t				qType;

	unsigned	char			rdata[MDNS_DOMAIN_MAXLEN];

	unsigned char			state;
	
	struct udp_pcb		*udpPcb;
	struct mdns_packet	*pkt;

	MuxNmosNode			node;
	
	EXT_RUNTIME_CFG	*runCfg;
}mdns_client_t;


char mdnsClientInit(mdns_client_t *mdnsClient, EXT_RUNTIME_CFG *runCfg);
char mdnsClientParseAnswer(mdns_client_t *mdnsClient, struct mdns_packet *pkt);

struct ptptime_t
{
	s32_t	tv_sec;
	s32_t	tv_nsec;
};


char extCmdConnect(EXT_RUNTIME_CFG  *runCfg, unsigned char isStart);

char extUdpRxPerfStart(void);


char *extLwipIpAddress(EXT_RUNTIME_CFG *runCfg);

//void bspConsoleDumpMemory(uint8_t *buffer, uint32_t size, uint32_t address);


/* clear updateInfo, so it will not be wrotten to flash */
#define	CANCEL_UPDATE(runCfg) \
	do{		memset(&(runCfg)->firmUpdateInfo, 0, sizeof(EXT_FM_UPDATE) );	\
	}while(0)


#if LWIP_EXT_UDP_TX_PERF
char extUdpTxPerfTask(void);

char extUdpTxPerfStart(unsigned int svrIpAddress);
#endif

#ifdef	ARM
void gmacBMCastEnable(unsigned char enable);
void gmacEnableWakeOnLan(unsigned int ipAddr);
#endif


char cmnCmdLwipStats(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);

#if LWIP_EXT_MQTT_CLIENT
void mqttClientConnect(unsigned int svrIp);
#endif

struct MAC_STATS
{
	uint32_t	isrCount;

	uint32_t	isrRecvCount;

	
	uint32_t	txPackets;
	uint32_t	txFailed;

	uint32_t	rxPackets;
	
	uint32_t	rxErrOverrun;	/* RX buffer overrun */
	uint32_t	rxErrOwnership;
	uint32_t	rxErrOOM;

	uint32_t	rxErrFrame;	/* not frame we are interested */
};


#define	EXT_TIMER_DEBUG					0

#if  EXT_TIMER_DEBUG
	#define	EXT_SYS_ARCH_DEBUG			EXT_DBG_ON
#else
	#define	EXT_SYS_ARCH_DEBUG			EXT_DBG_OFF
#endif


typedef enum
{
	os_timer_type_once = 0,
	os_timer_type_reload	
}os_timer_type;

typedef	void (*sys_time_callback)(void *arg);


typedef struct
{
#if EXT_TIMER_DEBUG
	char						name[32];
#endif
	void						*timeId;
	sys_time_callback			callback;
	os_timer_type				type;

	void						*arg;
	
} sys_timer_t;


err_t sys_timer_new(sys_timer_t *timer, sys_time_callback callback, os_timer_type type, void *argument);
void sys_timer_start(sys_timer_t *timer, uint32_t millisec);
void sys_timer_stop(sys_timer_t *timer);
void sys_timer_free(sys_timer_t *timer);

#if LWIP_EXT_PTP
void extPtpCmdStatus(char *buf, unsigned int len);
bool extPtpCmdDate(char *buf, unsigned int len);
#endif


void extMediaInit( void *arg);
void extMediaPollDevice(EXT_RUNTIME_CFG *runCfg);


err_t extHttpClientNewRequest(HttpClientReq *req);

#define	WITH_EXT_PBUF_LOCK		1

#if WITH_EXT_PBUF_LOCK
extern sys_mutex_t _ipBufLock;

#define	LOCK_IP_BUF()			sys_mutex_lock(&_ipBufLock)
#define	UNLOCK_IP_BUF()		sys_mutex_unlock(&_ipBufLock)
#else
#define	LOCK_IP_BUF()			

#define	UNLOCK_IP_BUF()		

#endif


uint32_t rs232StartRead(uint16_t timeout, char *buf);
void rs232StartRx(void);
void rs232StopRx(void);

int unescape_uri(char *uri);

#endif

