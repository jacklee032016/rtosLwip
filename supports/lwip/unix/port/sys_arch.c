/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * Wed Apr 17 16:05:29 EDT 2002 (James Roth)
 *
 *  - Fixed an unlikely sys_thread_new() race condition.
 *
 *  - Made current_thread() work with threads which where
 *    not created with sys_thread_new().  This includes
 *    the main thread and threads made with pthread_create().
 *
 *  - Catch overflows where more than SYS_MBOX_SIZE messages
 *    are waiting to be read.  The sys_mbox_post() routine
 *    will block until there is more room instead of just
 *    leaking messages.
 */
#define _GNU_SOURCE       
#include "lwip/debug.h"

#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "lwip/def.h"

#ifdef LWIP_UNIX_MACH
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

#include "lwip/sys.h"
#include "lwip/opt.h"
#include "lwip/stats.h"

#include "lwipExt.h"

static void get_monotonic_time(struct timespec *ts)
{
#ifdef LWIP_UNIX_MACH
  /* darwin impl (no CLOCK_MONOTONIC) */
  uint64_t t = mach_absolute_time();
  mach_timebase_info_data_t timebase_info = {0, 0};
  mach_timebase_info(&timebase_info);
  uint64_t nano = (t * timebase_info.numer) / (timebase_info.denom);
  uint64_t sec = nano/1000000000L;
  nano -= sec * 1000000000L;
  ts->tv_sec = sec;
  ts->tv_nsec = nano;
#else
  clock_gettime(CLOCK_MONOTONIC, ts);
#endif
}

#if !NO_SYS

static struct sys_thread *threads = NULL;
static pthread_mutex_t threads_mutex = PTHREAD_MUTEX_INITIALIZER;

struct sys_mbox_msg {
  struct sys_mbox_msg *next;
  void *msg;
};

#define SYS_MBOX_SIZE 128

struct sys_mbox {
  int first, last;
  void *msgs[SYS_MBOX_SIZE];
  struct sys_sem *not_empty;
  struct sys_sem *not_full;
  struct sys_sem *mutex;
  int wait_send;
};

struct sys_sem {
  unsigned int c;
  pthread_condattr_t condattr;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
};

struct sys_mutex {
  pthread_mutex_t mutex;
};

struct sys_thread {
  struct sys_thread *next;
  pthread_t pthread;
};

#if SYS_LIGHTWEIGHT_PROT
static pthread_mutex_t lwprot_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t lwprot_thread = (pthread_t)0xDEAD;
static int lwprot_count = 0;
#endif /* SYS_LIGHTWEIGHT_PROT */

static struct sys_sem *sys_sem_new_internal(u8_t count);
static void sys_sem_free_internal(struct sys_sem *sem);

static u32_t cond_wait(pthread_cond_t * cond, pthread_mutex_t * mutex, u32_t timeout);

/*-----------------------------------------------------------------------------------*/
/* Threads */
static struct sys_thread * 
introduce_thread(pthread_t id)
{
  struct sys_thread *thread;

  thread = (struct sys_thread *)malloc(sizeof(struct sys_thread));

  if (thread != NULL) {
    pthread_mutex_lock(&threads_mutex);
    thread->next = threads;
    thread->pthread = id;
    threads = thread;
    pthread_mutex_unlock(&threads_mutex);
  }

  return thread;
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn function, void *arg, int stacksize, int prio)
{
	int code;
	pthread_t tmp;
	struct sys_thread *st = NULL;
	LWIP_UNUSED_ARG(name);
	LWIP_UNUSED_ARG(stacksize);
	LWIP_UNUSED_ARG(prio);

	code = pthread_create(&tmp, NULL,  (void *(*)(void *)) function, arg);
	if (0 == code)
	{
		if( pthread_setname_np(tmp, name) != 0)
		{
			EXT_INFOF(("Error in set thread name '%s'", name ));
			return NULL;
		}

		st = introduce_thread(tmp);
	}

	if (NULL == st)
	{
		LWIP_DEBUGF(SYS_DEBUG, ("sys_thread_new: pthread_create %d, st = 0x%lx", code, (unsigned long)st));
		abort();
	}
	return st;
}

