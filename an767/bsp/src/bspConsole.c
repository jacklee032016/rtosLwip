/* 
* console(serial) interface outside of BSP
 */

#include <stdio.h>
#include <string.h>

#include "compact.h"
#include "bsp.h"

int bspConsolePutChar(char c)
{
	return ptr_put(stdio_base, c);
}

void bspConsolePutString(const char* str)
{
	while( (*str)!=0)
	{
		char c = *str; 
		bspConsolePutChar(c);
		str++;	
	}
//	seriald_put_string(&console, (const uint8_t*)str);
}


char bspConsoleGetChar(void)
{
	char		c;
	ptr_get(stdio_base,&c);
	return c;
}

bool bspConsoleIsRxReady(void)
{
	return (usart_serial_is_rx_ready((usart_if)stdio_base)!= 0);
}

void bspConsoleReset(const char *name)
{
	CONSOLE_CLEAR_SCREEN();

	CONSOLE_RESET_CURSOR();

	printf("-- %s --"MUX_NEW_LINE, name);
	bspConsolePutString(MUX_NEW_LINE);
}



void bspConsoleEcho(uint8_t c)
{
	switch (c)
	{
		case ASCII_KEY_CR:
		case ASCII_KEY_LF:
			bspConsolePutString("\r\n");
			break;
		case ASCII_KEY_DEL:
			bspConsolePutString("\033[1D\033[K");
			break;
		case '\b':
			bspConsolePutString("\033[1D\033[K");
			break;
		default:
			bspConsolePutChar(c);
	}
}

#if 0
void console_set_rx_handler(console_rx_handler_t handler)
{
//	seriald_set_rx_handler(&console, handler);
}

void console_enable_rx_interrupt(void)
{
//	seriald_enable_rx_interrupt(&console);
}

void console_disable_rx_interrupt(void)
{
//	seriald_disable_rx_interrupt(&console);
}

bool console_is_tx_empty(void)
{
//	return seriald_is_tx_empty(&console);
	return 1;
}

/**
 * \brief Send a sequence of bytes to USART device
 *
 * \param usart  Base address of the USART instance.
 * \param data   Data buffer to read
 * \param len    Length of data
 *
 */
char serial_write_packet(const uint8_t *data, size_t len)
{
	while (len)
	{
		usart_serial_putchar(stdio_base, *data);
		len--;
		data++;
	}
	return EXIT_SUCCESS;
}


/**
 * \brief Receive a sequence of bytes from USART device
 *
 * \param usart  Base address of the USART instance.
 * \param data   Data buffer to write
 * \param len    Length of data
 *
 */
char serial_read_packet(uint8_t *data, size_t len)
{
	while (len)
	{
		usart_serial_getchar(stdio_base, data);
		len--;
		data++;
	}
	return EXIT_SUCCESS;
}
#endif

char bspConsoleGetCharTimeout(char *c, unsigned int timeoutMs)
{
	int count =0;
	while(timeoutMs >0 )
	{
		if (bspConsoleIsRxReady() )
		{
			*c = bspConsoleGetChar();
			return MUX_TRUE;
		}
		
		MUX_DELAY_US(1);
		count++;
		if(count == 500)
		{
			timeoutMs--;
			count = 0;
		}
//		printf("Timeout %d"MUX_NEW_LINE, timeoutMs);
	}
	
	printf("Timeout on serial read"MUX_NEW_LINE);
	return MUX_FALSE;
}

void bspConsoleDumpFrame(uint8_t *frame, uint32_t size)
{
	uint32_t i;
	for (i = 0; i < size; i++)
	{
		printf("%02x ", frame[i]);
	}
	bspConsolePutString(MUX_NEW_LINE);
}

void bspConsoleDumpMemory(uint8_t *buffer, uint32_t size, uint32_t address)
{
	uint32_t i, j;
	uint32_t last_line_start;
	uint8_t *tmp;

	for (i = 0; i < (size / 16); i++)
	{
		printf("0x%08X: ", (unsigned int)(address + (i * 16)));
		tmp = (uint8_t *) & buffer[i * 16];
		for (j = 0; j < 4; j++)
		{
			printf("%02X %02X %02X %02X ", tmp[0], tmp[1], tmp[2], tmp[3]);
			tmp += 4;
		}
#if 0		
		tmp = (uint8_t *) & buffer[i * 16];
		for (j = 0; j < 16; j++)
		{
			bspConsolePutChar(*tmp++);
		}
#endif		
		printf(""MUX_NEW_LINE);
	}
	
	if ((size % 16) != 0)
	{
		last_line_start = size - (size % 16);
		printf("0x%08X: ", (unsigned int)(address + last_line_start));
		for (j = last_line_start; j < last_line_start + 16; j++)
		{
			if ((j != last_line_start) && (j % 4 == 0))
			{
				printf(" ");
			}
			if (j < size)
				printf("%02X ", buffer[j]);
			else
				printf("  ");
		}
#if 0		
		printf(" ");
		for (j = last_line_start; j < size; j++)
		{
			bspConsolePutChar(buffer[j]);
		}
#endif		
		printf(""MUX_NEW_LINE);
	}
}

int bspGetcXModem(void)
{
#if 0
	if (tstc())
		return (getc());
#else
	if(bspConsoleIsRxReady() )
		return bspConsoleGetChar();
#endif
	return -1;
}


