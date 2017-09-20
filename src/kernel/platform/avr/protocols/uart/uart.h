// uart.h -- Sargis Yonan

#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>

void uart_initialize(uint32_t baudrate);

/* snOS specified functions for internal uart */
uint8_t receive_byte(void);
void send_byte(uint8_t byte);
uint8_t byte_is_available(void);

#endif