/*-----------------------------------------------------------------------------------*/
/* Mailbox */
err_t sys_mbox_new(struct sys_mbox **mb, int size)
{
	struct sys_mbox *mbox;
	LWIP_UNUSED_ARG(size);

	mbox = (struct sys_mbox *)malloc(sizeof(struct sys_mbox));
	if (mbox == NULL)
	{
		return ERR_MEM;
	}
	
	mbox->first = mbox->last = 0;
	mbox->not_empty = sys_sem_new_internal(0);
	mbox->not_full = sys_sem_new_internal(0);
	mbox->mutex = sys_sem_new_internal(1);
	mbox->wait_send = 0;

	SYS_STATS_INC_USED(mbox);
	*mb = mbox;

	return ERR_OK;
}

void sys_mbox_free(struct sys_mbox **mb) 
{
	if ((mb != NULL) && (*mb != SYS_MBOX_NULL))
	{
		struct sys_mbox *mbox = *mb;
		SYS_STATS_DEC(mbox.used);
		sys_arch_sem_wait(&mbox->mutex, 0);

		sys_sem_free_internal(mbox->not_empty);
		sys_sem_free_internal(mbox->not_full);
		sys_sem_free_internal(mbox->mutex);
		mbox->not_empty = mbox->not_full = mbox->mutex = NULL;
		/*  LWIP_DEBUGF("sys_mbox_free: mbox 0x%lx\n", mbox); */
		free(mbox);
	}
}

err_t sys_mbox_trypost(struct sys_mbox **mb, void *msg)
{
	u8_t first;
	struct sys_mbox *mbox;
	LWIP_ASSERT(("invalid mbox"), (mb != NULL) && (*mb != NULL));
	mbox = *mb;

	sys_arch_sem_wait(&mbox->mutex, 0);

	LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_trypost: mbox %p msg %p\n", (void *)mbox, (void *)msg));

	if ((mbox->last + 1) >= (mbox->first + SYS_MBOX_SIZE))
	{
		sys_sem_signal(&mbox->mutex);
		return ERR_MEM;
	}

	mbox->msgs[mbox->last % SYS_MBOX_SIZE] = msg;

	if (mbox->last == mbox->first)
	{
		first = 1;
	}
	else
	{
		first = 0;
	}

	mbox->last++;

	if (first)
	{
		sys_sem_signal(&mbox->not_empty);
	}

	sys_sem_signal(&mbox->mutex);

	return ERR_OK;
}

void sys_mbox_post(struct sys_mbox **mb, void *msg)
{
  u8_t first;
  struct sys_mbox *mbox;
  LWIP_ASSERT(("invalid mbox"), (mb != NULL) && (*mb != NULL));
  mbox = *mb;

  sys_arch_sem_wait(&mbox->mutex, 0);

  LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_post: mbox %p msg %p\n", (void *)mbox, (void *)msg));

  while ((mbox->last + 1) >= (mbox->first + SYS_MBOX_SIZE)) {
    mbox->wait_send++;
    sys_sem_signal(&mbox->mutex);
    sys_arch_sem_wait(&mbox->not_full, 0);
    sys_arch_sem_wait(&mbox->mutex, 0);
    mbox->wait_send--;
  }

  mbox->msgs[mbox->last % SYS_MBOX_SIZE] = msg;

  if (mbox->last == mbox->first) {
    first = 1;
  } else {
    first = 0;
  }

  mbox->last++;

  if (first) {
    sys_sem_signal(&mbox->not_empty);
  }

  sys_sem_signal(&mbox->mutex);
}

u32_t
sys_arch_mbox_tryfetch(struct sys_mbox **mb, void **msg)
{
  struct sys_mbox *mbox;
  LWIP_ASSERT(("invalid mbox"), (mb != NULL) && (*mb != NULL));
  mbox = *mb;

  sys_arch_sem_wait(&mbox->mutex, 0);

  if (mbox->first == mbox->last) {
    sys_sem_signal(&mbox->mutex);
    return SYS_MBOX_EMPTY;
  }

  if (msg != NULL) {
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_tryfetch: mbox %p msg %p\n", (void *)mbox, *msg));
    *msg = mbox->msgs[mbox->first % SYS_MBOX_SIZE];
  }
  else{
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_tryfetch: mbox %p, null msg\n", (void *)mbox));
  }

  mbox->first++;

  if (mbox->wait_send) {
    sys_sem_signal(&mbox->not_full);
  }

  sys_sem_signal(&mbox->mutex);

  return 0;
}

