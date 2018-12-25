/**
 * \brief User Interface
 */

#include "ucBsp.h"


void ui_init(void)
{
	// Initialize LEDs
	LED_On(LED0);
//	LED_Off(LED1);
}

void ui_powerdown(void)
{
	LED_Off(LED0);
//	LED_Off(LED1);
}

void ui_wakeup(void)
{
	LED_On(LED0);
}

void ui_start_read(void)
{
//	LED_On(LED1);
}

void ui_stop_read(void)
{
//	LED_Off(LED1);
}

void ui_start_write(void)
{
//	LED_On(LED1);
}

void ui_stop_write(void)
{
//	LED_Off(LED1);
}

void ui_process(uint16_t framenumber)
{
	if (0 == framenumber) {
		LED_On(LED0);
	}
	if (1000 == framenumber) {
		LED_Off(LED0);
	}
}

void ui_loop_back_state(bool b_started)
{
	if (b_started)
	{
		LED_On(LED0);
	}
	else
	{
		LED_Off(LED0);
	}
}

void ui_com_open(uint8_t port)
{
	UNUSED(port);
//	LED_On(LED1);
}

void ui_com_close(uint8_t port)
{
	UNUSED(port);
//	LED_Off(LED1);
}

void ui_com_rx_start(void)
{
//	LED_On(LED1);
}

void ui_com_rx_stop(void)
{
//	LED_Off(LED1);
}

void ui_com_tx_start(void)
{
//	LED_On(LED1);
}

void ui_com_tx_stop(void)
{
//	LED_Off(LED1);
}

void ui_com_error(void)
{

}

void ui_com_overflow(void)
{

}

