#include "uart.h"
#include "uart_driver.h"

void uart_initialize(uint32_t baudrate) {
	_uart_driver_Init(baudrate);
}

/* snOS specified functions for internal uart */
uint8_t receive_byte(void) {
	return ((uint8_t)_uart_driver_GetByte());
}

void send_byte(uint8_t byte) {
	_uart_driver_SendByte(byte);
}

uint8_t byte_is_available(void) {
	return _uart_is_available();
}