#ifndef _SNOS_PORT_H_
#define _SNOS_PORT_H_

typedef uint32_t snOSTimeVector;

#define SNOS_STACK_SIZE (32)
// macros to port specific context switching mechanisms
#define __SNOS_SAVE_CONTEXT
#define __SNOS_RESTORE_CONTEXT 

#define SNOS_SYSTEM_TIMER_MAX (65535)
// The system tick period set by snos_set_system_tick_period() defines the
// the period of the system tick pointing to snos_system_tick()
void snos_set_system_tick_period(snOSTimeVector period_ms);
void snos_system_tick(void);

snOSTimeVector snos_get_system_time(void);

// period_ms must be greater than that set in snos_set_system_tick_period()
void snos_set_context_switch_period(snOSTimeVector period_ms);
void snos_enable_context_switching(void);
void snos_save_context(void);
void snos_restore_context(void);

#endif // _SNOS_PORT_H_