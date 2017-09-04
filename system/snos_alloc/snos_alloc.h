#ifndef _SNOS_ALLOC_H_
#define _SNOS_ALLOC_H_

#include <stdint.h>

/* 
BY DEFAULT:
	The snos_alloc() function contiguously allocates enough space for count 
	objects that are size bytes of memory each and returns a pointer to the 
	allocated memory.
	The allocated memory is filled with bytes of value zero.
	
	This implementation uses the system's malloc() and the standard library 
	must therefore be linked.
*/

void *snos_alloc(uint64_t size);

/* 
BY DEFAULT:
    The snos_free() function deallocates the memory allocation pointed to by ptr. 
    If ptr is a NULL pointer, no operation is performed.

    This implementation uses the system's free() and the standard library 
	must therefore be linked.
*/
void snos_free(void *ptr);

#endif