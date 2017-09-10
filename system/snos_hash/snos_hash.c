#include "snos_hash.h"

#define FNA_1A_32_OFFSET_BASIS (2166136261)
#define FNA_1A_32_PRIME (16777619)

uint32_t snos_hash(const uint8_t *src, const uint64_t n) {
	uint32_t hash = FNA_1A_32_OFFSET_BASIS;
	uint64_t count = 0;

    if (!src) {
    	return 0;
    } 

    for (count = 0; count < n; count++) {
        hash = hash ^ (src[count] & 0xFF);
        hash = hash * FNA_1A_32_PRIME;
    }

    return hash;
}

uint32_t snos_get_hash_from_array(const uint8_t *src) {
	uint8_t i = 0;
	uint32_t hash = 0x00000000;
	uint32_t vector = 0x00000000;

	if (!src) {
		return 0;
	}

	for (i = 0; i < FNV_1A_HASH_SIZE; i++) {
		vector = src[i];
		hash |= (uint32_t)(vector << (i * 8));
	}

	return hash;
}

uint8_t snos_store_hash(const uint32_t hash, uint8_t *src) {
	uint8_t i = 0;
	uint32_t __hash = hash; // keep it const

	if (!src) {
		return 0;
	}

	for (i = 0; i < FNV_1A_HASH_SIZE; i++) {
		src[i] = (uint8_t)(__hash & 0x000000FF);
		__hash >>= (i * 8);
	}

	return i;
}