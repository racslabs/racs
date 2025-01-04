#include "cache.h"

LRUCache* create_lru_cache(size_t capacity) {
    LRUCache* cache = malloc(sizeof(LRUCache));
    if (!cache) {
        perror("Failed to allocate LRUCache");
        exit(-1);
    }

    cache->size = 0;
    cache->capacity = capacity;
    cache->list = create_list();
    cache->cache = AUXTS__Hashmap_construct(capacity);

    return cache;
}

LRUCacheEntry* create_lru_cache_entry(const uint64_t* key, uint8_t* value) {
    LRUCacheEntry* entry = malloc(sizeof(LRUCacheEntry));
    if (!entry) {
        perror("Failed to allocate LRUCacheEntry");
        exit(-1);
    }

    entry->key[0] = key[0];
    entry->key[1] = key[1];
    entry->value = value;

    return entry;
}

void put_lru_cache(LRUCache* cache, uint64_t* key, uint8_t* value) {
    LRUCacheEntry* curr = AUXTS__Hashmap_get(cache->cache, key);
    if (curr) return;

    if (cache->size >= cache->capacity) {
        evict_lru_cache(cache);
    }

    curr = create_lru_cache_entry(key, value);
    Node* node = create_node(curr);

    insert_at_head(cache->list, node);
    AUXTS__Hashmap_put(cache->cache, key, curr);

    ++cache->size;
}

uint8_t* get_lru_cache(LRUCache* cache, uint64_t* key) {
    LRUCacheEntry* curr = AUXTS__Hashmap_get(cache->cache, key);
    if (curr) {
        return curr->value;
    }

    return NULL;
}

void evict_lru_cache(LRUCache* cache) {
    Node* tail = (Node*) cache->list->tail;
    LRUCacheEntry* entry = tail->data;

    free(entry->value);
    AUXTS__Hashmap_delete(cache->cache, entry->key);

    cache->list->tail = (Node*) tail->prev;
    cache->list->tail->next = NULL;

    --cache->size;
}

int test_lru_cache() {
    uint64_t key1[2];
    key1[0] = 1;
    key1[1] = 2;

    uint64_t key2[2];
    key2[0] = 3;
    key2[1] = 4;

    uint64_t key3[2];
    key2[0] = 5;
    key2[1] = 6;

    uint8_t* data1 = (uint8_t*)strdup("data1");
    uint8_t* data2 = (uint8_t*)strdup("data2");
    uint8_t* data3 = (uint8_t*) strdup("data3");

    LRUCache* cache = create_lru_cache(2);
    put_lru_cache(cache, key1, data1);
    put_lru_cache(cache, key2, data2);
    put_lru_cache(cache, key3, data3);

    printf("%s\n", get_lru_cache(cache, key3));
    printf("%s\n", get_lru_cache(cache, key2));

    return 0;
}
