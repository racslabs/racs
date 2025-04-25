#ifndef AUXTS_KVSTORE_H
#define AUXTS_KVSTORE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef uint64_t (*rats_kvstore_hash_callback)(void* key);
typedef int (*rats_kvstore_cmp_callback)(void* a, void* b);
typedef void (*rats_kvstore_destroy_callback)(void* key, void* value);

typedef struct {
    rats_kvstore_hash_callback hash;
    rats_kvstore_cmp_callback cmp;
    rats_kvstore_destroy_callback destroy;
} rats_kvstore_ops;

typedef struct {
    void* key;
    void* value;
} rats_kvstore_entry;

typedef struct {
    size_t count;
    size_t capacity;
    rats_kvstore_entry* entries;
} rats_kvstore_bin;

typedef struct {
    size_t capacity;
    rats_kvstore_ops ops;
    rats_kvstore_bin* bins;
} rats_kvstore;

rats_kvstore* rats_kvstore_create(size_t capacity, rats_kvstore_hash_callback hash, rats_kvstore_cmp_callback cmp, rats_kvstore_destroy_callback destroy);
void rats_kvstore_put(rats_kvstore* kv, void* key, void* value);
void* rats_kvstore_get(rats_kvstore* kv, void* key);
void rats_kvstore_delete(rats_kvstore* kv, void* key);
void rats_kvstore_destroy(rats_kvstore* kv);

#endif //AUXTS_KVSTORE_H
