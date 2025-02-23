
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
} lru_cache_entry;

typedef struct {
    lru_cache_entry* entry;
    struct lru_cache_node* prev;
    struct lru_cache_node* next;
} lru_cache_node;

typedef struct {
    size_t size;
    size_t capacity;
    lru_cache_node* head;
    lru_cache_node* tail;
    hashtable* cache;
    pthread_rwlock_t rwlock;
} lru_cache;

lru_cache* auxts_lru_cache_create(size_t capacity);
uint8_t* auxts_lru_cache_get(lru_cache* cache, const uint64_t* key);
void auxts_lru_cache_put(lru_cache* cache, const uint64_t* key, uint8_t* value);
void auxts_lru_cache_evict(lru_cache* cache);
void auxts_lru_cache_destroy(lru_cache* cache);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_CACHE_H
