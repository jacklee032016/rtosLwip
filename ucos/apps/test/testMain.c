/*
* Multi-tasks app in uCoS III for SAMEQ7X
*
*/

#include "app_cfg.h"

#include "extSys.h"
#include "ucBsp.h"

#include <os.h>
#include <lib_mem.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static OS_TCB	testAppTaskStartTcb;
static OS_TCB	testTask1Tcb;
static OS_TCB	testTask2Tcb;

static CPU_STK	testAppTaskStk[TEST_APP_TASK_START_STK_SIZE];
static CPU_STK	testTask1Stk[TEST_APP_TASK_SUB_STK_SIZE];
static CPU_STK	testTask2Stk[TEST_APP_TASK_SUB_STK_SIZE];


static OS_MUTEX		testMutex;
static OS_Q			testQ;

_CODE char *versionString = EXT_OS_NAME;

void XDMAC_Handler(void)
{
	OS_ERR   err;
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();
	OSIntEnter();                                               /* Tell the OS we're in an interrupt                    */
	CPU_CRITICAL_EXIT();

	OSIntExit();
}


/* higher priority */
static void testTaskProducer(void *pArg)
{
	OS_ERR err;
	CPU_TS ts;
	int value = 0;

	pArg = pArg;

	while(1)
	{
		OSTimeDly((OS_TICK)500, (OS_OPT)OS_OPT_TIME_DLY, (OS_ERR *)&err);

		value ++;
		/* send msg to other task by msg queue */
		OSQPost((OS_Q *)&testQ, (void *)&value, (OS_MSG_SIZE)sizeof(int), (OS_OPT)OS_OPT_POST_FIFO, (OS_ERR *)&err);
		

		/* lock to access shared resource */
		OSMutexPend((OS_MUTEX *)&testMutex, (OS_TICK)10/* 0: forever */, (OS_OPT)OS_OPT_PEND_BLOCKING, (CPU_TS *)&ts, (OS_ERR *)&err);

		/* shared resource access */

		/*unlock */
		OSMutexPost((OS_MUTEX *)&testMutex, (OS_OPT)OS_OPT_POST_NONE, (OS_ERR *)&err);
	}
	
}


static void testTaskRecv(void *pArg)
{
	OS_ERR err;
	int 	*msg;
	OS_MSG_SIZE  msgSize;
	CPU_TS ts, tsDelta;

	pArg = pArg;

	while(1)
	{
		/* wait on msg queue */
		msg = OSQPend((OS_Q *)&testQ, (OS_TICK)0, (OS_OPT)OS_OPT_PEND_BLOCKING, (OS_MSG_SIZE *)&msgSize, (CPU_TS *)&ts, (OS_ERR *)&err);
		tsDelta = OS_TS_GET() - ts;

		/* process msg */
		EXT_INFOF(("Recv '%d' after delay %d ticks", *msg, tsDelta));
		LED_TOGGLE(LED0);
		
	}
	
}


