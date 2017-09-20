#include "button.h"
#include <avr/io.h>

uint8_t get_button_state(void) {
	DDRC = 0x00;

	if ((PINC & (1 << PC0))) {
        return BUTTON_ON;
    } else {
        return BUTTON_OFF;
    }
}