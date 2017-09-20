#ifndef _SNOS_PORT_H_
#define _SNOS_PORT_H_

#define __AVR_328P__

#include <stdint.h>
#include "interrupts.h"
#include "snos_error_codes.h"

typedef uint64_t snOSTimeVector;

snOSError initialize_snos_port(void);

#define __SNOS_SYSTEM_TICK_PERIOD_MS (1)
#define SNOS_SYSTEM_TIMER_MAX (0xFFFFFFFFFFFFFFFF)

#define __SNOS_ENABLE_INTERRUPTS sei
#define __SNOS_DISABLE_INTERRUPTS cli
#define __SNOS_INITIALIZE_SYSTEM_TICK (initialize_avr_system_tick)

// The system tick period set by snos_set_system_tick_period() defines the
// the period of the system tick pointing to snos_system_tick()
extern void snos_set_system_tick_period(snOSTimeVector period_ms);
extern void __SNOS_SYSTEM_TICK(void);
snOSTimeVector snos_get_system_time(void);

#define SNOS_STACK_SIZE (32)
// macros to port specific context switching mechanisms
#define __SNOS_SAVE_CONTEXT NULL
#define __SNOS_RESTORE_CONTEXT NULL

// period_ms must be greater than that set in snos_set_system_tick_period()
void snos_set_context_switch_period(snOSTimeVector period_ms);
void snos_enable_context_switching(void);
void snos_save_context(void);
void snos_restore_context(void);

#endif // _SNOS_PORT_H_