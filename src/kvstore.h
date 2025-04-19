#ifndef AUXTS_KVSTORE_H
#define AUXTS_KVSTORE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef uint64_t (*auxts_kvstore_hash_callback)(void* key);
typedef int (*auxts_kvstore_cmp_callback)(void* a, void* b);
typedef void (*auxts_kvstore_destroy_callback)(void* key, void* value);

typedef struct {
    auxts_kvstore_hash_callback hash;
    auxts_kvstore_cmp_callback cmp;
    auxts_kvstore_destroy_callback destroy;
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

auxts_kvstore* auxts_kvstore_create(size_t capacity, auxts_kvstore_hash_callback hash, auxts_kvstore_cmp_callback cmp, auxts_kvstore_destroy_callback destroy);
void auxts_kvstore_put(auxts_kvstore* kv, void* key, void* value);
void* auxts_kvstore_get(auxts_kvstore* kv, void* key);
void auxts_kvstore_delete(auxts_kvstore* kv, void* key);
void auxts_kvstore_destroy(auxts_kvstore* kv);

#endif //AUXTS_KVSTORE_H
