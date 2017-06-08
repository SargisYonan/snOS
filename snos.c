#include "snos.h"

snOSError snos_initialize(void) {

	snOSError ret;

	ret = initialize_snos_system_queue();

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

snOSError snos_new_task(
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