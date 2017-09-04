#include <snos_task_manager.h>
#include <list.h>
#include <stdlib.h>

static list_t *snOS_tasks = NULL;
static snOSTask *current_task = NULL;

snOSError initialize_snos_system_queue(void) {
	snOS_tasks = list_create();
	if (snOS_tasks) {
		return snOS_SUCCESS;
	}

	return snOS_SYSTEM_OOM_ERROR;
}

snOSError start_snos_system_queue(void) {
	list_move_cursor_to_head(snOS_tasks);
	current_task = list_get_cursor_data(snOS_tasks);

	if (current_task && ((current_task->process_type == RUN_FOREVER) || (current_task->process_type == RUN_ONCE))) {
		// execute first function
		return current_task->task_handler();
	}

	return snOS_OUT_OF_TASK_ERROR;
}

snOSTask *snos_task_manager_add_task(
	snOSError (*task_handler)(void), 
	snOSTaskRunType process_type) {

	snOSTask *new_task = snos_alloc(sizeof(snOSTask));

	if (new_task) {
		new_task->task_handler = task_handler;
		new_task->process_type = process_type;
		new_task->lock = 0;	
		list_append(snOS_tasks, new_task);
	}

	return new_task;
}

snOSError snos_scheduler_run_next_task(void) {
	snOSError return_code = snOS_SUCCESS;

	current_task = list_get_cursor_data(snOS_tasks);

	if (!current_task) {
		return snOS_OUT_OF_TASK_ERROR;
	}

	snos_receiver();

	switch(current_task->process_type) {

		case RUN_FOREVER:
			return_code = current_task->task_handler();
			
			if (snos_task_is_locked(current_task)) {
				return return_code;
			}

			if (current_task == list_get_tail_data(snOS_tasks)) {
				list_move_cursor_to_head(snOS_tasks);
			} else {
				list_move_cursor_right(snOS_tasks);
			}
			break;

		case RUN_ONCE:
			return_code = current_task->task_handler();

			if (snos_task_is_locked(current_task)) {
				return return_code;
			}

			if (current_task == list_get_tail_data(snOS_tasks)) {
				list_detete_current_item(snOS_tasks); // move right
				list_move_cursor_to_head(snOS_tasks);
			} else {
				list_detete_current_item(snOS_tasks); // move right
			}
			break;

		case RUN_ON_REQUEST:
			if (snos_task_requested(current_task)) {
				return_code = current_task->task_handler();
				snos_task_fulfilled(current_task);
			} else {
				return_code = snOS_NO_REQUESTS_CONTINUE_PROCESS;
			}

			if (snos_task_is_locked(current_task)) {
				return return_code;
			}

			if (current_task == list_get_tail_data(snOS_tasks)) {
				list_move_cursor_to_head(snOS_tasks);
			} else {
				list_move_cursor_right(snOS_tasks);
			}
			break;

		default:
			break;
	}

	return return_code;
}

snOSError snos_task_set_request(snOSTask *task) {
	if (task) {
		task->requested = 1;
		return snOS_SUCCESS;
	} else {
		return snOS_NO_TASK_ERROR;
	}
}

uint8_t snos_task_requested(snOSTask *task) {
	if (task) {
		return task->requested;
	} else {
		return 0;
	}
}

void snos_task_fulfilled(snOSTask *task) {
	if (task) {
		task->requested = 0;
	}
}
	
snOSError snos_task_set_lock(snOSTask *task) {
	if (task) {
		task->lock = 1;
		return snOS_SUCCESS;
	} else {
		return snOS_NO_TASK_ERROR;
	}
}

uint8_t snos_task_is_locked(snOSTask *task) {
	if (task) {
		return task->lock;
	} else {
		return 0;
	}
}

snOSError snos_task_unlock(snOSTask *task) {
	if (task) {
		task->lock = 0;
		return snOS_SUCCESS;
	}

	return snOS_NO_TASK_ERROR;
}

snOSTask *snos_this_task_id(void) {
	if (current_task) {
		return current_task;
	}
		return NULL;
}
