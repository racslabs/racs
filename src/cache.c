#include "cache.h"

static cache_node_t* lru_cache_node_create(cache_entry_t* entry);
static cache_entry_t* lru_cache_entry_create(const uint64_t* key, uint8_t* value);
static void lru_cache_node_destroy(cache_node_t* node);
static void lru_cache_insert_at_head(cache_t* cache, cache_node_t* node);

cache_t* auxts_cache_create(size_t capacity) {
    cache_t* cache = malloc(sizeof(cache_t));
    if (!cache) {
        perror("Failed to allocate cache_t");
        return NULL;
    }

    pthread_rwlock_init(&cache->rwlock, NULL);

    cache->size = 0;
    cache->capacity = capacity;
    cache->head = NULL;
    cache->tail = NULL;
    cache->cache = auxts_hashtable_create(capacity);

    return cache;
}

void auxts_cache_put(cache_t* cache, const uint64_t* key, uint8_t* value) {
    if (!cache) {
        return;
    }

    pthread_rwlock_wrlock(&cache->rwlock);

    cache_entry_t* entry = auxts_hashtable_get(cache->cache, key);
    if (entry) {
        pthread_rwlock_unlock(&cache->rwlock);
        return;
    }

    if (cache->size >= cache->capacity) {
        auxts_cache_evict(cache);
    }

    entry = lru_cache_entry_create(key, value);
    cache_node_t* node = lru_cache_node_create(entry);

    lru_cache_insert_at_head(cache, node);
    auxts_hashtable_put(cache->cache, key, entry);

    ++cache->size;

    pthread_rwlock_unlock(&cache->rwlock);
}

uint8_t* auxts_cache_get(cache_t* cache, const uint64_t* key) {
    if (!cache) {
        return NULL;
    }

    pthread_rwlock_rdlock(&cache->rwlock);

    cache_entry_t* entry = auxts_hashtable_get(cache->cache, key);
    pthread_rwlock_unlock(&cache->rwlock);

    if (entry) {
        return entry->value;
    }

    return NULL;
}

void auxts_cache_destroy(cache_t* cache) {
    if (!cache) {
        return;
    }

    pthread_rwlock_wrlock(&cache->rwlock);

    cache_node_t* node = cache->head;

    while (node) {
        cache_node_t* next = (cache_node_t*) node->next;
        lru_cache_node_destroy(node);
        node = next;
    }

    auxts_hashtable_destroy(cache->cache);

    pthread_rwlock_unlock(&cache->rwlock);
    pthread_rwlock_destroy(&cache->rwlock);

    free(cache);
}

void auxts_cache_evict(cache_t* cache) {
    if (!cache) {
        return;
    }

    cache_node_t* tail = cache->tail;
    cache_entry_t* entry = tail->entry;

    free(entry->value);
    auxts_hashtable_delete(cache->cache, entry->key);

    cache->tail = (cache_node_t*) tail->prev;
    cache->tail->next = NULL;

    --cache->size;
}

cache_entry_t* lru_cache_entry_create(const uint64_t* key, uint8_t* value) {
    cache_entry_t* entry = malloc(sizeof(cache_entry_t));
    if (!entry) {
        perror("Failed to allocate cache_entry_t");
        return NULL;
    }

    entry->key[0] = key[0];
    entry->key[1] = key[1];
    entry->value = value;

    return entry;
}

cache_node_t* lru_cache_node_create(cache_entry_t* entry) {
    if (!entry) {
        return NULL;
    }

    cache_node_t* node = malloc(sizeof(cache_node_t));
    if (!node) {
        perror("Failed to allocate cache_node_t");
        return NULL;
    }

    node->entry = entry;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

void lru_cache_insert_at_head(cache_t* cache, cache_node_t* node) {
    if (!cache || !node) {
        return;
    }

    node->prev = NULL;
    node->next = (struct cache_node_t*)cache->head;

    if (cache->head) {
        cache->head->prev = (struct cache_node_t*)node;
    } else {
        cache->tail = node;
    }

    cache->head = node;
}

void lru_cache_node_destroy(cache_node_t* node) {
    if (!node) {
        return;
    }

    cache_entry_t* entry = node->entry;
    free(entry->value);
    free(node);
}
