/**
 * \brief Main functions for MSC example
 */

#include "usbConf.h"
#include "ucBsp.h"

static volatile bool main_b_msc_enable = false;

/*! \brief Main function. Execution starts here.
 */
int usbMscMain(void)
{
#if 0
	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize the sleep manager
	sleepmgr_init();
#if !SAM0
	sysclk_init();
	board_init();
#else
	system_init();
#endif
	ui_init();
	ui_powerdown();

	memories_initialization();

	// Start USB stack to authorize VBus monitoring
	udc_start();
#endif

	// The main loop manages only the power mode
	// because the USB management is done by interrupt
	while (true) {

		if (main_b_msc_enable) {
			if (!udi_msc_process_trans()) {
				sleepmgr_enter_sleep();
			}
		}else{
			sleepmgr_enter_sleep();
		}
	}
}

void main_suspend_action(void)
{
	ui_powerdown();
}

void main_resume_action(void)
{
	ui_wakeup();
}

void main_sof_action(void)
{
	if (!main_b_msc_enable)
		return;
	ui_process(udd_get_frame_number());
}

bool main_msc_enable(void)
{
	main_b_msc_enable = true;
	return true;
}

void main_msc_disable(void)
{
	main_b_msc_enable = false;
}

/**
 * \mainpage ASF USB Device MSC
 *
 * \section intro Introduction
 * This example shows how to implement a USB Device Mass Storage
 * on Atmel MCU with USB module.
 *
 * \section startup Startup
 * The example uses all memories available on the board and connects these to
 * USB Device Mass Storage stack. After loading firmware, connect the board
 * (EVKxx,Xplain,...) to the USB Host. When connected to a USB host system
 * this application allows to display all available memories as a
 * removable disks in the Unix/Mac/Windows operating systems.
 * \note
 * This example uses the native MSC driver on Unix/Mac/Windows OS, except for Win98.
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and MSC modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/msc/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (leds)
 *
 * <SUP>1</SUP> The memory data transfers are done outside USB interrupt routine.
 * This is done in the MSC process ("udi_msc_process_trans()") called by main loop.
 */
