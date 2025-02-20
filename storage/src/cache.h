
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
} LRUCacheEntry;

typedef struct {
    LRUCacheEntry* entry;
    struct LRUCacheNode* prev;
    struct LRUCacheNode* next;
} LRUCacheNode;

typedef struct {
    size_t size;
    size_t capacity;
    LRUCacheNode* head;
    LRUCacheNode* tail;
    Hashtable* cache;
    pthread_rwlock_t rwlock;
} LRUCache;

LRUCache* auxts_lru_cache_create(size_t capacity);
uint8_t* auxts_lru_cache_get(LRUCache* cache, const uint64_t* key);
void auxts_lru_cache_put(LRUCache* cache, const uint64_t* key, uint8_t* value);
void auxts_lru_cache_evict(LRUCache* cache);
void auxts_lru_cache_destroy(LRUCache* cache);

int test_lru_cache();

#ifdef __cplusplus
}
#endif

#endif //AUXTS_CACHE_H
