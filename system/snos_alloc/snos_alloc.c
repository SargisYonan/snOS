#include <snos_alloc.h>
#include <stdlib.h>
#include <string.h> // memset

void *snos_alloc(uint64_t size) {
	void *new_mem = calloc((size_t)size, 1);
	return new_mem;
}

void snos_free(void *ptr) {
	free(ptr);
}