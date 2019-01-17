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

#define	CONSOLE_ISR_EN	0

/* common/utils/stdio */
#include "compact.h"

#if CONSOLE_ISR_EN
#if EXTLAB_BOARD
#include "uart.h"
#else
#include "usart.h"
#endif

#endif

#include "lwipExt.h"

#include "eos.h"

/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE		50

/* Dimentions a buffer to be used by the UART driver, if the UART driver uses a buffer at all. */
#define cmdQUEUE_LENGTH			25


/* The maximum time to wait for the mutex that guards the UART to become available. */
#define cmdMAX_MUTEX_WAIT		pdMS_TO_TICKS( 300 )



/* Const messages output by the command console. */
static const char * const pcWelcomeMessage = "MuxLab command line interface."EXT_NEW_LINE"Type 'help' to view a list of commands."EXT_NEW_LINE EXT_PROMPT ;
static const char * const pcEndOfOutputMessage = EXT_NEW_LINE"[Press ENTER to execute the previous command again]"EXT_NEW_LINE EXT_PROMPT;
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
static char cOutputBuffer[ EXT_COMMAND_BUFFER_SIZE ];

#if CONSOLE_ISR_EN

static sys_sem_t	_consoleRxSem;
static uint8_t		_inputChar;

static void _consoleInitRecvIrq(void)
{
#if EXTLAB_BOARD
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_SetPriority(UART0_IRQn ,1);

	uart_enable_interrupt(UART0, (UART_IER_RXRDY | UART_IER_OVRE | UART_IER_FRAME	| UART_IER_PARE) );
	
	/* Enable interrupt  */
	NVIC_EnableIRQ(UART0_IRQn);
#else
	NVIC_ClearPendingIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn ,1);

	usart_enable_interrupt(USART1, (US_IER_RXRDY | US_IER_OVRE |US_IER_FRAME|US_IER_PARE) );
	
	/* Enable interrupt  */
	NVIC_EnableIRQ(USART1_IRQn);
#endif
}


#if EXTLAB_BOARD
void UART0_Handler(void)
{
	portBASE_TYPE 	consoleTaskWaking = pdFALSE;
	volatile uint32_t status;
	
	status = uart_get_status(UART0);
	if (status & (UART_SR_OVRE | UART_SR_FRAME | UART_SR_PARE))
	{
		UART0->UART_CR = UART_CR_RSTSTA;
//		printf("Error \n\r");
	}
	
	if(( status & UART_SR_RXRDY) )
	{
		_inputChar = (uint8_t)UART0->UART_RHR;
//		printf("%c, Post flags:%x"EXT_NEW_LINE, (char )_inputChar, flags );
		xSemaphoreGiveFromISR(_consoleRxSem/* not pointer*/, &consoleTaskWaking);
	}
	
	portEND_SWITCHING_ISR(consoleTaskWaking);

}

#else
void USART1_Handler(void)
{
	portBASE_TYPE 	consoleTaskWaking = pdFALSE;
	volatile uint32_t status;
	
	status = usart_get_status(USART1);
	if (status & (US_CSR_OVRE | US_CSR_FRAME | US_CSR_PARE))
	{
		USART1->US_CR = US_CR_RSTSTA;
		printf("Error \n\r");
	}
	
	if(( status & US_CSR_RXRDY) )
	{
		_inputChar = (uint8_t)USART1->US_RHR;
//		printf("%c", (char )_inputChar);
		xSemaphoreGiveFromISR(_consoleRxSem/* not pointer*/, &consoleTaskWaking);
	}
	
	portEND_SWITCHING_ISR(consoleTaskWaking);

}
#endif
#endif

char *extBspCLIGetOutputBuffer( void )
{
	return cOutputBuffer;
}


void vMuxUartPutString( const char *pcString, unsigned short usStringLength )
{
	write(1,  ( signed char *) pcString, usStringLength);
}

static signed portBASE_TYPE xSerialGetChar( signed char *pcRxedChar, TickType_t xBlockTime )
{
//	ptr_get(stdio_base, (char*)pcRxedChar);

#if CONSOLE_ISR_EN
	sys_arch_sem_wait(&_consoleRxSem, 0);
	*pcRxedChar = _inputChar;
#else
	*pcRxedChar = bspConsoleGetChar();
#endif
	return pdTRUE;
}

static signed portBASE_TYPE xSerialPutChar( signed char outChar, TickType_t xBlockTime )
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
//	strcpy( lastInputString, EXT_CMD_DEFAULT );
	memcpy( lastInputString, EXT_CMD_DEFAULT, 4 );

	pcOutputString = extBspCLIGetOutputBuffer();

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
					ret = cmnCmdLineProcess( cInputString, pcOutputString, EXT_COMMAND_BUFFER_SIZE );

					/* Write the generated string to the UART. */
					vMuxUartPutString( ( const char * ) pcOutputString, ( unsigned short ) strlen( pcOutputString ) );

				} while( ret != EXT_FALSE );

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
#if CONSOLE_ISR_EN
	err_t err;
#endif

	_xTxMutex = xSemaphoreCreateMutex();
	configASSERT( _xTxMutex );

#if CONSOLE_ISR_EN
	/* Incoming packet notification semaphore. */
	err = sys_sem_new(&_consoleRxSem, 0);
	EXT_ASSERT(("Console: RX semaphore allocation ERROR!"), (err == ERR_OK));
	if (err != ERR_OK)
		return;

	_consoleInitRecvIrq();
#endif
#if 0
	/* Create that task that handles the console itself. */
	xTaskCreate( _prvUartCmdConsoleTask, EXT_TASK_CONSOLE, usStackSize, NULL, uxPriority, NULL );
#else
	sys_thread_new(EXT_TASK_CONSOLE, _prvUartCmdConsoleTask, NULL, usStackSize, uxPriority);
#endif
}



