/*
 */

#include "compact.h"

#include <stdio.h>
#include <stdarg.h>

/**
 * \brief Implementation of fputc using the UART as the standard output. Required
 * for printf().
 *
 * \param c  Character to write.
 * \param pStream  Output stream.
 *
 * \return The character written if successful, or -1 if the output stream is
 * not stdout or stderr.
 */
signed int fputc(signed int c, FILE *pStream)
{
	if ((pStream == stdout) || (pStream == stderr))
	{//        DBG_PutChar(c);
		bspConsolePutChar( (char )c); 	
		return c;
	}
	else
	{
		return EOF;
	}
}

/**
 * \brief Implementation of fputs using the UART as the standard output. Required for printf(). Does NOT currently use the PDC.
 *
 * \param pStr  String to write.
 * \param pStream  Output stream.
 *
 * \return Number of characters written if successful, or -1 if the output
 * stream is not stdout or stderr.
 */
signed int fputs(const char *pStr, FILE *pStream)
{
	signed int num = 0;

	while (*pStr != 0)
	{
		if (fputc((unsigned int)*pStr, pStream) == -1)
		{
			return -1;
		}
		num++;
		pStr++;
	}

	return num;
}

#undef putchar

/**
 * \brief Outputs a character on the UART.
 *
 * \param c  Character to output.
 *
 * \return The character that was output.
 */
signed int putchar(signed int c)
{
	return fputc(c, stdout);
}

