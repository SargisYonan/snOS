#ifndef _SNOS_H_
#define _SNOS_H_

#include <snos_task_manager.h>
#include <snos_alloc.h>
#include <snos_error_codes.h>

#ifdef SNOS_NETWORK_ENABLE
	#include <snos_connect.h>
#endif

snOSError snos_initialize(void);
snOSError snos_start(void);
snOSError snos_new_task(
	snOSError (*task_handler)(void), 
	snOSTaskRunType process_type);

snOSTask *snos_task_id(void);
snOSError snos_lock(snOSTask *task);
snOSError snos_unlock(snOSTask *task);

#endif