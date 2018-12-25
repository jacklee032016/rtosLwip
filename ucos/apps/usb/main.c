/*
*/

#include "ext.h"

#include <lib_mem.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "same70_xplained.h"
#include "same70.h"
#include <xdmac.h>

#define	XDMA_2_UART_TEST				0


#define  UART_DEMO_BANNER				"uC/OS-III: LwIP/Console"

#define  APP_DMA_COMPLETE_FLAG		DEF_BIT_00

#if XDMA_2_UART_TEST
static  uint32_t    DMA_ChannelID;
static  OS_FLAG_GRP AppDMACompleteFlagGrp;
#endif

OS_TCB	mainTaskTCB;
static  CPU_STK mainTaskStk[MAIN_TASK_STK_SIZE];

_CODE char *versionString = EXT_OS_NAME;


//EXT_SYS_T	_extSys;

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

//		OS_ENTER_CRITICAL();
		OS_CRITICAL_ENTER();
		printf("Name: %s\n", sTaskName);
		OS_CRITICAL_EXIT()
//		OS_EXIT_CRITICAL();
		

		/* Delay so other tasks may execute. */
		OSTimeDly(50);
	}			/* while */
}
#endif

#if XDMA_2_UART_TEST
/*
* DMA Controller ISR. Posts to an event flag group when DMA is complete.
*/
void XDMAC_Handler(void)
{
	OS_ERR   err;
	uint32_t dma_status;
	OS_FLAGS 	flag;
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();
	OSIntEnter();                                               /* Tell the OS we're in an interrupt                    */
	CPU_CRITICAL_EXIT();

		
	dma_status = xdmac_channel_get_interrupt_status(XDMAC, DMA_ChannelID);
	if (dma_status & XDMAC_CIS_BIS)
	{
//		EXT_DEBUGF(EXT_DBG_ON, ("XDMAC send event to flags") ); /* it may overwrite the TX hold buffer sent by XDMAC */
		flag = OSFlagPost((OS_FLAG_GRP *)&AppDMACompleteFlagGrp, (OS_FLAGS )APP_DMA_COMPLETE_FLAG, (OS_OPT)OS_OPT_POST_FLAG_SET, &err);
		if(err != OS_ERR_NONE )
		{
			EXT_ERRORF(("FlagPost error '%x'", err));
		}
		//EXT_DEBUGF(EXT_DBG_ON, ("Flag#%x after POST", flag) );
	}


	OSIntExit();
}

static void _initXdmac2Uart(void)
{
	OS_FLAGS flags;
	OS_ERR err;
	static char src_buf[128];
	uint32_t    xfer_len;
	
	OSFlagCreate(&AppDMACompleteFlagGrp, "DMA Complete Flag Group", 0, &err);
	if(err != OS_ERR_NONE )
	{
		EXT_ERRORF(("FlagCreate error '%x'", err));
	}

	pmc_enable_periph_clk(ID_XDMAC);                            /* Enable DMA peripheral clock                          */

	NVIC_ClearPendingIRQ(XDMAC_IRQn);
	NVIC_SetPriority(XDMAC_IRQn, 1);
	NVIC_EnableIRQ(XDMAC_IRQn);                                 /* Enable XDMAC interrupt source                        */

	strcpy(src_buf, "Hello, world from the DMA controller!\r\n"); /* Initialize source buffer string.                     */

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
	}
//	DMA_ChannelID = XDAMC_CHANNEL_HWID_UART0_TX;
	EXT_DEBUGF(EXT_DBG_ON, ("DMA Channel#%ld", DMA_ChannelID) );

	/* Clear the interrupt status for the selected channel  */
	volatile uint32_t dummy_read = XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CIS;
	(void)dummy_read;

	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CSA = (uint32_t)src_buf;       /* Set source address                         */
#ifdef	__SAME70Q20__
	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CDA = (uint32_t)&UART0->UART_THR;/* Set UART Tx hold register as destination*/
#else
	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CDA = (uint32_t)&USART1->US_THR;/* Set USART1 Tx hold register as destination*/
#endif

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
#ifdef	__SAME70Q20__
	                                    XDMAC_CC_PERID(XDAMC_CHANNEL_HWID_UART0_TX);
#else
	                                    XDMAC_CC_PERID(XDAMC_CHANNEL_HWID_USART1_TX);
