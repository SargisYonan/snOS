#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#ifdef __AVR_328P__

#include <avr/io.h>
#include <avr/interrupt.h>

void initialize_avr_system_tick(void);

void initialize_system_tick_interrupt(void (*interrupt_callback)(void), const uint64_t ms);
void set_system_tick_period(const uint64_t ms);
void stop_system_tick_interrupt(void);

#endif // __AVR_328P__
#endif // _INTERRUPTS_H_