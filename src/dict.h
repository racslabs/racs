// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#ifndef RACS_DICT_H
#define RACS_DICT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"


typedef racs_uint64 (*racs_dict_hash_callback)   (const void *key);
typedef int         (*racs_dict_eq_callback)     (const void *a, const void *b);
typedef void        (*racs_dict_destroy_callback)(void *key, void *value);

typedef struct {
    racs_dict_hash_callback    hash;
    racs_dict_eq_callback      eq;
    racs_dict_destroy_callback destroy;
} racs_dict_callbacks;

typedef struct racs_dict_entry {
    void *key;
    void *value;
    struct racs_dict_entry *next;
} racs_dict_entry;

typedef struct {
    size_t count;
    racs_dict_entry *head;
} racs_dict_bucket;

typedef struct {
    size_t              size;
    size_t              capacity;
    racs_dict_callbacks callbacks;
    racs_dict_bucket   *buckets;
} racs_dict;


racs_dict *racs_dict_create(size_t capacity, racs_dict_callbacks callbacks);

void *racs_dict_get(racs_dict *dict, const void *key);

void racs_dict_put(racs_dict *dict, const void *key, void *value);

void racs_dict_delete(racs_dict *dict, const void *key);

void racs_dict_destroy(racs_dict *dict);

#ifdef __cplusplus
}
#endif

#endif //RACS_DICT_H
