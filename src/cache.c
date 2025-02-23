#include "cache.h"

static lru_cache_node* lru_cache_node_create(lru_cache_entry* entry);
static lru_cache_entry* lru_cache_entry_create(const uint64_t* key, uint8_t* value);
static void lru_cache_node_destroy(lru_cache_node* node);
static void lru_cache_insert_at_head(lru_cache* cache, lru_cache_node* node);

lru_cache* auxts_lru_cache_create(size_t capacity) {
    lru_cache* cache = malloc(sizeof(lru_cache));
    if (!cache) {
        perror("Failed to allocate lru_cache");
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

void auxts_lru_cache_put(lru_cache* cache, const uint64_t* key, uint8_t* value) {
    if (!cache) {
        return;
    }

    pthread_rwlock_wrlock(&cache->rwlock);

    lru_cache_entry* entry = auxts_hashtable_get(cache->cache, key);
    if (entry) {
        pthread_rwlock_unlock(&cache->rwlock);
        return;
    }

    if (cache->size >= cache->capacity) {
        auxts_lru_cache_evict(cache);
    }

    entry = lru_cache_entry_create(key, value);
    lru_cache_node* node = lru_cache_node_create(entry);

    lru_cache_insert_at_head(cache, node);
    auxts_hashtable_put(cache->cache, key, entry);

    ++cache->size;

    pthread_rwlock_unlock(&cache->rwlock);
}

uint8_t* auxts_lru_cache_get(lru_cache* cache, const uint64_t* key) {
    if (!cache) {
        return NULL;
    }

    pthread_rwlock_rdlock(&cache->rwlock);

    lru_cache_entry* entry = auxts_hashtable_get(cache->cache, key);
    pthread_rwlock_unlock(&cache->rwlock);

    if (entry) {
        return entry->value;
    }

    return NULL;
}

void auxts_lru_cache_destroy(lru_cache* cache) {
    if (!cache) {
        return;
    }

    pthread_rwlock_wrlock(&cache->rwlock);

    lru_cache_node* node = cache->head;

    while (node) {
        lru_cache_node* next = (lru_cache_node*) node->next;
        lru_cache_node_destroy(node);
        node = next;
    }

    auxts_hashtable_destroy(cache->cache);

    pthread_rwlock_unlock(&cache->rwlock);
    pthread_rwlock_destroy(&cache->rwlock);

    free(cache);
}

void auxts_lru_cache_evict(lru_cache* cache) {
    if (!cache) {
        return;
    }

    lru_cache_node* tail = cache->tail;
    lru_cache_entry* entry = tail->entry;

    free(entry->value);
    auxts_hashtable_delete(cache->cache, entry->key);

    cache->tail = (lru_cache_node*) tail->prev;
    cache->tail->next = NULL;

    --cache->size;
}

lru_cache_entry* lru_cache_entry_create(const uint64_t* key, uint8_t* value) {
    lru_cache_entry* entry = malloc(sizeof(lru_cache_entry));
    if (!entry) {
        perror("Failed to allocate lru_cache_entry");
        return NULL;
    }

    entry->key[0] = key[0];
    entry->key[1] = key[1];
    entry->value = value;

    return entry;
}

lru_cache_node* lru_cache_node_create(lru_cache_entry* entry) {
    if (!entry) {
        return NULL;
    }

    lru_cache_node* node = malloc(sizeof(lru_cache_node));
    if (!node) {
        perror("Failed to allocate lru_cache_node");
        return NULL;
    }

    node->entry = entry;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

void lru_cache_insert_at_head(lru_cache* cache, lru_cache_node* node) {
    if (!cache || !node) {
        return;
    }

    node->prev = NULL;
    node->next = (struct lru_cacheNode *)cache->head;

    if (cache->head) {
        cache->head->prev = (struct lru_cache_node*)node;
    } else {
        cache->tail = node;
    }

    cache->head = node;
}

void lru_cache_node_destroy(lru_cache_node* node) {
    if (!node) {
        return;
    }

    lru_cache_entry* entry = node->entry;
    free(entry->value);
    free(node);
}
