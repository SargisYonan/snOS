#include "snos_port.h"
#include "snos_timer.h"

static snOSTimeVector __system_tick_period = 0;
static snOSTimeVector __ticks = 0;

snOSError initialize_snos_port(void) {
	__SNOS_INITIALIZE_SYSTEM_TICK();
	return snOS_SUCCESS;
}

// The system tick period set by snos_set_system_tick_period() defines the
// the period of the system tick pointing to snos_system_tick()
void snos_set_system_tick_period(snOSTimeVector period_ms) {
	__system_tick_period = period_ms;
}

void __SNOS_SYSTEM_TICK(void) {
	snos_tick();
}

snOSTimeVector snos_get_system_time(void) {
	return __ticks * __system_tick_period;
}

#define SNOS_STACK_SIZE (32)
// macros to port specific context switching mechanisms
#define __SNOS_SAVE_CONTEXT NULL
#define __SNOS_RESTORE_CONTEXT NULL

// period_ms must be greater than that set in snos_set_system_tick_period()
void snos_set_context_switch_period(snOSTimeVector period_ms);
void snos_enable_context_switching(void);
void snos_save_context(void);
void snos_restore_context(void);