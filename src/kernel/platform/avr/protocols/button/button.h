#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <stdint.h>

#define BUTTON_ON 1
#define BUTTON_OFF 0

uint8_t get_button_state(void);

#endif