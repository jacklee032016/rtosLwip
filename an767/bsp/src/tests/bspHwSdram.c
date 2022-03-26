
/** SDRAM benchmark test size */
#define SDRAMC_TEST_BUFF_SIZE        (30 * 1024)

/** SDRAM benchmark test page number */
#define SDRAMC_TEST_PAGE_NUM        (1024)

/** SDRAMC access test length */
#define SDRAMC_TEST_LENGTH        (10 * 1024)

/** SDRAMC access test data in even address */
#define SDRAMC_TEST_EVEN_TAG        (0x55aaaa55)

/** SDRAMC access test data in odd address */
#define SDRAMC_TEST_ODD_TAG        (0xaa5555aa)

/** SDRAM benchmark buffer */
static volatile uint16_t gs_pus_mem_buff[SDRAMC_TEST_BUFF_SIZE];

/* Ticks in 1ms */
static volatile uint32_t gs_ul_ms_ticks = 0;

/** SDRAMC status */
typedef enum {
	SDRAMC_OK = 0,
	SDRAMC_TIMEOUT = 1,
	SDRAMC_ERROR = 2,
} sdramc_status_t;

/**
 * \brief Systick handler to manage timestamp and delay.
 */
void SysTick_Handler(void)
{
	gs_ul_ms_ticks++;
}

/**
 *  \brief Get current tick count (in ms).
 *
 * \return Current tick count value.
 */
static uint32_t tick_get_count(void)
{
	return gs_ul_ms_ticks;
}

/**
 * \brief Get delayed number of ticks.
 *
 * \param ul_start_tick Start tick point.
 * \param ul_end_tick   End tick point.
 *
 * \return Delayed number of ticks.
 */
static uint32_t get_delay_in_ticks(uint32_t ul_start_tick, uint32_t ul_end_tick)
{
	if (ul_end_tick > ul_start_tick) {
		return (ul_end_tick - ul_start_tick);
	}

	return (ul_end_tick + (0xFFFFFFFF - ul_start_tick));
}

/**
 * \brief Access test of the first 10K bytes of external SDRAM.
 *
 * \return SDRAMC_OK if test is passed, else SDRAMC_ERROR.
 */
static uint8_t sdram_access_test(void)
{
	uint32_t i;
	uint32_t *pul = (uint32_t *)BOARD_SDRAM_ADDR;

	for (i = 0; i < SDRAMC_TEST_LENGTH; ++i) {
		if (i & 1) {
			pul[i] = SDRAMC_TEST_ODD_TAG | (1 << i);
		} else {
			pul[i] = SDRAMC_TEST_EVEN_TAG | (1 << i);
		}
	}

	for (i = 0; i < SDRAMC_TEST_LENGTH; ++i) {
		if (i & 1) {
			if (pul[i] != (SDRAMC_TEST_ODD_TAG | (1 << i))) {
				return SDRAMC_ERROR;
			}
		} else {
			if (pul[i] != (SDRAMC_TEST_EVEN_TAG | (1 << i))) {
				return SDRAMC_ERROR;
			}
		}
	}

	return SDRAMC_OK;
}

/**
 * \brief SDRAM benchmark test.
 *
 * \return SDRAMC_OK for success and SDRAMC_ERROR for error.
 */
static uint32_t sdram_benchmarks(void)
{
	uint16_t i;
	uint16_t *pus = (uint16_t *)BOARD_SDRAM_ADDR;
	uint32_t ul_tick_start, ul_tick_end, ul_rw_speed;
	uint32_t ul_ticks = 0;
	uint32_t ul_page_num = 0;

	puts("Start SDRAMC benchmarks.\r");

	for (i = 0; i < SDRAMC_TEST_BUFF_SIZE; i++) {
		gs_pus_mem_buff[i] = (uint16_t)(i & 0xffff);
	}

	/* Start SDRAM write test */
	ul_tick_start = tick_get_count();
	for (ul_page_num = 0; ul_page_num < SDRAMC_TEST_PAGE_NUM;
			ul_page_num++) {
		for (i = 0; i < SDRAMC_TEST_BUFF_SIZE; i++) {
			pus[i + ul_page_num *
			SDRAMC_TEST_BUFF_SIZE]
				= gs_pus_mem_buff[i];
		}
	}
	ul_tick_end = tick_get_count();
	ul_ticks = get_delay_in_ticks(ul_tick_start, ul_tick_end);
	ul_rw_speed = SDRAMC_TEST_BUFF_SIZE * SDRAMC_TEST_PAGE_NUM *
			sizeof(uint16_t) / ul_ticks;
	printf("SDRAMC write speed: %uK/s\n\r", (uint32_t)ul_rw_speed);

	/* Start SDRAM read test */
	ul_tick_start = tick_get_count();
	for (ul_page_num = 0; ul_page_num < SDRAMC_TEST_PAGE_NUM;
			ul_page_num++) {
		for (i = 0; i < SDRAMC_TEST_BUFF_SIZE; i++) {
			gs_pus_mem_buff[i]
				= pus[i + ul_page_num *
					SDRAMC_TEST_BUFF_SIZE];
		}
	}
	ul_tick_end = tick_get_count();
	ul_ticks = get_delay_in_ticks(ul_tick_start, ul_tick_end);
	ul_rw_speed = SDRAMC_TEST_BUFF_SIZE * SDRAMC_TEST_PAGE_NUM *
			sizeof(uint16_t) / ul_ticks;
	printf("SDRAMC read speed: %uK/s\n\r", (uint32_t)ul_rw_speed);

	for (i = 0; i < SDRAMC_TEST_BUFF_SIZE; i++) {
		if (gs_pus_mem_buff[i] != (i & 0xffff)) {
			puts("-F- SDRAMC read data error\r");
			return SDRAMC_ERROR;
		}
	}

	return SDRAMC_OK;
}

