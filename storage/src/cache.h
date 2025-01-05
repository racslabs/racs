
#ifndef AUXTS_CACHE_H
#define AUXTS_CACHE_H

#include "hashmap.h"

typedef struct {
    uint64_t key[2];
    uint8_t* value;
} AUXTS__LRUCacheEntry;

typedef struct {
    AUXTS__LRUCacheEntry* entry;
    struct AUXTS__LRUCacheNode* prev;
    struct AUXTS__LRUCacheNode* next;
} AUXTS__LRUCacheNode;

typedef struct {
    size_t size;
    size_t capacity;
    AUXTS__LRUCacheNode* head;
    AUXTS__LRUCacheNode* tail;
    AUXTS__Hashmap* cache;
} AUXTS__LRUCache;

AUXTS__LRUCache* AUXTS__LRUCache_construct(size_t capacity);
void AUXTS__LRUCache_put(AUXTS__LRUCache* cache, uint64_t* key, uint8_t* value);
uint8_t* AUXTS__LRUCache_get(AUXTS__LRUCache* cache, uint64_t* key);
void AUXTS__LRUCache_evict(AUXTS__LRUCache* cache);
void AUXTS__LRUCache_destroy(AUXTS__LRUCache* cache);

int test_lru_cache();

#endif //AUXTS_CACHE_H
