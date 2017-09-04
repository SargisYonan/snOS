#include "uart.h"
#include <avr/io.h>

#include "snos.h"
#include "snos_connect.h"

#include <stdlib.h>

#define MINIMUM_LIGHT_THRESHOLD 50
#define TURN_ON_LIGHT_PACKET {0x0A, 0x0B, 0x0C}
#define TURN_ON_LIGHT_PACKET_SIZE 3

uint16_t get_light_value(void) {
	return 49; // fake
}

static snOSTransceiver *light_tx_channel = NULL;
static snOSTransceiver *light_rx_channel = NULL;
static snOSTask *check_light_task = NULL;
static snOSTask *turn_light_task = NULL;

snOSError initialize_light_send_channel(void) {
    light_tx_channel = snos_connect_initialize_channel(check_light_task, &(byte_is_available), &(receive_byte), &(send_byte), TURN_ON_LIGHT_PACKET_SIZE);
    return snos_connect_start(light_tx_channel);
}

snOSError check_light_sensor(void) {
    uint16_t light_value = 0;
    uint8_t packet[3] = TURN_ON_LIGHT_PACKET;

    light_value = get_light_value(); // get_light_value() is a user-defined function to interface with a hardware light sensor
    
    if (light_value < MINIMUM_LIGHT_THRESHOLD) {
        snos_connect_send_packet(light_tx_channel, packet, TURN_ON_LIGHT_PACKET_SIZE);
    }
    
    return snOS_SUCCESS;
}

snOSError initialize_light_recv_channel(void) {
    light_rx_channel = snos_connect_initialize_channel(turn_light_task, &(byte_is_available), &(receive_byte), &(send_byte), TURN_ON_LIGHT_PACKET_SIZE);
    return snos_connect_start(light_rx_channel);;
}

snOSError turn_on_light(void) {
    send_byte('o');
    send_byte('n');

    PORTB |= (1<<PB5);

    return snOS_SUCCESS;
}

int main (void) {
    snOSError ret;
    DDRB = 0xFF;

    uart_initialize(9600);
    ret = snos_initialize();
    send_byte(ret);
    

    // Running on Controller 1
    //check_light_task = snos_new_task(&check_light_sensor, RUN_FOREVER);
    //initialize_light_send_channel();

    // Running on Controller 2
    turn_light_task = snos_new_task(&turn_on_light, RUN_ON_REQUEST);
    initialize_light_recv_channel();

    snos_start();
    

    while(1) {
        // do nothing
    }

    return 0;
}