#endif

	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CNDC = 0;
	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CBC = 0;
	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CDS_MSP = 0;
	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CSUS = 0;
	XDMAC->XDMAC_CHID[DMA_ChannelID].XDMAC_CDUS = 0;


	xdmac_enable_interrupt(XDMAC, DMA_ChannelID);
	xdmac_channel_enable_interrupt(XDMAC, DMA_ChannelID, XDMAC_CIE_BIE);
	
	EXT_DEBUGF(EXT_DBG_ON, ("waiting event from ISR..."));
	/* Start the transfer */
	xdmac_channel_enable(XDMAC, DMA_ChannelID);
//		OSTimeDly(500, 0, &err);
	flags = OSFlagPend(&AppDMACompleteFlagGrp, APP_DMA_COMPLETE_FLAG, (OS_TICK)100, (OS_OPT)OS_OPT_PEND_FLAG_SET_ANY, (CPU_TS *)0, &err);
	if(err != OS_ERR_NONE || flags == 0 )
	{
		EXT_ERRORF(("FlagPend error '%x' or timeout : '%x'", err, flags));
	}
	else
	{
		EXT_DEBUGF(EXT_DBG_ON, ("flags: '%x' after PENDed", flags));
	}
}
#endif

/*
* Application entry point. At this point, the OS has already been initialized and started by libbsp, so
* multitasking has already started and clocks and pins have been configured. To get a handle on the
* task that called main(), declare the MainTaskTCB as 'extern' like so:
*   extern  OS_TCB  MainTaskTCB;
*/
static void mainTask(void *arg)
{
	uint32_t index = 0;
//	EXT_SYS_T	*extSys = (EXT_SYS_T *)arg;
	EXT_RUNTIME_CFG *runCfg = (EXT_RUNTIME_CFG *)arg;

	OS_ERR      err;
	
	board_init();
	LED_On(LED0);
	
	CPU_Init();
	Mem_Init();

	/* Configure and enable the OS tick interrupt */
	OS_CPU_SysTickInit(sysCoreClock);
	
	printf(EXT_OS_NAME);
	puts(UART_DEMO_BANNER EXT_NEW_LINE);
	
#if TASK_CONSOLE_EN
	consoleTaskStart(0, 0);
	EXT_DEBUGF(EXT_DBG_ON, ("Console has been created") );
#endif


	extCfgFromFactory(runCfg);
	if(macTaskStart(runCfg) < 0)
	{
		EXT_ERRORF(("Mac Task start failed"));
		return;
	}
	
	extBspNetStackInit(runCfg);

#if XDMA_2_UART_TEST
	_initXdmac2Uart();
#endif

TRACE();
	while (1)
	{
//		printf(".");
		OSTimeDly((OS_TICK)5000, (OS_OPT)OS_OPT_TIME_DLY, &err);
		//putchar('.');
		index++;

//		LED_TOGGLE(LED0);
	}
}


int main( void )
{
	OS_ERR  err;
//	EXT_SYS_T	*extSys = &_extSys;
	EXT_RUNTIME_CFG *runCfg = &extRun;

	OSInit(&err);/* os and its default tasks, such as idle, tick, timer, stat, queue  */
	if(err != OS_ERR_NONE)
	{
		EXT_ERRORF(("OS initialization failed"));
	}


	OSTaskCreate((OS_TCB *)&mainTaskTCB, 
		(CPU_CHAR *)"Main Task", 
		(OS_TASK_PTR )mainTask, 
		(void *)runCfg,	/* arg to task's routine */
		(OS_PRIO )MAIN_TASK_PRIO-4, /* lower number is high priority */
		(CPU_STK *)mainTaskStk, /* base address of stack */
		(CPU_STK_SIZE )MAIN_TASK_STK_SIZE /10,  /* watermark of stack */
		(CPU_STK_SIZE )MAIN_TASK_STK_SIZE, 
		(OS_MSG_QTY )MAIN_TASK_Q_SIZE,
		(OS_TICK)0,  
		(void *)0,  
		(OS_OPT )(OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR)/* OS_OPT_NONE*/ , 
		(OS_ERR *)&err);

	/* tasks and other kernel objects must be created before OSStart() */
//	TRACE();
	/* normally OS_IntQTask() is the highest */
	OSStart(&err);
	if(err != OS_ERR_NONE)
	{
	}
	EXT_ERRORF(("Scheduler returned"));
	while (1);

	return 1;
}

