#ifndef _SNOS_BROKER_H_
#define _SNOS_BROKER_H_

#include <stdint.h>

int8_t snos_initialize_listener(void);
int8_t snos_initialize_broker(void);

int8_t snos_subscribe_all_topics(void);
int8_t snos_subscribe_topic(const char *topic);

int8_t snos_publish(const char *topic, const char *message);

int8_t snos_switch_message(const char *src_topic, const char *dst_topic);

#endif // _SNOS_BROKER_H_