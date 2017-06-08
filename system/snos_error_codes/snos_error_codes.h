#ifndef _SNOS_SYSTEM_ERROR_CODES_H_
#define _SNOS_SYSTEM_ERROR_CODES_H_

#include <stdint.h>

typedef int8_t snOSError;

#define snOS_NO_DATA_AVAILABLE				(4)
#define snOS_CHANNEL_DISABLED				(3)
#define snOS_NO_REQUESTS_CONTINUE_PROCESS 	(2)
#define snOS_SUCCESS 						(1)
#define snOS_NO_ERROR 						(0)
#define snOS_GENERAL_ERROR 					(-1)
#define snOS_SYSTEM_OOM_ERROR 				(-2)
#define snOS_OUT_OF_TASK_ERROR 				(-3)
#define snOS_NO_TASK_ERROR 					(-4)

#endif