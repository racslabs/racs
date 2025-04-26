
#ifndef RATS_CACHE_H
#define RATS_CACHE_H

#include "kvstore.h"
#include "murmur3.h"
#include "types.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    rats_uint64 key[2];
    rats_uint8* value;
} rats_cache_entry;

typedef struct {
    rats_cache_entry entry;
    struct rats_cache_node* prev;
    struct rats_cache_node* next;
} rats_cache_node;

typedef struct {
    size_t size;
    size_t capacity;
    rats_cache_node* head;
    rats_cache_node* tail;
    rats_kvstore* kv;
    pthread_rwlock_t rwlock;
} rats_cache;

rats_cache* rats_scache_create(size_t capacity);
rats_uint8* rats_cache_get(rats_cache* cache, const rats_uint64* key);
void rats_cache_put(rats_cache* cache, const rats_uint64* key, rats_uint8* value);
void rats_cache_evict(rats_cache* cache);
void rats_cache_destroy(rats_cache* cache);
rats_uint64 rats_cache_hash(void* key);
int rats_cache_cmp(void* a, void* b);
void rats_scache_destroy(void* key, void* value);

#ifdef __cplusplus
}
#endif

#endif //RATS_CACHE_H