void bspHwSdramInit(void)
{
	pio_configure_pin(SDRAM_BA0_PIO, SDRAM_BA0_FLAGS);
	pio_configure_pin(SDRAM_SDCK_PIO, SDRAM_SDCK_FLAGS);
	pio_configure_pin(SDRAM_SDCKE_PIO, SDRAM_SDCKE_FLAGS);
	pio_configure_pin(SDRAM_SDCS_PIO, SDRAM_SDCS_FLAGS);
	pio_configure_pin(SDRAM_RAS_PIO, SDRAM_RAS_FLAGS);
	pio_configure_pin(SDRAM_CAS_PIO, SDRAM_CAS_FLAGS);
	pio_configure_pin(SDRAM_SDWE_PIO, SDRAM_SDWE_FLAGS);
	pio_configure_pin(SDRAM_NBS0_PIO, SDRAM_NBS0_FLAGS);
	pio_configure_pin(SDRAM_NBS1_PIO, SDRAM_NBS1_FLAGS);
	pio_configure_pin(SDRAM_A2_PIO, SDRAM_A_FLAGS);  
	pio_configure_pin(SDRAM_A3_PIO, SDRAM_A_FLAGS);  
	pio_configure_pin(SDRAM_A4_PIO, SDRAM_A_FLAGS);  
	pio_configure_pin(SDRAM_A5_PIO, SDRAM_A_FLAGS);  
	pio_configure_pin(SDRAM_A6_PIO, SDRAM_A_FLAGS);  
	pio_configure_pin(SDRAM_A7_PIO, SDRAM_A_FLAGS);  
	pio_configure_pin(SDRAM_A8_PIO, SDRAM_A_FLAGS);  
	pio_configure_pin(SDRAM_A9_PIO, SDRAM_A_FLAGS);  
	pio_configure_pin(SDRAM_A10_PIO, SDRAM_A_FLAGS);  
	pio_configure_pin(SDRAM_A11_PIO, SDRAM_A_FLAGS);  
	pio_configure_pin(SDRAM_SDA10_PIO, SDRAM_SDA10_FLAGS);
	pio_configure_pin(SDRAM_D0_PIO, SDRAM_D_FLAGS);  
	pio_configure_pin(SDRAM_D1_PIO, SDRAM_D_FLAGS);  
	pio_configure_pin(SDRAM_D2_PIO, SDRAM_D_FLAGS);  
	pio_configure_pin(SDRAM_D3_PIO, SDRAM_D_FLAGS);  
	pio_configure_pin(SDRAM_D4_PIO, SDRAM_D_FLAGS);  
	pio_configure_pin(SDRAM_D5_PIO, SDRAM_D_FLAGS);  
	pio_configure_pin(SDRAM_D6_PIO, SDRAM_D_FLAGS);  
	pio_configure_pin(SDRAM_D7_PIO, SDRAM_D_FLAGS);  
	pio_configure_pin(SDRAM_D8_PIO, SDRAM_D_FLAGS);  
	pio_configure_pin(SDRAM_D9_PIO, SDRAM_D_FLAGS);  
	pio_configure_pin(SDRAM_D10_PIO, SDRAM_D_FLAGS); 
	pio_configure_pin(SDRAM_D11_PIO, SDRAM_D_FLAGS); 
	pio_configure_pin(SDRAM_D12_PIO, SDRAM_D_FLAGS); 
	pio_configure_pin(SDRAM_D13_PIO, SDRAM_D_FLAGS); 
	pio_configure_pin(SDRAM_D14_PIO, SDRAM_D_FLAGS); 
	pio_configure_pin(SDRAM_D15_PIO, SDRAM_D_FLAGS); 
	
	MATRIX->CCFG_SMCNFCS = CCFG_SMCNFCS_SDRAMEN;

	sleepmgr_init();


	/* Systick configuration */
	if (SysTick_Config(SystemCoreClock / (1000))) {
		puts("-F- Systick configuration error.\r");
	}

	/* Enable SDRAMC peripheral clock */
	pmc_enable_periph_clk(ID_SDRAMC);

	/* Complete SDRAM configuration */
	sdramc_init((sdramc_memory_dev_t *)&SDRAM_ISSI_IS42S16100E, sysclk_get_cpu_hz());
	sdram_enable_unaligned_support();

	/* Test external SDRAM access */
	puts("Test external SDRAM access. \r");
	
	SCB_CleanInvalidateDCache();
	
	if (sdram_access_test() == SDRAMC_OK) {
		puts("SDRAM access is successful.\n\r");
	} else {
		puts("SDRAM access is failed.\r");
	}

	if (sdram_benchmarks() == SDRAMC_OK) {
		puts("SDRAM test: pass.\r");
	}

	for (;;) {
		sleepmgr_enter_sleep();
	}

}

