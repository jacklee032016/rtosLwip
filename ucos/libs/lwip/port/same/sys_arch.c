/*
 *
 */

#define SYS_ARCH_GLOBALS

#include "os.h"
/* lwIP includes. */
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"

#include "ext.h"


#include "include/arch/sys_arch.h"

#include <string.h>

#define LWIP_ARCH_TICK_PER_MS       1000/OS_CFG_TICK_RATE_HZ


static OS_MEM			StackMem;

const void * const	pvNullPointer = (mem_ptr_t*)0xffffffff;

//#pragma pack(4)
static CPU_STK			_lwipTaskStacks[LWIP_TASK_MAX*LWIP_STK_SIZE];
//#pragma pack(4)
static CPU_INT08U			_lwipTaskPriopityStasks[LWIP_TASK_MAX];
static OS_TCB				_lwipTaskTCBs[LWIP_TASK_MAX];


err_t	sys_mbox_new( sys_mbox_t *mbox, int size)
{
	OS_ERR       ucErr;

	OSQCreate(mbox, "LWIP queue", size, &ucErr);
	EXT_ASSERT(("OSQCreate "), ucErr == OS_ERR_NONE );
	if( ucErr == OS_ERR_NONE)
	{ 
		return 0; 
	}
	return -1;
}

/*
  Deallocates a mailbox. If there are messages still present in the
  mailbox when the mailbox is deallocated, it is an indication of a
  programming error in lwIP and the developer should be notified.
*/
void sys_mbox_free(sys_mbox_t * mbox)
{
	OS_ERR     ucErr;
	EXT_ASSERT(("sys_mbox_free "), mbox != SYS_MBOX_NULL );      

	OSQFlush(mbox,& ucErr);

	OSQDel(mbox, OS_OPT_DEL_ALWAYS, &ucErr);
	EXT_ASSERT(("OSQDel"), ucErr == OS_ERR_NONE );
}

/*
 *   Posts the "msg" to the mailbox.
 */
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
	OS_ERR     ucErr;
	CPU_INT08U  i=0;
	
	if( msg == NULL )
		msg = (void*)&pvNullPointer;
	
	/* try 10 times */
	while(i<10)
	{
		OSQPost(mbox, msg,0,OS_OPT_POST_ALL,&ucErr);
		if(ucErr == OS_ERR_NONE)
			break;

		i++;
		OSTimeDly(5,OS_OPT_TIME_DLY,&ucErr);
	}
	
	EXT_ASSERT(("sys_mbox_post error!"), i !=10 );  
}

/* Try to post the "msg" to the mailbox. */
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
	OS_ERR     ucErr;
	if(msg == NULL )
		msg = (void*)&pvNullPointer;  
	
	OSQPost(mbox, msg,0,OS_OPT_POST_ALL,&ucErr);    
	if(ucErr != OS_ERR_NONE)
	{
		return ERR_MEM;
	}
	
	return ERR_OK;
}

/*
  Blocks the thread until a message arrives in the mailbox, but does
  not block the thread longer than "timeout" milliseconds (similar to
  the sys_arch_sem_wait() function). The "msg" argument is a result
  parameter that is set by the function (i.e., by doing "*msg =
  ptr"). The "msg" parameter maybe NULL to indicate that the message
  should be dropped.

  The return values are the same as for the sys_arch_sem_wait() function:
  Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
  timeout.

  Note that a function with a similar name, sys_mbox_fetch(), is
  implemented by lwIP. 
*/
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{ 
	OS_ERR	ucErr;
	OS_MSG_SIZE   msg_size;
	CPU_TS        ucos_timeout;  
	CPU_TS        in_timeout = timeout/LWIP_ARCH_TICK_PER_MS;
	if(timeout && in_timeout == 0)
		in_timeout = 1;
	*msg  = OSQPend (mbox,in_timeout,OS_OPT_PEND_BLOCKING,&msg_size, &ucos_timeout,&ucErr);

	if ( ucErr == OS_ERR_TIMEOUT ) 
		ucos_timeout = SYS_ARCH_TIMEOUT;  
	
	return ucos_timeout; 
}

