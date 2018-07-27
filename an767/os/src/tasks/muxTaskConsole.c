/*
 * 
 */


/* Standard includes. */
#include "string.h"
#include "stdio.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


/* common/utils/stdio */
#include "compact.h"
#include "lwipMux.h"

#include "muxOs.h"

/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE		50

/* Dimentions a buffer to be used by the UART driver, if the UART driver uses a buffer at all. */
#define cmdQUEUE_LENGTH			25


/* The maximum time to wait for the mutex that guards the UART to become available. */
#define cmdMAX_MUTEX_WAIT		pdMS_TO_TICKS( 300 )



/* Const messages output by the command console. */
static const char * const pcWelcomeMessage = "MuxLab command line interface."MUX_NEW_LINE"Type 'help' to view a list of commands."MUX_NEW_LINE MUX_PROMPT ;
static const char * const pcEndOfOutputMessage = MUX_NEW_LINE"[Press ENTER to execute the previous command again]"MUX_NEW_LINE MUX_PROMPT;
static const char * const pcNewLine = "\r\n";

/* Used to guard access to the UART in case messages are sent to the UART from more than one task. */
static SemaphoreHandle_t _xTxMutex = NULL;

/* A buffer into which command outputs can be written is declared here, rather
than in the command console implementation, to allow multiple command consoles
to share the same buffer.  For example, an application may allow access to the
command interpreter by UART and by Ethernet.  Sharing a buffer is done purely
to save RAM.  Note, however, that the command console itself is not re-entrant,
so only one command interpreter interface can be used at any one time.  For that
reason, no attempt at providing mutual exclusion to the cOutputBuffer array is
attempted.
 */
static char cOutputBuffer[ MUX_COMMAND_BUFFER_SIZE ];


char *muxBspCLIGetOutputBuffer( void )
{
	return cOutputBuffer;
}


void vMuxUartPutString( const char *pcString, unsigned short usStringLength )
{
	write(1,  ( signed char *) pcString, usStringLength);
}

signed portBASE_TYPE xSerialGetChar( signed char *pcRxedChar, TickType_t xBlockTime )
{
//	ptr_get(stdio_base, (char*)pcRxedChar);
	*pcRxedChar = bspConsoleGetChar();
	return pdTRUE;
}

signed portBASE_TYPE xSerialPutChar( signed char outChar, TickType_t xBlockTime )
{
#if 0
//	if (ptr_put(stdio_base, outChar) == 0)
	if(bspConsolePutChar(outChar) == 0 )
	{
		return pdFAIL;
	}
#else
	bspConsoleEcho(outChar);
#endif
	return pdTRUE;
}


static void _prvUartCmdConsoleTask( void *pvParameters )
{
	signed char cRxedChar;
	uint8_t ucInputIndex = 0;
	char *pcOutputString;
	static char cInputString[ cmdMAX_INPUT_SIZE ], lastInputString[ cmdMAX_INPUT_SIZE ];
	char	ret;

	( void ) pvParameters;

	memset( lastInputString, 0x00, cmdMAX_INPUT_SIZE );
//	strcpy( lastInputString, MUX_CMD_DEFAULT );
	memcpy( lastInputString, MUX_CMD_DEFAULT, 4 );

	pcOutputString = muxBspCLIGetOutputBuffer();

	/* Send the welcome message. */
#if 1	
	vMuxUartPutString((const char * )pcWelcomeMessage, ( unsigned short ) strlen(pcWelcomeMessage) );
#else
	printf("%s", pcWelcomeMessage);
#endif

	for( ;; )
	{
		/* Wait for the next character.  The while loop is used in case
		INCLUDE_vTaskSuspend is not set to 1 - in which case portMAX_DELAY will
		be a genuine block time rather than an infinite block time. */
		while( xSerialGetChar(&cRxedChar, portMAX_DELAY ) != pdPASS );

		/* Ensure exclusive access to the UART Tx. */
		if( xSemaphoreTake( _xTxMutex, cmdMAX_MUTEX_WAIT ) == pdPASS )
		{
			/* Echo the character back. */
			xSerialPutChar( cRxedChar, portMAX_DELAY );

			/* Was it the end of the line? */
			if( cRxedChar == '\n' || cRxedChar == '\r' )
			{
				/* Just to space the output from the input. */
				vMuxUartPutString( (const char * ) pcNewLine, ( unsigned short ) strlen( pcNewLine ) );

				/* See if the command is empty, indicating that the last command
				is to be executed again. */
				if( ucInputIndex == 0 )
				{/* Copy the last command back into the input string. */
//					strcpy( cInputString, lastInputString );
					memset( cInputString, 0x00, cmdMAX_INPUT_SIZE );
					memcpy( cInputString, lastInputString, strlen(lastInputString) );
				}

//printf("'%s':'%s'\r\n", cInputString, lastInputString);
				do
				{
					/* Get the next output string from the command interpreter. */
					ret = bspCmdProcess( cInputString, pcOutputString, MUX_COMMAND_BUFFER_SIZE );

					/* Write the generated string to the UART. */
					vMuxUartPutString( ( const char * ) pcOutputString, ( unsigned short ) strlen( pcOutputString ) );

				} while( ret != MUX_FALSE );

				memset( lastInputString, 0x00, cmdMAX_INPUT_SIZE );
//				strcpy( lastInputString, cInputString );
				memcpy(lastInputString, cInputString, strlen(cInputString) );
//				lastInputString[ ucInputIndex ] = '\0';
				ucInputIndex = 0;
				memset( cInputString, 0x00, cmdMAX_INPUT_SIZE );

				vMuxUartPutString( ( const char * ) pcEndOfOutputMessage, ( unsigned short ) strlen( pcEndOfOutputMessage ) );
			}
			else
			{
				if(cRxedChar == ASCII_KEY_TAB )
					cRxedChar = ' ';
				
				if( cRxedChar == '\r' )
				{
					/* Ignore the character. */
				}
				else if( ( cRxedChar == '\b' ) || ( cRxedChar == ASCII_KEY_DEL ) )
				{
					/* Backspace was pressed.  Erase the last character in the string - if any. */
					if( ucInputIndex > 0 )
					{
						ucInputIndex--;
						cInputString[ ucInputIndex ] = '\0';
					}
				}
				else
				{
					/* A character was entered.  Add it to the string entered so
					far.  When a \n is entered the complete string will be
					passed to the command interpreter. */
					if( ( cRxedChar >= ' ' ) && ( cRxedChar <= '~' ) )
					{
						if( ucInputIndex < cmdMAX_INPUT_SIZE )
						{
							cInputString[ ucInputIndex ] = cRxedChar;
							ucInputIndex++;
						}
					}
				}
			}

			/* Must ensure to give the mutex back. */
			xSemaphoreGive( _xTxMutex );
		}
	}
}

/* mutex output string in console of UART */
void vMuxConsoleOutput( const char *pcMessage )
{
	if( xSemaphoreTake( _xTxMutex, cmdMAX_MUTEX_WAIT ) == pdPASS )
	{
		vMuxUartPutString(pcMessage, ( unsigned short ) strlen( pcMessage ));
		xSemaphoreGive( _xTxMutex );
	}
}


void vMuxUartCmdConsoleStart( uint16_t usStackSize, unsigned long uxPriority )
{
	_xTxMutex = xSemaphoreCreateMutex();
	configASSERT( _xTxMutex );

	/* Create that task that handles the console itself. */
	xTaskCreate( _prvUartCmdConsoleTask, MUX_TASK_CONSOLE, usStackSize, NULL, uxPriority, NULL );
}



