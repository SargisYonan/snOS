#ifndef _SNOS_TIMER_H_
#define _SNOS_TIMER_H_

#include "snos_error.h"

struct snos_timer_block_s {
	uint8_t enabled : 1;
	snOSTimeVector start_time_ms;
	snOSTimeVector duration_ms;
	snOSTimeVector elapsed_ms;
	snOSError (*timer_callback)(void);
} snOSTimer;

void snos_tick(void);

snOSError snos_enable_system_timer(void);
snOSError snos_disable_system_timer(void);

snOSTimer *snos_create_timer(snOSError (*timer_callback)(void), snOSTimeVector duration_ms);
snOSError snos_disable_timer(snOSTimer *timer);
snOSError snos_start_timer(snOSTimer *timer);
snOSError snos_set_duration(snOSTimer *timer, snOSTimeVector duration_ms);
snOSError snos_remove_timer(snOSTimer *timer);
snOSError snos_change_callback(snOSTimer *timer, snOSError (*new_timer_callback)(void));


#endif // _SNOS_TIMER_H_