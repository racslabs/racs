
#ifndef AUXTS_HASHMAP_H
#define AUXTS_HASHMAP_H

#include "murmur3.h"
#include <stdlib.h>
#include <string.h>

#define AUXTS_INITIAL_BUCKET_CAPACITY 2

typedef struct {
    uint64_t key[2];
    void* value;
} AUXTS__HashmapEntry;

typedef struct {
    size_t count;
    size_t capacity;
    AUXTS__HashmapEntry** entries;
} AUXTS__HashmapBucket;

typedef struct {
    size_t size;
    AUXTS__HashmapBucket** buckets;
} AUXTS__Hashmap;

AUXTS_FORCE_INLINE uint64_t AUXTS__hash(void* data, int len, size_t size) {
    uint64_t _hash[2];
    AUXTS__murmur3_x64_128(data, len, 0, _hash);
    return _hash[0] % size;
}

AUXTS__Hashmap* AUXTS__Hashmap_construct(size_t size);

void AUXTS__Hashmap_put(AUXTS__Hashmap* map, uint64_t* key, void* value);

void* AUXTS__Hashmap_get(AUXTS__Hashmap* map, uint64_t* key);

void AUXTS__Hashmap_delete(AUXTS__Hashmap* map, uint64_t* key);

void AUXTS__Hashmap_destroy(AUXTS__Hashmap* map);

int test_hashmap();

#endif //AUXTS_HASHMAP_H
