
#include "extSys.h"
#include "ucBsp.h"

/**
 * \brief Run Application which is flashed at given address.
 */
void loadApplication(uint32_t appAddress)
{
	uint32_t __Start_SP;
	uint32_t (*__Start_New)(void);
	uint32_t *pCode = (uint32_t *)appAddress;

	/* set PC and SP */
	__Start_New = (uint32_t(*) (void) ) pCode[1];
	__Start_SP = pCode[0];

	printf("\n\r Starting application at flash 0x%08x! "EXT_NEW_LINE, (unsigned int)appAddress);
	printf("========================================================= "EXT_NEW_LINE );

	EXT_DELAY_MS(100 +100 );

//	WDT_Restart(WDT);
//	__disable_irq();
	__set_MSP(__Start_SP);
	__Start_New();
}


void rstc_reset_processor_and_peripherals_and_ext(void)
{
//	RSTC->RSTC_CR = RSTC_CR_EXTRST | RSTC_MR_KEY_PASSWD;
	RSTC->RSTC_CR = RSTC_MR_KEY_PASSWD | RSTC_CR_PROCRST;
}

/*
// Delay loop is put to SRAM so that FWS will not affect delay time
 * \brief ARM functions for busy-wait delay loops
*/

OPTIMIZE_HIGH
RAMFUNC
void portable_delay_cycles(unsigned long n)
{
	UNUSED(n);

	__asm (
		"loop: DMB	\n"
		"SUBS R0, R0, #1  \n"
		"BNE.N loop         "
	);
}

