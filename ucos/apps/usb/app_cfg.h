/*
* application definition
*/

#ifndef __APP_CFG_H__
#define __APP_CFG_H__


#define	TASK_CONSOLE_EN		DEF_ENABLED
#define	TASK_MAC				DEF_ENABLED

/*
*/


#define  MAIN_TASK_STK_SIZE		256u
#define  MAIN_TASK_PRIO				10u
#define  MAIN_TASK_Q_SIZE			100u


#define	USB_TASK_START_STK_SIZE		256
#define	USB_TASK_2_STK_SIZE			USB_TASK_START_STK_SIZE



#define	CONSOLE_TASK_STK_SIZE		256u
#define	CONSOLE_TASK_PRIO				10u
#define	CONSOLE_TASK_Q_SIZE			10u


#define	MAC_TASK_STK_SIZE				1024u
#define	MAC_TASK_PRIO					5u
#define	MAC_TASK_Q_SIZE				20u


#endif

