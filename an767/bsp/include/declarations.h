/*
* function declarations for bsp
*/

#ifndef	__DECLARATIONS_H__
#define	__DECLARATIONS_H__


void board_init(void);

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



void rstc_reset_processor_and_peripherals_and_ext(void);
void loadApplication(uint32_t appAddress);

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

char cmdCmdDebugUdpCmd(const struct _EXT_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );

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

