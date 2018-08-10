/*
 * FreeRTOS Kernel V10.0.0
 */


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* Atmel includes. */

//#include "compact.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#if defined (__GNUC__) || defined (__ICCARM__)
/* Important: put #includes here unless they are also meant for the assembler.
 */
#include <stdint.h>
#endif
#define configUSE_PREEMPTION					1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION	1
#define configUSE_QUEUE_SETS					1
#define configUSE_IDLE_HOOK						0
#define configUSE_TICK_HOOK						1
#if 0
#define configCPU_CLOCK_HZ						( SystemCoreClock ) 		/* 	( sysclk_get_cpu_hz() ) */
#else
#define configCPU_CLOCK_HZ						 (300000000UL) 
#endif
#define configTICK_RATE_HZ						( 1000 )
//#define configTICK_RATE_HZ						( 100 )
#define configMAX_PRIORITIES					( 5 )
#define configMINIMAL_STACK_SIZE				( ( unsigned short ) 130 )
#define configTOTAL_HEAP_SIZE					( ( size_t ) ( 26 * 1024 ) )
#define configMAX_TASK_NAME_LEN					( 10 )
#define configUSE_TRACE_FACILITY				1
#define configUSE_16_BIT_TICKS					0
#define configIDLE_SHOULD_YIELD					1
#define configUSE_MUTEXES						1
#define configQUEUE_REGISTRY_SIZE				8
#define configCHECK_FOR_STACK_OVERFLOW			2
#define configUSE_RECURSIVE_MUTEXES				1
#define configUSE_MALLOC_FAILED_HOOK			1
#define configUSE_APPLICATION_TASK_TAG			0
#define configUSE_COUNTING_SEMAPHORES			1

/* The full demo always has tasks to run so the tick will never be turned off.
The blinky demo will use the default tickless idle implementation to turn the
tick off. */
#define configUSE_TICKLESS_IDLE					0

/* following options are used to test. Jan.12, 2018, J.L. */
/* Run time stats gathering definitions. */
#define configGENERATE_RUN_TIME_STATS					0
/* only for test to define these 2 macros which is function pointer  */
//#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS		1
//#define portGET_RUN_TIME_COUNTER_VALUE				1


/* This demo makes use of one or more example stats formatting functions.  These
format the raw data provided by the uxTaskGetSystemState() function in to human
readable ASCII form.  See the notes in the implementation of vTaskList() within
FreeRTOS/Source/tasks.c for limitations. */
#define configUSE_STATS_FORMATTING_FUNCTIONS	1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 					0
#define configMAX_CO_ROUTINE_PRIORITIES			( 2 )

/* Software timer definitions. */
#define configUSE_TIMERS						1
#define configTIMER_TASK_PRIORITY				( configMAX_PRIORITIES - 2 )
#define configTIMER_QUEUE_LENGTH				5
#define configTIMER_TASK_STACK_DEPTH			( configMINIMAL_STACK_SIZE * 2 )

/* add 07.19,2018 JL*/
//#define configSUPPORT_STATIC_ALLOCATION			1


/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet				1
#define INCLUDE_uxTaskPriorityGet				1
#define INCLUDE_vTaskDelete						1
#define INCLUDE_vTaskCleanUpResources			1
#define INCLUDE_vTaskSuspend					1
#define INCLUDE_vTaskDelayUntil					1
#define INCLUDE_vTaskDelay						1
#define INCLUDE_eTaskGetState					1


/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
	/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
	#define configPRIO_BITS       				__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		3        /*  */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0x0f

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	10

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ) __asm volatile( "NOP" ); }
#define INCLUDE_MODULE_TEST 0

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names.  Defining the constants removes the need to edit the vector 
table in order to install the RTOS interrupt handlers. */

//#define xPortPendSVHandler PendSV_Handler
//#define vPortSVCHandler SVC_Handler
//#define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */

