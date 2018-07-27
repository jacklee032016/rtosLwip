/*
* $Id: lcdStateMachine.c,v 1.8 2007/09/27 17:48:29 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include "lcdDeclarations.h"

#if WITH_VERSION_STATE
code transition_t	_verinfoWriteStates[] =
{
	{
		LCD_EVENT_UART_VERSION,
		lcdUartSendVerInfo,
	},
};
#endif

code transition_t	_initStates[] =
{
#if 0
	{
		LCD_EVENT_BTN_UP,
		lookupStateBtnUp,
	},
	{
		LCD_EVENT_BTN_DOWN,
		lookupStateBtnDown,
	},
#endif	
	{
		LCD_EVENT_BTN_TIMEOUT,
		lookupStateTimeout,
	},
	{
		LCD_EVENT_UART_NOTIFY,
		initStateUartNotify,
	},
	
	{
		LCD_EVENT_UART_VERSION,
		lcdUartSendVerInfo,
	},
};

code transition_t	_idleStates[] =
{
	{
		LCD_EVENT_BTN_UP,
		idleStateBtn,
	},
	{
		LCD_EVENT_BTN_DOWN,
		idleStateBtn,
	},
	{
		LCD_EVENT_BTN_MENU,
		idleStateBtn,
	},
	{
		LCD_EVENT_BTN_MENU_NEXT,
		idleStateBtn,
	},
	{
		LCD_EVENT_BTN_TIMEOUT,
		lookupStateTimeout,
	},
	
	{
		LCD_EVENT_UART_VERSION,
		lcdUartSendVerInfo,
	},
};


code transition_t	_lookupStates[] =
{
	{
		LCD_EVENT_BTN_MENU,
		lookupStateBtnMenu,
	},
	{
		LCD_EVENT_BTN_UP,
		lookupStateBtnUp,
	},
	{
		LCD_EVENT_BTN_DOWN,
		lookupStateBtnDown,
	},
	{
		LCD_EVENT_UART_NOTIFY,
		lookupStateUartNotify,
	},
	{
		LCD_EVENT_BTN_TIMEOUT,
		lookupStateTimeout,
	},
	{
		LCD_EVENT_REFRESH_TIMEOUT,
		allStateRefreshTimeout,
	},
	
	{
		LCD_EVENT_UART_VERSION,
		lcdUartSendVerInfo,
	},
};

code transition_t	_modifyStates[] =
{
	{
		LCD_EVENT_BTN_MENU,
		modifyStateBtnMenu,
	},
	{
		LCD_EVENT_BTN_MENU_NEXT,
		modifyStateBtnMenuNext,
	},
	{
		LCD_EVENT_BTN_UP,
		modifyStateBtnUp,
	},
	{
		LCD_EVENT_BTN_DOWN,
		modifyStateBtnDown,
	},
	{
		LCD_EVENT_BTN_TIMEOUT,
		modifyStateTimeout,
	},
	{
		LCD_EVENT_REFRESH_TIMEOUT,
		allStateRefreshTimeout,
	},
	/* no handler for UART Notify, so ignore UART Msg when in NET Mode Configuration */
};

code statemachine_t	lcdStateMachine[] =
{
#if WITH_VERSION_STATE
	{
		LCD_STATE_VERSION,
		sizeof(_verinfoWriteStates)/sizeof(transition_t),
		_verinfoWriteStates,
	},
#endif	
	{
		LCD_STATE_INITING,
		sizeof(_initStates)/sizeof(transition_t),
		_initStates,
	},
	{
		LCD_STATE_IDLE,
		sizeof(_idleStates)/sizeof(transition_t),
		_idleStates,
	},
	{
		LCD_STATE_LOOKUP,
		sizeof(_lookupStates)/sizeof(transition_t),
		_lookupStates,
	},
	{
		LCD_STATE_MODIFY,
		sizeof(_modifyStates)/sizeof(transition_t),
		_modifyStates,
	},
};


INT8U _lcdStateHandle(statemachine_t code *sm)
{
	INT8U data	i;
	transition_t code *handle = sm->eventHandlers;
	
	for(i=0; i < sm->size; i++)
	{
		if(stcLcd.currentEvent == handle->event )
		{
			stcLcd.currentEvent = LCD_EVENT_NONE;
			return (handle->handle)();
		}
		
		handle++;
	}
	
	/* clear this unknown event */
	stcLcd.currentEvent = LCD_EVENT_NONE;

	return LCD_STATE_CONTINUE;
}


void	lcdStatemachineHandle(void)
{
	INT8U	data i;
	INT8U	data newState = LCD_STATE_CONTINUE;

	if(stcLcd.currentEvent == LCD_EVENT_NONE)
		return;

	for(i=0; i< sizeof(lcdStateMachine)/sizeof(statemachine_t); i++)
	{
		if( lcdStateMachine[i].state == stcLcd.currentState )
		{
			newState = _lcdStateHandle( &lcdStateMachine[i]);
			break;
		}
	}
	
	if(newState != LCD_STATE_CONTINUE && newState != stcLcd.currentState )
	{
		stcLcd.currentState = newState;
		START_TIMER(120);
	}
	
	lcdRefreshScreen();

}

