#include <snos_alloc.h>

#include <string.h> // memset

void *snos_alloc(uint64_t size) {
	void *new_mem = NULL;
	uint64_t itor = 0;

	new_mem = malloc((size_t)size);
	if (new_mem) {
		for (itor = 0; itor < size; itor++) {
			new_mem[i] = 0;
		}
	}

	return new_mem;
}

void snos_free(void *ptr) {
	free(ptr);
}