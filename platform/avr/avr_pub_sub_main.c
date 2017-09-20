#include "uart.h"
#include "led.h"
#include "button.h"

#include "snos.h"

#include <stdlib.h>

//#define LIGHT_SWITCHER 1
#define BUTTON_SENDER 1

#ifdef BUTTON_SENDER
    extern snOSError button_event_checker(void) {
        static uint8_t button_state = BUTTON_OFF;

        button_state = get_button_state();

        if (button_state == BUTTON_ON) {
            snos_publish("button", 6, "1", 1);
        } else {
            snos_publish("button", 6, "0", 1);
        }
        
        return snOS_SUCCESS;
    }

    extern snOSError switch_local_light(void) {
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

    extern void led_blink(void) {
        static uint8_t led_state = 0;

        switch (led_state) {
            case 0:
                turn_on_light();
                led_state = 1;
                break;
            case 1:
                turn_off_light();
                led_state = 0;
                break;
            default:
                break;
        }
    }
#endif 

#ifdef LIGHT_SWITCHER
    extern snOSError switch_light_service(void) {
        uint8_t *message = snos_task_get_message(snos_this_task_id());

        if (snos_task_get_message_length(snos_this_task_id()) == 1) {
            switch (message[0]) {
                case '1':
                    turn_on_light();
                    break;
                case '0':
                    turn_off_light();
                    break;
                default:
                    turn_off_light();
                    break;
            }
        }
        
        return snOS_SUCCESS;
    }
#endif


int main (void) {
    uart_initialize(115200);
    //snos_initialize();    

    //snos_initialize_pub_sub(&(byte_is_available), &(receive_byte), &(send_byte));

    #ifdef BUTTON_SENDER
    snOSTimer *led_timer = NULL;
    
    initialize_avr_system_tick();
    initialize_snos_timers();
    led_timer = snos_create_timer(&led_blink, 500);
    snos_start_timer(led_timer);

    turn_off_light();
    while(1){}
        // Running on Controller 1
    //    snos_new_task(&button_event_checker, RUN_FOREVER);
    //    snos_new_task(&switch_local_light, RUN_FOREVER);
    #endif

    #ifdef LIGHT_SWITCHER
        // Running on Controller 2
        snos_subscribe(snos_new_task(&switch_light_service, RUN_ON_REQUEST), "button", 6);
    #endif

    //snos_start_pub_sub();
    snos_start();
    

    while(1);

    return 0;
}
