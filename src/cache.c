// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#include "cache.h"


void racs_cache_evict(racs_cache *cache);

racs_cache_node *racs_cache_node_create(void *key, void *value);

void racs_cache_move_to_head(racs_cache *cache, racs_cache_node *node);

void racs_cache_empty_destroy_cb(void *key, void *value);


racs_cache *racs_cache_create(size_t capacity, racs_cache_cb cb) {
    racs_cache *cache = malloc(sizeof(racs_cache));
    if (!cache) {
        return NULL;
    }

    racs_dict_cb dict_cb = {
        .hash = cb.hash,
        .eq = cb.eq,
        .destroy = racs_cache_empty_destroy_cb,
    };

    pthread_mutex_init(&cache->mutex, NULL);

    cache->size = 0;
    cache->capacity = capacity;
    cache->head = NULL;
    cache->tail = NULL;
    cache->dict = racs_dict_create(capacity, dict_cb);
    cache->cb = cb;

    return cache;
}

void *racs_cache_get(racs_cache *cache, const void *key) {
    if (!cache || !cache->dict) {
        return NULL;
    }

    pthread_mutex_lock(&cache->mutex);

    racs_cache_node *node = racs_dict_get(cache->dict, key);
    if (!node) {
        pthread_mutex_unlock(&cache->mutex);
        return NULL;
    }

    racs_cache_move_to_head(cache, node);
    pthread_mutex_unlock(&cache->mutex);

    return node->entry.value;
}

void racs_cache_put(racs_cache *cache, const void *key, void *value) {
    if (!cache || !cache->dict) {
        return;
    }

    pthread_mutex_lock(&cache->mutex);

    racs_cache_node *node = racs_dict_get(cache->dict, key);
    if (node) {
        racs_cache_move_to_head(cache, node);
        pthread_mutex_unlock(&cache->mutex);
        return;
    }

    if (cache->size >= cache->capacity) {
        racs_cache_evict(cache);
    }

    node = racs_cache_node_create((void *)key, value);
    racs_cache_move_to_head(cache, node);
    racs_dict_put(cache->dict, key, node);

    ++cache->size;

    pthread_mutex_unlock(&cache->mutex);
}


void racs_cache_destroy(racs_cache *cache) {
    if (!cache) {
        return;
    }

    pthread_mutex_lock(&cache->mutex);
    racs_cache_destroy_cb destroy = cache->cb.destroy;

    for (racs_cache_node *curr = cache->head, *next; curr; curr = next) {
        next = curr->next;

        if (destroy) {
            destroy(curr->entry.key, curr->entry.value);
        }

        free(curr);
    }

    if (cache->dict) {
        racs_dict_destroy(cache->dict);
    }

    pthread_mutex_unlock(&cache->mutex);
    pthread_mutex_destroy(&cache->mutex);

    free(cache);
}

void racs_cache_evict(racs_cache *cache) {
    if (!cache->tail) {
        return;
    }

    racs_cache_node *tail = cache->tail;
    racs_cache_node *prev = cache->tail->prev;
    racs_cache_destroy_cb destroy = cache->cb.destroy;

    racs_dict_delete(cache->dict, tail->entry.key);

    if (destroy) {
        destroy(tail->entry.key, tail->entry.value);
    }

    cache->tail = prev;
    if (cache->tail) {
        cache->tail->next = NULL;
    } else {
        cache->head = NULL;
    }

    free(tail);
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

void racs_cache_empty_destroy_cb(void *key, void *value) {}
