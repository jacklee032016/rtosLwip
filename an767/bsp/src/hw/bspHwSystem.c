/**
 */


#include "compact.h"

/* %ATMEL_SYSTEM% */
/* Clock Settings (600MHz PLL VDDIO 3.3V and VDDCORE 1.2V) */
/* Clock Settings (300MHz HCLK, 150MHz MCK)=> PRESC = 2, MDIV = 2 */
#define SYS_BOARD_OSCOUNT   (CKGR_MOR_MOSCXTST(0x8U))
#define SYS_BOARD_PLLAR     (CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(0x31U) | \
                            CKGR_PLLAR_PLLACOUNT(0x3fU) | CKGR_PLLAR_DIVA(0x1U))
#define SYS_BOARD_MCKR      (PMC_MCKR_PRES_CLK_2 | PMC_MCKR_CSS_PLLA_CLK | (1<<8))

uint32_t SystemCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;

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

/* Initialize flash when sysclk initialization */
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


