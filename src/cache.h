
#ifndef AUXTS_CACHE_H
#define AUXTS_CACHE_H

#include "kvstore.h"
#include "mmh3-128.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint64_t key[2];
    uint8_t* value;
} auxts_cache_entry;

typedef struct {
    auxts_cache_entry entry;
    struct auxts_cache_node* prev;
    struct auxts_cache_node* next;
} auxts_cache_node;

typedef struct auxts_cache {
    size_t size;
    size_t capacity;
    auxts_cache_node* head;
    auxts_cache_node* tail;
    auxts_kvstore* kv;
    pthread_rwlock_t rwlock;
} auxts_cache;

auxts_cache* auxts_cache_create(size_t capacity);
uint8_t* auxts_cache_get(auxts_cache* cache, const uint64_t* key);
void auxts_cache_put(auxts_cache* cache, const uint64_t* key, uint8_t* value);
void auxts_cache_evict(auxts_cache* cache);
void auxts_cache_destroy(auxts_cache* cache);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_CACHE_H
