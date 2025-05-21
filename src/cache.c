#include "cache.h"

racs_cache *racs_scache_create(size_t capacity) {
    racs_cache *cache = malloc(sizeof(racs_cache));
    if (!cache) {
        perror("Failed to allocate racs_cache");
        return NULL;
    }

    pthread_rwlock_init(&cache->rwlock, NULL);

    cache->size = 0;
    cache->capacity = capacity;
    cache->head = NULL;
    cache->tail = NULL;
    cache->kv = racs_kvstore_create(capacity, racs_cache_hash, racs_cache_cmp, racs_scache_destroy);

    return cache;
}

void racs_cache_put(racs_cache *cache, const racs_uint64 *key, racs_uint8 *value) {
    if (!cache) return;

    racs_cache_entry *entry = racs_kvstore_get(cache->kv, key);
    if (entry) return;

    pthread_rwlock_wrlock(&cache->rwlock);

    if (cache->size >= cache->capacity)
        racs_cache_evict(cache);

    racs_cache_node *node = racs_cache_node_create(key, value);

    racs_cache_move_to_head(cache, node);
    racs_kvstore_put(cache->kv, key, &node->entry);

    ++cache->size;

    pthread_rwlock_unlock(&cache->rwlock);
}

racs_uint8 *racs_cache_get(racs_cache *cache, const racs_uint64 *key) {
    if (!cache) return NULL;

    pthread_rwlock_rdlock(&cache->rwlock);

    racs_cache_entry *entry = racs_kvstore_get(cache->kv, key);
    pthread_rwlock_unlock(&cache->rwlock);

    if (entry) return entry->value;

    return NULL;
}

void racs_cache_destroy(racs_cache *cache) {
    if (!cache) return;

    pthread_rwlock_wrlock(&cache->rwlock);
    racs_kvstore_destroy(cache->kv);

    racs_cache_node *node = cache->head;
    while (node) {
        racs_cache_node *next = (racs_cache_node *) node->next;
        free(node);
        node = next;
    }

    pthread_rwlock_unlock(&cache->rwlock);
    pthread_rwlock_destroy(&cache->rwlock);

    free(cache);
}

void racs_cache_evict(racs_cache *cache) {
    if (!cache) return;

    racs_cache_node *tail = cache->tail;
    racs_cache_entry *entry = &tail->entry;

    racs_kvstore_delete(cache->kv, entry->key);

    cache->tail = (racs_cache_node *) tail->prev;
    cache->tail->next = NULL;

    --cache->size;
}

racs_cache_node *racs_cache_node_create(const racs_uint64 *key, racs_uint8 *value) {
    racs_cache_node *node = malloc(sizeof(racs_cache_node));
    if (!node) {
        perror("Failed to allocate racs_cache_node");
        return NULL;
    }

    node->entry.key[0] = key[0];
    node->entry.key[1] = key[1];
    node->entry.value = value;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

void racs_cache_move_to_head(racs_cache *cache, racs_cache_node *node) {
    if (!cache || !node) return;

    node->prev = NULL;
    node->next = (struct racs_cache_node *) cache->head;

    if (cache->head) {
        cache->head->prev = (struct racs_cache_node *) node;
    } else {
        cache->tail = node;
    }

    cache->head = node;
}

racs_uint64 racs_cache_hash(void *key) {
    racs_uint64 hash[2];
    murmur3_x64_128(key, 2 * sizeof(racs_uint64), 0, hash);
    return hash[0];
}

int racs_cache_cmp(void *a, void *b) {
    racs_uint64 *x = (racs_uint64 *) a;
    racs_uint64 *y = (racs_uint64 *) b;
    return x[0] == y[0] && x[1] == y[1];
}

void racs_scache_destroy(void *key, void *value) {
    racs_cache_entry *entry = (racs_cache_entry *) value;
    free(entry->value);
}
