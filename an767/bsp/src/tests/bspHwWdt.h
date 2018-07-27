
/** Reset counter 10 seconds */
#define RST_MS_COUNTER                    10000
/** Watchdog period 3000ms */
#define WDT_PERIOD                        3000
/** LED blink time 300ms */
#define BLINK_PERIOD                      300
/** Watchdog restart 2000ms */
#define WDT_RESTART_PERIOD                2000

/** System Tick event flag */
volatile bool g_b_systick_event = false;
/** System tick increased by 1 every millisecond */
volatile uint32_t g_ul_ms_ticks = 0;
/** Reset counter decreased by 1 every millisecond */
volatile uint32_t ul_rst_counter;

/**
 *  \brief Handler for System Tick interrupt.
 */
void SysTick_Handler(void)
{
	/* Set systick event flag (g_b_systick_event) and add 1 to systick. */
	g_b_systick_event = true;
	g_ul_ms_ticks++;
	/* Update reset counter */
	ul_rst_counter--;
}

/**
 *  \brief Handler for watchdog interrupt.
 */
void WDT_Handler(void)
{
	puts("Enter watchdog interrupt.\r");

	/* Clear status bit to acknowledge interrupt by dummy read. */
	wdt_get_status(WDT);
	/* Restart the WDT counter. */
	wdt_restart(WDT);
	puts("The watchdog timer was restarted.\r");
}

int bspHwWdtTest(void)
{
	uint32_t wdt_mode, timeout_value;


	/* Initialize reset counter */
	ul_rst_counter = RST_MS_COUNTER;

	/* Systick configuration. */
	puts("Configure systick to get 1ms tick period.\r");
	if (SysTick_Config(sysclk_get_cpu_hz() / 1000)) {
		puts("-F- Systick configuration error\r");
	}

	/* Get timeout value. */
	timeout_value = wdt_get_timeout_value(WDT_PERIOD * 1000,
			BOARD_FREQ_SLCK_XTAL);
	if (timeout_value == WDT_INVALID_ARGUMENT) {
		while (1) {
			/* Invalid timeout value, error. */
		}
	}
	/* Configure WDT to trigger an interrupt (or reset). */
	wdt_mode = WDT_MR_WDFIEN |  /* Enable WDT fault interrupt. */
			WDT_MR_WDRPROC   |  /* WDT fault resets processor only. */
			WDT_MR_WDDBGHLT  |  /* WDT stops in debug state. */
			WDT_MR_WDIDLEHLT;   /* WDT stops in idle state. */
	/* Initialize WDT with the given parameters. */
	wdt_init(WDT, wdt_mode, timeout_value, timeout_value);
	printf("Enable watchdog with %d microseconds period\n\r",
			(int)wdt_get_us_timeout_period(WDT, BOARD_FREQ_SLCK_XTAL));

	/* Configure and enable WDT interrupt. */
	NVIC_DisableIRQ(WDT_IRQn);
	NVIC_ClearPendingIRQ(WDT_IRQn);
	NVIC_SetPriority(WDT_IRQn, 0);
	NVIC_EnableIRQ(WDT_IRQn);

	printf("After %u seconds the system enters in a deadlock loop.\n\r", (int)ul_rst_counter/1000);

	while (1) {

		if (g_b_systick_event == true) {
			g_b_systick_event = false;

			/* Toggle LED at the given period. */
			if ((g_ul_ms_ticks % BLINK_PERIOD) == 0) {
			}

			/* Restart watchdog at the given period. */
			if ((g_ul_ms_ticks % WDT_RESTART_PERIOD) == 0) {
				wdt_restart(WDT);
			}
		}

		/* Simulate deadlock when reset counter is reset. */
		if (!ul_rst_counter) {
			puts("Program enters infinite loop for triggering watchdog interrupt.\r");
			while (1) {
			}
		}
	}
}

