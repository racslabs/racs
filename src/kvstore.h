// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_KVSTORE_H
#define RACS_KVSTORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "log.h"

typedef racs_uint64 (*racs_kvstore_hash_callback)(void *key);

typedef int (*racs_kvstore_cmp_callback)(void *a, void *b);

typedef void (*racs_kvstore_destroy_callback)(void *key, void *value);

typedef struct {
    racs_kvstore_hash_callback hash;
    racs_kvstore_cmp_callback cmp;
    racs_kvstore_destroy_callback destroy;
} racs_kvstore_ops;

typedef struct {
    void *key;
    void *value;
    struct racs_kvstore_entry *next;
} racs_kvstore_entry;

typedef struct {
    size_t count;
    racs_kvstore_entry *node;
} racs_kvstore_bin;

typedef struct {
    size_t capacity;
    racs_kvstore_ops ops;
    racs_kvstore_bin *bins;
} racs_kvstore;

void racs_kvstore_bin_init(racs_kvstore_bin *bin);

void racs_kvstore_bin_append(racs_kvstore_bin *bin, void *key, void *value);

racs_kvstore_bin *racs_kvstore_get_bin(racs_kvstore *kv, void *key);

racs_kvstore *racs_kvstore_create(size_t capacity, racs_kvstore_hash_callback hash, racs_kvstore_cmp_callback cmp,
                                  racs_kvstore_destroy_callback destroy);

void racs_kvstore_put(racs_kvstore *kv, void *key, void *value);

void *racs_kvstore_get(racs_kvstore *kv, void *key);

void racs_kvstore_delete(racs_kvstore *kv, void *key);

void racs_kvstore_destroy(racs_kvstore *kv);

#ifdef __cplusplus
}
#endif

#endif //RACS_KVSTORE_H
