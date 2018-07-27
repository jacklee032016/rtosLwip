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


#endif