u32_t sys_arch_mbox_fetch(struct sys_mbox **mb, void **msg, u32_t timeout)
{
  u32_t time_needed = 0;
  struct sys_mbox *mbox;
  LWIP_ASSERT(("invalid mbox"), (mb != NULL) && (*mb != NULL));
  mbox = *mb;

  /* The mutex lock is quick so we don't bother with the timeout
     stuff here. */
  sys_arch_sem_wait(&mbox->mutex, 0);

  while (mbox->first == mbox->last) {
    sys_sem_signal(&mbox->mutex);

    /* We block while waiting for a mail to arrive in the mailbox. We
       must be prepared to timeout. */
    if (timeout != 0) {
      time_needed = sys_arch_sem_wait(&mbox->not_empty, timeout);

      if (time_needed == SYS_ARCH_TIMEOUT) {
        return SYS_ARCH_TIMEOUT;
      }
    } else {
      sys_arch_sem_wait(&mbox->not_empty, 0);
    }

    sys_arch_sem_wait(&mbox->mutex, 0);
  }

  if (msg != NULL) {
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_fetch: mbox %p msg %p\n", (void *)mbox, *msg));
    *msg = mbox->msgs[mbox->first % SYS_MBOX_SIZE];
  }
  else{
    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_fetch: mbox %p, null msg\n", (void *)mbox));
  }

  mbox->first++;

  if (mbox->wait_send) {
    sys_sem_signal(&mbox->not_full);
  }

  sys_sem_signal(&mbox->mutex);

  return time_needed;
}

/*-----------------------------------------------------------------------------------*/
/* Semaphore */
static struct sys_sem *
sys_sem_new_internal(u8_t count)
{
  struct sys_sem *sem;

  sem = (struct sys_sem *)malloc(sizeof(struct sys_sem));
  if (sem != NULL) {
    sem->c = count;
    pthread_condattr_init(&(sem->condattr));
#if !(defined(LWIP_UNIX_MACH) || (defined(LWIP_UNIX_ANDROID) && __ANDROID_API__ < 21))
    pthread_condattr_setclock(&(sem->condattr), CLOCK_MONOTONIC);
#endif
    pthread_cond_init(&(sem->cond), &(sem->condattr));
    pthread_mutex_init(&(sem->mutex), NULL);
  }
  return sem;
}

err_t
sys_sem_new(struct sys_sem **sem, u8_t count)
{
  SYS_STATS_INC_USED(sem);
  *sem = sys_sem_new_internal(count);
  if (*sem == NULL) {
    return ERR_MEM;
  }
  return ERR_OK;
}

static u32_t
cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex, u32_t timeout)
{
  struct timespec rtime1, rtime2, ts;
  int ret;

  if (timeout == 0) {
    pthread_cond_wait(cond, mutex);
    return 0;
  }

  /* Get a timestamp and add the timeout value. */
  get_monotonic_time(&rtime1);
#if defined(LWIP_UNIX_MACH) || (defined(LWIP_UNIX_ANDROID) && __ANDROID_API__ < 21)
  ts.tv_sec = timeout / 1000L;
  ts.tv_nsec = (timeout % 1000L) * 1000000L;
  ret = pthread_cond_timedwait_relative_np(cond, mutex, &ts);
#else
  ts.tv_sec = rtime1.tv_sec + timeout / 1000L;
  ts.tv_nsec = rtime1.tv_nsec + (timeout % 1000L) * 1000000L;
  if (ts.tv_nsec >= 1000000000L) {
    ts.tv_sec++;
    ts.tv_nsec -= 1000000000L;
  }

  ret = pthread_cond_timedwait(cond, mutex, &ts);
#endif
  if (ret == ETIMEDOUT) {
    return SYS_ARCH_TIMEOUT;
  }

  /* Calculate for how long we waited for the cond. */
  get_monotonic_time(&rtime2);
  ts.tv_sec = rtime2.tv_sec - rtime1.tv_sec;
  ts.tv_nsec = rtime2.tv_nsec - rtime1.tv_nsec;
  if (ts.tv_nsec < 0) {
    ts.tv_sec--;
    ts.tv_nsec += 1000000000L;
  }
  return (u32_t)(ts.tv_sec * 1000L + ts.tv_nsec / 1000000L);
}

