
#ifndef AUXTS_HASHMAP_H
#define AUXTS_HASHMAP_H

#include "murmur3.h"
#include "list.h"

typedef struct {
    uint64_t* key;
    uint8_t* value;
} HashmapEntry;

typedef struct {
    size_t size;
    LinkedList** buckets;
} Hashmap;

AUXTS_FORCE_INLINE uint64_t hash(uint8_t* data, int len, size_t size) {
    uint64_t _hash[2];
    murmur3_x64_128(data, len, 0, _hash);
    return _hash[0] % size;
}

Hashmap* create_hashmap(size_t size);

HashmapEntry* create_hashmap_entry(uint64_t* key, uint8_t* value);

void put_hashmap(Hashmap* map, uint64_t* key, uint8_t* value);

uint8_t* get_hashmap(Hashmap* map, uint64_t* key);

int test_hashmap();

#endif //AUXTS_HASHMAP_H