/** 
  * Check if an mbox is valid/allocated: 
  * @param sys_mbox_t *mbox pointer mail box
  * @return 1 for valid, 0 for invalid 
  */ 
int sys_mbox_valid(sys_mbox_t *mbox)
{
	if(mbox->NamePtr)  
		return (strcmp(mbox->NamePtr,"?Q"))? 1:0;
	else
		return 0;
}

/** 
  * Set an mbox invalid so that sys_mbox_valid returns 0 
  */      
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
	if(sys_mbox_valid(mbox))
		sys_mbox_free(mbox);
}
/*
 *  Creates and returns a new semaphore. The "count" argument specifies
 *  the initial state of the semaphore. TBD finish and test
 */

err_t sys_sem_new(sys_sem_t * sem, u8_t count)
{  
	OS_ERR	ucErr;

	OSSemCreate (sem,"LWIP Sem",count,&ucErr);

	if(ucErr != OS_ERR_NONE )
	{
		EXT_ASSERT(("OSSemCreate"), ucErr == OS_ERR_NONE );
		return -1;
	}
	
	return 0;
}

/*
  Blocks the thread while waiting for the semaphore to be
  signaled. If the "timeout" argument is non-zero, the thread should
  only be blocked for the specified time (measured in
  milliseconds).

  If the timeout argument is non-zero, the return value is the number of
  milliseconds spent waiting for the semaphore to be signaled. If the
  semaphore wasn't signaled within the specified time, the return value is
  SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
  (i.e., it was already signaled), the function may return zero.

  Notice that lwIP implements a function with a similar name,
  sys_sem_wait(), that uses the sys_arch_sem_wait() function.
*/
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{ 
	OS_ERR	ucErr;
	CPU_TS        ucos_timeout;
	CPU_TS        in_timeout = timeout/LWIP_ARCH_TICK_PER_MS;
	if(timeout && in_timeout == 0)
		in_timeout = 1;  
	
	OSSemPend (sem,in_timeout,OS_OPT_PEND_BLOCKING,&ucos_timeout,&ucErr);
	/*  only when timeout! */
	if(ucErr == OS_ERR_TIMEOUT)
		ucos_timeout = SYS_ARCH_TIMEOUT;	
	
	return ucos_timeout;
}

/*
 * Signals a semaphore
 */
void sys_sem_signal(sys_sem_t *sem)
{
	OS_ERR	err;  
	OSSemPost(sem,OS_OPT_POST_ALL,&err);
	EXT_ASSERT(("OSSemPost"), err == OS_ERR_NONE );  
}

/*
 * Deallocates a semaphore
 */
void sys_sem_free(sys_sem_t *sem)
{
	OS_ERR     err;
	OSSemDel(sem, OS_OPT_DEL_ALWAYS, &err );
	EXT_ASSERT(("OSSemDel"), err == OS_ERR_NONE );
}

int sys_sem_valid(sys_sem_t *sem)
{
	if(sem->NamePtr)
		return (strcmp(sem->NamePtr,"?SEM"))? 1:0;
	else
		return 0;
}

/** Set a semaphore invalid so that sys_sem_valid returns 0 */
void sys_sem_set_invalid(sys_sem_t *sem)
{
	if(sys_sem_valid(sem))
	{
		sys_sem_free(sem);
	}
}


/*
 * Initialize sys arch
 */
void sys_init(void)
{
	OS_ERR ucErr;
	memset(_lwipTaskPriopityStasks, 0, sizeof(_lwipTaskPriopityStasks));

	EXT_DEBUGF(EXT_DBG_ON, ("LWIP System initialization for uC/OS"));
	/* init mem used by sys_mbox_t, use ucosII functions */
	OSMemCreate(&StackMem, "LWIP TASK STK",(void*)_lwipTaskStacks, LWIP_TASK_MAX,  LWIP_STK_SIZE*sizeof(CPU_STK), &ucErr);
	EXT_ASSERT(("sys_init: failed OSMemCreate STK"), ucErr == OS_ERR_NONE );
}


