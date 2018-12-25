/**
 * \file
 *
 * \brief SAMV71-XULTRA board init.
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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

#include <os.h>
#include <lib_mem.h>

#include <ucBsp.h>
#include "compiler.h"
#include "same70_xplained.h"
#include "ioport.h"

/**
 * \brief Set peripheral mode for IOPORT pins.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param port IOPORT port to configure
 * \param masks IOPORT pin masks to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_port_peripheral_mode(port, masks, mode) \
	do {\
		ioport_set_port_mode(port, masks, mode);\
		ioport_disable_port(port, masks);\
	} while (0)

/**
 * \brief Set peripheral mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_pin_peripheral_mode(pin, mode) \
	do {\
		ioport_set_pin_mode(pin, mode);\
		ioport_disable_pin(pin);\
	} while (0)

/**
 * \brief Set input mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 * \param sense Sense for interrupt detection (\ref ioport_sense)
 */
#define ioport_set_pin_input_mode(pin, mode, sense) \
	do {\
		ioport_set_pin_dir(pin, IOPORT_DIR_INPUT);\
		ioport_set_pin_mode(pin, mode);\
		ioport_set_pin_sense_mode(pin, sense);\
	} while (0)



#define  CONSOLE_BAUD_RATE				115200
#define  CONSOLE_CHAR_LENGTH			US_MR_CHRL_8_BIT
#define  CONSOLE_PARITY_TYPE			US_MR_PAR_NO
#define  CONSOLE_STOP_BITS				US_MR_NBSTOP_1_BIT


static void _consoleInit(void)
{
	const usart_serial_options_t uart_serial_options =
	{
		.baudrate = CONSOLE_BAUD_RATE,
		.charlength = CONSOLE_CHAR_LENGTH,
		.paritytype = CONSOLE_PARITY_TYPE,
		.stopbits = CONSOLE_STOP_BITS,
	};


	/* Configure console UART. */
#ifdef	__SAME70Q20__
	/* configure UART pins */
	ioport_set_port_mode(IOPORT_PIOA, PIO_PA9A_URXD0 | PIO_PA10A_UTXD0, IOPORT_MODE_MUX_A);
	ioport_disable_port(IOPORT_PIOA, PIO_PA9A_URXD0 | PIO_PA10A_UTXD0);
	
//	sysclk_enable_peripheral_clock(ID_UART0); 	/* 7 */
	
	stdio_serial_init(UART0, &uart_serial_options); /* eg. 0x400E0800U */

#else
	/* Configure USART pins */
//	ioport_set_pin_peripheral_mode(USART1_RXD_GPIO, USART1_RXD_FLAGS);
//	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;
//	ioport_set_pin_peripheral_mode(USART1_TXD_GPIO, USART1_TXD_FLAGS);
	
//	sysclk_enable_peripheral_clock(ID_USART1);		/* 14 */
	stdio_serial_init(USART1, &uart_serial_options); /* eg. 0x40028000U */
#endif

	
}


void board_init(void)
{
#ifndef CONF_BOARD_KEEP_WATCHDOG_AT_INIT
	/* Disable the watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;
#endif

	/* to support USB */
	sleepmgr_init();
	sysclk_init();
	

	/* Initialize IOPORTs */
	ioport_init();

	/* Configure Push Button pins */
	ioport_set_pin_input_mode(GPIO_PUSH_BUTTON_1, GPIO_PUSH_BUTTON_1_FLAGS, GPIO_PUSH_BUTTON_1_SENSE);
	
	ioport_set_pin_input_mode(GPIO_PUSH_BUTTON_2, GPIO_PUSH_BUTTON_2_FLAGS, GPIO_PUSH_BUTTON_2_SENSE);
	
	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO12; // configure PB12 for IO mode
    
	/* Configure UART pins */
	ioport_set_pin_peripheral_mode(USART1_RXD_GPIO, USART1_RXD_FLAGS);
	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;
	ioport_set_pin_peripheral_mode(USART1_TXD_GPIO, USART1_TXD_FLAGS);

	ioport_set_pin_peripheral_mode(TWIHS0_DATA_GPIO, TWIHS0_DATA_FLAGS);
	ioport_set_pin_peripheral_mode(TWIHS0_CLK_GPIO, TWIHS0_CLK_FLAGS);

	ioport_set_pin_peripheral_mode(SPI0_MISO_GPIO, SPI0_MISO_FLAGS);
	ioport_set_pin_peripheral_mode(SPI0_MOSI_GPIO, SPI0_MOSI_FLAGS);
	ioport_set_pin_peripheral_mode(SPI0_NPCS0_GPIO, SPI0_NPCS0_FLAGS);
	ioport_set_pin_peripheral_mode(SPI0_SPCK_GPIO, SPI0_SPCK_FLAGS);


	_consoleInit();
	LED_INIT(LED0);

	LED_On(LED0);
#if EXT_USB_DEVICE_ON
	ui_init();
	ui_powerdown();

//	memories_initialization();

	// Start USB stack to authorize VBus monitoring
	udc_start();
#endif

}

