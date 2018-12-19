/*
 * 
 */

#include "compact.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "sysclk.h"
#include "ioport.h"

#include "lwipExt.h"

#include "eos.h"

_CODE char *versionString = EXT_OS_NAME;

void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}

void vApplicationIdleHook( void )
{
volatile size_t xFreeHeapSpace;

	/* This is just a trivial example of an idle hook.  It is called on each
	cycle of the idle task.  It must *NOT* attempt to block.  In this case the
	idle task just queries the amount of FreeRTOS heap that remains.  See the
	memory management section on the http://www.FreeRTOS.org web site for memory
	management options.  If there is a lot of heap memory free then the
	configTOTAL_HEAP_SIZE value in FreeRTOSConfig.h can be reduced to free up
	RAM. */
	xFreeHeapSpace = xPortGetFreeHeapSize();

	/* Remove compiler warning about xFreeHeapSpace being set but never used. */
	( void ) xFreeHeapSpace;
}

void vApplicationTickHook( void )
{
}

#if EXTLAB_BOARD
#else
static void _consoleTestTask(void *param)
{
	uint16_t i =0;
	
	param = param;
	while (1)
	{
		if(i==1000)
		{
			i=0;
		}
		
		printf("FreeRTOS 10.0.0: Now Iteration number is: %d"EXT_NEW_LINE, i);
		vTaskDelay(EXT_OS_MILL_SECOND(250));
		i++;
	}
}

static void _ledTestTask(void *param)
{
	param = param;
	while(1)
	{
//		vMuxConsoleOutput("LED");
		ioport_toggle_pin_level(LED0_GPIO);
		vTaskDelay( EXT_OS_MILL_SECOND(250));
	}
}
#endif

#define	WITH_MUX_WAIT		1

#if WITH_MUX_WAIT
static sys_sem_t		factorySem;
#else
static char isFactory = 0;
#endif

void wakeResetInIsr(void)
{
//	TRACE();
#if WITH_MUX_WAIT
	portBASE_TYPE	xTaskWoken = pdFALSE;
	
	xSemaphoreGiveFromISR(factorySem, &xTaskWoken);
#else
	isFactory = 1;
#endif
//	TRACE();
}

static void _factoryTask(void *param)
{
	param = param;

	while(1)
	{
//		TRACE();
#if WITH_MUX_WAIT
//		EXT_INFOF(("reset task is waiting....."));
		sys_arch_sem_wait(&factorySem, 0);
#else
		if(isFactory != 0)
#endif			
		{
			EXT_INFOF(("reset task is waken up"));
			extSysBlinkTimerInit(BTN_FACTORY_DURATION);
		}
	}

	EXT_ERRORF(("Reset Task error"));
}

static char _rs232ReadBuf[128];

static void _rs232Task(void *param)
{
	param = param;

	while(1)
	{
		memset(_rs232ReadBuf, 0, sizeof(_rs232ReadBuf));
		if(extRs232Read((unsigned char *)_rs232ReadBuf, sizeof(_rs232ReadBuf)) >0)
		{
			EXT_INFOF(("RS232 RX:'%s'", _rs232ReadBuf));
		}
	}

	EXT_ERRORF(("RS232 read Task error"));
}



int main( void )
{
#if (RESET_BTN_MODE == _RESET_BTN_RESTORE_FACTORY)
	char restoringFactory = 0;
#endif

	unsigned int debugOption;

#if TX_VERSION	
	bspHwInit(BOOT_MODE_RTOS, EXT_TRUE);	/* TX version */
#else
	bspHwInit(BOOT_MODE_RTOS, EXT_FALSE);	/* RX version */
#endif

#if SYS_DEBUG_STARTUP
	printf("Start BspHwInit..."EXT_NEW_LINE);
#endif

#if (RESET_BTN_MODE == _RESET_BTN_RESTORE_FACTORY)
	restoringFactory = gpio_pin_is_low(PIO_PA30_IDX);
	if(restoringFactory)
	{
		EXT_DEBUGF(EXT_DBG_ON, ("RESET button is pressed, check it again"EXT_NEW_LINE));
		EXT_DELAY_S(RESET_BTN_DELAY_SECONDS);

		restoringFactory = gpio_pin_is_low(PIO_PA30_IDX);
		if(restoringFactory)
		{
			EXT_DEBUGF(EXT_DBG_ON, ("RESET button is pressed for about %d second"EXT_NEW_LINE, RESET_BTN_DELAY_SECONDS));
			extCmdFactory(NULL, NULL, 0);
		}
	}

#if 0
	else
	{
//		printf("reset button is High"EXT_NEW_LINE);
	}
#endif
	bspButtonConfig(BOOT_MODE_RTOS, EXT_TRUE);

#endif


#if 0
	debugOption = 0xFFFFFFFF;//EXT_DEBUG_FLAG_IP_IN| EXT_DEBUG_FLAG_UDP_IN|EXT_DEBUG_FLAG_IGMP|EXT_DEBUG_FLAG_CMD;
#else
	debugOption = 0;
#endif
	EXT_DEBUG_SET_ENABLE(debugOption);

//	printf(ANSI_COLOR_RED "Task initializing..."EXT_NEW_LINE);
#if EXTLAB_BOARD
#else
	printf(ANSI_COLOR_RED"LED Task initializing..."EXT_NEW_LINE);
	xTaskCreate(_ledTestTask, "ledTask", EXT_TASK_LED_STACK_SIZE, NULL, EXT_TASK_LED_PRIORITY, NULL);
#endif

	/* Initialize timer before button configuration */
	printf("Initializing timer..."EXT_NEW_LINE);
	sys_init_timing();

#if WITH_MUX_WAIT
	err_t err;
	/* Incoming packet notification semaphore. */
	err = sys_sem_new(&factorySem, 0);
	EXT_ASSERT(("reset semaphore allocation ERROR!"), (err == ERR_OK) );
	if (err == ERR_MEM)
		return ERR_MEM;
#endif
	sys_thread_new(EXT_TASK_RESET, _factoryTask, NULL, EXT_TASK_LED_STACK_SIZE*4, EXT_TASK_LED_PRIORITY);

	sys_thread_new(EXT_TASK_RS232, _rs232Task, NULL, EXT_TASK_LED_STACK_SIZE*2, EXT_TASK_LED_PRIORITY-1);
	
	extBspFpgaReload();
	extBspNetStackInit(&extRun);

	/* console task must start finally to make the command line prompt */
	vMuxUartCmdConsoleStart(EXT_TASK_CONSOLE_STACK_SIZE, EXT_TASK_CONSOLE_PRIORITY);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();
	for( ;; );


	return 0;
}



