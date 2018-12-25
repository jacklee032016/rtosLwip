/*
 *
 */
#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__

#include "ext.h"
#include "arch/cc.h"


#ifdef SYS_ARCH_GLOBALS
#define SYS_ARCH_EXT
#else
#define SYS_ARCH_EXT extern
#endif



/*-----------------macros-----------------------------------------------------*/
#define LWIP_STK_SIZE		1024u
#define LWIP_TASK_MAX		8	

/* The user can change this priority level. 
 * It is important that there was no crossing with other levels.
 */
#define LWIP_TSK_PRIO				5 
#define LWIP_TASK_START_PRIO		LWIP_TSK_PRIO
#define LWIP_TASK_END_PRIO			LWIP_TSK_PRIO +LWIP_TASK_MAX

/* the max size of each mailbox */
#define MAX_QUEUE_ENTRIES			20	 

#define SYS_MBOX_NULL				(void *)0
#define SYS_SEM_NULL				(void *)0


#define sys_arch_mbox_tryfetch(mbox,msg) \
				sys_arch_mbox_fetch(mbox,msg,1)

/*-----------------type define------------------------------------------------*/

/** struct of LwIP mailbox */

typedef OS_SEM			sys_sem_t; // type of semiphores
typedef OS_MUTEX		sys_mutex_t; // type of mutex
typedef OS_Q			sys_mbox_t; // type of mailboxes
typedef CPU_INT08U		sys_thread_t; // type of id of the new thread

typedef CPU_INT08U		sys_prot_t;


/*-----------------global variables-------------------------------------------*/
/**
 * MEMCPY: override this if you have a faster implementation at hand than the
 * one included in your C library
 */
#ifndef MEMCPY
#define MEMCPY(dst,src,len)             Mem_Copy(dst,src,len)
#endif

#ifndef SMEMCPY
#define SMEMCPY(dst,src,len)            OS_MemCopy(dst,src,len)
#endif


#endif /* __SYS_RTXC_H__ */

