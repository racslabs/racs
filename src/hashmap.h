#ifndef AUXTS_HASHMAP_H
#define AUXTS_HASHMAP_H

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
} auxts_hashmap_ops;

typedef struct {
    void* key;
    void* value;
} auxts_hashmap_entry;

typedef struct {
    size_t count;
    size_t capacity;
    auxts_hashmap_entry* entries;
} auxts_hashmap_bucket;

typedef struct {
    size_t num_buckets;
    auxts_hashmap_ops ops;
    auxts_hashmap_bucket* buckets;
} auxts_hashmap;

auxts_hashmap* auxts_hashmap_create(size_t capacity, auxts_hash_func hash, auxts_cmp_func cmp, auxts_destroy_func destroy);
void auxts_hashmap_put(auxts_hashmap* map, void* key, void* value);
void* auxts_hashmap_get(auxts_hashmap* map, void* key);
void auxts_hashmap_delete(auxts_hashmap* map, void* key);
void auxts_hashmap_destroy(auxts_hashmap* map);

#endif //AUXTS_HASHMAP_H
