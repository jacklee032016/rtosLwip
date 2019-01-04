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
#include "usart.h"

#include "lwipExt.h"

#include "eos.h"
#include "jsmn.h"

#define	RS232_DEFAULT_TIMEOUT_1S			1000

typedef	struct _RS232_CTRL
{
	uint32_t		writeIndex;
	uint32_t		readIndex;
	sys_sem_t	rxSema;
	uint32_t		timeoutMs; 
	
	char			buffer[HEX_DATA_MAX_LENGTH];
}RS232_CTRL;


static RS232_CTRL		_rs232Dev;

#if 0
static uint8_t		_rs232InputChar;
static sys_sem_t	_rs232RxSem;
static char cInputString[ RS232_INPUT_SIZE ];
#endif

void USART1_Handler(void)
{
	volatile uint32_t status;
	RS232_CTRL *rs232Dev = &_rs232Dev;
//	portBASE_TYPE 	rs232TaskWaking = pdFALSE;
	
	status = usart_get_status(USART1);
	if (status & (US_CSR_OVRE | US_CSR_FRAME | US_CSR_PARE))
	{
		USART1->US_CR = US_CR_RSTSTA;
		EXT_ERRORF(("RS232 status error: 0x%lx, return", status) );
		return;
	}
	
	if(( status & US_CSR_RXRDY) )
	{
		rs232Dev->buffer[rs232Dev->writeIndex] = (uint8_t)USART1->US_RHR;
		
#if EXT_RS232_DEBUG
		printf("USART1: %ld:'0x%x' ",rs232Dev->writeIndex, (char )rs232Dev->buffer[rs232Dev->writeIndex]);
#endif		
		rs232Dev->writeIndex = (rs232Dev->writeIndex+1)%HEX_DATA_MAX_LENGTH;
//		xSemaphoreGiveFromISR(_rs232RxSem/* not pointer*/, &rs232TaskWaking);
	}
	
//	portEND_SWITCHING_ISR(rs232TaskWaking);

}



static void _rs232InitRecvIrq(void)
{
	NVIC_ClearPendingIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn ,1);

	usart_enable_interrupt(USART1, (US_IER_RXRDY | US_IER_OVRE |US_IER_FRAME|US_IER_PARE) );
	
	/* Enable interrupt  */
//	NVIC_EnableIRQ(USART1_IRQn);
}

void rs232StartRx(void)
{/* init IRQ, index of buffer */
	RS232_CTRL *rs232Dev = &_rs232Dev;
	
	rs232Dev->writeIndex = 0;
	rs232Dev->readIndex = 0;
	rs232Dev->timeoutMs = RS232_DEFAULT_TIMEOUT_1S;
	memset(rs232Dev->buffer, 0, HEX_DATA_MAX_LENGTH);

	/* reset register, so clear RS232 ERROR IRQ */
	USART1->US_CR = US_CR_RSTSTA;
	/* clear existed content in hw */
	rs232Dev->buffer[rs232Dev->writeIndex] = (uint8_t)USART1->US_RHR;
	
	/* Enable interrupt  */
	NVIC_EnableIRQ(USART1_IRQn);
}

void rs232StopRx(void)
{
	/* Disable interrupt  */
	NVIC_DisableIRQ(USART1_IRQn);
}

/* return buffer length */
uint32_t rs232StartRead(uint16_t timeout, char *buf)
{/* init IRQ, index of buffer */
	u32_t timeElapes = 0;
	RS232_CTRL *rs232Dev = &_rs232Dev;
	char ret;
	uint32_t i;
	
	timeElapes = sys_arch_sem_wait(&rs232Dev->rxSema, (timeout<= 0)?rs232Dev->timeoutMs:timeout);
	if(timeElapes == SYS_ARCH_TIMEOUT)
	{
#if EXT_RS232_DEBUG
		EXT_DEBUGF(EXT_DBG_ON, ("Timeout RS232, RX: Total %ld rxed", rs232Dev->writeIndex) );
#endif
		for(i=0; i< rs232Dev->writeIndex; i++)
		{
			extBcd2Ascii(rs232Dev->buffer[i], buf+i*2);
		}	

//		memcpy(buf, rs232Dev->buffer, rs232Dev->writeIndex);
		ret = rs232Dev->writeIndex*2;
	}
	else
	{
		EXT_ERRORF(("Wakeup RS232 RX") );
		ret = EXIT_FAILURE;
	}

	/* Disable interrupt  */
	NVIC_DisableIRQ(USART1_IRQn);

	return ret;
}


#if 0

static char _rs232GetChar( signed char *pcRxedChar, TickType_t xBlockTime )
{
	u32_t timeElapes = sys_arch_sem_wait(&_rs232RxSem, xBlockTime);
	if(timeElapes == SYS_ARCH_TIMEOUT)
	{
		return EXIT_FAILURE;
	}
	else
	{
		*pcRxedChar = _rs232InputChar;
		EXT_INFOF(("Wakeup RS232 RX: %c", (char )*pcRxedChar));
		return EXIT_SUCCESS;
	}
}


static void _rs232ReadTask( void *pvParameters )
{
	signed char cRxedChar;
	uint8_t ucInputIndex = 0;
//	char	ret;

	memset( cInputString, 0x00, HEX_DATA_MAX_LENGTH );

	for( ;; )
	{
		while( _rs232GetChar(&cRxedChar, 100 ) != EXIT_SUCCESS);

		if( ucInputIndex < HEX_DATA_MAX_LENGTH )
		{
			printf("0x%x ", cRxedChar);
			cInputString[ ucInputIndex ] = cRxedChar;
			ucInputIndex++;
		}
	}
}
#endif

void rs232TaskStart( uint16_t usStackSize, unsigned long uxPriority )
{
	err_t err;
	RS232_CTRL *rs232Dev = &_rs232Dev;

	memset(rs232Dev, 0, sizeof(RS232_CTRL));

	/* Incoming packet notification semaphore. */
	err = sys_sem_new(&rs232Dev->rxSema, 0);
	EXT_ASSERT(("RS232: RX semaphore allocation ERROR!"), (err == ERR_OK));
	if (err != ERR_OK)
		return;

	_rs232InitRecvIrq();

//	sys_thread_new(EXT_TASK_RS232, _rs232ReadTask, rs232Dev, usStackSize, uxPriority);
}


