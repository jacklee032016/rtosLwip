/**
 *
 */


#ifndef _STDIO_SERIAL_H_
#define _STDIO_SERIAL_H_

/**
 * \defgroup group_common_utils_stdio_stdio_serial Standard serial I/O (stdio)
 * \ingroup group_common_utils_stdio
 *
 * Common standard serial I/O management driver that
 * implements a stdio serial interface on AVR and SAM devices.
 *
 * \{
 */

#include <stdio.h>
#include "compact.h"
#include "compiler.h"

#include "serial.h"


//! Pointer to the base of the USART module instance to use for stdio.
extern volatile void *volatile stdio_base;
//! Pointer to the external low level write function.
extern int (*ptr_put)(void volatile*, char);

//! Pointer to the external low level read function.
extern void (*ptr_get)(void volatile*, char*);

/*! \brief Initializes the stdio in Serial Mode.
 *
 * \param usart       Base address of the USART instance.
 * \param opt         Options needed to set up RS232 communication (see \ref usart_options_t).
 *
 */
static inline void stdio_serial_init(volatile void *usart, const usart_serial_options_t *opt)
{
	stdio_base = (void *)usart;
	ptr_put = (int (*)(void volatile*,char))&usart_serial_putchar;
	ptr_get = (void (*)(void volatile*,char*))&usart_serial_getchar;
#if SAM
	usart_serial_init((Usart *)usart,(usart_serial_options_t *)opt);
#else
#  error Unsupported chip type
#endif

# if defined(__GNUC__)
#if 0//UC3 || SAM
	// For AVR32 and SAM GCC
	// Specify that stdout and stdin should not be buffered.
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	// Note: Already the case in IAR's Normal DLIB default configuration
	// and AVR GCC library:
	// - printf() emits one character at a time.
	// - getchar() requests only 1 byte to exit.
#  endif
# endif
}

/**
 * \}
 */

#endif  // _STDIO_SERIAL_H_
