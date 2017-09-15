#ifndef _SNOS_H_
#define _SNOS_H_

#define SNOS_CONNECT (1)

#include <stdbool.h>
#include <stdint.h>

#include <snos_connect.h>
#include <snos_pub_sub.h>
#include <snos_task_manager.h>
#include <snos_alloc.h>
#include <snos_error_codes.h>
#include <snos_threads.h>

snOSError snos_initialize(void);
snOSError snos_start(void);

snOSThread *snos_new_thread(
	void (*thread_entry)(void), 
	const snOSTaskRunType process_type,
	const snOSPriority priority,
	const snOSTimeVector run_period_ms, // 0 if always running
	const bool use_snos_connect,
	uint8_t (*is_byte_available)(void),
	uint8_t (*packet_byte_receiver)(void), 
	void (*packet_byte_transmitter)(uint8_t), 
	const uint8_t max_packet_size,
	const char *subscribed_topic);

snOSThread *snos_thread_id(void);

snOSError snos_mutex_lock(snOSThread *thread);
snOSError snos_mutex_unlock(snOSThread *thread);

#endif