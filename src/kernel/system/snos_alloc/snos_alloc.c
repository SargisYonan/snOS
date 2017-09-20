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

uint64_t snos_copy(uint8_t *dst, uint8_t *src, uint64_t n) {
	uint64_t count = 0;
		
	if (!dst || !src) {
		return 0;
	}

	for (count = 0; count < n; count++) {
		dst[count] = src[count];
	}

	return count + 1;
}

int8_t snos_mem_cmp(uint8_t *src_a, uint8_t *src_b, uint64_t n) {
	uint64_t count = 0;
		
	if (!src_a || !src_b) {
		return 0;
	}

	for (count = 0; count < n; count++) {
		if (src_a[count] != src_b[count]) {
			return -1;
		}
	}

	return 0;
}