u32_t
sys_arch_sem_wait(struct sys_sem **s, u32_t timeout)
{
  u32_t time_needed = 0;
  struct sys_sem *sem;
  LWIP_ASSERT(("invalid sem"), (s != NULL) && (*s != NULL));
  sem = *s;

  pthread_mutex_lock(&(sem->mutex));
  while (sem->c <= 0) {
    if (timeout > 0) {
      time_needed = cond_wait(&(sem->cond), &(sem->mutex), timeout);

      if (time_needed == SYS_ARCH_TIMEOUT) {
        pthread_mutex_unlock(&(sem->mutex));
        return SYS_ARCH_TIMEOUT;
      }
      /*      pthread_mutex_unlock(&(sem->mutex));
              return time_needed; */
    } else {
      cond_wait(&(sem->cond), &(sem->mutex), 0);
    }
  }
  sem->c--;
  pthread_mutex_unlock(&(sem->mutex));
  return (u32_t)time_needed;
}

void
sys_sem_signal(struct sys_sem **s)
{
  struct sys_sem *sem;
  LWIP_ASSERT(("invalid sem"), (s != NULL) && (*s != NULL));
  sem = *s;

  pthread_mutex_lock(&(sem->mutex));
  sem->c++;

  if (sem->c > 1) {
    sem->c = 1;
  }

  pthread_cond_broadcast(&(sem->cond));
  pthread_mutex_unlock(&(sem->mutex));
}

static void
sys_sem_free_internal(struct sys_sem *sem)
{
  pthread_cond_destroy(&(sem->cond));
  pthread_condattr_destroy(&(sem->condattr));
  pthread_mutex_destroy(&(sem->mutex));
  free(sem);
}

void sys_sem_free(struct sys_sem **sem)
{
	if ((sem != NULL) && (*sem != SYS_SEM_NULL))
	{
		SYS_STATS_DEC(sem.used);
		sys_sem_free_internal(*sem);
	}
}

/*-----------------------------------------------------------------------------------*/
/* Mutex */
/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex */
err_t sys_mutex_new(struct sys_mutex **mutex)
{
	struct sys_mutex *mtx;

	mtx = (struct sys_mutex *)malloc(sizeof(struct sys_mutex));
	if (mtx != NULL)
	{
		pthread_mutex_init(&(mtx->mutex), NULL);
		*mutex = mtx;
		return ERR_OK;
	}
	else
	{
		return ERR_MEM;
	}
}

/** Lock a mutex
 * @param mutex the mutex to lock */
void sys_mutex_lock(struct sys_mutex **mutex)
{
	pthread_mutex_lock(&((*mutex)->mutex));
//	pthread_mutex_lock(&mutex->mutex);
}

/** Unlock a mutex
 * @param mutex the mutex to unlock */
void sys_mutex_unlock(struct sys_mutex **mutex)
{
	pthread_mutex_unlock(&((*mutex)->mutex));
//	pthread_mutex_unlock(&mutex->mutex);
}

/** Delete a mutex
 * @param mutex the mutex to delete */
void sys_mutex_free(struct sys_mutex **mutex)
{
	pthread_mutex_destroy(&((*mutex)->mutex));
//	pthread_mutex_destroy(&mutex->mutex);
	free(*mutex);
}

#endif /* !NO_SYS */

/*-----------------------------------------------------------------------------------*/
/* Time */
u32_t sys_now(void)
{
	struct timespec ts;

	get_monotonic_time(&ts);
	return (u32_t)(ts.tv_sec * 1000L + ts.tv_nsec / 1000000L);
}

