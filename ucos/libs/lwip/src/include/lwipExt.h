
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

#include "ext.h"

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
			EXT_DEBUGF(EXT_DBG_ON, ("pbuf :%p, size:%d, next:%p; payload:%p, total:%d, len:%d, type:%d, ref:%d",  \
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

err_t macEthernetInit(struct netif *_netif);


char	 bspCmdFactory(const struct _EXT_CLI_CMD *cmd, char *outBuffer,  unsigned int bufferLen );

char cmnCmdUpdate(const struct _EXT_CLI_CMD *cmd,  char *outBuffer,  unsigned int bufferLen );


void bspSpiFlashInit(unsigned int startSector, unsigned int startAddress, char isWrite);
int	bspSpiFlashRead(unsigned char *buf, unsigned int size);
int	bspSpiFlashWrite(unsigned char *data, unsigned int size);
int	bspSpiFlashFlush(void);


char bspCmdInternalFlash(const struct _EXT_CLI_CMD *cmd, char *outBuffer,  unsigned int bufferLen);
char	bspCmdSpiFlashRead(const struct _EXT_CLI_CMD *cmd, char *outBuffer,  unsigned int bufferLen );
char	bspCmdSpiFlashErase(const struct _EXT_CLI_CMD *cmd, char *outBuffer,  unsigned int bufferLen );

char bspCmdSpiFlashXmodemLoad(const struct _EXT_CLI_CMD *cmd, char *outBuffer,  unsigned int bufferLen);
char bspCmdSpiFlashYmodemLoad(const struct _EXT_CLI_CMD *cmd, char *outBuffer,  unsigned int bufferLen);

int	bspBootUpdateFpga(EXT_RUNTIME_CFG *runCfg);

extern uint32_t SystemCoreClock; /* System Clock Frequency (Core Clock) */

/**
 * @brief Setup the microcontroller system.
 * Initialize the System and update the SystemCoreClock variable.
 */
void SystemInit(void);

/**
 * @brief Updates the SystemCoreClock with current core Clock
 * retrieved from cpu registers.
 */
void SystemCoreClockUpdate(void);

/**
 * Initialize flash.
 */
void system_init_flash(uint32_t dw_clk);


typedef void (*console_rx_handler_t)(uint8_t received_char);

int bspConsolePutChar(char c);
void bspConsolePutString(const char* str);
char bspConsoleGetChar(void);

bool bspConsoleIsRxReady(void);
bool console_is_tx_empty(void);
void console_set_rx_handler(console_rx_handler_t handler);
void console_enable_rx_interrupt(void);
void console_disable_rx_interrupt(void);
void bspConsoleDumpFrame(uint8_t *frame, uint32_t size);
void bspConsoleDumpMemory(uint8_t *buffer, uint32_t size, uint32_t address);

#ifndef __EXT_RELEASE__
#define	CONSOLE_DEBUG_MEM(buffer, size, address, msg)	\
	{printf("[%s-%u.%s()]-%s: "EXT_NEW_LINE, __FILE__, __LINE__, __FUNCTION__, (msg) );bspConsoleDumpMemory((buffer), (size), (address));}

#else
#define	CONSOLE_DEBUG_MEM(buffer, size, address, msg)	

#endif


void bspConsoleEcho(uint8_t c);
void console_clear_screen(void);


#define	CONSOLE_RESET_CURSOR()		bspConsolePutString("\033[0;0f")
#define	CONSOLE_CLEAR_SCREEN()		bspConsolePutString("\033[2J\033[0;0f")


void bspConsoleReset(const char *name);


char bspConsoleGetCharTimeout(char *c, unsigned int timeoutMs);

int cmdSerialUpload(char *outBuffer, size_t bufferLen );

char efcFlashRead(uint32_t pageNo, unsigned char *data, uint32_t size);
char efcFlashProgram(uint32_t pageNo, const uint8_t* buffer, uint32_t size);

char	efcFlashInit(void);
void efcFlashUpdateGpnvm(void);

char bspCfgRead( EXT_RUNTIME_CFG *cfg, EXT_CFG_TYPE cfgType);
//char bspCfgSave( EXT_RUNTIME_CFG *cfg, EXT_CFG_TYPE cfgType );

char bspHwI2cInit(void);

char extI2CRead(unsigned char chanNo, unsigned char deviceAddress, unsigned int regAddress, unsigned char regAddressSize, unsigned char *regVal, unsigned char regSize);
char extI2CWrite(unsigned char chanNo, unsigned char deviceAddress, unsigned int regAddress, unsigned char regAddressSize,  unsigned char *regVal, unsigned char regSize);

/* 8 bits address, and 8 bit data */
#define	FPGA_I2C_WRITE(address, val, size)		\
	extI2CWrite(EXT_I2C_PCA9554_CS_NONE, EXT_I2C_ADDRESS_FPGA, (address), 1, (val), size)

#define	FPGA_I2C_READ(address, val, size)		\
	extI2CRead(EXT_I2C_PCA9554_CS_NONE, EXT_I2C_ADDRESS_FPGA, (address), 1, (val), size)



void extEepromWrite(unsigned char startAddress, unsigned char *value, unsigned int size);
void extEepromRead(unsigned char startAddress, unsigned char *value, unsigned int size);


char extBspSpiSelectChip(uint32_t pcs);
char extBspSpiUnselectChip(uint32_t pcs);
char extBspSpiReadPacket(unsigned char *data, unsigned int len);
char extBspSpiWritePacket(const unsigned char *data, unsigned int len);


void extBspSpiMasterTransfer(void *buf, uint32_t size);

/* bsp hw interfaces of SPI flash, 5 functions */
void bspHwSpiFlashInit(void);
void bspHwSpiFlashReset(void);
char bspHwSpiFlashReadID(unsigned char *outBuffer, size_t bufferSize);
char bspHwSpiFlashEraseSector(unsigned int  sectorNo);
char bspHwSpiFlashRead(unsigned int address, unsigned char *data, unsigned int size);
char bspHwSpiFlashWritePage(unsigned int pageNo, unsigned char *data, unsigned int length);



void  bspHwClockInit(void);
void  bspHwClockSetDac( uint32_t pll_dac);
void  bspHwClockSwitchClk( unsigned char  sw);



void extBspBoardInit(void);
void extBspBIST(void );
short extSensorGetTemperatureCelsius(void);

unsigned long  bspRandom(void);

void bspHwConsoleConfig(void);

void bspHwInit(boot_mode bMode, uint8_t isTx);
//void bspButtonConfig(boot_mode bMode);
void bspButtonConfig(boot_mode bMode, char isRiseEdge);



void bspSpiMasterInitialize(uint32_t pcs);


char bspCmdBIST(const struct _EXT_CLI_CMD *cmd, char *outBuffer, size_t bufferLen );

char  bspBistSpiFlashReadDeviceID(char *outBuffer, size_t bufferSize);
char  bspBistSpiFlashReadWrite(char *outBuffer, size_t bufferSize);

char  bspBistI2cSensor(char *outBuffer, size_t bufferSize);
char  bspBistI2cEeprom(char *outBuffer, size_t bufferSzie );

char  bspBistDipSwitch(char *outBuffer, size_t bufferSize);
char  bspBistClock(char *outBuffer, size_t bufferSize);


int bspGetcXModem(void);


void extHwRs232Init(EXT_RUNTIME_CFG *runCfg);

void extHwRs232Config(EXT_RUNTIME_CFG *runCfg);

#ifdef	ARM
int extRs232Write(unsigned char *data, unsigned short size);
int extRs232Read(unsigned char *data, unsigned short size);


char	 bspCmdReboot(const struct _EXT_CLI_CMD *cmd, char *outBuffer, size_t bufferLen );

char extFpgaConfigParams(EXT_RUNTIME_CFG *runCfg);

char extFpgaReadParams(EXT_RUNTIME_CFG *runCfg);

char	extFpgaConfig(EXT_RUNTIME_CFG *runCfg);
void	extFpgaEnable(char	isEnable);
void	extFpgaBlinkPowerLED(char	isEnable);

char *extFgpaReadVersion(void);


#endif

void extSysBlinkTimerInit(unsigned short milliseconds);

unsigned char extMediaPostEvent(unsigned char eventType, void *ctx);;

char extCmdFactory(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );

extern	int 		argc;
extern	char		argv[EXT_CMD_MAX_ARGUMENTS][EXT_CMD_MAX_LENGTH];

#if LWIP_EXT_UDP_TX_PERF
char	extCmdUdpTxPerf(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);
#endif

char cmnCmdTime(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, unsigned int bufferLen);

char cmnCmdLwipPing(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );
char cmnCmdLwipIgmp(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );
char	cmnCmdNetInfo(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);
char	cmnCmdPtpInfo(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);

char	cmnCmdHttpClient(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);


char	cmnCmdMacInfo(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);
char	cmnCmdDestInfo(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);
char	cmnCmdLocalInfo(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);
char	extCmdChangeName(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);

char cmdCmdDebuggable(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );

char cmdCmdDebugHttp(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );
char cmdCmdDebugHttpClient(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );

char *extCmnIp4addr_ntoa(uint32_t *ipp);
void extCmnNewDestIpEffective(EXT_RUNTIME_CFG *runCfg, unsigned int newIp);


char cmnCmdParams(const struct _EXT_CLI_CMD *cmd, char *outBuffer, size_t bufferLen);
char cmnCmdTx(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );

char bspMultiAddressFromDipSwitch(void);


void extDelayReboot(unsigned short delayMs);

void extNetPingInit(void);
void extNetPingSendNow(unsigned int destIp);


int	cmnParseGetHexIntValue(char *hexString);

#ifdef	ARM
void extFpgaTimerJob(MuxRunTimeParam  *mediaParams);;

void wakeResetInIsr(void);

void extJobPeriod(EXT_RUNTIME_CFG *runCfg);
#endif

#endif

