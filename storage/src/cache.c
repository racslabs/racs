#include "cache.h"

LRUCache* create_lru_cache(size_t capacity) {
    LRUCache* cache = malloc(sizeof(LRUCache));
    if (!cache) {
        perror("Failed to allocate LRUCache");
        exit(-1);
    }

    cache->capacity = capacity;
    cache->size = 0;
    cache->list = create_list();
    cache->cache = create_hashmap(capacity);

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
    LRUCacheEntry* curr = get_hashmap(cache->cache, key);
    if (curr) return;

    if (cache->size >= cache->capacity) {
        Node* tail = (Node*) cache->list->tail;

        LRUCacheEntry* entry = tail->data;
        free(entry->value);

        delete_hashmap(cache->cache, entry->key);
        delete_at_tail(cache->list);

        --cache->size;
    }

    curr = create_lru_cache_entry(key, value);
    Node* node = create_node(curr);

    insert_at_head(cache->list, node);
    put_hashmap(cache->cache, key, curr);

    ++cache->size;
}
