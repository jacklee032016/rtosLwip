/*
* $Id: lcdStates.c,v 1.12 2007/09/27 17:48:29 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include "lcdDeclarations.h"

void update_alter_bar(void)
{
	stcLcd.flagAlertBar =!stcLcd.flagAlertBar;
	
	if(stcLcd.flagAlertBar == TRUE)
	{
		stcLcd.alertInfo = &stcLcd.services[stcLcd.currentService].info.help[0];
	}
	else
	{
		stcLcd.alertInfo = (INT8U *)&(defaultStateNames[0][0])+stcLcd.services[stcLcd.currentService].state*(LCD_DRAW_AREA_MAX_ASCII5X8+1) ;
	}

	if(stcLcd.services[stcLcd.currentService].state == LCD_SERVICE_STATE_NULL||
		IS_IP_ADDRESS() ||IS_PROTOCOL()||stcLcd.currentState == LCD_STATE_INITING )
	{
		stcLcd.alertInfo = &stcLcd.services[stcLcd.currentService].info.help[0];
	}
}

void update_status_bar(void)
{
	stcLcd.flagStatusBar =!stcLcd.flagStatusBar;
	
	if(stcLcd.flagStatusBar == TRUE)
	{
		stcLcd.statusInfo = (INT8U *)&verInfo;
	}
	else
	{
		stcLcd.statusInfo = (INT8U *)stcLcd.timeInfo;
	}

}

void __ipAddressModify(lcd_ip_address_t xdata *address, INT8U offset)
{
	INT8U data maxValue;

	if( (address->index%3) == 0)
	{
		maxValue = 2;
	}
	else if( (address->index%3) == 1)
	{
		if(address->value[address->index - 1] == '2')
		{
			maxValue = 5;
		}
		else
		{
			maxValue  = 9;
		}
	}
	else if((address->index%3) == 2 )
	{
		if((address->value[address->index -2] == '2') && 
			(address->value[address->index-1] == '5') )
		{
			maxValue = 5;
		}
		else
		{
			maxValue = 9;
		}
	}

	address->value[address->index] = address->value[address->index] + offset;

	if(address->value[address->index] < ('0' + 0 ) )
	{
		address->value[address->index] = '0'+maxValue; 
	}
	if(address->value[address->index] > ('0'+maxValue) )
	{
		address->value[address->index] = '0' ;
	}
	
}

INT8U initStateUartNotify(void)
{
#if 1
	if(stcLcd.currentService == LCD_SERVICE_NET_MODE)
		return LCD_STATE_LOOKUP;
	return LCD_STATE_CONTINUE;
#else	
	lcdUartRequestState(LCD_SERVICE_TIME);
	return LCD_STATE_LOOKUP;
#endif	
}

INT8U idleStateBtn(void)
{
	lcdUartRequestState(LCD_SERVICE_TIME);
	return LCD_STATE_LOOKUP;
}

INT8U lookupStateBtnMenu(void)
{
	if(stcLcd.currentService == LCD_SERVICE_LAN || stcLcd.currentService == LCD_SERVICE_WAN )
	{
		if(stcLcd.currentService == LCD_SERVICE_LAN)
			stcLcd.lanAddress.index = 0;
		else
		{
			if(stcLcd.services[LCD_SERVICE_NET_MODE].state != LCD_NET_MODE_STATIC)
			{
				stcLcd.alertInfo = (INT8U *)stringWanError;
				return LCD_STATE_CONTINUE;
			}
			
			stcLcd.wanAddress.index = 0;
		}

		stcLcd.services[stcLcd.currentService].state = LCD_SERVICE_STATE_ENABLE;
	}

	START_TIMER(120);
	return LCD_STATE_MODIFY;
}

INT8U lookupStateBtnUp(void)
{
	stcLcd.currentService++;
	if(stcLcd.currentService > LCD_SERVICE_NET_MODE )
	{
		stcLcd.currentService = LCD_SERVICE_WEB;
	}
	
	lcdUartRequestState(stcLcd.currentService);
	
	update_alter_bar();
	START_TIMER(120);
	return LCD_STATE_CONTINUE;
}

INT8U lookupStateBtnDown(void)
{
	stcLcd.currentService--;
	if(stcLcd.currentService < LCD_SERVICE_WEB)
	{
		stcLcd.currentService = LCD_SERVICE_NET_MODE;
	}
	
	lcdUartRequestState(stcLcd.currentService);

	update_alter_bar();
	START_TIMER(120);
	return LCD_STATE_CONTINUE;
}

INT8U lookupStateUartNotify(void)
{/* stcLcd.currentService has been update in UART RX function, so only redraw the currentService */

