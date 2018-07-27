/**
 *
 */

#include "compact.h"

#if __FPU_USED /* CMSIS defined value to indicate usage of FPU */
//#include "fpu.h"
#endif

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
int main(void);
/** \endcond */

//void __libc_init_array(void);

typedef void( *IntFunc )( void );

__attribute__((section(".vectors")))
IntFunc exception_table[] =
{
	/* Configure Initial Stack Pointer, using linker-generated symbols */
	(IntFunc)&_estack,
	Reset_Handler,  /* Initial PC, set to entry point  */
	(uint32_t) 0 /* NMIException */,
	(uint32_t) 0 /* HardFaultException */,
	(uint32_t) 0 /* MemManageException */,
	(uint32_t) 0 /* BusFaultException */,
	(uint32_t) 0 /* UsageFaultException */,
	0, 0, 0, 0,      /* Reserved */
	(uint32_t) 0 /* SVCHandler */,
	(uint32_t) 0 /* DebugMonitor */,
	0,               /* Reserved */
	(uint32_t) 0 /* PendSVC */,
	(uint32_t) 0 /* SysTickHandler */,
	/*
	:
	*/

        /* Configurable interrupts */
       (uint32_t) 0,   /* 0  Supply Controller */
       (uint32_t) 0,   /* 1  Reset Controller */
       (uint32_t) 0,    /* 2  Real Time Clock */
       (uint32_t) 0,    /* 3  Real Time Timer */
       (uint32_t) 0,    /* 4  Watchdog Timer */
       (uint32_t) 0,    /* 5  Power Management Controller */
       (uint32_t) 0,    /* 6  Enhanced Embedded Flash Controller */
       (uint32_t) 0,  /* 7  UART 0 */
       (uint32_t) 0,  /* 8  UART 1 */
       (uint32_t) 0,          /* 9  Reserved */
       PIOA_Handler,   /* 10 Parallel I/O Controller A */
       PIOB_Handler,   /* 11 Parallel I/O Controller B */
#ifdef _SAME70_PIOC_INSTANCE_
       PIOC_Handler,   /* 12 Parallel I/O Controller C */
#else
       (uint32_t) 0,          /* 12 Reserved */
#endif /* _SAME70_PIOC_INSTANCE_ */
       (uint32_t) 0, /* 13 USART 0 */
       (uint32_t) 0, /* 14 USART 1 */
       (uint32_t) 0, /* 15 USART 2 */
       PIOD_Handler,   /* 16 Parallel I/O Controller D */
#ifdef _SAME70_PIOE_INSTANCE_
       PIOE_Handler,   /* 17 Parallel I/O Controller E */
#else
       (uint32_t) 0,          /* 17 Reserved */
#endif /* _SAME70_PIOE_INSTANCE_ */
        (uint32_t) 0,  /* 18 Multimedia Card Interface */
        (uint32_t) 0, /* 19 Two Wire Interface 0 HS */
        (uint32_t) 0, /* 20 Two Wire Interface 1 HS */
        (uint32_t) 0,   /* 21 Serial Peripheral Interface 0 */
        (uint32_t) 0,    /* 22 Synchronous Serial Controller */
        (uint32_t) 0,    /* 23 Timer/Counter 0 */
        (uint32_t) 0,    /* 24 Timer/Counter 1 */
        (uint32_t) 0,    /* 25 Timer/Counter 2 */
        (uint32_t) 0,    /* 26 Timer/Counter 3 */
        (uint32_t) 0,    /* 27 Timer/Counter 4 */

        (uint32_t) 0,    /* 28 Timer/Counter 5 */
        (uint32_t) 0,  /* 29 Analog Front End 0 */
        (uint32_t) 0,   /* 30 Digital To Analog Converter */
        (uint32_t) 0,   /* 31 Pulse Width Modulation 0 */
        (uint32_t) 0,    /* 32 Integrity Check Monitor */
        (uint32_t) 0,    /* 33 Analog Comparator */
        (uint32_t) 0,  /* 34 USB Host / Device Controller */
        (uint32_t) 0,  /* 35 MCAN Controller 0 */
        (uint32_t) 0,          /* 36 Reserved */
        (uint32_t) 0,  /* 37 MCAN Controller 1 */
        (uint32_t) 0,          /* 38 Reserved */
        (uint32_t) 0,   /* 39 Ethernet MAC */
        (uint32_t) 0,  /* 40 Analog Front End 1 */
        (uint32_t) 0, /* 41 Two Wire Interface 2 HS */
        (uint32_t) 0,   /* 42 Serial Peripheral Interface 1 */
        (uint32_t) 0,   /* 43 Quad I/O Serial Peripheral Interface */
        (uint32_t) 0,  /* 44 UART 2 */
        (uint32_t) 0,  /* 45 UART 3 */
        (uint32_t) 0,  /* 46 UART 4 */
        (uint32_t) 0,    /* 47 Timer/Counter 6 */

        (uint32_t) 0,    /* 48 Timer/Counter 7 */
        (uint32_t) 0,    /* 49 Timer/Counter 8 */
        (uint32_t) 0,    /* 50 Timer/Counter 9 */
        (uint32_t) 0,   /* 51 Timer/Counter 10 */
        (uint32_t) 0,   /* 52 Timer/Counter 11 */
        (uint32_t) 0,          /* 53 Reserved */
        (uint32_t) 0,          /* 54 Reserved */
        (uint32_t) 0,          /* 55 Reserved */
        (uint32_t) 0,    /* 56 AES */
        (uint32_t) 0,   /* 57 True Random Generator */
        (uint32_t) 0,  /* 58 DMA */
        (uint32_t) 0,    /* 59 Camera Interface */
        (uint32_t) 0,   /* 60 Pulse Width Modulation 1 */
        (uint32_t) 0,          /* 61 Reserved */
        (uint32_t) 0, /* 62 SDRAM Controller */
        (uint32_t) 0   /* 63 Reinforced Secure Watchdog Timer */

};


/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
        uint32_t *pSrc, *pDest;

        /* Initialize the relocate segment */
        pSrc = &_etext;
        pDest = &_srelocate;

        if (pSrc != pDest) {
                for (; pDest < &_erelocate;) {
                        *pDest++ = *pSrc++;
                }
        }

        /* Clear the zero segment */
        for (pDest = &_szero; pDest < &_ezero;) {
                *pDest++ = 0;
        }

        /* Set the vector table base address */
        pSrc = (uint32_t *) & _sfixed;
        SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

#if __FPU_USED
//	fpu_enable();
#endif

        /* Initialize the C library */
//        __libc_init_array();

        /* Branch to main function */
        main();

        /* Infinite loop */
        while (1);
}

