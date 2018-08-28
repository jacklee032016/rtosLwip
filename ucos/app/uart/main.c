/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at: https://doc.micrium.com
*
*               You can contact us at: www.micrium.com
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*
*                                          UART with DMA Demo
*
* Filename      : main.c
* Version       : 1.01
* Programmer(s) : JPC
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include <asf.h>
#include <os.h>
#include <stdio.h>
#include <string.h>
#include <xdmac.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  UART_DEMO_BANNER               "uC/OS-III + ASF Demo: UART with DMA "

#define  APP_DMA_COMPLETE_FLAG          DEF_BIT_00

#define  CONSOLE_UART                   UART0 //USART1
#define  CONSOLE_BAUD_RATE              115200
#define  CONSOLE_CHAR_LENGTH            US_MR_CHRL_8_BIT
#define  CONSOLE_PARITY_TYPE            US_MR_PAR_NO
#define  CONSOLE_STOP_BITS              US_MR_NBSTOP_1_BIT


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

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


/*
****************************************************************************************************
*                                         TASK STACK SIZES
****************************************************************************************************
*/

#define APP_TASK_START_STK_SIZE      256

#define APP_TASK_1_STK_SIZE          APP_TASK_START_STK_SIZE
#define APP_TASK_2_STK_SIZE          APP_TASK_START_STK_SIZE




/*
* Initializes stdio functions (printf, puts, scanf, etc...) to be used with the CONSOLE_UART.
*/

static void App_ConsoleInit(void)
{
	const usart_serial_options_t uart_serial_options =
	{
		.baudrate = CONSOLE_BAUD_RATE,
		.charlength = CONSOLE_CHAR_LENGTH,
		.paritytype = CONSOLE_PARITY_TYPE,
		.stopbits = CONSOLE_STOP_BITS,
	};

	stdio_serial_init(CONSOLE_UART, &uart_serial_options);
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
    
    OS_ERR      err;
    uint32_t    xfer_len;
    
    
    App_ConsoleInit();
    puts(UART_DEMO_BANNER);
    
    
    OSFlagCreate(&AppDMACompleteFlagGrp, "DMA Complete Flag Group", 0, &err);
    
    
    pmc_enable_periph_clk(ID_XDMAC);                            /* Enable DMA peripheral clock                          */
    
    NVIC_ClearPendingIRQ(XDMAC_IRQn);
    NVIC_SetPriority(XDMAC_IRQn, 1);
    NVIC_EnableIRQ(XDMAC_IRQn);                                 /* Enable XDMAC interrupt source                        */
    
    strcpy(src_buf, "Hello, world from the DMA controller!\n"); /* Initialize source buffer string.                     */
    
                                                                /* Find an available DMA channel.                       */
    uint32_t dma_channel_mask  = 1;
    while ((XDMAC->XDMAC_GS & dma_channel_mask) && (DMA_ChannelID < XDMACCHID_NUMBER)) {
        dma_channel_mask <<= 1;
        ++DMA_ChannelID;
    }
    
    if (DMA_ChannelID == XDMACCHID_NUMBER) {
        puts("All DMA channels are unavailable.\n");
        return 1;
    }
    
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

    if (dma_status & XDMAC_CIS_BIS) {
        OSFlagPost(&AppDMACompleteFlagGrp, APP_DMA_COMPLETE_FLAG, 0, &err);
    }
    
    
    OSIntExit();
}

