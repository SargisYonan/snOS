#include <avr/io.h>
#include "led.h"

void turn_on_light(void) {
    DDRB = 0xFF;
    PORTB |= (1<<PB5);
}

void turn_off_light(void) {
    DDRB = 0xFF;
    PORTB &= ~(1<<PB5);
}