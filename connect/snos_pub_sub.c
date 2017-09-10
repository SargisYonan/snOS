#include "snos_pub_sub.h"
#include "snos_connect.h"
#include "snos_hash.h"
#include "list.h"

#include <stdlib.h>
#include <string.h>

#define PUB_SUB_HASH_STRINGS (1)
#define HASH_SIZE (FNV_1A_HASH_SIZE)

#define MAX_PUB_SUB_PACKET_SIZE (255)

#ifdef PUB_SUB_HASH_STRINGS
	#define MIN_PUB_SUB_PACKET_SIZE (2)
#else
	#define PUB_SUB_TRANSMIT_STRING (1)
	#define MIN_PUB_SUB_PACKET_SIZE (3)
#endif

typedef struct pub_sub_task_s {
	snOSTask *handler;
	#ifdef PUB_SUB_HASH_STRINGS
		uint32_t hashed_topic;
	#else
		uint8_t *subscribed_topic;
		uint8_t topic_length;
	#endif
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

	#ifdef PUB_SUB_HASH_STRINGS
		uint32_t hashed_topic = 0;
	#else
		uint8_t *sent_topic = NULL;
		uint8_t sent_topic_length = 0;
	#endif

	uint8_t *sent_message = NULL;
	uint8_t sent_message_length = 0;

	if (__initialized == 1 && __enabled == 1 && __subscription_list) {
		packet_length = snos_conenct_is_packet_available(__pub_sub_channel);
		
		if (packet_length >= MIN_PUB_SUB_PACKET_SIZE) {

			sent_packet = snos_alloc(sizeof(uint8_t) * packet_length + 1);
			
			if (!sent_packet) {
				return snOS_SYSTEM_OOM_ERROR;
			}

			snos_connect_get_packet(__pub_sub_channel, sent_packet, packet_length);

			#ifdef PUB_SUB_HASH_STRINGS
				hashed_topic = snos_get_hash_from_array(sent_packet);
			#else
				sent_topic_length = sent_packet[0];
				sent_topic = snos_alloc(sizeof(uint8_t) * sent_topic_length + 1);
			
				if (sent_topic) {
					snos_copy(sent_topic, &sent_packet[1], sent_topic_length);
				} else {
					return snOS_SYSTEM_OOM_ERROR;
				}
			#endif

			list_move_cursor_to_head(__subscription_list);
			sub = (snOSSub*)list_get_cursor_data(__subscription_list);
			while (sub) {
					#ifdef PUB_SUB_HASH_STRINGS
						if (sub->hashed_topic == hashed_topic)
					#elif PUB_SUB_TRANSMIT_STRING
						if ((sub->topic_length == sent_topic_length) && (snos_mem_cmp(sub->subscribed_topic, sent_topic, sent_topic_length) == 0))
					#endif						
					{
						#ifdef PUB_SUB_HASH_STRINGS
							sent_message_length = sent_packet[HASH_SIZE + 1];
						#else
							sent_message_length = sent_packet[sent_topic_length + 1];
						#endif

						sent_message = snos_alloc(sizeof(uint8_t) * sent_message_length + 1);
			
						if (sent_message) {
							#ifdef PUB_SUB_HASH_STRINGS
								snos_copy(sent_message, &sent_packet[HASH_SIZE + 2], sent_message_length);
							#elif PUB_SUB_TRANSMIT_STRING
								snos_copy(sent_message, &sent_packet[sent_topic_length + 2], sent_message_length);
							#endif
						} else {
							return snOS_SYSTEM_OOM_ERROR;
						}

						snos_task_set_request(sub->handler);
						snos_task_write_message(sub->handler, sent_message, sent_message_length);
					}	

				if (sub != (snOSSub*)list_get_tail_data(__subscription_list)) {
					list_move_cursor_right(__subscription_list);
				} else {
					break;
				}
			}

			#ifndef PUB_SUB_HASH_STRINGS
				snos_free(sent_topic);
			#endif		

			snos_free(sent_message);
			snos_free(sent_packet);

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
	uint32_t hashed_topic = 0;

	if (__initialized == 1 && __enabled == 1) {

		#ifdef PUB_SUB_HASH_STRINGS
			payload = snos_alloc(sizeof(uint8_t) * (HASH_SIZE + message_n + 1));
		#else
			payload = snos_alloc(sizeof(uint8_t) * (topic_n + 1 + message_n + 1));
		#endif

			if (payload) {

			#ifdef PUB_SUB_HASH_STRINGS
				hashed_topic = snos_hash((uint8_t*)topic, topic_n);
				snos_store_hash(hashed_topic, payload);
				payload[HASH_SIZE] = message_n;
				snos_copy(&payload[HASH_SIZE + 1], (uint8_t*)message, message_n);
				snos_connect_send_packet(__pub_sub_channel, payload, HASH_SIZE + 1 + message_n + 1);
			#else
				payload[0] = topic_n;
				snos_copy(&payload[1], (uint8_t*)topic, topic_n);
				payload[topic_n + 1] = message_n;
				snos_copy(&payload[topic_n + 2], (uint8_t*)message, message_n);
				snos_connect_send_packet(__pub_sub_channel, payload, topic_n + 1 + message_n + 1);
			#endif

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
			#ifdef PUB_SUB_HASH_STRINGS
				new_sub->hashed_topic = snos_hash((uint8_t*)topic, topic_n);
			#else
				new_sub->subscribed_topic = snos_alloc(sizeof(uint8_t) * topic_n);
				if (new_sub->subscribed_topic) {
					snos_copy(new_sub->subscribed_topic, (uint8_t*)topic, topic_n);
				} else {
					return snOS_SYSTEM_OOM_ERROR;
				}
				new_sub->topic_length = topic_n;
			#endif			
		
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
				
				#ifndef PUB_SUB_HASH_STRINGS
					snos_free(sub->subscribed_topic);
				#endif

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
