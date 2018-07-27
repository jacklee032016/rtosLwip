
/**
 * \brief Refresh display on terminal.
 *
 * Update the terminal display to show the current menu and the current time
 * depending on the device state.
 */
static void refresh_display(void)
{
	printf("%c[2J\r", 27);
	printf("Time: %u\n\r", (unsigned int)rtt_read_timer_value(RTT));

}

/**
 * \brief RTT configuration function.
 *
 * Configure the RTT to generate a one second tick, which triggers the RTTINC interrupt.
 */
void configure_rtt(void)
{
	uint32_t ul_previous_time;

	/* Configure RTT for a 1 second tick interrupt */
	rtt_sel_source(RTT, false);
	rtt_init(RTT, 32768);

	ul_previous_time = rtt_read_timer_value(RTT);
	while (ul_previous_time == rtt_read_timer_value(RTT));

	/* Enable RTT interrupt */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 0);
	NVIC_EnableIRQ(RTT_IRQn);
	rtt_enable_interrupt(RTT, RTT_MR_RTTINCIEN);
}

/**
 * \brief Interrupt handler for the RTT.
 *
 * Display the current time on the terminal.
 */
void RTT_Handler(void)
{
	uint32_t ul_status;

	/* Get RTT status */
	ul_status = rtt_get_status(RTT);

	/* Time has changed, refresh display */
	if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) {
		refresh_display();
	}

	/* Alarm */
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
		g_uc_alarmed = 1;
		refresh_display();
	}
}


void testSleepmgr()
{
	/*
	 * Turn the activity status LED on to inform the user that the device
	 * is active.
	 */
	ioport_set_pin_level(LED_ACTIVITY_STATUS_PIN, LED_STATUS_ON);

	rtt_init(RTT, 32768);

	/* Enable RTT interrupt */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 0);
	NVIC_EnableIRQ(RTT_IRQn);
	rtt_enable_interrupt(RTT, RTT_MR_ALMIEN);

	/* Set wakeup source to rtt_alarm */
	pmc_set_fast_startup_input(PMC_FSMR_RTTAL);
#if (!(SAMG51 || SAMG53 || SAMG54))
	supc_set_wakeup_mode(SUPC, SUPC_WUMR_RTTEN_ENABLE);
#endif
	/* Initialize the sleep manager, lock initial mode. */
	sleepmgr_init();
	sleepmgr_lock_mode(current_sleep_mode);

	while (1) {

		rtt_write_alarm_time(RTT, rtt_read_timer_value(RTT) + SLEEP_TIME);
		/*
		 * Turn the activity status LED off to inform the user that the
		 * device is in a sleep mode.
		 */
		ioport_set_pin_level(LED_ACTIVITY_STATUS_PIN, LED_STATUS_OFF);

		/*
		 * Go to sleep in the deepest allowed sleep mode (i.e. no
		 * deeper than the currently locked sleep mode).
		 */
		sleepmgr_enter_sleep();

		/*
		 * Turn the activity status LED on to inform the user that the
		 * device is active.
		 */
		ioport_set_pin_level(LED_ACTIVITY_STATUS_PIN, LED_STATUS_ON);

		/* Unlock the current sleep mode. */
		sleepmgr_unlock_mode(current_sleep_mode);

		/* Add a 3s delay. */
		delay_s(ACTIVE_TIME);

		/* Lock the next sleep mode. */
		++current_sleep_mode;
		if ((current_sleep_mode >= SLEEPMGR_NR_OF_MODES)) {
			current_sleep_mode = SLEEPMGR_ACTIVE;
		}

		sleepmgr_lock_mode(current_sleep_mode);
	}
}

