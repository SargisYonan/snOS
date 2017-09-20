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

/* 
BY DEFAULT:
    The snos_copy(void *dst, void *src, uint64_t n) performs a copy of n bytes
    from src to dst 

    the number of bytes copied is returned
*/
uint64_t snos_copy(uint8_t *dst, uint8_t *src, uint64_t n);

/* 
BY DEFAULT:
    The snos_mem_cmp compares n bytes of memory between src_a and src_b
    it returns:
    0 : both pointers point to n identical bytes of memory
    -1 : otherwise

    the number of bytes copied is returned
*/
int8_t snos_mem_cmp(uint8_t *src_a, uint8_t *src_b, uint64_t n);

#endif