/**
 * \file
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
#include "same70_xplained.h"
#if 0 /// \todo fixme or removeme
#include <stdio_serial.h>
#endif
#include "same70.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/* %ATMEL_SYSTEM% */
/* Clock Settings (600MHz PLL VDDIO 3.3V and VDDCORE 1.2V) */
/* Clock Settings (300MHz HCLK, 150MHz MCK)=> PRESC = 2, MDIV = 2 */
#define SYS_BOARD_OSCOUNT   (CKGR_MOR_MOSCXTST(0x8U))
#define SYS_BOARD_PLLAR     (CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(0x31U) | \
                            CKGR_PLLAR_PLLACOUNT(0x3fU) | CKGR_PLLAR_DIVA(0x1U))
#define SYS_BOARD_MCKR      (PMC_MCKR_PRES_CLK_2 | PMC_MCKR_CSS_PLLA_CLK | (1<<8))



#define  MAIN_TASK_STK_SIZE    1024u
#define  MAIN_TASK_PRIO        10u
#define  MAIN_TASK_Q_SIZE      100u


        OS_TCB  MainTaskTCB;
static  CPU_STK MainTaskStk[MAIN_TASK_STK_SIZE];

uint32_t SystemCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;



void HardwareInit(void);
void SoftwareInit(void);

static void MainTask(void *p_arg);

extern int main(int argc, char *argv[]);


/**
 * \brief Setup the microcontroller system.
 * Initialize the System and update the SystemFrequency variable.
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

void SoftwareInit( void )
{
    OS_ERR  err;
    
    SystemCoreClock = CHIP_FREQ_CPU_MAX;
    
    OSInit(&err);
    
    OSTaskCreate(&MainTaskTCB,
                 "Main Task",
                  MainTask,
                  0,
                  MAIN_TASK_PRIO,
                  MainTaskStk,
                  MAIN_TASK_STK_SIZE / 10,
                  MAIN_TASK_STK_SIZE,
                  MAIN_TASK_Q_SIZE,
                  0,
                  0,
                  OS_OPT_NONE,
                 &err);
    
    OSStart(&err);
    
    while (1);
}

static void MainTask(void *p_arg)
{
#if 0 /// \todo fixme or removeme
    const usart_serial_options_t usart_options = {
        .baudrate = 115200,
        .charlength = 8,
        .paritytype = 0,
        .stopbits = 0
    };
    
    sysclk_enable_peripheral_clock(ID_USART1);
    stdio_serial_init(USART1, &usart_options);
    
    puts("Hello, world\n");
#endif

    board_init();
    
    CPU_Init();
    Mem_Init();
    
    /* Configure and enable the OS tick interrupt */
    OS_CPU_SysTickInit(SystemCoreClock);
    
    /* Call main, the application's entry point. */
    main(0,0);
    
    /** \todo Handle return value from main. */
    
    while(1);
}

void SystemCoreClockUpdate( void )
{
  /* Determine clock frequency according to clock register values */
  switch (PMC->PMC_MCKR & (uint32_t) PMC_MCKR_CSS_Msk)
  {
    case PMC_MCKR_CSS_SLOW_CLK: /* Slow clock */
      if ( SUPC->SUPC_SR & SUPC_SR_OSCSEL )
      {
        SystemCoreClock = CHIP_FREQ_XTAL_32K;
      }
      else
      {
        SystemCoreClock = CHIP_FREQ_SLCK_RC;
      }
    break;

    case PMC_MCKR_CSS_MAIN_CLK: /* Main clock */
      if ( PMC->CKGR_MOR & CKGR_MOR_MOSCSEL )
      {
        SystemCoreClock = CHIP_FREQ_XTAL_12M;
      }
      else
      {
        SystemCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;

        switch ( PMC->CKGR_MOR & CKGR_MOR_MOSCRCF_Msk )
        {
          case CKGR_MOR_MOSCRCF_4_MHz:
          break;

          case CKGR_MOR_MOSCRCF_8_MHz:
            SystemCoreClock *= 2U;
          break;

          case CKGR_MOR_MOSCRCF_12_MHz:
            SystemCoreClock *= 3U;
          break;

          default:
          break;
        }
      }
    break;

    case PMC_MCKR_CSS_PLLA_CLK:	/* PLLA clock */
      if ( PMC->CKGR_MOR & CKGR_MOR_MOSCSEL )
      {
        SystemCoreClock = CHIP_FREQ_XTAL_12M ;
      }
      else
      {
        SystemCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;

        switch ( PMC->CKGR_MOR & CKGR_MOR_MOSCRCF_Msk )
        {
          case CKGR_MOR_MOSCRCF_4_MHz:
          break;

          case CKGR_MOR_MOSCRCF_8_MHz:
            SystemCoreClock *= 2U;
          break;

          case CKGR_MOR_MOSCRCF_12_MHz:
            SystemCoreClock *= 3U;
          break;

          default:
          break;
        }
      }

      if ( (uint32_t) (PMC->PMC_MCKR & (uint32_t) PMC_MCKR_CSS_Msk) == PMC_MCKR_CSS_PLLA_CLK )
      {
        SystemCoreClock *= ((((PMC->CKGR_PLLAR) & CKGR_PLLAR_MULA_Msk) >> CKGR_PLLAR_MULA_Pos) + 1U);
        SystemCoreClock /= ((((PMC->CKGR_PLLAR) & CKGR_PLLAR_DIVA_Msk) >> CKGR_PLLAR_DIVA_Pos));
      }
    break;

    default:
    break;
  }

  if ( (PMC->PMC_MCKR & PMC_MCKR_PRES_Msk) == PMC_MCKR_PRES_CLK_3 )
  {
    SystemCoreClock /= 3U;
  }
  else
  {
    SystemCoreClock >>= ((PMC->PMC_MCKR & PMC_MCKR_PRES_Msk) >> PMC_MCKR_PRES_Pos);
  }
}
/**
 * Initialize flash.
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
/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */
