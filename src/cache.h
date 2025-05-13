
#ifndef RACS_CACHE_H
#define RACS_CACHE_H

#include "kvstore.h"
#include "murmur3.h"
#include "types.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    racs_uint64 key[2];
    racs_uint8 *value;
} racs_cache_entry;

typedef struct {
    racs_cache_entry entry;
    struct racs_cache_node *prev;
    struct racs_cache_node *next;
} racs_cache_node;

typedef struct {
    size_t size;
    size_t capacity;
    racs_cache_node *head;
    racs_cache_node *tail;
    racs_kvstore *kv;
    pthread_rwlock_t rwlock;
} racs_cache;

racs_cache *racs_scache_create(size_t capacity);

racs_uint8 *racs_cache_get(racs_cache *cache, const racs_uint64 *key);

void racs_cache_put(racs_cache *cache, const racs_uint64 *key, racs_uint8 *value);

void racs_cache_evict(racs_cache *cache);

void racs_cache_destroy(racs_cache *cache);

racs_uint64 racs_cache_hash(void *key);

int racs_cache_cmp(void *a, void *b);

void racs_scache_destroy(void *key, void *value);

racs_cache_node *racs_cache_node_create(const racs_uint64 *key, racs_uint8 *value);

void racs_cache_insert_at_head(racs_cache *cache, racs_cache_node *node);

#ifdef __cplusplus
}
#endif

#endif //RACS_CACHE_H
