/*
* function declarations for bsp
*/

#ifndef	__DECLARATIONS_H__
#define	__DECLARATIONS_H__


void board_init(void);

char	 bspCmdFactory(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen );

char cmnCmdUpdate(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );

char cmnCmdHelp(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);
char cmnCmdVersion(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);


void bspSpiFlashInit(unsigned int startSector, unsigned int startAddress, char isWrite);
int	bspSpiFlashRead(unsigned char *buf, unsigned int size);
int	bspSpiFlashWrite(unsigned char *data, unsigned int size);
int	bspSpiFlashFlush(void);


char bspCmdInternalFlash(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen);
char	bspCmdSpiFlashRead(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen );
char	bspCmdSpiFlashErase(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen );

char bspCmdSpiFlashXmodemLoad(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen);
char bspCmdSpiFlashYmodemLoad(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen);

int	bspBootUpdateFpga(MUX_RUNTIME_CFG *runCfg);

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

#ifndef __MUX_RELEASE__
#define	CONSOLE_DEBUG_MEM(buffer, size, address, msg)	\
	{printf("[%s-%u.%s()]-%s: "MUX_NEW_LINE, __FILE__, __LINE__, __FUNCTION__, (msg) );bspConsoleDumpMemory((buffer), (size), (address));}

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

char bspCfgRead( MUX_RUNTIME_CFG *cfg, MUX_CFG_TYPE cfgType);
//char bspCfgSave( MUX_RUNTIME_CFG *cfg, MUX_CFG_TYPE cfgType );

char bspHwI2cInit(void);

char muxI2CRead(unsigned char chanNo, unsigned char deviceAddress, unsigned int regAddress, unsigned char regAddressSize, unsigned char *regVal, unsigned char regSize);
char muxI2CWrite(unsigned char chanNo, unsigned char deviceAddress, unsigned int regAddress, unsigned char regAddressSize,  unsigned char *regVal, unsigned char regSize);

/* 8 bits address, and 8 bit data */
#define	FPGA_I2C_WRITE(address, val, size)		\
	muxI2CWrite(MUX_I2C_PCA9554_CS_NONE, MUX_I2C_ADDRESS_FPGA, (address), 1, (val), size)

#define	FPGA_I2C_READ(address, val, size)		\
	muxI2CRead(MUX_I2C_PCA9554_CS_NONE, MUX_I2C_ADDRESS_FPGA, (address), 1, (val), size)



void muxEepromWrite(unsigned char startAddress, unsigned char *value, unsigned int size);
void muxEepromRead(unsigned char startAddress, unsigned char *value, unsigned int size);


char muxBspSpiSelectChip(uint32_t pcs);
char muxBspSpiUnselectChip(uint32_t pcs);
char muxBspSpiReadPacket(unsigned char *data, unsigned int len);
char muxBspSpiWritePacket(const unsigned char *data, unsigned int len);


void muxBspSpiMasterTransfer(void *buf, uint32_t size);

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



void muxBspBoardInit(void);
void muxBspBIST(void );
short muxSensorGetTemperatureCelsius(void);

unsigned long  bspRandom(void);

void bspHwConsoleConfig(void);

void bspHwInit(boot_mode bMode);
void bspButtonConfig(boot_mode bMode);



void bspSpiMasterInitialize(uint32_t pcs);


char bspCmdBIST(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen );

char  bspBistSpiFlashReadDeviceID(char *outBuffer, size_t bufferSize);
char  bspBistSpiFlashReadWrite(char *outBuffer, size_t bufferSize);

char  bspBistI2cSensor(char *outBuffer, size_t bufferSize);
char  bspBistI2cEeprom(char *outBuffer, size_t bufferSzie );

char  bspBistDipSwitch(char *outBuffer, size_t bufferSize);
char  bspBistClock(char *outBuffer, size_t bufferSize);


int bspGetcXModem(void);


void muxHwRs232Init(MUX_RUNTIME_CFG *runCfg);

void muxHwRs232Config(MUX_RUNTIME_CFG *runCfg);

#ifdef	ARM
int muxRs232Write(unsigned char *data, unsigned short size);


char cmnCmdLineProcess( const char * const cmdInput, char *outBuffer, size_t bufferSize );

char	 bspCmdReboot(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen );

char bspCfgSave( MUX_RUNTIME_CFG *cfg, MUX_CFG_TYPE cfgType );
char	muxFpgaConfig(MUX_RUNTIME_CFG *runCfg);
void	muxFpgaEnable(char	isEnable);
char *muxFgpaReadVersion(void);


#endif

char muxCmdFactory(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );

extern	int 		argc;
extern	char		argv[MUX_CMD_MAX_ARGUMENTS][MUX_CMD_MAX_LENGTH];

#if LWIP_EXT_UDP_TX_PERF
char	muxCmdUdpTxPerf(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);
#endif
char cmnCmdLwipPing(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );
char cmnCmdLwipIgmp(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );
char	cmnCmdNetInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);
char	cmnCmdMacInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);
char	cmnCmdDestInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);
char	cmnCmdLocalInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);
char	muxCmdChangeName(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);

char cmdCmdDebuggable(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );

char cmnCmdParams(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferLen);
char cmnCmdTx(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );

char bspMultiAddressFromDipSwitch(void);


void muxDelayReboot(unsigned short delayMs);

void muxNetPingInit(void);
void muxNetPingSendNow(unsigned int destIp);


int	cmnParseGetHexIntValue(char *hexString);

#endif

