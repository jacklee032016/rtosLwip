/* 
 * 
 */

#include "bootload.h"
#include <board.h>
#include <string.h>

/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/
/* Initialization mode */
#define EK_MODE 0
#define USER_DEFINED_CRYSTAL 1
#define BYASS_MODE 2

/*----------------------------------------------------------------------------
 *        Local structures
 *----------------------------------------------------------------------------*/
/** 
 * \brief Structure for storing parameters for each command that can be performed by the applet.
 */
struct _Mailbox {

    /* Command send to the monitor to be executed. */
    uint32_t command;
    /* Returned status, updated at the end of the monitor execution. */
    uint32_t status;

    /* Input Arguments in the argument area */
    union {

        /* Input arguments for the Init command. */
        struct {

            /* Communication link used. */
            uint32_t comType;
            /* Trace level. */
            uint32_t traceLevel;
            /* Low initialization mode */
            uint32_t mode;
            /* frequency of user-defined crystal */
            uint32_t crystalFreq;
            /* frequency of external clock in bypass mode */
            uint32_t extClk;

        } inputInit;

        /* Output arguments for the Init command. */
        /* None */

        /* Input arguments for the Write command. */
        /* None */

        /* Output arguments for the Write command. */
        /* None */

        /* Input arguments for the Read command. */
        /* None */

        /* Output arguments for the Read command. */
        /* None */

        /* Input arguments for the Full Erase command. */
        /* NONE */

        /* Output arguments for the Full Erase command. */
        /* NONE */

        /* Input arguments for the Buffer Erase command. */
        /* None */

        /* Output arguments for the Buffer Erase command. */
        /* NONE */
    } argument;
};

/**
 * \brief  Configure the PMC if the frequency of the external oscillator is different from the one mounted on EK
 * \param crystalFreq  The frequency of the external oscillator
 */
static void user_defined_LowlevelInit (uint32_t crystalFreq)
{
}

/**
 * \brief  Configure the PMC in bypass mode. An external clock should be input to XIN as the source clock.
 *
 * \param extClk  The frequency of the external clock
 */
static void bypass_LowLevelInit (uint32_t extClk)
{
}

/**
 * \brief  Configure the PMC as EK setting
 */
static void EK_LowLevelInit (void)
{
    LowLevelInit();
}

/*----------------------------------------------------------------------------
 *         Global functions
 *----------------------------------------------------------------------------*/
/**
 * \brief  Applet main entry. This function decodes received command and executes it.
 *
 * \param argc  always 1
 * \param argv  Address of the argument area..
 */
//int main(int argc, char **argv)
int main(void)
{
#if 0
	struct _Mailbox *pMailbox = (struct _Mailbox *) argv;
	uint32_t mode, crystalFreq, extClk;
	uint32_t comType = 0;

	// INIT:
	if (pMailbox->command == APPLET_CMD_INIT)
	{
		/* Save info of communication link */
		comType = pMailbox->argument.inputInit.comType;

		mode = pMailbox->argument.inputInit.mode;
		crystalFreq = pMailbox->argument.inputInit.crystalFreq;
		extClk = pMailbox->argument.inputInit.extClk;

		switch (mode)
		{
			case EK_MODE: 
				EK_LowLevelInit();
				pMailbox->status = APPLET_SUCCESS;
				break;

			case USER_DEFINED_CRYSTAL:
				user_defined_LowlevelInit(crystalFreq);
				pMailbox->status = APPLET_SUCCESS;
				break;

			case BYASS_MODE:
				bypass_LowLevelInit(extClk);
				pMailbox->status = APPLET_SUCCESS;
				break;

			default:
				pMailbox->status = APPLET_DEV_UNKNOWN;
				break;
		}
	}
	else
	{
		pMailbox->status = APPLET_DEV_UNKNOWN;
	}
#endif

	LowLevelInit();

	    const Pin ledPins = PIN_LED_00 ;

	    PIO_Configure( &ledPins, 1) ;

	DBG_Configure(115200, BOARD_MCK);
	printf("APPLET_CMD_INIT Done");
#if 0	
	/* Notify the host application of the end of the command processing */
	pMailbox->command = ~(pMailbox->command);
	if (comType == DBGU_COM_TYPE)
	{
	}
#endif
		DBG_PutChar(0x6);
	return 0;
}