/* %ATMEL_SYSTEM% */
/* Clock Settings (600MHz PLL VDDIO 3.3V and VDDCORE 1.2V) */
/* Clock Settings (300MHz HCLK, 150MHz MCK)=> PRESC = 2, MDIV = 2 */
#define SYS_BOARD_OSCOUNT   (CKGR_MOR_MOSCXTST(0x8U))
#define SYS_BOARD_PLLAR     (CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(0x31U) | \
                            CKGR_PLLAR_PLLACOUNT(0x3fU) | CKGR_PLLAR_DIVA(0x1U))
#define SYS_BOARD_MCKR      (PMC_MCKR_PRES_CLK_2 | PMC_MCKR_CSS_PLLA_CLK | (1<<8))


uint32_t sysCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;


/* called in sysclk_init */
void SystemCoreClockUpdate( void )
{
	/* Determine clock frequency according to clock register values */
	switch (PMC->PMC_MCKR & (uint32_t) PMC_MCKR_CSS_Msk)
	{
		case PMC_MCKR_CSS_SLOW_CLK: /* Slow clock */
			if ( SUPC->SUPC_SR & SUPC_SR_OSCSEL )
			{
				sysCoreClock = CHIP_FREQ_XTAL_32K;
			}
			else
			{
				sysCoreClock = CHIP_FREQ_SLCK_RC;
			}
			break;

		case PMC_MCKR_CSS_MAIN_CLK: /* Main clock */
			if ( PMC->CKGR_MOR & CKGR_MOR_MOSCSEL )
			{
				sysCoreClock = CHIP_FREQ_XTAL_12M;
			}
			else
			{
				sysCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;

				switch ( PMC->CKGR_MOR & CKGR_MOR_MOSCRCF_Msk )
				{
					case CKGR_MOR_MOSCRCF_4_MHz:
						break;

					case CKGR_MOR_MOSCRCF_8_MHz:
						sysCoreClock *= 2U;
						break;

					case CKGR_MOR_MOSCRCF_12_MHz:
						sysCoreClock *= 3U;
						break;

					default:
						break;
				}
			}
			break;

		case PMC_MCKR_CSS_PLLA_CLK:	/* PLLA clock */
			if ( PMC->CKGR_MOR & CKGR_MOR_MOSCSEL )
			{
				sysCoreClock = CHIP_FREQ_XTAL_12M;
			}
			else
			{
				sysCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;

				switch ( PMC->CKGR_MOR & CKGR_MOR_MOSCRCF_Msk )
				{
					case CKGR_MOR_MOSCRCF_4_MHz:
						break;

					case CKGR_MOR_MOSCRCF_8_MHz:
						sysCoreClock *= 2U;
						break;

					case CKGR_MOR_MOSCRCF_12_MHz:
						sysCoreClock *= 3U;
						break;

					default:
						break;
				}
			}

			if ( (uint32_t) (PMC->PMC_MCKR & (uint32_t) PMC_MCKR_CSS_Msk) == PMC_MCKR_CSS_PLLA_CLK )
			{
				sysCoreClock *= ((((PMC->CKGR_PLLAR) & CKGR_PLLAR_MULA_Msk) >> CKGR_PLLAR_MULA_Pos) + 1U);
				sysCoreClock /= ((((PMC->CKGR_PLLAR) & CKGR_PLLAR_DIVA_Msk) >> CKGR_PLLAR_DIVA_Pos));
			}
			break;

		default:
			break;
	}

	if ( (PMC->PMC_MCKR & PMC_MCKR_PRES_Msk) == PMC_MCKR_PRES_CLK_3 )
	{
		sysCoreClock /= 3U;
	}
	else
	{
		sysCoreClock >>= ((PMC->PMC_MCKR & PMC_MCKR_PRES_Msk) >> PMC_MCKR_PRES_Pos);
	}
}

