#ifdef _SNOS_THREADS_H_
#define _SNOS_THREADS_H_

#include "snos.h"

snOSError snos_initialize_threading(void);
snOSThread *snos_add_thread(snOSTask *thread_entry, snOSPriority priority, snOSTimeVector thread_timeout);
snOSThread *snos_switch_thread(void);
snOSError snos_start_threading(void);
snOSError snos_scheduler(void);

#endif