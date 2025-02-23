
#ifndef AUXTS_HASHTABLE_H
#define AUXTS_HASHTABLE_H

#include "murmurhash3.h"
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AUXTS_INITIAL_BUCKET_CAPACITY 2

typedef struct {
    uint64_t key[2];
    uint8_t* value;
} hashtable_entry;

typedef struct {
    size_t count;
    size_t capacity;
    hashtable_entry** entries;
} hashtable_bucket;

typedef struct {
    size_t num_buckets;
    hashtable_bucket** buckets;
} hashtable;

AUXTS_FORCE_INLINE uint64_t auxts_hash(void* data, int len, size_t size) {
    uint64_t _hash[2];
    auxts_murmurhash3_x64_128(data, len, 0, _hash);
    return _hash[0] % size;
}

hashtable* auxts_hashtable_create(size_t num_entries);
void auxts_hashtable_put(hashtable* ht, const uint64_t* key, uint8_t* value);
void* auxts_hashtable_get(hashtable* ht, const uint64_t* key);
void auxts_hashtable_delete(hashtable* ht, uint64_t* key);
void auxts_hashtable_destroy(hashtable* ht);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_HASHTABLE_H
