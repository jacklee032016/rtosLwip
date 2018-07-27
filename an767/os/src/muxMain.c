/*
 * 
 */

#include "compact.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "sysclk.h"
#include "ioport.h"

#include "lwipMux.h"

#include "muxOs.h"

_CODE char *versionString = MUX_OS_NAME;


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

#if MUXLAB_BOARD
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
		
		printf("FreeRTOS 10.0.0: Now Iteration number is: %d"MUX_NEW_LINE, i);
		vTaskDelay(MUX_OS_MILL_SECOND(250));
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
		vTaskDelay( MUX_OS_MILL_SECOND(250));
	}
}
#endif


int main( void )
{
#if (RESET_BTN_MODE == _RESET_BTN_RESTORE_FACTORY)
	char restoringFactory = 0;
#endif

	unsigned int debugOption;
	
	bspHwInit(BOOT_MODE_RTOS);


#if (RESET_BTN_MODE == _RESET_BTN_RESTORE_FACTORY)
	restoringFactory = gpio_pin_is_low(PIO_PA30_IDX);
	if(restoringFactory)
	{
		MUX_DEBUGF(MUX_DBG_ON, ("RESET button is pressed, check it again"MUX_NEW_LINE));
		MUX_DELAY_S(RESET_BTN_DELAY_SECONDS);

		restoringFactory = gpio_pin_is_low(PIO_PA30_IDX);
		if(restoringFactory)
		{
			MUX_DEBUGF(MUX_DBG_ON, ("RESET button is pressed for about %d second"MUX_NEW_LINE, RESET_BTN_DELAY_SECONDS));
			muxCmdFactory(NULL, NULL, 0);
		}
	}

#if 0
	else
	{
//		printf("reset button is High"MUX_NEW_LINE);
	}
#endif
	bspButtonConfig(BOOT_MODE_RTOS);

#endif


#if 0
	debugOption = 0xFFFFFFFF;//MUX_DEBUG_FLAG_IP_IN| MUX_DEBUG_FLAG_UDP_IN|MUX_DEBUG_FLAG_IGMP|MUX_DEBUG_FLAG_CMD;
#else
	debugOption = 0;
#endif
	MUX_DEBUG_SET_ENABLE(debugOption);

//	printf(ANSI_COLOR_RED "Task initializing..."MUX_NEW_LINE);
#if MUXLAB_BOARD
#else
	printf(ANSI_COLOR_RED"LED Task initializing..."MUX_NEW_LINE);
	xTaskCreate(_ledTestTask, "ledTask", MUX_TASK_LED_STACK_SIZE, NULL, MUX_TASK_LED_PRIORITY, NULL);
#endif

	muxBspFpgaReload();

	muxBspNetStackInit(&muxRun);
	
	muxFpgaConfig(&muxRun);

	printf(""MUX_NEW_LINE);
	
	vMuxUartCmdConsoleStart(MUX_TASK_CONSOLE_STACK_SIZE, MUX_TASK_CONSOLE_PRIORITY);

//	printf("OS Scheduler beginning..."MUX_NEW_LINE);

	muxRs232Write((unsigned char *)"OS startup", 10);

	muxJobPeriod(&muxRun);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();
	for( ;; );


	return 0;
}


int	cmnParseGetHexIntValue(char *hexString)
{
	int value =0;
	
//	sscanf(hexString, "%x", &value);
	value = (int)strtol(hexString, NULL, 16);

	return value;
}