/* Function common to all tasks */
static void testAppMainTask(void *p_arg)
{
	OS_ERR err;

	/* hardware init can be in first task : printf must be used after hardware initialized */
	board_init();
	LED_On(LED0);

	printf(EXT_OS_NAME);

	CPU_Init();
	Mem_Init();

	/* Configure and enable the OS tick interrupt */
	OS_CPU_SysTickInit(sysCoreClock);

	p_arg = p_arg;

	OSTaskCreate(
		(OS_TCB *)&testTask1Tcb, 
		(CPU_CHAR *)"TestProducer", 
		(OS_TASK_PTR )testTaskProducer, 
		(void *)0,	/* arg to task's routine */
		(OS_PRIO )TEST_TASK_PRIO-4, /* lower number is high priority */
		(CPU_STK *)testTask1Stk, /* base address of stack */
		(CPU_STK_SIZE )TEST_APP_TASK_SUB_STK_SIZE /10,  /* watermark of stack */
		(CPU_STK_SIZE )TEST_APP_TASK_SUB_STK_SIZE, 
		(OS_MSG_QTY )TEST_TASK_Q_SIZE,
		(OS_TICK)0,  
		(void *)0,  
		(OS_OPT )(OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR)/* OS_OPT_NONE*/ , 
		(OS_ERR *)&err);
	if (err != OS_ERR_NONE)
	{
		EXT_ERRORF(("OSTaskCreate() failed for Err = %d",  (int)err));
	}

	OSTaskCreate(
		(OS_TCB *)&testTask2Tcb, 
		(CPU_CHAR *)"TestConsumer", 
		(OS_TASK_PTR )testTaskRecv, 
		(void *)0,	/* arg to task's routine */
		(OS_PRIO )TEST_TASK_PRIO-2, /* lower number is high priority */
		(CPU_STK *)testTask2Stk, /* base address of stack */
		(CPU_STK_SIZE )TEST_APP_TASK_SUB_STK_SIZE /10,  /* watermark of stack */
		(CPU_STK_SIZE )TEST_APP_TASK_SUB_STK_SIZE, 
		(OS_MSG_QTY )TEST_TASK_Q_SIZE,
		(OS_TICK)0,  
		(void *)0,  
		(OS_OPT )(OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR)/* OS_OPT_NONE*/ , 
		(OS_ERR *)&err);
	if (err != OS_ERR_NONE)
	{
		EXT_ERRORF(("OSTaskCreate() failed for Err = %d",  (int)err));
	}

	EXT_DEBUGF(EXT_DBG_ON, ("all tasks created") );

	while (1)
	{
		/*
		 * printf uses mutex to get terminal access, therefore
		 * must enter critical section
		 */

//		OS_ENTER_CRITICAL();
//		printf("Name: %s\n", sTaskName);
//		OS_EXIT_CRITICAL();
		printf(".");
		OSTimeDly((OS_TICK)300, (OS_OPT)OS_OPT_TIME_DLY, (OS_ERR *)&err);
#if 0
		/* Delay so other tasks may execute. */
		OSTimeDlyHMSM(
			(CPU_INT16U)0,
			(CPU_INT16U)0,
			(CPU_INT16U)0,
			(CPU_INT32U)100,
			(OS_OPT)OS_OPT_TIME_HMSM_STRICT,
			(OS_ERR *)&err
		);
#endif			
	}			/* while */
}

int main(void)
{
	OS_ERR err;

	/* disable interrupts when os starts 
	//hardware init */
	/* must be called before OSInit */
	OSInit(&err);
	if(err != OS_ERR_NONE)
	{
		EXT_ERRORF(("OS initialization failed"));
		return err;
	}

#if 1
	OSMutexCreate(
		(OS_MUTEX *)&testMutex, 
		(CPU_CHAR *)"FirstAppMutex",
		(OS_ERR *)&err);
	if (err != OS_ERR_NONE)
	{
		EXT_ERRORF(("OSMutexCreate() failed for Err = %d", (int)err));
		return err;
	}

	OSQCreate(
		(OS_Q *)&testQ,
		(CPU_CHAR *)"TestQueue",
		(OS_MSG_QTY )10,
		(OS_ERR *)&err);
	if (err != OS_ERR_NONE)
	{
		EXT_ERRORF(("OSQCreate() failed for Err = %d", (int)err));
		return err;	
	}
#endif

	OSTaskCreate(
		(OS_TCB *)&testAppTaskStartTcb, 
		(CPU_CHAR *)"Main Task", 
		(OS_TASK_PTR )testAppMainTask, 
		(void *)0,	/* arg to task's routine */
		(OS_PRIO )TEST_TASK_PRIO, /* lower number is high priority */
		(CPU_STK *)testAppTaskStk, /* base address of stack */
		(CPU_STK_SIZE )TEST_APP_TASK_START_STK_SIZE /10,  /* watermark of stack */
		(CPU_STK_SIZE )TEST_APP_TASK_START_STK_SIZE, 
		(OS_MSG_QTY )TEST_TASK_Q_SIZE,
		(OS_TICK)0,  
		(void *)0,  
		(OS_OPT )(OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR)/* OS_OPT_NONE*/ , 
		(OS_ERR *)&err);
	if (err != OS_ERR_NONE)
	{
		EXT_ERRORF(("OSTaskCreate() failed for Err = %d",  (int)err));
	}


	OSStart(&err);
	while(1)
	{
		EXT_ERRORF(("returned from OSStart()"));
	}

	return 0;
}

