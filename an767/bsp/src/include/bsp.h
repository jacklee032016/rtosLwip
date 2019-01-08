/*
* header local to BSP only, used to hide the detail of hardware and BSP
*/

#ifndef	__BSP_H__
#define	__BSP_H__


// From module: Standard serial I/O (stdio) - SAM implementation
#include <stdio_serial.h>

// From module: UART - Univ. Async Rec/Trans
#include <uart.h>

// From module: USART - Serial interface - SAM implementation for devices with both UART and USART
#include <serial.h>

// From module: USART - Univ. Syn Async Rec/Trans
#include <usart.h>

#include <spi.h>

#include "trng.h"

#define	RNG_MODE_NONE				0
#define	RNG_MODE_MAC_ADDRESS		1
#define	RNG_MODE_SC_CHALLENGE		2

void bspHwTrngConfig(char isEnable, char mode);
void bspHwTrngWait(void);

#endif

