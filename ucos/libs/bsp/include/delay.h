/**
 * Common Delay Service
 */

#ifndef _DELAY_H_
#define _DELAY_H_


#include <sysclk.h>

#include <compiler.h>

/**
 * @name Convenience functions for busy-wait delay loops
 *
 * @def delay_cycles
 * @brief Delay program execution for a specified number of CPU cycles.
 * @param n number of CPU cycles to wait
 *
 * @def cpu_delay_ms
 * @brief Delay program execution for a specified number of milliseconds.
 * @param delay number of milliseconds to wait
 * @param f_cpu CPU frequency in Hertz
 *
 * @def cpu_delay_us
 * @brief Delay program execution for a specified number of microseconds.
 * @param delay number of microseconds to wait
 * @param f_cpu CPU frequency in Hertz
 *
 * @def cpu_ms_2_cy
 * @brief Convert milli-seconds into CPU cycles.
 * @param ms number of milliseconds
 * @param f_cpu CPU frequency in Hertz
 * @return the converted number of CPU cycles
 *
 * @def cpu_us_2_cy
 * @brief Convert micro-seconds into CPU cycles.
 * @param ms number of microseconds
 * @param f_cpu CPU frequency in Hertz
 * @return the converted number of CPU cycles
 *
 * @{
 */

/**
 * \brief Delay loop to delay n number of cycles
 *
 * \note The function runs in internal RAM so that flash wait states
 *       will not affect the delay time.
 *
 * \param n Number of cycles
 */
void portable_delay_cycles(unsigned long n);

/* Cortex-M7 is faster than Cortex-M3/M4/M0+ */
#ifdef __CM7_REV

#  define cpu_ms_2_cy(ms, f_cpu)  \
	(((uint64_t)(ms) * (f_cpu) + (uint64_t)(5.932e3 - 1ul)) / (uint64_t)5.932e3)
#  define cpu_us_2_cy(us, f_cpu)  \
	(((uint64_t)(us) * (f_cpu) + (uint64_t)(5.932e6 - 1ul)) / (uint64_t)5.932e6)

#else

#  define cpu_ms_2_cy(ms, f_cpu)  \
	(((uint64_t)(ms) * (f_cpu) + (uint64_t)(14e3 - 1ul)) / (uint64_t)14e3)
#  define cpu_us_2_cy(us, f_cpu)  \
	(((uint64_t)(us) * (f_cpu) + (uint64_t)(14e6 - 1ul)) / (uint64_t)14e6)

#endif

#define delay_cycles               portable_delay_cycles

#define cpu_delay_ms(delay, f_cpu) delay_cycles(cpu_ms_2_cy(delay, f_cpu))
#define cpu_delay_us(delay, f_cpu) delay_cycles(cpu_us_2_cy(delay, f_cpu))
//! @}

/**
 * @defgroup group_common_services_delay Busy-Wait Delay Routines
 *
 * This module provides simple loop-based delay routines for those
 * applications requiring a brief wait during execution. Common API
 * for UC3, XMEGA, and AVR MEGA.
 *
 * @{
 */

/**
 * @def F_CPU
 * @brief MCU Clock Frequency (Hertz)
 *
 * @deprecated
 * The \ref F_CPU configuration constant is used for compatibility with the
 * \ref group_common_services_delay routines. The common loop-based delay
 * routines are designed to use the \ref clk_group modules while anticipating
 * support for legacy applications assuming a statically defined clock
 * frequency. Applications using a statically configured MCU clock frequency
 * can define \ref F_CPU (Hertz), in which case the common delay routines will
 * use this value rather than calling sysclk_get_cpu_hz() to get the current
 * MCU clock frequency.
 */
#ifndef F_CPU
#       define F_CPU sysclk_get_cpu_hz()
#endif

/**
 * @def delay_init
 *
 * @brief Initialize the delay driver.
 * @param fcpu_hz CPU frequency in Hz
 *
 * @deprecated
 * This function is provided for compatibility with ASF applications that
 * may not have been updated to configure the system clock via the common
 * clock service; e.g. sysclk_init() and a configuration header file are
 * used to configure clocks.
 *
 * The functions in this module call \ref sysclk_get_cpu_hz() function to
 * obtain the system clock frequency.
 */
#define delay_init(fcpu_hz)

/**
 * @def delay_s
 * @brief Delay in seconds.
 * @param delay Delay in seconds
 */
#define delay_s(delay)      ((delay) ? cpu_delay_ms(1000 * delay, F_CPU) : cpu_delay_us(1, F_CPU))

/**
 * @def delay_ms
 * @brief Delay in milliseconds.
 * @param delay Delay in milliseconds
 */
#define delay_ms(delay)     ((delay) ? cpu_delay_ms(delay, F_CPU) : cpu_delay_us(1, F_CPU))

/**
 * @def delay_us
 * @brief Delay in microseconds.
 * @param delay Delay in microseconds
 */
#define delay_us(delay)     ((delay) ? cpu_delay_us(delay, F_CPU) : cpu_delay_us(1, F_CPU))


#endif /* _DELAY_H_ */