u32_t sys_jiffies(void)
{
	struct timespec ts;

	get_monotonic_time(&ts);
	return (u32_t)(ts.tv_sec * 1000000000L + ts.tv_nsec);
}

/*-----------------------------------------------------------------------------------*/
/* Init */
static int _sys_timer_init(void);

void sys_init(void)
{
	_sys_timer_init();
}

/*-----------------------------------------------------------------------------------*/
/* Critical section */
#if SYS_LIGHTWEIGHT_PROT
/** sys_prot_t sys_arch_protect(void)

This optional function does a "fast" critical region protection and returns
the previous protection level. This function is only called during very short
critical regions. An embedded system which supports ISR-based drivers might
want to implement this function by disabling interrupts. Task-based systems
might want to implement this by using a mutex or disabling tasking. This
function should support recursive calls from the same task or interrupt. In
other words, sys_arch_protect() could be called while already protected. In
that case the return value indicates that it is already protected.

sys_arch_protect() is only required if your port is supporting an operating
system.
*/
sys_prot_t
sys_arch_protect(void)
{
    /* Note that for the UNIX port, we are using a lightweight mutex, and our
     * own counter (which is locked by the mutex). The return code is not actually
     * used. */
    if (lwprot_thread != pthread_self())
    {
        /* We are locking the mutex where it has not been locked before *
        * or is being locked by another thread */
        pthread_mutex_lock(&lwprot_mutex);
        lwprot_thread = pthread_self();
        lwprot_count = 1;
    }
    else
        /* It is already locked by THIS thread */
        lwprot_count++;
    return 0;
}

/** void sys_arch_unprotect(sys_prot_t pval)

This optional function does a "fast" set of critical region protection to the
value specified by pval. See the documentation for sys_arch_protect() for
more information. This function is only required if your port is supporting
an operating system.
*/
void
sys_arch_unprotect(sys_prot_t pval)
{
    LWIP_UNUSED_ARG(pval);
    if (lwprot_thread == pthread_self())
    {
        if (--lwprot_count == 0)
        {
            lwprot_thread = (pthread_t) 0xDEAD;
            pthread_mutex_unlock(&lwprot_mutex);
        }
    }
}
#endif /* SYS_LIGHTWEIGHT_PROT */


void cmn_delay(int ms)
{
	int was_error;
	struct timeval tv;

	/* Set the timeout interval - Linux only needs to do this once */
	tv.tv_sec = ms/1000;
	tv.tv_usec = (ms%1000)*1000;
	do
	{
		errno = 0;

#if _POSIX_THREAD_SYSCALL_SOFT
		pthread_yield_np();
#endif

		was_error = select(0, NULL, NULL, NULL, &tv);

	} while ( was_error && (errno == EINTR) );

}

/*** sys_timer ***/
#define CMN_TIMESLICE						10

/* This is the maximum resolution of the timer on all platforms */
#define CMN_TIMER_RESOLUTION				10	/* Experimentally determined */

#define ROUND_RESOLUTION(X)	\
	(((X+CMN_TIMER_RESOLUTION-1)/CMN_TIMER_RESOLUTION)*CMN_TIMER_RESOLUTION)

/* used by other applications */
#define	CMN_TIMER_1_SECOND		1000

typedef enum
{
	sys_timer_id_state_waiting = 0,
	sys_timer_id_state_servicing,		/* is servicing the callback */
	sys_timer_id_state_wait_stop,	/* when it is in servicing state, someone want it stop (remove) */
}sys_timer_id_state;

typedef struct _sys_timer_id
{
#if EXT_TIMER_DEBUG
	char							name[32];
#endif
	int							interval;		/* milliseconds */

	sys_time_callback				callback;
	unsigned char					state;
	
	os_timer_type					type;

	void 						*param;
	
	int							lastAlarm;	/* ticks of last alarm, initialized as current ticks */
	
	struct _sys_timer_id			*next;

}sys_timer_id_t;

typedef	struct cmn_timer
{
	/* used in threaded timers */
	int							numOfTimers;
	sys_timer_id_t				*timers;
	
	struct sys_mutex			*mutex;
	sys_thread_t					tId;

	int							timersChanged;
	
	struct timeval					startTimeStamp;		/* The first ticks value of the application */

}cmn_timer_t;


