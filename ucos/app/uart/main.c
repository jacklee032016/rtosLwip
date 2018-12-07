/*
*/

#include <ucBsp.h>
#include <os.h>
#include <stdio.h>
#include <string.h>
#include <xdmac.h>


#define  UART_DEMO_BANNER				"\nuC/OS-III + Demo: UART with DMA\n"

#define  APP_DMA_COMPLETE_FLAG		DEF_BIT_00

#define  CONSOLE_BAUD_RATE				115200
#define  CONSOLE_CHAR_LENGTH            US_MR_CHRL_8_BIT
#define  CONSOLE_PARITY_TYPE            US_MR_PAR_NO
#define  CONSOLE_STOP_BITS              US_MR_NBSTOP_1_BIT



static  uint32_t    DMA_ChannelID;
static  OS_FLAG_GRP AppDMACompleteFlagGrp;


#define APP_TASK_START_PRIO     0

#define APP_TASK_1_PRIO         11
#define APP_TASK_2_PRIO         12
#define APP_TASK_3_PRIO         13
#define APP_TASK_4_PRIO         14
#define APP_TASK_5_PRIO         15
#define APP_TASK_6_PRIO         16

#define OS_TASK_TMR_PRIO        8



#define APP_TASK_START_STK_SIZE      256

#define APP_TASK_1_STK_SIZE          APP_TASK_START_STK_SIZE
#define APP_TASK_2_STK_SIZE          APP_TASK_START_STK_SIZE




/*
* Initializes stdio functions (printf, puts, scanf, etc...) to be used with the CONSOLE_UART.
*/
#define ioport_set_pin_peripheral_mode(pin, mode) \
	do {\
		ioport_set_pin_mode(pin, mode);\
		ioport_disable_pin(pin);\
	} while (0)

static void App_ConsoleInit(void)
{
	const usart_serial_options_t uart_serial_options =
	{
		.baudrate = CONSOLE_BAUD_RATE,
		.charlength = CONSOLE_CHAR_LENGTH,
		.paritytype = CONSOLE_PARITY_TYPE,
		.stopbits = CONSOLE_STOP_BITS,
	};


	/* Configure console UART. */
#if EXTLAB_BOARD
	/* configure UART pins */
	ioport_set_port_mode(IOPORT_PIOA, PIO_PA9A_URXD0 | PIO_PA10A_UTXD0, IOPORT_MODE_MUX_A);
	ioport_disable_port(IOPORT_PIOA, PIO_PA9A_URXD0 | PIO_PA10A_UTXD0);
	
	sysclk_enable_peripheral_clock(ID_UART0); 	/* 7 */
	
	stdio_serial_init(UART0, &uart_serial_options); /* eg. 0x400E0800U */
	stdio_serial_init(UART0, &uart_serial_options);

#else
	/* Configure USART pins */
//	ioport_set_pin_peripheral_mode(USART1_RXD_GPIO, USART1_RXD_FLAGS);
//	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;
//	ioport_set_pin_peripheral_mode(USART1_TXD_GPIO, USART1_TXD_FLAGS);
	
//	sysclk_enable_peripheral_clock(ID_USART1);		/* 14 */
	stdio_serial_init(USART1, &uart_serial_options); /* eg. 0x40028000U */
#endif

	/* Configure the pins connected to LED as output and set their default initial state to high (LED off).
	 */
	ioport_set_pin_dir(LED0_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED0_GPIO, LED0_INACTIVE_LEVEL);

	ioport_set_pin_level(LED0_GPIO, 0);
	
}

#if 0
/* Function common to all tasks */
void MyTask(void *p_arg)
{
	char *sTaskName = (char *)p_arg;
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR cpu_sr = 0;
#endif

	while (1)
	{
		/*
		 * printf uses mutex to get terminal access, therefore
		 * must enter critical section
		 */

		OS_ENTER_CRITICAL();
		printf("Name: %s\n", sTaskName);
		OS_EXIT_CRITICAL();

		/* Delay so other tasks may execute. */
		OSTimeDly(50);
	}			/* while */
}
#endif

/*
* Application entry point. At this point, the OS has already been initialized and started by libbsp, so
* multitasking has already started and clocks and pins have been configured. To get a handle on the
* task that called main(), declare the MainTaskTCB as 'extern' like so:
*
*   extern  OS_TCB  MainTaskTCB;
*/

