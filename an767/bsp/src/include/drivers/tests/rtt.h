/**
 * \Real-time Timer (RTT) driver for SAM.
 *
 */

#ifndef RTT_H_INCLUDED
#define RTT_H_INCLUDED

#include "compiler.h"

uint32_t rtt_init(Rtt *p_rtt, uint16_t us_prescaler);
void rtt_sel_source(Rtt *p_rtt, bool is_rtc_sel);
void rtt_enable(Rtt *p_rtt);
void rtt_disable(Rtt *p_rtt);
void rtt_enable_interrupt(Rtt *p_rtt, uint32_t ul_sources);
void rtt_disable_interrupt(Rtt *p_rtt, uint32_t ul_sources);
uint32_t rtt_read_timer_value(Rtt *p_rtt);
uint32_t rtt_get_status(Rtt *p_rtt);
uint32_t rtt_write_alarm_time(Rtt *p_rtt, uint32_t ul_alarm_time);


#endif

