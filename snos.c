#include "snos.h"

snOSError snos_initialize(void) {
	snOSError ret = snOS_SUCCESS;

	ret = initialize_snos_system_queue();
	if (ret != snOS_SUCCESS) {
		return ret;
	}

	#ifdef SNOS_THREADING

		ret = initialize_snos_port();
		if (ret != snOS_SUCCESS) {
			return ret;
		}

		ret = initialize_snos_timers();
		if (ret != snOS_SUCCESS) {
			return ret;
		}
		snos_enable_system_timer();

	#endif

	return ret;
}

snOSError snos_start(void) {
	snOSError ret;

	ret = start_snos_system_queue();
	do {
		ret = snos_scheduler_run_next_task();
	} while (ret >= snOS_NO_ERROR);

	return ret;
}

snOSTask *snos_new_task(
	snOSError (*task_handler)(void), 
	snOSTaskRunType process_type) {

	return snos_task_manager_add_task(task_handler, process_type);
}

snOSError snos_lock(snOSTask *task) {
	return snos_task_set_lock(task);
}

snOSError snos_unlock(snOSTask *task) {
	return snos_task_unlock(task);
}

snOSTask *snos_task_id(void) {
	return snos_this_task_id();
}