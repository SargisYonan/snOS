#ifndef _SNOS_HASH_
#define _SNOS_HASH_

#include <stdint.h>

#define FNV_1A_HASH_SIZE (4) // 4 bytes for 32-but fnv-1a

// uses the 32-bit FNV-1A hashing function
uint32_t snos_hash(const uint8_t *src, const uint64_t n);
uint32_t snos_get_hash_from_array(const uint8_t *src);
uint8_t snos_store_hash(const uint32_t hash, uint8_t *src);

#endif // _SNOS_HASH_