#include "cache.h"

static AUXTS__LRUCacheNode* LRUCacheNode_construct(AUXTS__LRUCacheEntry* entry);
static AUXTS__LRUCacheEntry* LRUCacheEntry_construct(const uint64_t* key, uint8_t* value);
static void LRUCache_insert_at_head(AUXTS__LRUCache * cache, AUXTS__LRUCacheNode* node);

AUXTS__LRUCache* AUXTS__LRUCache_construct(size_t capacity) {
    AUXTS__LRUCache* cache = malloc(sizeof(AUXTS__LRUCache));
    if (!cache) {
        perror("Failed to allocate AUXTS__LRUCache");
        exit(-1);
    }

    cache->size = 0;
    cache->capacity = capacity;
    cache->head = NULL;
    cache->tail = NULL;
    cache->cache = AUXTS__Hashmap_construct(capacity);

    return cache;
}

AUXTS__LRUCacheEntry* LRUCacheEntry_construct(const uint64_t* key, uint8_t* value) {
    AUXTS__LRUCacheEntry* entry = malloc(sizeof(AUXTS__LRUCacheEntry));
    if (!entry) {
        perror("Failed to allocate AUXTS__LRUCacheEntry");
        exit(-1);
    }

    entry->key[0] = key[0];
    entry->key[1] = key[1];
    entry->value = value;

    return entry;
}

AUXTS__LRUCacheNode* LRUCacheNode_construct(AUXTS__LRUCacheEntry* entry) {
    AUXTS__LRUCacheNode* node = malloc(sizeof(AUXTS__LRUCacheNode));
    if (!node) {
        perror("Failed to allocate AUXTS__LRUCacheNode");
        exit(-1);
    }

    node->entry = entry;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

void LRUCache_insert_at_head(AUXTS__LRUCache * cache, AUXTS__LRUCacheNode* node) {
    if (!cache) return;

    node->prev = NULL;
    node->next = (struct AUXTS__LRUCacheNode*) cache->head;

    if (cache->head) {
        cache->head->prev = (struct AUXTS__LRUCacheNode*) node;
    } else {
        cache->tail = node;
    }

    cache->head = node;
}

void AUXTS__LRUCache_put(AUXTS__LRUCache* cache, uint64_t* key, uint8_t* value) {
    AUXTS__LRUCacheEntry* entry = AUXTS__Hashmap_get(cache->cache, key);
    if (entry) return;

    if (cache->size >= cache->capacity) {
        AUXTS__LRUCache_evict(cache);
    }

    entry = LRUCacheEntry_construct(key, value);
    AUXTS__LRUCacheNode* node = LRUCacheNode_construct(entry);

    LRUCache_insert_at_head(cache, node);
    AUXTS__Hashmap_put(cache->cache, key, entry);

    ++cache->size;
}

uint8_t* AUXTS__LRUCache_get(AUXTS__LRUCache* cache, uint64_t* key) {
    AUXTS__LRUCacheEntry* entry = AUXTS__Hashmap_get(cache->cache, key);
    if (entry) {
        return entry->value;
    }

    return NULL;
}

void AUXTS__LRUCache_evict(AUXTS__LRUCache* cache) {
    AUXTS__LRUCacheNode* tail = cache->tail;
    AUXTS__LRUCacheEntry* entry = tail->entry;

    free(entry->value);
    AUXTS__Hashmap_delete(cache->cache, entry->key);

    cache->tail = (AUXTS__LRUCacheNode*) tail->prev;
    cache->tail->next = NULL;

    --cache->size;
}

void AUXTS__LRUCache_destroy(AUXTS__LRUCache* cache) {

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

    AUXTS__LRUCache* cache = AUXTS__LRUCache_construct(2);
    AUXTS__LRUCache_put(cache, key1, data1);
    AUXTS__LRUCache_put(cache, key2, data2);
    AUXTS__LRUCache_put(cache, key3, data3);

    printf("%s\n", AUXTS__LRUCache_get(cache, key3));
    printf("%s\n", AUXTS__LRUCache_get(cache, key2));

    return 0;
}
