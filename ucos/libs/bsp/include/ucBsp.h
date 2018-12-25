/**
 *
 */

#ifndef __UC_BSP_H__
#define __UC_BSP_H__

/*
 * This file includes all API header files for the selected drivers from ASF.
 * Note: There might be duplicate includes required by more than one driver.
 *
 * The file is automatically generated and will be re-written when
 * running the ASF driver selector tool. Any changes will be discarded.
 */

// From module: Common SAM compiler driver
#include <compiler.h>
#include <status_codes.h>

// From module: Generic board support
#include "same70_xplained.h"

// From module: System Clock Control - SAMV71 implementation
#include <sysclk.h>

// From module: IOPORT - General purpose I/O service
#include <ioport.h>

// From module: Interrupt management - SAM implementation
#include <interrupt.h>

// From module: PMC - Power Management Controller
#include <pmc.h>
#include <sleep.h>

// From module: Part identification macros
#include <parts.h>

// From module: SAM FPU driver
#include <fpu.h>

// From module: SAMV71 XULTRA LED support enabled
//#include <led.h>

// From module: Standard serial I/O (stdio) - SAM implementation
#include <stdio_serial.h>

// From module: UART - Univ. Async Rec/Trans
#include <uart.h>

// From module: USART - Serial interface - SAM implementation for devices with both UART and USART
#include <serial.h>

// From module: USART - Univ. Syn Async Rec/Trans
#include <usart.h>

// From module: XDMAC - XDMA Controller
#include <xdmac.h>

// From module: MATRIX - Bus Matrix
#include <matrix.h>

// From module: PWM - Pulse Width Modulation
#include <pwm.h>

// From module: Part identification macros
#include <parts.h>

#include <delay.h>

/**** board params *****/
/** Enable Com Port. */
#define CONF_BOARD_UART_CONSOLE

/** Enable Two Wire Interface */
#define CONF_BOARD_TWIHS0

/** Enable SPI */
#define CONF_BOARD_SPI

/** Enable PWM for LED0 and LED1 */
#define CONF_BOARD_PWM_LED0
#define CONF_BOARD_PWM_LED1

/** PWM clock values */
#define PWM_FREQUENCY		1000
#define PERIOD_VALUE		100

//#define CONF_BOARD_KEEP_WATCHDOG_AT_INIT


#define CONFIG_SLEEPMGR_ENABLE


#include <sleepmgr.h>




#define	EXT_USB_D_NONE				0
#define	EXT_USB_D_MSC					1		/* Mass Storage Class */
#define	EXT_USB_D_CDC					2		/* Communication Device Class */
#define	EXT_USB_D_VENDOR				3		/* Vendor class */


#define	EXT_USB_DEVICE_ON				EXT_USB_D_VENDOR


#define	LED0							LED0_GPIO

#ifdef	__SAME70Q20__
#define	LED_INIT(pin) 	{}

#define	LED_On(pin) 		{}

#define	LED_Off(pin) 	{}

#define	LED_TOGGLE(pin)		{}

#else
/* Configure the pins connected to LED as output and set their default initial state to high (LED off). */
#define	LED_INIT(pin) 	\
	ioport_set_pin_dir(pin, IOPORT_DIR_OUTPUT)

#define	LED_On(pin) 		\
	ioport_set_pin_level(pin, IOPORT_PIN_LEVEL_LOW)

#define	LED_Off(pin) 	\
	ioport_set_pin_level(pin, IOPORT_PIN_LEVEL_HIGH)

#define	LED_TOGGLE(pin)	\
		ioport_toggle_pin_level(pin)
#endif

//! \brief Initializes the user interface
void ui_init(void);

//! \brief Enters the user interface in power down mode
void ui_powerdown(void);

//! \brief Exits the user interface of power down mode
void ui_wakeup(void);

//! \name Callback to show the MSC read and write access
//! @{
void ui_start_read(void);
void ui_stop_read(void);
void ui_start_write(void);
void ui_stop_write(void);
//! @}

/*! \brief This process is called each 1ms
 * It is called only if the USB interface is enabled.
 *
 * \param framenumber  Current frame number
 */
void ui_process(uint16_t framenumber);


#include "usbConf.h"

/* memory mapping of internal flash */
#if 0
#define	AN767_MCU_MMAP_OS			0x00410000	/* 64K */
#else
/* Atmel Studio Programming tool erase in 128KB */
#define	AN767_MCU_MMAP_OS			0x00420000	/* 128K */
#endif

#define	AN767_MCU_MMAP_BL			0x00400000	/* start of flash */


void board_init(void);

/* from delay.h*/
#define	EXT_DELAY_S(ms)		delay_s((ms))
#define	EXT_DELAY_MS(ms)		delay_ms((ms))
#define	EXT_DELAY_US(ms)		delay_us((ms))


//#define	EXT_REBOOT()			rstc_start_software_reset(RSTC)
#define	EXT_REBOOT()			rstc_reset_processor_and_peripherals_and_ext()


#define	EXT_LOAD_OS()					loadApplication(AN767_MCU_MMAP_OS);

#define	EXT_LOAD_BOOLOADER()			loadApplication(AN767_MCU_MMAP_BL);


void loadApplication(uint32_t appAddress);
void rstc_reset_processor_and_peripherals_and_ext(void);

extern uint32_t sysCoreClock; /* System Clock Frequency (Core Clock) */


#define		EXT_TICK_PER_MS       			(1000/OS_CFG_TICK_RATE_HZ)
/* ms > 10 ms */
#define		EXT_TIME_MS_2_TICK(ms)		((ms)/(EXT_TICK_PER_MS))


#endif

