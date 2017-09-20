#ifndef _SNOS_H_
#define _SNOS_H_

#define SNOS_THREADING (1)
#define SNOS_CONNECT (1)

#include <snos_connect.h>
#include <snos_pub_sub.h>
#include <snos_task_manager.h>
#include <snos_alloc.h>
#include <snos_error_codes.h>

#ifdef SNOS_THREADING
	#include <snos_port.h>
	#include <snos_timer.h>
#endif

snOSError snos_initialize(void);
snOSError snos_start(void);
snOSTask *snos_new_task(
	snOSError (*task_handler)(void), 
	snOSTaskRunType process_type);

snOSTask *snos_task_id(void);
snOSError snos_lock(snOSTask *task);
snOSError snos_unlock(snOSTask *task);

#endif