static cmn_timer_t		_timers;

/* 1K HZ tick */
int cmn_get_ticks (void)
{
	struct timeval now;
	int		ticks;

	gettimeofday(&now, NULL);
	ticks=(now.tv_sec - _timers.startTimeStamp.tv_sec)*1000+(now.tv_usec - _timers.startTimeStamp.tv_usec)/1000;

	return(ticks);
}

static void __timer_remove(cmn_timer_t *timers, sys_timer_id_t *prev, sys_timer_id_t *t)
{
	if ( prev )
	{
		prev->next = t->next;
	}
	else
	{
		timers->timers = t->next;
	}

	-- timers->numOfTimers;
	EXT_DEBUGF(EXT_DBG_ON,  ("Removed Timer %s:%p, num_timers = %d ", t->name, t, _timers.numOfTimers) );
	free(t);
}

static void __threaded_timer_check(cmn_timer_t *timers)
{
	int now, ms;
	sys_timer_id_t	*t, *prev, *next;
	int removed;
	
	now = cmn_get_ticks( );

	sys_mutex_lock(&timers->mutex);
	for ( t = timers->timers; t; t = next )
	{
		ms = t->interval - CMN_TIMESLICE;
		next = t->next;
		
		if ( (t->lastAlarm < now) && ((now - t->lastAlarm) > ms) )
		{
			if ( (now - t->lastAlarm) < t->interval )
			{
				t->lastAlarm += t->interval;
			}
			else
			{
				t->lastAlarm = now;
			}

			timers->timersChanged = EXT_FALSE;
			t->state = sys_timer_id_state_servicing;

#if EXT_TIMER_DEBUG
			EXT_DEBUGF( EXT_DBG_ON, ("Executing timer %s(%p) ", t->name, t ));
#endif
			sys_mutex_unlock(&timers->mutex );

			t->callback(t->param);
			
			sys_mutex_lock(&timers->mutex);
			
			if ( timers->timersChanged )
			{/* Abort, list of timers has been modified by other threads */
				break;
			}
		}
	}


//	sys_mutex_lock(&timers->mutex);
	for ( prev = NULL, t = timers->timers; t; t = next )
	{
		removed = 0;
		ms = t->interval - CMN_TIMESLICE;
		next = t->next;

#if 0
			if ( ms != t->interval )
			{
				if ( ms )
				{
					t->interval = ROUND_RESOLUTION(ms);
				}
				else
				{ /* Remove the timer from the linked list */

#if EXT_TIMER_DEBUG
					EXT_DEBUGF(EXT_DBG_ON, ("Removing timer %s(%p) after it has been executed", t->name, t) );
#endif
					if ( prev )
					{
						prev->next = next;
					}
					else
					{
						timers->timers = next;
					}

					free(t);
					-- timers->numOfTimers;
					removed = 1;
				}
			}
		}
#else
		if( t->state == sys_timer_id_state_wait_stop || 
			(t->state == sys_timer_id_state_servicing && t->type != os_timer_type_reload) )
		{
			__timer_remove(timers, prev, t);
			removed = 1;
		}
		else if(t->state== sys_timer_id_state_servicing)
		{
			t->state = sys_timer_id_state_waiting;
			t->interval = ROUND_RESOLUTION(ms);
#if EXT_TIMER_DEBUG
			EXT_DEBUGF( EXT_DBG_ON, ("Reload timer %s(%p) with interval of %d", t->name, t, t->interval ));
#endif
		}
#endif
		/* Don't update prev if the timer has disappeared */
		if ( ! removed )
		{
			prev = t;
		}
	}
	
	sys_mutex_unlock(&timers->mutex);
}

static void _lwip_timer_thread(void *data)
{
	cmn_timer_t *timers = (cmn_timer_t *)data;
	
	while(1)// _timers.state != CMN_TIMER_STATE_UNINIT )
	{
//		if ( _timers.state == CMN_TIMER_STATE_RUNNING)
		{
		//	MUX_DEBUG( "wakeup to check timers...");
			__threaded_timer_check(timers);
		}
		
		cmn_delay(1);
	}
	
	return;
}


