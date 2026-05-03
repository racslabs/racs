// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#ifndef RACS_CACHE_H
#define RACS_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dict.h"
#include "murmurhash3.h"
#include <pthread.h>


typedef struct {
    void *key;
    void *value;
} racs_cache_entry;

typedef struct racs_cache_node {
    racs_cache_entry        entry;
    struct racs_cache_node *prev;
    struct racs_cache_node *next;
} racs_cache_node;

typedef struct {
    size_t            size;
    size_t            capacity;
    racs_cache_node  *head;
    racs_cache_node  *tail;
    racs_dict        *dict;
    pthread_rwlock_t  rwlock;
} racs_cache;


racs_cache *racs_cache_create(size_t capacity, racs_dict_callbacks callbacks);

void *racs_cache_get(racs_cache *cache, const void *key);

void racs_cache_put(racs_cache *cache, const void *key, void *value);

void racs_cache_destroy(racs_cache *cache);


#ifdef __cplusplus
}
#endif

#endif //RACS_CACHE_H
