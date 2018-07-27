/*
* $Id: lcdStateMachine.h,v 1.6 2007/09/26 19:47:32 lizhijie Exp $
*/
#ifndef	__LCD_STATE_MACHINE_H__
#define	__LCD_STATE_MACHINE_H__

#define		WITH_VERSION_STATE			0

typedef	enum
{
	LCD_EVENT_NONE 			= 0,
	LCD_EVENT_BTN_MENU,				/* pressed last more than 2 second */
	LCD_EVENT_BTN_MENU_NEXT,			/* pressed no more than 1 second which is used for next Digit for IP Address */
	LCD_EVENT_BTN_UP,
	LCD_EVENT_BTN_DOWN,
	LCD_EVENT_BTN_TIMEOUT,
	LCD_EVENT_UART_NOTIFY,
	LCD_EVENT_UART_VERSION,
	LCD_EVENT_REFRESH_TIMEOUT,
}LCD_EVENT_T;

typedef	enum
{
#if WITH_VERSION_STATE
	LCD_STATE_VERSION,		/* version info, 2007.09.26 */
#endif	
	LCD_STATE_INITING,			/*waiting msg from UART */
	LCD_STATE_IDLE,			/* something just like screen protection */	
	LCD_STATE_LOOKUP,
	LCD_STATE_MODIFY,
	LCD_STATE_CONTINUE
}LCD_STATE_T;

#if WITH_VERSION_STATE
#define	LCD_STATE_FIRST			LCD_STATE_VERSION
#else
#define	LCD_STATE_FIRST			LCD_STATE_INITING
#endif

struct _transition_t
{
	LCD_EVENT_T		event;
	
	INT8U (code *handle)(void);
};

typedef	struct _transition_t	transition_t;

struct	_transition_table_t
{
	INT8U				state;
	
	INT8U				size;
	transition_t	code		*eventHandlers;
	
//	void	(code *enter_handle)(INT8U devIndex);
};

typedef	struct _transition_table_t	statemachine_t;


INT8U lookupStateBtnMenu(void);
INT8U lookupStateBtnUp(void);
INT8U lookupStateBtnDown(void);
INT8U lookupStateUartNotify(void);
INT8U lookupStateTimeout(void);


INT8U modifyStateTimeout(void);
INT8U modifyStateBtnMenu(void);
INT8U modifyStateBtnMenuNext(void);
INT8U modifyStateBtnUp(void);
INT8U modifyStateBtnDown(void);

INT8U initStateUartNotify(void);
INT8U idleStateBtn(void);

INT8U allStateRefreshTimeout(void);

INT8U lcdUartSendVerInfo(void);

#endif

