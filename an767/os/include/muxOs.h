
/*
* Header for OS and its applications
*/

#ifndef	__MUX_OS_H__
#define	__MUX_OS_H__

#include "compact.h"

#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>


#ifdef	TEST_SDRAM
// From module: SAM SDRAMC Driver
#include <sdramc.h>

// From module: SDRAMC configuration for IS42S16100E
#include <is42s16100e.h>

// From module: Sleep manager - SAM implementation
#include <sam/sleepmgr.h>
#include <sleepmgr.h>
#endif

/* add following for flash embedded in MCU. Jack.Lee */
//#include <efc.h>
//#include <flash_efc.h>
#include <interrupt_nvic.h>


/********** build options **************/

#define	PING_USE_SOCKETS						0




#define 	MUX_OS_MILL_SECOND(ms)				((ms)/portTICK_PERIOD_MS)


#define MUX_TASK_LED_PRIORITY             			(tskIDLE_PRIORITY + 2)

#define MUX_TASK_CONSOLE_PRIORITY				(tskIDLE_PRIORITY + 2)
#define MUX_TASK_ETHERNET_PRIORITY			(tskIDLE_PRIORITY + 2)


#define	MUX_TASK_LED_STACK_SIZE				(configMINIMAL_STACK_SIZE)
#define	MUX_TASK_CONSOLE_STACK_SIZE			(configMINIMAL_STACK_SIZE*5)
#define	MUX_TASK_ETHERNET_STACK_SIZE		(configMINIMAL_STACK_SIZE*10)


/* following commands are implemented in MuxLab library, not RTOS */
#define configINCLUDE_QUERY_HEAP_COMMAND					1
#define configINCLUDE_TRACE_RELATED_CLI_COMMANDS				0




/*
 * A buffer into which command outputs can be written is declared in the
 * main command interpreter, rather than in the command console implementation,
 * to allow application that provide access to the command console via multiple
 * interfaces to share a buffer, and therefore save RAM.  Note, however, that
 * the command interpreter itself is not re-entrant, so only one command
 * console interface can be used at any one time.  For that reason, no attempt
 * is made to provide any mutual exclusion mechanism on the output buffer.
 *
 */
char *muxBspCLIGetOutputBuffer( void );


/* commands */

char muxCmdNetPing(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );
char muxCmdIgmp(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen );
char	muxCmdNetInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);
char	muxCmdMacInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);
char	muxCmdDestInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);
char	muxCmdLocalInfo(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);
char	muxCmdChangeName(const struct _MUX_CLI_CMD *cmd,  char *outBuffer, size_t bufferLen);


/************** RTOS ************/
/* 4 hooks */
void vApplicationMallocFailedHook( void );
//void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationIdleHook( void );
void vApplicationTickHook( void );


void vMuxUartCmdConsoleStart( uint16_t usStackSize, unsigned long uxPriority );
void muxBspUartConsoleConfig(void);

void vMuxConsoleOutput( const char *pcMessage );

void vMuxUartPutString( const char *pcString, unsigned short usStringLength );


/* EFC flash interfacec */
void muxBspEfcFlashReadInfo(void);
void muxBspEfcFlashTestWrite(void);


/************** LwIP ************/

void muxNetPingInit(void);
void muxNetPingSendNow(unsigned int destIp);


void muxNetShellInit(void);
void muxNetTelnetInit(void);
void muxNetHttpConnAPIThread(void *arg);

void ethernet_task(void);

void muxBspNetStackInit(MUX_RUNTIME_CFG *runCfg);
char muxBspRtl8305Config(void);

void muxFpgaInit(void);
char  muxBspFpgaReload(void);


char	muxFpgaRegisterWrite(unsigned char baseAddr, unsigned char *data, unsigned char size);
//void	muxFpgaEnable(char	isEnable);

void muxFgpaRegisterDebug(void);



/************** ATMEL and others ************/

void muxEtherDebug(void);



int	cmnParseGetHexIntValue(char *hexString);

/* output for used in OS */
extern	struct netif			guNetIf;

#define	MUX_NET_IGMP_JOIN(groupAddress)	\
			muxLwipGroupMgr(&guNetIf, (groupAddress), 1)


#define	MUX_NET_IGMP_LEAVE(groupAddress)	\
			muxLwipGroupMgr(&guNetIf, (groupAddress), 0)


void muxJobPeriod(MUX_RUNTIME_CFG *runCfg);



#endif