int main (void)
{
	static char src_buf[128];
	uint32_t index = 0;

	OS_ERR      err;
	uint32_t    xfer_len;


	App_ConsoleInit();
	puts(UART_DEMO_BANNER);

	printf("Init-1...\n");
	printf("Init-2...\n");

	OSFlagCreate(&AppDMACompleteFlagGrp, "DMA Complete Flag Group", 0, &err);


	pmc_enable_periph_clk(ID_XDMAC);                            /* Enable DMA peripheral clock                          */

	NVIC_ClearPendingIRQ(XDMAC_IRQn);
	NVIC_SetPriority(XDMAC_IRQn, 1);
	NVIC_EnableIRQ(XDMAC_IRQn);                                 /* Enable XDMAC interrupt source                        */

	strcpy(src_buf, "Hello, world from the DMA controller!\n"); /* Initialize source buffer string.                     */

	                                                    /* Find an available DMA channel.                       */
	uint32_t dma_channel_mask  = 1;
	while ((XDMAC->XDMAC_GS & dma_channel_mask) && (DMA_ChannelID < XDMACCHID_NUMBER))
	{
		dma_channel_mask <<= 1;
		++DMA_ChannelID;
	}

	if (DMA_ChannelID == XDMACCHID_NUMBER)
	{
		puts("All DMA channels are unavailable.\n");
		return 1;
	}

	puts(UART_DEMO_BANNER);
	                                                    /* Clear the interrupt status for the selected channel  */
	volatile uint32_t dummy_read = XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CIS;
	(void)dummy_read;

	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CSA = (uint32_t)src_buf;       /* Set source address                         */
	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CDA = (uint32_t)&USART1->US_THR;/* Set USART1 Tx hold register as destination*/

	xfer_len = strlen(src_buf) - 1;
	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CUBC = XDMAC_UBC_UBLEN(xfer_len); /* Set transfer length              */
	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CC = XDMAC_CC_TYPE_PER_TRAN |
	                                    XDMAC_CC_MBSIZE_SINGLE |
	                                    XDMAC_CC_DSYNC_MEM2PER |
	                                    XDMAC_CC_PROT_UNSEC |
	                                    XDMAC_CC_SWREQ_HWR_CONNECTED |
	                                    XDMAC_CC_CSIZE_CHK_1 |
	                                    XDMAC_CC_DWIDTH_BYTE |
	                                    XDMAC_CC_SIF_AHB_IF0 |
	                                    XDMAC_CC_DIF_AHB_IF1 |
	                                    XDMAC_CC_SAM_INCREMENTED_AM |
	                                    XDMAC_CC_DAM_FIXED_AM |
	                                    XDMAC_CC_PERID(XDAMC_CHANNEL_HWID_USART1_TX);
	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CNDC = 0;
	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CBC = 0;
	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CDS_MSP = 0;
	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CSUS = 0;
	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CDUS = 0;


	xdmac_enable_interrupt(XDMAC, DMA_ChannelID);
	xdmac_channel_enable_interrupt(XDMAC, DMA_ChannelID, XDMAC_CIE_BIE);
	                                                    /* Start the transfer                                   */
	xdmac_channel_enable(XDMAC, DMA_ChannelID);


#if 0
	INT8U Stk1[APP_TASK_1_STK_SIZE];
	INT8U Stk2[APP_TASK_2_STK_SIZE];
	INT8U Stk3[APP_TASK_3_STK_SIZE];
	INT8U Stk4[APP_TASK_4_STK_SIZE];
	INT8U Stk5[APP_TASK_5_STK_SIZE];

	char sTask1[] = "Task 1";
	char sTask2[] = "Task 2";


	err = OSTaskCreate(MyTask, sTask1,
	 (void *)&Stk1[STK_HEAD(APP_TASK_1_STK_SIZE)],
	 APP_TASK_1_PRIO);

	if (err != OS_ERR_NONE) {
	printf("OSTaskCreate() failed for %s: Err = %d\n", sTask1,
	   (int)err);
	}
#endif


	OSFlagPend(&AppDMACompleteFlagGrp, APP_DMA_COMPLETE_FLAG, 0, OS_OPT_PEND_FLAG_SET_ALL, 0, &err);

	while (1)
	{
		OSTimeDly(500, 0, &err);
		putchar('.');
		index++;
		//ioport_set_pin_level(LED0_GPIO, (index%2)? LED0_ACTIVE_LEVEL:LED0_INACTIVE_LEVEL);
		ioport_toggle_pin_level(LED0_GPIO);

	//        LED_Toggle(0);
	}
}


/*
* DMA Controller ISR. Posts to an event flag group when DMA is complete.
*/

void XDMAC_Handler(void)
{
	OS_ERR   err;
	uint32_t dma_status;
	CPU_SR_ALLOC();


	CPU_CRITICAL_ENTER();
	OSIntEnter();                                               /* Tell the OS we're in an interrupt                    */
	CPU_CRITICAL_EXIT();


	dma_status = xdmac_channel_get_interrupt_status(XDMAC, DMA_ChannelID);
	if (dma_status & XDMAC_CIS_BIS)
	{
		OSFlagPost(&AppDMACompleteFlagGrp, APP_DMA_COMPLETE_FLAG, 0, &err);
	}


	OSIntExit();
}

