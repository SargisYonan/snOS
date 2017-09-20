#include "interrupts.h"
#include "snos_port.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static void (*__isr_callback)(void);

uint64_t _isr_timer_ms = 1; // Start count from here, down to 0. Default 1 ms.
uint64_t _isr_countdown = 0; // Current internal count of 1ms ticks


void initialize_system_tick_interrupt(void (*interrupt_callback)(void), const uint64_t ms) {
	// AVR timer/counter controller register TCCR1
	// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s
	TCCR1B = 0x0B;
	// AVR output compare register OCR1A.
	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	OCR1A = 125;
	// AVR timer interrupt mask register
	// bit1: OCIE1A -- enables compare match interrupt
	TIMSK1 = 0x02;
	//Initialize avr counter
	TCNT1=0;
	
	_isr_countdown = ms;
	__isr_callback = interrupt_callback;

	//Enable global interrupts: 0x80: 1000000
	SREG |= 0x80;
}

void stop_system_tick_interrupt(void) {
	// bit3bit1bit0=000: timer off
	TCCR1B = 0x00;
}

ISR(TIMER1_COMPA_vect) {
	if (_isr_countdown > 0) {
		_isr_countdown--;
		if (_isr_countdown == 0) {
			__isr_callback();
			_isr_countdown = _isr_timer_ms;
		}
	}
}

void set_system_tick_period(const uint64_t ms) {
	_isr_timer_ms = ms;
	_isr_countdown = _isr_timer_ms;
	snos_set_system_tick_period(ms);
}


void initialize_avr_system_tick(void) {
	initialize_system_tick_interrupt(&__SNOS_SYSTEM_TICK, __SNOS_SYSTEM_TICK_PERIOD_MS);
	snos_set_system_tick_period(__SNOS_SYSTEM_TICK_PERIOD_MS);
}