//	update_status_bar();
	lcdUartRequestState(LCD_SERVICE_TIME);
	return LCD_STATE_CONTINUE;
}

INT8U lookupStateTimeout(void)
{/* re-request the state of current service and redraw area with UART Notify event */
	START_TIMER(120);
	return LCD_STATE_IDLE;
}


INT8U modifyStateTimeout(void)
{/* re-request the state of current service and redraw area with UART Notify event */
	
	lcdUartRequestState(stcLcd.currentService);
//	stcLcd.services[stcLcd.currentService].state = LCD_SERVICE_STATE_DISABLE;
	return LCD_STATE_LOOKUP;
}

INT8U modifyStateBtnUp(void)
{/* update state of current service and redraw area with UART Notify event */
	lcd_ip_address_t xdata *address = NULL;

	if(IS_IP_ADDRESS() )
	{
		GET_IP_ADDRESS_PTR(address);
		if(address == NULL)
		{
			stcLcd.services[stcLcd.currentService].state = LCD_SERVICE_STATE_DISABLE;
			return LCD_STATE_LOOKUP;
		}

		__ipAddressModify(address, LCD_IP_ADDRESS_ADD);
	}
	else if(IS_PROTOCOL() )
	{
		stcLcd.services[LCD_SERVICE_NET_MODE].state ++;
		if(stcLcd.services[LCD_SERVICE_NET_MODE].state > LCD_NET_MODE_STATIC)
		{
			stcLcd.services[LCD_SERVICE_NET_MODE].state = LCD_NET_MODE_PPPOE;
		}

	}
	START_TIMER(120);

	return LCD_STATE_CONTINUE;
}

INT8U modifyStateBtnDown(void)
{/* update state of current service and redraw area with UART Notify event */
	lcd_ip_address_t xdata *address = NULL;

	if(IS_IP_ADDRESS() )
	{
		GET_IP_ADDRESS_PTR(address);

		if(address == NULL)
		{
			stcLcd.services[stcLcd.currentService].state = LCD_SERVICE_STATE_DISABLE;
			return LCD_STATE_LOOKUP;
		}

		__ipAddressModify(address, LCD_IP_ADDRESS_MINUS);
	}
	else if(IS_PROTOCOL() )
	{
		stcLcd.services[LCD_SERVICE_NET_MODE].state --;
		if(stcLcd.services[LCD_SERVICE_NET_MODE].state < LCD_NET_MODE_PPPOE)
		{
			stcLcd.services[LCD_SERVICE_NET_MODE].state = LCD_NET_MODE_STATIC;
		}

	}
	START_TIMER(120);
	
	return LCD_STATE_CONTINUE;
}


INT8U modifyStateBtnMenuNext(void)
{/* */
	lcd_ip_address_t xdata *address = NULL;

	if(IS_IP_ADDRESS())
	{
		GET_IP_ADDRESS_PTR(address);

		if(address == NULL)
		{
			stcLcd.services[stcLcd.currentService].state = LCD_SERVICE_STATE_DISABLE;
			return LCD_STATE_LOOKUP;
		}

		address->index++;
		if(address->index > LCD_IP_ADRRESS_LENGTH-1)
		{
			address->index= 0 ;//return LCD_STATE_LOOKUP;
		}

	}
	else if(IS_PROTOCOL())
	{
		modifyStateBtnUp();
	}
	START_TIMER(120);

	return LCD_STATE_CONTINUE;
}


INT8U modifyStateBtnMenu(void)
{/* update state of current service and redraw area with UART Notify event */

	if(IS_IP_ADDRESS())
	{
		lcdUartRequestUpdateIpAddress(stcLcd.currentService);
	}
	else
	{
		lcdUartRequestUpdateState(stcLcd.currentService);
	}
	stcLcd.services[stcLcd.currentService].state = LCD_SERVICE_STATE_DISABLE;

	if(stcLcd.currentService == LCD_SERVICE_LAN ||
		stcLcd.currentService == LCD_SERVICE_WAN ||
		stcLcd.currentService == LCD_SERVICE_NET_MODE )
	{
		stcLcd.alertInfo = (INT8U *)stringReboot;
	}
	else
	{
		stcLcd.alertInfo = (INT8U *)stringWaiting;
	}

	START_TIMER(120);
	return LCD_STATE_LOOKUP;
}


INT8U allStateRefreshTimeout(void)
{
	START_REFRESH_TIMER(6);	/*12*5seconds = 1 minutes */
	lcdUartRequestState(LCD_SERVICE_TIME);
	
	update_status_bar();
	update_alter_bar();

	lcdRefreshScreen();
	return LCD_STATE_CONTINUE;
}


