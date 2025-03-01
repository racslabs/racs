#ifndef AUXTS_KVSTORE_H
#define AUXTS_KVSTORE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef uint64_t (*auxts_hash_func)(void* key);
typedef int (*auxts_cmp_func)(void* a, void* b);
typedef void (*auxts_destroy_func)(void* key, void* value);

typedef struct {
    auxts_hash_func hash;
    auxts_cmp_func cmp;
    auxts_destroy_func destroy;
} auxts_kvstore_ops;

typedef struct {
    void* key;
    void* value;
} auxts_kvstore_entry;

typedef struct {
    size_t count;
    size_t capacity;
    auxts_kvstore_entry* entries;
} auxts_kvstore_bin;

typedef struct {
    size_t capacity;
    auxts_kvstore_ops ops;
    auxts_kvstore_bin* bins;
} auxts_kvstore;

auxts_kvstore* auxts_kvstore_create(size_t capacity, auxts_hash_func hash, auxts_cmp_func cmp, auxts_destroy_func destroy);
void auxts_kvstore_put(auxts_kvstore* kv, void* key, void* value);
void* auxts_kvstore_get(auxts_kvstore* kv, void* key);
void auxts_kvstore_delete(auxts_kvstore* kv, void* key);
void auxts_kvstore_destroy(auxts_kvstore* kv);

#endif //AUXTS_KVSTORE_H