static int _sys_timer_init(void)
{
	err_t err = ERR_OK;
	memset(&_timers, 0 , sizeof(cmn_timer_t) );
	
	gettimeofday(&_timers.startTimeStamp, NULL);
	
	err = sys_mutex_new(&_timers.mutex);
	_timers.tId = sys_thread_new("SysTimer", _lwip_timer_thread, &_timers, TCPIP_THREAD_STACKSIZE, TCPIP_THREAD_PRIO);
	if(!_timers.tId || err != ERR_OK )
	{
		return (-EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

/** Create a new timer
 * @param timer pointer to the timer to create
 * @param ptimer pointer to timer callback function
 * @param type timer type
 * @param argument generic argument type
 * @return a new mutex */
err_t sys_timer_new(sys_timer_t *timer, sys_time_callback callback, os_timer_type type, void *argument)
{
	timer->callback = callback;
	timer->type = type;
	timer->arg = argument;

	return ERR_OK;
}

/** Start or restart a timer
 * @param timer the timer to start
 * @param millisec the value of the timer */
void sys_timer_start(sys_timer_t *timer, uint32_t millisec)
{
	sys_timer_id_t	*t;

	if(timer->timeId != NULL)
	{
		EXT_ERRORF(("Timer %s:%p has been started", timer->name, timer->timeId));
		return;
	}

	sys_mutex_lock(&_timers.mutex);
	t = (sys_timer_id_t *)malloc(sizeof( sys_timer_id_t));
	if ( t )
	{
//		t->interval = ROUND_RESOLUTION(millisec);
		memset(t, 0, sizeof(sys_timer_id_t));

		t->interval = millisec;
		t->callback = timer->callback;
		t->type = timer->type;
		
		t->param = timer->arg;
		t->lastAlarm = cmn_get_ticks();
#if EXT_TIMER_DEBUG
		snprintf(t->name, sizeof(t->name), "%s", timer->name );
#else
		snprintf(t->name, sizeof(t->name), "%s", "sysTimer" );
#endif
		t->next = _timers.timers;
		_timers.timers = t;
		timer->timeId = t;

		++ _timers.numOfTimers;
		
		_timers.timersChanged = EXT_TRUE;
	}
	
	EXT_DEBUGF(EXT_DBG_ON, ("Added Timer(%s(%p): %d millisecond) num_timers = %d", t->name, t, millisec, _timers.numOfTimers ) );

	sys_mutex_unlock(&_timers.mutex);
	return;
}

/** Stop a timer
 * @param timer the timer to stop */
void sys_timer_stop(sys_timer_t *timer)
{
	sys_timer_id_t *t, *prev = NULL;
	int removed = EXT_FALSE;
	
	sys_timer_id_t *id = (sys_timer_id_t *)timer->timeId;
	if(id == NULL)
	{
		return;
	}

	sys_mutex_lock(&_timers.mutex);
	/* Look for id in the linked list of timers */
	for (t = _timers.timers; t; prev=t, t = t->next )
	{
		if ( t == id )
		{
			if(t->state == sys_timer_id_state_servicing)
			{/* if this timer is working now, it must be waiting and removed(freed) in timer thread */
				t->state = sys_timer_id_state_wait_stop;
				_timers.timersChanged = EXT_TRUE;
				break;
			}

#if 0
			if(prev)
			{
				prev->next = t->next;
			}
			else
			{
				_timers.timers = t->next;
			}
			
			EXT_DEBUGF(EXT_DBG_ON,  ("Timer %s = %d num_timers = %d is removed", t->name, removed, _timers.numOfTimers ) );
			free(t);
			
			-- _timers.numOfTimers;
#else
			__timer_remove(&_timers, prev, t);
#endif
			removed = EXT_TRUE;
			_timers.timersChanged = EXT_TRUE;
			break;
		}
	}

	timer->timeId = NULL;

	sys_mutex_unlock(&_timers.mutex);
	return;
}


/** Delete a timer
 * @param timer the timer to delete */
void sys_timer_free(sys_timer_t *timer)
{
}

