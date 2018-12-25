/*
 * 
 */


#include "app_cfg.h"

#include "extSys.h"
#include "ucBsp.h"

#include <os.h>

/* Standard includes. */
#include "string.h"
#include "stdio.h"

#if TASK_CONSOLE_EN

/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE		50

/* Dimentions a buffer to be used by the UART driver, if the UART driver uses a buffer at all. */
#define cmdQUEUE_LENGTH			25


/* The maximum time to wait for the mutex that guards the UART to become available. */
#define cmdMAX_MUTEX_WAIT		( 300 )

#define	CONSOLE_FLAG_COMPLETE_BIT			DEF_BIT_00

static	OS_FLAG_GRP	_consoleRecvedFlagGrp;
static 	uint8_t	_inputChar;




/* Const messages output by the command console. */
static const char * const pcWelcomeMessage = "MuxLab command line interface."EXT_NEW_LINE"Type 'help' to view a list of commands."EXT_NEW_LINE EXT_PROMPT ;
static const char * const pcEndOfOutputMessage = EXT_NEW_LINE"[Press ENTER to execute the previous command again]"EXT_NEW_LINE EXT_PROMPT;
static const char * const pcNewLine = "\r\n";

/* Used to guard access to the UART in case messages are sent to the UART from more than one task. */
#if __UCOS__
static OS_SEM			_txMutex;

static OS_TCB	_consoleTaskTcb;
static CPU_STK	_consoleTaskStk[CONSOLE_TASK_STK_SIZE];

#else
static SemaphoreHandle_t 	_txMutex = NULL;
#endif

static char cOutputBuffer[ EXT_COMMAND_BUFFER_SIZE ];

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
	uint32_t status;
	OS_ERR err;
	OS_FLAGS flags;

	CPU_SR_ALLOC();
	CPU_CRITICAL_ENTER();
	OSIntEnter();                                               /* Tell the OS we're in an interrupt                    */
	CPU_CRITICAL_EXIT();
	
	status = uart_get_status(UART0);
	if (status & (UART_SR_OVRE | UART_SR_FRAME | UART_SR_PARE))
	{
		UART0->UART_CR = UART_CR_RSTSTA;
		printf("Error \n\r");
	}
	
	if(( status & UART_SR_RXRDY) )
	{
		_inputChar = (uint8_t)UART0->UART_RHR;
//	TRACE();
		flags = OSFlagPost(&_consoleRecvedFlagGrp, CONSOLE_FLAG_COMPLETE_BIT, OS_OPT_POST_FLAG_SET, &err);
//		printf("%c, Post flags:%x"EXT_NEW_LINE, (char )_inputChar, flags );
	}
	
	OSIntExit();
	
}
#else
void USART1_Handler(void)
{
	uint32_t status;
	OS_ERR err;

	CPU_SR_ALLOC();
	
	CPU_CRITICAL_ENTER();
	OSIntEnter();                                               /* Tell the OS we're in an interrupt                    */
	CPU_CRITICAL_EXIT();
	
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
		OSFlagPost(&_consoleRecvedFlagGrp, CONSOLE_FLAG_COMPLETE_BIT, OS_OPT_POST_FLAG_SET, &err);
	}

	OSIntExit();
}

#endif

char *extBspCLIGetOutputBuffer( void )
{
	return cOutputBuffer;
}

void vMuxUartPutString( const char *pcString, unsigned short usStringLength )
{
	write(1,  ( signed char *) pcString, usStringLength);
}

char bspConsoleGetChar(void)
{
	char		c;
	ptr_get(stdio_base,&c);
	return c;
}

uint8_t xSerialGetChar(uint8_t *pcRxedChar, int blockTime )
{
	OS_ERR err;
	OS_FLAGS flags;
//	ptr_get(stdio_base, (char*)pcRxedChar);

	flags = OSFlagPend(&_consoleRecvedFlagGrp, (OS_FLAGS)CONSOLE_FLAG_COMPLETE_BIT, (OS_TICK)0, OS_OPT_PEND_FLAG_SET_ANY+OS_OPT_PEND_FLAG_CONSUME, (CPU_TS *)0, &err);
	EXT_ASSERT(("Console get char from ISR failed: %x", err), (err == OS_ERR_NONE));
//	printf("%c Pend flags:%x"EXT_NEW_LINE, _inputChar, flags);
	*pcRxedChar = _inputChar;
//	*pcRxedChar = bspConsoleGetChar();
	return EXIT_SUCCESS;
}

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


uint8_t xSerialPutChar( signed char outChar, int blockTime )
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
	return EXIT_SUCCESS;
}


