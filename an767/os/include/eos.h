
/*
* Header for OS and its applications
*/

#ifndef	__EXT_OS_H__
#define	__EXT_OS_H__

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


#define 	EXT_OS_MILL_SECOND(ms)				((ms)/portTICK_PERIOD_MS)


#define EXT_TASK_LED_PRIORITY             			(tskIDLE_PRIORITY + 2)

#define EXT_TASK_CONSOLE_PRIORITY				(tskIDLE_PRIORITY + 2)
#define EXT_TASK_ETHERNET_PRIORITY			(tskIDLE_PRIORITY + 2)


#define	EXT_TASK_LED_STACK_SIZE				(configMINIMAL_STACK_SIZE)
#define	EXT_TASK_CONSOLE_STACK_SIZE			(configMINIMAL_STACK_SIZE*5)
#define	EXT_TASK_ETHERNET_STACK_SIZE		(configMINIMAL_STACK_SIZE*10)


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
char *extBspCLIGetOutputBuffer( void );


/* commands */


/************** RTOS ************/
/* 4 hooks */
void vApplicationMallocFailedHook( void );
//void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationIdleHook( void );
void vApplicationTickHook( void );


void vMuxUartCmdConsoleStart( uint16_t usStackSize, unsigned long uxPriority );
void extBspUartConsoleConfig(void);

void vMuxConsoleOutput( const char *pcMessage );

void vMuxUartPutString( const char *pcString, unsigned short usStringLength );


/* EFC flash interfacec */
void extBspEfcFlashReadInfo(void);
void extBspEfcFlashTestWrite(void);


/************** LwIP ************/


void extNetShellInit(void);
void extNetTelnetInit(void);
void extNetHttpConnAPIThread(void *arg);

void ethernet_task(void);

void extBspNetStackInit(EXT_RUNTIME_CFG *runCfg);
char extBspRtl8305Config(void);

void extFpgaInit(void);
char  extBspFpgaReload(void);

char extFpgaReadParams(MuxRunTimeParam *mediaParams);


//void	extFpgaEnable(char	isEnable);

unsigned int  extFgpaRegisterDebug( char *data, unsigned int size);



/************** ATMEL and others ************/

void sys_init_timing(void);
uint32_t sys_get_ms(void);

void extEtherDebug(void);


#endif

