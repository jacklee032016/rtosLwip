/**
 * \file
 *
 * \brief SAM3/SAM4 Sleep manager implementation.
 *
 * Copyright (c) 2012-2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef SAM_SLEEPMGR_INCLUDED
#define SAM_SLEEPMGR_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "ucBsp.h"

#include <interrupt.h>

/**
 * \weakgroup sleepmgr_group
 * @{
 */
enum sleepmgr_mode {
	//! Active mode.
	SLEEPMGR_ACTIVE = 0,
	/*! WFE sleep mode.
	 *  Potential Wake Up sources:
	 *  fast startup events (USB, RTC, RTT, WKUPs),
	 *  interrupt, and events. */
	SLEEPMGR_SLEEP_WFE,
	/*! WFI sleep mode.
	 * Potential Wake Up sources: fast startup events and interrupt. */
	SLEEPMGR_SLEEP_WFI,
	/*! Wait mode, wakeup fast (in 3ms).
	 *  XTAL is not disabled when sleep.
	 *  Potential Wake Up sources: fast startup events */
	SLEEPMGR_WAIT_FAST,
	/*! Wait mode.
	 *  Potential Wake Up sources: fast startup events */
	SLEEPMGR_WAIT,
#if (!(SAMG51 || SAMG53 || SAMG54))
	//! Backup mode. Potential Wake Up sources: WKUPs, SM, RTT, RTC.
	SLEEPMGR_BACKUP,
#endif
	SLEEPMGR_NR_OF_MODES,
};

#if defined(CONFIG_SLEEPMGR_ENABLE) || defined(__DOXYGEN__)
//! Sleep mode lock counters
extern uint8_t sleepmgr_locks[];
#endif /* CONFIG_SLEEPMGR_ENABLE */
//! @}



/**
 * \internal
 * \name Internal arrays
 * @{
 */

static inline void sleepmgr_sleep(const enum sleepmgr_mode sleep_mode)
{
	Assert(sleep_mode != SLEEPMGR_ACTIVE);
#ifdef CONFIG_SLEEPMGR_ENABLE
	cpu_irq_disable();

	// Atomically enable the global interrupts and enter the sleep mode.
	pmc_sleep(sleep_mode);
#else
	UNUSED(sleep_mode);
	cpu_irq_enable();
#endif /* CONFIG_SLEEPMGR_ENABLE */

}

/**
 * \enum sleepmgr_mode
 * \brief Sleep mode locks
 *
 * Identifiers for the different sleep mode locks.
 */

/**
 * \brief Initialize the lock counts
 *
 * Sets all lock counts to 0, except the very last one, which is set to 1. This
 * is done to simplify the algorithm for finding the deepest allowable sleep
 * mode in \ref sleepmgr_enter_sleep.
 */
static inline void sleepmgr_init(void)
{
#ifdef CONFIG_SLEEPMGR_ENABLE
	uint8_t i;

	for (i = 0; i < SLEEPMGR_NR_OF_MODES - 1; i++) {
		sleepmgr_locks[i] = 0;
	}
	sleepmgr_locks[SLEEPMGR_NR_OF_MODES - 1] = 1;
#endif /* CONFIG_SLEEPMGR_ENABLE */
}

/**
 * \brief Increase lock count for a sleep mode
 *
 * Increases the lock count for \a mode to ensure that the sleep manager does
 * not put the device to sleep in the deeper sleep modes.
 *
 * \param mode Sleep mode to lock.
 */
static inline void sleepmgr_lock_mode(enum sleepmgr_mode mode)
{
#ifdef CONFIG_SLEEPMGR_ENABLE
	irqflags_t flags;

	if(sleepmgr_locks[mode] >= 0xff) {
		while (true) {
			// Warning: maximum value of sleepmgr_locks buffer is no more than 255.
			// Check APP or change the data type to uint16_t.
		}
	}

	// Enter a critical section
	flags = cpu_irq_save();

	++sleepmgr_locks[mode];

	// Leave the critical section
	cpu_irq_restore(flags);
#else
	UNUSED(mode);
#endif /* CONFIG_SLEEPMGR_ENABLE */
}

/**
 * \brief Decrease lock count for a sleep mode
 *
 * Decreases the lock count for \a mode. If the lock count reaches 0, the sleep
 * manager can put the device to sleep in the deeper sleep modes.
 *
 * \param mode Sleep mode to unlock.
 */
static inline void sleepmgr_unlock_mode(enum sleepmgr_mode mode)
{
#ifdef CONFIG_SLEEPMGR_ENABLE
	irqflags_t flags;

	if(sleepmgr_locks[mode] == 0) {
		while (true) {
			// Warning: minimum value of sleepmgr_locks buffer is no less than 0.
			// Check APP.
		}
	}

	// Enter a critical section
	flags = cpu_irq_save();

	--sleepmgr_locks[mode];

	// Leave the critical section
	cpu_irq_restore(flags);
#else
	UNUSED(mode);
#endif /* CONFIG_SLEEPMGR_ENABLE */
}

 /**
 * \brief Retrieves the deepest allowable sleep mode
 *
 * Searches through the sleep mode lock counts, starting at the shallowest sleep
 * mode, until the first non-zero lock count is found. The deepest allowable
 * sleep mode is then returned.
 */
static inline enum sleepmgr_mode sleepmgr_get_sleep_mode(void)
{
	enum sleepmgr_mode sleep_mode = SLEEPMGR_ACTIVE;

#ifdef CONFIG_SLEEPMGR_ENABLE
	uint8_t *lock_ptr = sleepmgr_locks;

	// Find first non-zero lock count, starting with the shallowest modes.
	while (!(*lock_ptr)) {
		lock_ptr++;
		sleep_mode = (enum sleepmgr_mode)(sleep_mode + 1);
	}

	// Catch the case where one too many sleepmgr_unlock_mode() call has been
	// performed on the deepest sleep mode.
	Assert((uintptr_t)(lock_ptr - sleepmgr_locks) < SLEEPMGR_NR_OF_MODES);

#endif /* CONFIG_SLEEPMGR_ENABLE */

	return sleep_mode;
}

/**
 * \fn sleepmgr_enter_sleep
 * \brief Go to sleep in the deepest allowed mode
 *
 * Searches through the sleep mode lock counts, starting at the shallowest sleep
 * mode, until the first non-zero lock count is found. The device is then put to
 * sleep in the sleep mode that corresponds to the lock.
 *
 * \note This function enables interrupts before going to sleep, and will leave
 * them enabled upon return. This also applies if sleep is skipped due to ACTIVE
 * mode being locked.
 */

static inline void sleepmgr_enter_sleep(void)
{
#ifdef CONFIG_SLEEPMGR_ENABLE
	enum sleepmgr_mode sleep_mode;

	cpu_irq_disable();

	// Find the deepest allowable sleep mode
	sleep_mode = sleepmgr_get_sleep_mode();
	// Return right away if first mode (ACTIVE) is locked.
	if (sleep_mode==SLEEPMGR_ACTIVE) {
		cpu_irq_enable();
		return;
	}
	// Enter the deepest allowable sleep mode with interrupts enabled
	sleepmgr_sleep(sleep_mode);
#else
	cpu_irq_enable();
#endif /* CONFIG_SLEEPMGR_ENABLE */
}

//! @}

#ifdef __cplusplus
}
#endif

#endif /* SAM_SLEEPMGR_INCLUDED */