static void _consoleTaskMain( void *arg )
{
	signed char cRxedChar;
	uint8_t ucInputIndex = 0;
	char *pcOutputString;
	static char cInputString[ cmdMAX_INPUT_SIZE ], lastInputString[ cmdMAX_INPUT_SIZE ];
	char	ret;
#if __UCOS__
	OS_ERR err;
#endif
	arg = arg;

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

#if 0
		printf(".");
TRACE();
		OSTimeDly((OS_TICK)500, (OS_OPT)OS_OPT_TIME_DLY, &err);
#else	
		/* Wait for the next character.  The while loop is used in case
		INCLUDE_vTaskSuspend is not set to 1 - in which case portMAX_DELAY will
		be a genuine block time rather than an infinite block time. */
		while( xSerialGetChar(&cRxedChar, 100 ) != EXIT_SUCCESS);

		/* Ensure exclusive access to the UART Tx. */
#if __UCOS__
		OSSemPend(&_txMutex, (OS_TICK)0, (OS_OPT)OS_OPT_PEND_BLOCKING, (CPU_TS *)NULL, &err);
		EXT_ASSERT(("Return :%x", err), (err == OS_ERR_NONE));
		if(err == OS_ERR_NONE)
#else
		if( xSemaphoreTake( _txMutex, cmdMAX_MUTEX_WAIT ) == EXIT_SUCCESS)
#endif			
		{
			/* Echo the character back. */
			xSerialPutChar( cRxedChar, 100 );

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
#if __UCOS__
			OSSemPost(&_txMutex, (OS_OPT)OS_OPT_POST_1, &err);
			EXT_ASSERT(("Post ConsoleMutex failed"), (err== OS_ERR_NONE));
#else			
			xSemaphoreGive( _txMutex );
#endif
		}
#endif
	}
}

/* mutex output string in console of UART */
void consoleOutput( const char *pcMessage )
{
#if __UCOS__
	OS_ERR err;
	OSSemPend(&_txMutex, (OS_TICK)0, OS_OPT_PEND_BLOCKING, (CPU_TS *)NULL, &err);
	if(err == OS_ERR_NONE)
	{
		vMuxUartPutString(pcMessage, ( unsigned short ) strlen( pcMessage ));
		OSSemPost(&_txMutex, OS_OPT_POST_1, &err);
	}
#else
	if( xSemaphoreTake( _txMutex, cmdMAX_MUTEX_WAIT ) == pdPASS )
	{
		vMuxUartPutString(pcMessage, ( unsigned short ) strlen( pcMessage ));
		xSemaphoreGive( _txMutex );
	}
#endif

}


void consoleTaskStart( uint16_t usStackSize, unsigned long uxPriority )
{
#if __UCOS__
	OS_ERR err;

	OSSemCreate(&_txMutex, "ConsoleMutex", (OS_SEM_CTR)1, &err);
	EXT_ASSERT(("ConsoleMutex can't be created: %x", err), (err==OS_ERR_NONE) );

	OSFlagCreate((OS_FLAG_GRP *)&_consoleRecvedFlagGrp, "ConsoleRecvFlag", (OS_FLAGS)0/* initial value, 0 */, &err);
	EXT_ASSERT(("ConsoleRecvdFlag failed"), (err== OS_ERR_NONE));

	_consoleInitRecvIrq();

	OSTaskCreate(
		(OS_TCB *)&_consoleTaskTcb, 
		(CPU_CHAR *)"console", 
		(OS_TASK_PTR )_consoleTaskMain, 
		(void *)0,	/* arg to task's routine */
		(OS_PRIO )CONSOLE_TASK_PRIO + 2, /* lower number is high priority */
		(CPU_STK *)_consoleTaskStk, /* base address of stack */
		(CPU_STK_SIZE )CONSOLE_TASK_STK_SIZE /10,  /* watermark of stack */
		(CPU_STK_SIZE )CONSOLE_TASK_STK_SIZE, 
		(OS_MSG_QTY )CONSOLE_TASK_Q_SIZE,
		(OS_TICK)0,  
		(void *)0,  
		(OS_OPT )(OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR)/* OS_OPT_NONE*/ , 
		(OS_ERR *)&err);
	if (err != OS_ERR_NONE)
	{
		EXT_ERRORF(("consoleTask failed for Err = %x",  (int)err));
	}

#else
	_txMutex = xSemaphoreCreateMutex();
	configASSERT( _txMutex );

#if 0
	/* Create that task that handles the console itself. */
	xTaskCreate( _prvUartCmdConsoleTask, EXT_TASK_CONSOLE, usStackSize, NULL, uxPriority, NULL );
#else
	sys_thread_new(EXT_TASK_CONSOLE, _consoleTaskMain, NULL, usStackSize, uxPriority);
#endif

#endif
}

#endif	/* CONSOLE_TASK_EN */


