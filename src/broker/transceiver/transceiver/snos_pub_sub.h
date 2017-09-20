#ifndef _SNOS_PUB_SUB_H_
#define _SNOS_PUB_SUB_H_

#include "snos.h"

snOSError snos_initialize_pub_sub(uint8_t (*is_byte_available)(void),uint8_t (*packet_byte_receiver)(void), void (*packet_byte_transmitter)(uint8_t));
snOSError snos_start_pub_sub(void);
snOSError snos_stop_pub_sub(void);
snOSError snos_publish(char *topic, uint8_t topic_n, char *message, uint8_t message_n);
snOSError snos_subscribe(snOSTask *callback, char *topic, uint8_t topic_n);

#endif //_SNOS_PUB_SUB_H_