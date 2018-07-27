/**
 * Watchdog Timer (WDT) driver for SAM.
 *
 */

#include "wdt.h"


/**
 * \defgroup sam_drivers_wdt_group Watchdog Timer (WDT)
 *
 * Driver for the WDT (Watchdog Timer). This driver provides access to the main
 * features of the WDT controller.
 * The Watchdog Timer can be used to prevent system lock-up if the software
 * becomes trapped in a deadlock. It features a 12-bit down counter that allows
 * a watchdog period of up to 16 seconds (slow clock at 32.768 kHz). It can
 * generate a general reset or a processor reset only. In addition, it can be
 * stopped while the processor is in debug mode or idle mode.
 *
 * @{
 */

#define WDT_KEY_PASSWORD  0xA5000000
#define WDT_SLCK_DIV      128
#define WDT_MAX_VALUE     4095

/**
 * \brief Get counter value or permitted range value of watchdog timer from the
 * desired timeout period (in us).
 *
 * \note The value returned by this function can be used by wdt_init() if it is
 * not WDT_INVALID_ARGUMENT.
 *
 * \param ul_us The desired timeout period (in us).
 * \param ul_sclk The slow clock on board (in Hz).
 *
 * \return If the desired period is beyond the watchdog period, this function
 * returns WDT_INVALID_ARGUMENT. Otherwise it returns valid value.
 */
uint32_t wdt_get_timeout_value(uint32_t ul_us, uint32_t ul_sclk)
{
	uint32_t max, min;

	min = WDT_SLCK_DIV * 1000000 / ul_sclk;
	max = min * WDT_MAX_VALUE;

	if ((ul_us < min) || (ul_us > max)) {
		return WDT_INVALID_ARGUMENT;
	}

	return WDT_MR_WDV(ul_us / min);
}

/**
 * \brief Initialize watchdog timer with the given mode.
 *
 * \param p_wdt Pointer to a WDT instance.
 * \param ul_mode Bitmask of watchdog timer mode.
 * \param us_counter The value loaded in the 12-bit Watchdog Counter.
 * \param us_delta The permitted range for reloading the Watchdog Timer.
 */
void wdt_init(Wdt *p_wdt, uint32_t ul_mode, uint16_t us_counter,
		uint16_t us_delta)
{
	p_wdt->WDT_MR = ul_mode |
			WDT_MR_WDV(us_counter) | WDT_MR_WDD(us_delta);
}

/**
 * \brief Disable the watchdog timer.
 */
void wdt_disable(Wdt *p_wdt)
{
	p_wdt->WDT_MR = WDT_MR_WDDIS;

}

/**
 * \brief Restart the watchdog timer.
 */
void wdt_restart(Wdt *p_wdt)
{
	if (p_wdt == WDT) {
		p_wdt->WDT_CR = WDT_KEY_PASSWORD | WDT_CR_WDRSTT;
	}
#if (SAM4C || SAM4CM || SAM4CP)
	else {
		p_wdt->WDT_CR = RSWDT_CR_KEY(0xC4u) | RSWDT_CR_WDRSTT;
	}
#endif
}

/**
 * \brief Check the watchdog timer status.
 *
 * \return Bitmask of watchdog timer status.
 */
uint32_t wdt_get_status(Wdt *p_wdt)
{
	return p_wdt->WDT_SR;
}

/**
 * \brief Get the timeout period of the WatchDog Timer in microseconds.
 *
 * \param p_wdt Pointer to a WDT instance.
 * \param ul_sclk The slow clock frequency (in Hz).
 *
 * \return The timeout period in microseconds.
 */
uint32_t wdt_get_us_timeout_period(Wdt *p_wdt, uint32_t ul_sclk)
{
	return WDT_MR_WDV(p_wdt->WDT_MR) * WDT_SLCK_DIV / ul_sclk * 1000000;
}

