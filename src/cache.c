// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "cache.h"

racs_cache *racs_scache_create(size_t capacity) {
    racs_cache *cache = malloc(sizeof(racs_cache));
    if (!cache) {
        racs_log_fatal("Failed to allocate racs_cache");
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

    pthread_rwlock_wrlock(&cache->rwlock);

    racs_cache_node *_node = racs_kvstore_get(cache->kv, key);
    if (_node) {
        racs_cache_move_to_head(cache, _node);
        pthread_rwlock_unlock(&cache->rwlock);
        return;
    }

    if (cache->size >= cache->capacity)
        racs_cache_evict(cache);

    racs_cache_node *node = racs_cache_node_create(key, value);

    racs_cache_move_to_head(cache, node);
    racs_kvstore_put(cache->kv, key, node);

    ++cache->size;

    pthread_rwlock_unlock(&cache->rwlock);
}

racs_uint8 *racs_cache_get(racs_cache *cache, const racs_uint64 *key) {
    if (!cache) return NULL;

    pthread_rwlock_rdlock(&cache->rwlock);

    racs_cache_node *node = racs_kvstore_get(cache->kv, key);
    if (!node) {
        pthread_rwlock_unlock(&cache->rwlock);
        return NULL;
    }

    racs_cache_move_to_head(cache, node);
    pthread_rwlock_unlock(&cache->rwlock);

    return node->entry.value;
}

void racs_cache_destroy(racs_cache *cache) {
    if (!cache) return;

    pthread_rwlock_wrlock(&cache->rwlock);
    racs_kvstore_destroy(cache->kv);

    pthread_rwlock_unlock(&cache->rwlock);
    pthread_rwlock_destroy(&cache->rwlock);

    free(cache);
}

void racs_cache_evict(racs_cache *cache) {
    if (!cache) return;

    racs_cache_node *tail = cache->tail;
    racs_cache_node *prev = (racs_cache_node *) cache->tail->prev;

    racs_kvstore_delete(cache->kv, tail->entry.key);

    cache->tail = prev;
    if (cache->tail) cache->tail->next = NULL;

    --cache->size;
}

racs_cache_node *racs_cache_node_create(const racs_uint64 *key, racs_uint8 *value) {
    racs_cache_node *node = malloc(sizeof(racs_cache_node));
    if (!node) {
        racs_log_fatal("Failed to allocate racs_cache_node");
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

    if (!cache->head) {
        cache->head = node;
        cache->tail = node;
        node->prev = NULL;
        node->next = NULL;
        return;
    }

    if (cache->head == node)
        return;

    if (cache->tail == node) {
        cache->tail = (racs_cache_node *) node->prev;
        if (cache->tail) cache->tail->next = NULL;
    }

    racs_cache_node *prev = (racs_cache_node *) node->prev;
    racs_cache_node *next = (racs_cache_node *) node->next;

    if (prev) prev->next = (struct racs_cache_node *) next;
    if (next) next->prev = (struct racs_cache_node *) prev;

    node->prev = NULL;
    node->next = (struct racs_cache_node *) cache->head;
    cache->head->prev = (struct racs_cache_node *) node;
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
    free(key);
    racs_cache_node *node = (racs_cache_node *) value;
    free(node->entry.value);
    free(node);
}
