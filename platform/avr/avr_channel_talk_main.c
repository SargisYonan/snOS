#include "uart.h"
#include <avr/io.h>

#include "snos.h"
#include "snos_connect.h"

#include <stdlib.h>

//#define LIGHT_SWITCHER 1
#define BUTTON_SENDER 1

#define BUTTON_ON 1
#define BUTTON_OFF 0

#define BUTTON_STATE_PACKET_SIZE 1

uint16_t get_button_state(void) {
	if ((PINC & (1 << PC0))) {
        return BUTTON_ON;
    } else {
        return BUTTON_OFF;
    }
}
void turn_on_light(void) {
    PORTB |= (1<<PB5);
}

void turn_off_light(void) {
    PORTB &= ~(1<<PB5);
}

#ifdef BUTTON_SENDER
    static snOSTransceiver *button_tx_channel = NULL;
    static snOSTask *check_button_task = NULL;
    static snOSTask *local_switcher_task = NULL;

    snOSError initialize_button_send_channel(void) {
        button_tx_channel = snos_connect_initialize_channel(check_button_task, &(byte_is_available), &(receive_byte), &(send_byte), BUTTON_STATE_PACKET_SIZE);
        return snos_connect_start(button_tx_channel);
    }

    snOSError button_event_checker(void) {
        static uint8_t button_state = BUTTON_OFF;

        if (button_state != get_button_state()) {
            button_state = get_button_state();
        }
        
        snos_connect_send_packet(button_tx_channel, &button_state, BUTTON_STATE_PACKET_SIZE);
        
        return snOS_SUCCESS;
    }

    snOSError switch_local_light(void) {
        switch (get_button_state()) {
            case BUTTON_ON:
                turn_on_light();
                break;
            case BUTTON_OFF:
                turn_off_light();
                break;
            default:
                turn_off_light();
                break;
        }

        return snOS_SUCCESS;
    }
#endif 

#ifdef LIGHT_SWITCHER
    static snOSTransceiver *button_rx_channel = NULL;
    static snOSTask *switch_light_task = NULL;

    snOSError initialize_switch_channel(void) {
        button_rx_channel = snos_connect_initialize_channel(switch_light_task, &(byte_is_available), &(receive_byte), &(send_byte), BUTTON_STATE_PACKET_SIZE);
        return snos_connect_start(button_rx_channel);;
    }

    snOSError switch_light_service(void) {
        uint8_t received_button_state = 0;
        snos_connect_get_packet(button_rx_channel, &received_button_state, BUTTON_STATE_PACKET_SIZE);

        switch (received_button_state) {
            case BUTTON_ON:
                turn_on_light();
                break;
            case BUTTON_OFF:
                turn_off_light();
                break;
            default:
                turn_off_light();
                break;
        }

        return snOS_SUCCESS;
    }
#endif

int main (void) {
    snOSError ret;
    DDRB = 0xFF;
    DDRC = 0x00;

    uart_initialize(9600);
    ret = snos_initialize();
    send_byte(ret);
    

    #ifdef BUTTON_SENDER
        // Running on Controller 1
        check_button_task = snos_new_task(&button_event_checker, RUN_FOREVER);
        initialize_button_send_channel();

        local_switcher_task = snos_new_task(&switch_local_light, RUN_FOREVER);
    #endif

    #ifdef LIGHT_SWITCHER
        // Running on Controller 2
        switch_light_task = snos_new_task(&switch_light_service, RUN_ON_REQUEST);
        initialize_switch_channel();
    #endif

    snos_start();
    

    while(1) {
        // do nothing
    }

    return 0;
}
