
#ifndef AUXTS_HASHMAP_H
#define AUXTS_HASHMAP_H

#include "murmur3.h"
#include "list.h"

#define AUXTS_INITIAL_BUCKET_CAP 2

typedef struct {
    uint64_t key[2];
    void* value;
} HashmapEntry;

typedef struct {
    size_t count;
    size_t capacity;
    HashmapEntry** entries;
} Bucket;

typedef struct {
    size_t size;
    Bucket** buckets;
} Hashmap;

AUXTS_FORCE_INLINE uint64_t hash(void* data, int len, size_t size) {
    uint64_t _hash[2];
    murmur3_x64_128(data, len, 0, _hash);
    return _hash[0] % size;
}

Hashmap* create_hashmap(size_t size);

HashmapEntry* create_hashmap_entry(const uint64_t* key, void* value);

Bucket* create_bucket();

void destroy_hashmap_entry(HashmapEntry* entry);

void put_hashmap(Hashmap* map, uint64_t* key, void* value);

void* get_hashmap(Hashmap* map, uint64_t* key);

void delete_hashmap(Hashmap* map, uint64_t* key);

void destroy_hashmap(Hashmap* map);

int test_hashmap();

#endif //AUXTS_HASHMAP_H