/**
 * Initialize flash. called in sysclk_init
 */
void system_init_flash( uint32_t ul_clk )
{
	/* Set FWS for embedded Flash access according to operating frequency */
	if ( ul_clk < CHIP_FREQ_FWS_0 )
	{
		EFC->EEFC_FMR = EEFC_FMR_FWS(0)|EEFC_FMR_CLOE;
	}
	else
	{
		if (ul_clk < CHIP_FREQ_FWS_1)
		{
			EFC->EEFC_FMR = EEFC_FMR_FWS(1)|EEFC_FMR_CLOE;
		}
		else
		{
			if (ul_clk < CHIP_FREQ_FWS_2)
			{
				EFC->EEFC_FMR = EEFC_FMR_FWS(2)|EEFC_FMR_CLOE;
			}
			else
			{
				if ( ul_clk < CHIP_FREQ_FWS_3 )
				{
					EFC->EEFC_FMR = EEFC_FMR_FWS(3)|EEFC_FMR_CLOE;
				}
				else
				{
					if ( ul_clk < CHIP_FREQ_FWS_4 )
					{
						EFC->EEFC_FMR = EEFC_FMR_FWS(4)|EEFC_FMR_CLOE;
					}
					else
					{
						EFC->EEFC_FMR = EEFC_FMR_FWS(5)|EEFC_FMR_CLOE;
					}
				}
			}
		}
	}
}


/**
 * \brief Setup the microcontroller system.
 * Initialize the System and update the SystemFrequency variable.
 * called in reset_handler
 */
 void HardwareInit( void )
{
	/* Enable Bus, Mem, and Usage fault handlers */
	SCB->SHCSR |= DEF_BIT_16 | DEF_BIT_17 | DEF_BIT_18;
	
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
#endif
  
	/* Set FWS according to SYS_BOARD_MCKR configuration */
	EFC->EEFC_FMR = EEFC_FMR_FWS(5);

	/* Initialize main oscillator */
	if ( !(PMC->CKGR_MOR & CKGR_MOR_MOSCSEL) )
	{
		PMC->CKGR_MOR = CKGR_MOR_KEY_PASSWD | SYS_BOARD_OSCOUNT | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN;

		while ( !(PMC->PMC_SR & PMC_SR_MOSCXTS) )
		{
		}
	}

	/* Switch to 3-20MHz Xtal oscillator */
	PMC->CKGR_MOR = CKGR_MOR_KEY_PASSWD | SYS_BOARD_OSCOUNT | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN | CKGR_MOR_MOSCSEL;

	while ( !(PMC->PMC_SR & PMC_SR_MOSCSELS) )
	{
	}

	PMC->PMC_MCKR = (PMC->PMC_MCKR & ~(uint32_t)PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS_MAIN_CLK;

	while ( !(PMC->PMC_SR & PMC_SR_MCKRDY) )
	{
	}

	/* Initialize PLLA */
	PMC->CKGR_PLLAR = SYS_BOARD_PLLAR;
	while ( !(PMC->PMC_SR & PMC_SR_LOCKA) )
	{
	}

	/* Switch to main clock */
	PMC->PMC_MCKR = (SYS_BOARD_MCKR & ~PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS_MAIN_CLK;
	while ( !(PMC->PMC_SR & PMC_SR_MCKRDY) )
	{
	}

	/* Switch to PLLA */
	PMC->PMC_MCKR = SYS_BOARD_MCKR;
	while ( !(PMC->PMC_SR & PMC_SR_MCKRDY) )
	{
	}
}

