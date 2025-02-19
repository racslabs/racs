#include "cache.h"

static LRUCacheNode* lru_cache_node_create(LRUCacheEntry* entry);
static LRUCacheEntry* lru_cache_entry_create(const uint64_t* key, uint8_t* value);
static void lru_cache_node_destroy(LRUCacheNode* node);
static void lru_cache_insert_at_head(LRUCache * cache, LRUCacheNode* node);

LRUCache* auxts_lru_cache_create(size_t capacity) {
    LRUCache* cache = malloc(sizeof(LRUCache));
    if (!cache) {
        perror("Failed to allocate LRUCache");
        return NULL;
    }

    pthread_rwlock_init(&cache->rwlock, NULL);

    cache->size = 0;
    cache->capacity = capacity;
    cache->head = NULL;
    cache->tail = NULL;
    cache->cache = AUXTS__Hashmap_construct(capacity);

    return cache;
}

void auxts_lru_cache_put(LRUCache* cache, const uint64_t* key, const uint8_t* value) {
    if (!cache) return;

    pthread_rwlock_wrlock(&cache->rwlock);

    LRUCacheEntry* entry = AUXTS__Hashmap_get(cache->cache, key);
    if (entry) {
        pthread_rwlock_unlock(&cache->rwlock);
        return;
    }

    if (cache->size >= cache->capacity) {
        auxts_lru_cache_evict(cache);
    }

    entry = lru_cache_entry_create(key, value);
    LRUCacheNode* node = lru_cache_node_create(entry);

    lru_cache_insert_at_head(cache, node);
    AUXTS__Hashmap_put(cache->cache, key, entry);

    ++cache->size;

    pthread_rwlock_unlock(&cache->rwlock);
}

uint8_t* auxts_lru_cache_get(LRUCache* cache, const uint64_t* key) {
    if (!cache) {
        return NULL;
    }

    pthread_rwlock_rdlock(&cache->rwlock);

    LRUCacheEntry* entry = AUXTS__Hashmap_get(cache->cache, key);
    pthread_rwlock_unlock(&cache->rwlock);

    if (entry) {
        return entry->value;
    }

    return NULL;
}

void auxts_lru_cache_destroy(LRUCache* cache) {
    if (!cache) {
        return;
    }

    pthread_rwlock_wrlock(&cache->rwlock);

    LRUCacheNode* node = cache->head;

    while (node) {
        LRUCacheNode* next = (LRUCacheNode*) node->next;
        lru_cache_node_destroy(node);
        node = next;
    }

    AUXTS__Hashmap_destroy(cache->cache);

    pthread_rwlock_unlock(&cache->rwlock);
    pthread_rwlock_destroy(&cache->rwlock);

    free(cache);
}

void auxts_lru_cache_evict(LRUCache* cache) {
    if (!cache) {
        return;
    }

    LRUCacheNode* tail = cache->tail;
    LRUCacheEntry* entry = tail->entry;

    free(entry->value);
    AUXTS__Hashmap_delete(cache->cache, entry->key);

    cache->tail = (LRUCacheNode*) tail->prev;
    cache->tail->next = NULL;

    --cache->size;
}

LRUCacheEntry* lru_cache_entry_create(const uint64_t* key, uint8_t* value) {
    LRUCacheEntry* entry = malloc(sizeof(LRUCacheEntry));
    if (!entry) {
        perror("Failed to allocate LRUCacheEntry");
        return NULL;
    }

    entry->key[0] = key[0];
    entry->key[1] = key[1];
    entry->value = value;

    return entry;
}

LRUCacheNode* lru_cache_node_create(LRUCacheEntry* entry) {
    if (!entry) {
        return NULL;
    }

    LRUCacheNode* node = malloc(sizeof(LRUCacheNode));
    if (!node) {
        perror("Failed to allocate LRUCacheNode");
        return NULL;
    }

    node->entry = entry;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

void lru_cache_insert_at_head(LRUCache* cache, LRUCacheNode* node) {
    if (!cache || !node) {
        return;
    }

    node->prev = NULL;
    node->next = (struct LRUCacheNode *)cache->head;

    if (cache->head) {
        cache->head->prev = (struct LRUCacheNode *)node;
    } else {
        cache->tail = node;
    }

    cache->head = node;
}

void lru_cache_node_destroy(LRUCacheNode* node) {
    if (!node) {
        return;
    }

    LRUCacheEntry* entry = node->entry;
    free(entry->value);
    free(node);
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

    LRUCache* cache = auxts_lru_cache_create(2);
    auxts_lru_cache_put(cache, key1, data1);
    auxts_lru_cache_put(cache, key2, data2);
    auxts_lru_cache_put(cache, key3, data3);

    printf("%s\n", auxts_lru_cache_get(cache, key3));
    printf("%s\n", auxts_lru_cache_get(cache, key2));

    auxts_lru_cache_destroy(cache);

    return 0;
}
