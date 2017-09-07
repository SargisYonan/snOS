#include "snos_pub_sub.h"
#include "snos_connect.h"
#include "list.h"
#include <stdlib.h>
#include <string.h>

#define MAX_PUB_SUB_PACKET_SIZE (255)
#define MIN_PUB_SUB_PACKET_SIZE (3)

typedef struct pub_sub_task_s {
	snOSTask *handler;
	uint8_t *subscribed_topic;
	uint8_t topic_length;
} snOSSub;

static snOSTransceiver *__pub_sub_channel = NULL;
static uint8_t __initialized = 0;
static uint8_t __enabled = 0;
static list_t *__subscription_list = NULL;
static snOSTask *__pub_sub_task = NULL;

static uint8_t _already_subscribed(snOSTask *task);
extern snOSError __snos_pub_sub_handler(void);

snOSError snos_initialize_pub_sub(uint8_t (*is_byte_available)(void),uint8_t (*packet_byte_receiver)(void), void (*packet_byte_transmitter)(uint8_t)) {
	snOSError ret = snOS_ERROR;
	
	if (!__subscription_list) {
		__subscription_list = list_create();
	}

	if (!__pub_sub_task) {
		__pub_sub_task = snos_new_task(&__snos_pub_sub_handler, RUN_ON_REQUEST);
	}

	if (!__pub_sub_channel && __subscription_list) {
		__pub_sub_channel = snos_connect_initialize_channel(__pub_sub_task, is_byte_available, packet_byte_receiver, packet_byte_transmitter, MAX_PUB_SUB_PACKET_SIZE);
	}

	if (__pub_sub_channel) {
		__initialized = 1;
		__enabled = 0;
	}

	return ret;
}

snOSError snos_start_pub_sub(void) {
	if (__initialized == 1) {
		snos_connect_start(__pub_sub_channel);	
		__enabled = 1;
		return snOS_SUCCESS;
	} else {
		return snOS_ERROR;
	}
}

snOSError snos_stop_pub_sub(void) {
	if (__initialized == 1 && __enabled == 1) {
		snos_connect_stop(__pub_sub_channel);
		__enabled = 0;
		return snOS_SUCCESS;
	} else {
		return snOS_ERROR;
	}
}

extern snOSError __snos_pub_sub_handler(void) {
	snOSSub *sub = NULL;
	uint64_t packet_length = 0;
	uint8_t *sent_packet = NULL;
	uint8_t *sent_topic = NULL;
	uint8_t *sent_message = NULL;
	uint8_t sent_topic_length = 0;
	uint8_t sent_message_length = 0;

	if (__initialized == 1 && __enabled == 1 && __subscription_list) {
		packet_length = snos_conenct_is_packet_available(__pub_sub_channel);
		
		if (packet_length >= MIN_PUB_SUB_PACKET_SIZE) {

			sent_packet = snos_alloc(sizeof(uint8_t) * packet_length + 1);
			
			if (!sent_packet) {
				return snOS_SYSTEM_OOM_ERROR;
			}

			snos_connect_get_packet(__pub_sub_channel, sent_packet, packet_length);

			sent_topic_length = sent_packet[0];
			sent_topic = snos_alloc(sizeof(uint8_t) * sent_topic_length + 1);
			
			if (sent_topic) {
				memcpy(sent_topic, &sent_packet[1], sent_topic_length);
			} else {
				return snOS_SYSTEM_OOM_ERROR;
			}

			sent_message_length = sent_packet[sent_topic_length + 1];
			
			sent_message = snos_alloc(sizeof(uint8_t) * sent_message_length + 1);
			
			if (sent_message) {
				memcpy(sent_message, &sent_packet[sent_topic_length + 2], sent_message_length);
			} else {
				return snOS_SYSTEM_OOM_ERROR;
			}

			snos_free(sent_packet);

			list_move_cursor_to_head(__subscription_list);
			while ((sub = ((snOSSub*)list_get_cursor_data(__subscription_list))) != (snOSSub*)list_get_tail_data(__subscription_list)) {
				if (sub) {
					if (sub->topic_length == sent_topic_length) {
						if (memcpy(sub->subscribed_topic, sent_topic, sent_topic_length) == 0) {
							snos_task_set_request(sub->handler);
							snos_task_write_message(sub->handler, sent_message, sent_message_length);
						}	
					}
				} else {
					list_move_cursor_right(__subscription_list);
				}
			}

			snos_free(sent_topic);
			snos_free(sent_message);

			return snOS_SUCCESS;
		} else {
			return snOS_ERROR;
		}
	} else {
		return snOS_ERROR;
	}	
}

snOSError snos_publish(char *topic, uint8_t topic_n, char *message, uint8_t message_n) {
	uint8_t *payload = NULL;

	if (__initialized == 1 && __enabled == 1) {
		payload = snos_alloc(sizeof(uint8_t) * (topic_n + 1 + message_n + 1));
		if (payload) {
			payload[0] = topic_n;
			memcpy(&payload[1], (uint8_t*)topic, topic_n);
			payload[topic_n + 1] = message_n;
			memcpy(&payload[topic_n + 2], (uint8_t*)message, message_n);

			snos_connect_send_packet(__pub_sub_channel, payload, topic_n + 1 + message_n + 1);
			snos_free(payload);

			return snOS_SUCCESS;
		} else {
			return snOS_SYSTEM_OOM_ERROR;
		}
	} else {
		return snOS_ERROR;
	}
}

snOSError snos_subscribe(snOSTask *handler, char *topic, uint8_t topic_n) {
	snOSSub *new_sub = NULL;

	if (__initialized == 1) {

		if (_already_subscribed(handler)) {
			return snOS_ERROR;
		}

		new_sub = snos_alloc(sizeof(snOSSub));

		if (new_sub) {
			new_sub->handler = handler;

			new_sub->subscribed_topic = snos_alloc(sizeof(uint8_t) * topic_n);
			if (new_sub->subscribed_topic) {
				memcpy(new_sub->subscribed_topic, (uint8_t*)topic, topic_n);
			} else {
				return snOS_SYSTEM_OOM_ERROR;
			}
			new_sub->topic_length = topic_n;

			list_append(__subscription_list, new_sub);
			return snOS_SUCCESS;
		} else {
			return snOS_SYSTEM_OOM_ERROR;
		}
	} else {
		return snOS_ERROR;
	}
}


snOSError snos_unsubscribe(snOSTask *subscribed_task) {
	snOSSub *sub = NULL;

	if (__subscription_list && __initialized == 1) {
		list_move_cursor_to_head(__subscription_list);
		while ((sub = ((snOSSub*)list_get_cursor_data(__subscription_list))) != (snOSSub*)list_get_tail_data(__subscription_list)) {
			if (sub && (sub->handler == subscribed_task)) {
				snos_free(sub->subscribed_topic);
				list_detete_current_item(__subscription_list);
				return snOS_SUCCESS;
			} else {
				list_move_cursor_right(__subscription_list);
			}
		}
	}

	return snOS_ERROR;
}

static uint8_t _already_subscribed(snOSTask *task) {
	snOSSub *sub = NULL;

	if (__subscription_list && __initialized == 1) {
		list_move_cursor_to_head(__subscription_list);
		while ((sub = ((snOSSub*)list_get_cursor_data(__subscription_list))) != (snOSSub*)list_get_tail_data(__subscription_list)) {
			if (sub && (sub->handler == task)) {
				return 1;
			} else {
				list_move_cursor_right(__subscription_list);
			}
		}
	}

	return 0;
}
