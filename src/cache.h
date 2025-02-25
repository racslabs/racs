
#ifndef AUXTS_CACHE_H
#define AUXTS_CACHE_H

#include "hashtable.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint64_t key[2];
    uint8_t* value;
} cache_entry_t;

typedef struct {
    cache_entry_t* entry;
    struct cache_node_t* prev;
    struct cache_node_t* next;
} cache_node_t;

typedef struct {
    size_t size;
    size_t capacity;
    cache_node_t* head;
    cache_node_t* tail;
    hashtable_t* cache;
    pthread_rwlock_t rwlock;
} cache_t;

cache_t* auxts_cache_create(size_t capacity);
uint8_t* auxts_cache_get(cache_t* cache, const uint64_t* key);
void auxts_cache_put(cache_t* cache, const uint64_t* key, uint8_t* value);
void auxts_cache_evict(cache_t* cache);
void auxts_cache_destroy(cache_t* cache);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_CACHE_H
