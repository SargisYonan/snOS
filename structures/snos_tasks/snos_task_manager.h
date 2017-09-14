#ifndef _SNOS_TASK_MANAGER_H_
#define _SNOS_TASK_MANAGER_H_

#include "snos_error_codes.h"

#include "snos_connect.h"

typedef enum snOSTaskType {
	RUN_FOREVER, 
	RUN_ONCE, 
	RUN_ON_REQUEST,
} snOSTaskRunType;

struct snos_system_task_s {
	snOSError (*task_handler)(void);
	uint8_t *mailbox;
	uint8_t mailbox_length;
	snOSTaskRunType process_type;
	uint8_t requested;
	uint8_t lock;
	struct snos_transceiver_s *channel;
};
typedef struct snos_system_task_s snOSTask;

snOSError start_snos_system_queue(void);
snOSError initialize_snos_system_queue(void);

snOSTask *snos_task_manager_add_task(
	snOSError (*task_handler)(void), 
	snOSTaskRunType process_type);

snOSError snos_scheduler_run_next_task(void);

snOSError snos_task_write_message(snOSTask *task, const uint8_t* message, const uint8_t length);
uint8_t *snos_task_get_message(snOSTask *task);
uint8_t snos_task_get_message_length(snOSTask *task);

snOSError snos_task_set_request(snOSTask *task);
uint8_t snos_task_requested(snOSTask *task);
void snos_task_fulfilled(snOSTask *task);

// these locks exclude all but the locked task to run
snOSError snos_task_set_lock(snOSTask *task);
uint8_t snos_task_is_locked(snOSTask *task);
snOSError snos_task_unlock(snOSTask *task);

snOSTask *snos_this_task_id(void);

#endif