/*
  Starts a new thread with priority "prio" that will begin its execution in the
  function "thread()". The "arg" argument will be passed as an argument to the
  thread() function. The id of the new thread is returned. Both the id and
  the priority are system dependent.
*/
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
	CPU_INT08U  ubPrio = LWIP_TASK_START_PRIO;
	OS_ERR      ucErr;
	arg = arg;
	int i; 

	if(prio)
	{
		ubPrio +=(prio-1);
		
		for(i=0; i<LWIP_TASK_MAX; ++i)
		{
			if(_lwipTaskPriopityStasks[i] == ubPrio)
			{
				break;
			}
		}
			
		if(i == LWIP_TASK_MAX)
		{
			for(i=0; i<LWIP_TASK_MAX; ++i)
			{
				if(_lwipTaskPriopityStasks[i]==0)
				{
					_lwipTaskPriopityStasks[i] = ubPrio;
					break;
				}
			}

			if(i == LWIP_TASK_MAX)
			{
				EXT_ASSERT(("sys_thread_new: there is no space for priority"), 0 );
				return (-1);
			}        
		}
		else
			prio = 0;
	}
	
	/* Search for a suitable priority */     
	if(!prio)
	{
		ubPrio = LWIP_TASK_START_PRIO;
		while(ubPrio < (LWIP_TASK_START_PRIO+LWIP_TASK_MAX))
		{
			for(i=0; i<LWIP_TASK_MAX; ++i)
			{
				if(_lwipTaskPriopityStasks[i] == ubPrio)
				{
					++ubPrio;
					break;
				}
			}

			if(i == LWIP_TASK_MAX)
			{
				break;
			}
		}

		if(ubPrio < (LWIP_TASK_START_PRIO+LWIP_TASK_MAX))
		{
			for(i=0; i<LWIP_TASK_MAX; ++i)
			{
				if(_lwipTaskPriopityStasks[i]==0)
				{
					_lwipTaskPriopityStasks[i] = ubPrio;
					break;
				}
			}
		}

		if(ubPrio >= (LWIP_TASK_START_PRIO+LWIP_TASK_MAX) || i == LWIP_TASK_MAX)
		{
			EXT_ASSERT( ("sys_thread_new: there is no free priority"), 0 );
			return (-1);
		}
	}
	
	if(stacksize > LWIP_STK_SIZE || !stacksize)   
		stacksize = LWIP_STK_SIZE;
	
	/* get Stack from pool */
	CPU_STK * task_stk = OSMemGet( &StackMem, &ucErr );
	if(ucErr != OS_ERR_NONE)
	{
		EXT_ASSERT(("sys_thread_new: impossible to get a stack"), 0 );
		return (-1);
	} 
	
	int tsk_prio = ubPrio-LWIP_TASK_START_PRIO;

	EXT_DEBUGF(EXT_DBG_ON, ("Task '%s': priority:%d; stack size:%d",name, ubPrio, stacksize));
	OSTaskCreate(&_lwipTaskTCBs[tsk_prio],
		(CPU_CHAR  *)name,
		(OS_TASK_PTR)thread, 
		(void      *)0,
		(OS_PRIO    )5,
		(CPU_STK   *)&task_stk[0],
		(CPU_STK_SIZE )stacksize/10,
		(CPU_STK_SIZE )stacksize,
		(OS_MSG_QTY )0,
		(OS_TICK    )0,
		(void      *)0,
		(OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR    *)&ucErr
	);
	EXT_ASSERT(("Task failed, error : 0x%x", ucErr), ucErr == OS_ERR_NONE);

	return ubPrio;
}


/**
 * Sleep for some ms. Timeouts are NOT processed while sleeping.
 * @param ms number of milliseconds to sleep
 */
void sys_msleep(u32_t ms)
{
	OS_ERR      ucErr;  
	OSTimeDly(ms, OS_OPT_TIME_DLY,&ucErr);  
}


u32_t sys_now(void)
{
	OS_TICK tick;
	OS_ERR err;
	tick = OSTimeGet(&err);
	EXT_ASSERT(("OSTimeGet failed"), (err == OS_ERR_NONE));

	return (u32_t)tick;
}


