// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#include "cache.h"


static void racs_cache_evict(racs_cache *cache);

static racs_cache_node *racs_cache_node_create(void *key, void *value);

static void racs_cache_move_to_head(racs_cache *cache, racs_cache_node *node);


racs_cache *racs_cache_create(size_t capacity, racs_dict_callbacks callbacks) {
    racs_cache *cache = malloc(sizeof(racs_cache));
    if (!cache) {
        return NULL;
    }

    pthread_rwlock_init(&cache->rwlock, NULL);

    cache->size = 0;
    cache->capacity = capacity;
    cache->head = NULL;
    cache->tail = NULL;
    cache->dict = racs_dict_create(capacity, callbacks);

    return cache;
}

void *racs_cache_get(racs_cache *cache, const void *key) {
    if (!cache || !cache->dict) {
        return NULL;
    }

    pthread_rwlock_wrlock(&cache->rwlock);

    racs_cache_node *node = racs_dict_get(cache->dict, key);
    if (!node) {
        pthread_rwlock_unlock(&cache->rwlock);
        return NULL;
    }

    racs_cache_move_to_head(cache, node);
    pthread_rwlock_unlock(&cache->rwlock);

    return node->entry.value;
}

void racs_cache_put(racs_cache *cache, const void *key, void *value) {
    if (!cache || !cache->dict) {
        return;
    }

    pthread_rwlock_wrlock(&cache->rwlock);

    racs_cache_node *node = racs_dict_get(cache->dict, key);
    if (node) {
        racs_cache_move_to_head(cache, node);
        pthread_rwlock_unlock(&cache->rwlock);
        return;
    }

    if (cache->size >= cache->capacity) {
        racs_cache_evict(cache);
    }

    node = racs_cache_node_create((void *)key, value);
    racs_cache_move_to_head(cache, node);
    racs_dict_put(cache->dict, key, node);

    ++cache->size;

    pthread_rwlock_unlock(&cache->rwlock);
}


void racs_cache_destroy(racs_cache *cache) {
    if (!cache) {
        return;
    }

    pthread_rwlock_wrlock(&cache->rwlock);

    if (cache->dict) {
        racs_dict_destroy(cache->dict);
    }

    pthread_rwlock_unlock(&cache->rwlock);
    pthread_rwlock_destroy(&cache->rwlock);

    free(cache);
}

void racs_cache_evict(racs_cache *cache) {
    racs_cache_node *tail = cache->tail;
    racs_cache_node *prev = cache->tail->prev;

    racs_dict_delete(cache->dict, tail);
    cache->tail = prev;

    if (cache->tail) {
        cache->tail->next = NULL;
    }

    --cache->size;
}

racs_cache_node *racs_cache_node_create(void *key, void *value) {
    racs_cache_node *node = malloc(sizeof(racs_cache_node));
    if (!node) {
        return NULL;
    }

    node->entry.key = key;
    node->entry.value = value;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

void racs_cache_move_to_head(racs_cache *cache, racs_cache_node *node) {
    if (!node) {
        return;
    }

    if (!cache->head) {
        cache->head = node;
        cache->tail = node;
        node->prev = NULL;
        node->next = NULL;
        return;
    }

    if (cache->head == node) {
        return;
    }

    if (cache->tail == node) {
        cache->tail = node->prev;
        if (cache->tail) {
            cache->tail->next = NULL;
        }
    }

    racs_cache_node *prev = node->prev;
    racs_cache_node *next = node->next;

    if (prev) {
        prev->next = next;
    }

    if (next) {
        next->prev = prev;
    }

    node->prev = NULL;
    node->next = cache->head;
    cache->head->prev = node;
    cache->head = node;
}
