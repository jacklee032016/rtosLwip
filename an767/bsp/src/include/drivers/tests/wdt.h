/**
 *
 * \brief Watchdog Timer (WDT) driver for SAM.
 *
 */

#ifndef WDT_H_INCLUDED
#define WDT_H_INCLUDED

#include "compiler.h"


#define WDT_INVALID_ARGUMENT 0xFFFF

uint32_t wdt_get_timeout_value(uint32_t ul_us, uint32_t ul_sclk);
void wdt_init(Wdt *p_wdt, uint32_t ul_mode, uint16_t us_counter,
		uint16_t us_delta);
void wdt_disable(Wdt *p_wdt);
void wdt_restart(Wdt *p_wdt);
uint32_t wdt_get_status(Wdt *p_wdt);
uint32_t wdt_get_us_timeout_period(Wdt *p_wdt, uint32_t ul_sclk);

#endif

