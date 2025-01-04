
#ifndef AUXTS_CACHE_H
#define AUXTS_CACHE_H

#include "list.h"
#include "hashmap.h"

typedef struct {
    size_t capacity;
    size_t size;
    LinkedList* list;
    AUXTS__Hashmap* cache;
} LRUCache;

typedef struct {
    uint64_t key[2];
    uint8_t* value;
} LRUCacheEntry;

LRUCache* create_lru_cache(size_t capacity);

LRUCacheEntry* create_lru_cache_entry(const uint64_t* key, uint8_t* value);

void put_lru_cache(LRUCache* cache, uint64_t* key, uint8_t* value);

uint8_t* get_lru_cache(LRUCache* cache, uint64_t* key);

void evict_lru_cache(LRUCache* cache);

int test_lru_cache();

#endif //AUXTS_CACHE_H
