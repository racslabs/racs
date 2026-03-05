// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_VERSION_H
#define RACS_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "export.h"
#include "kvstore.h"
#include "murmur3.h"
#include "metadata.h"
#include <stdio.h>

#define RACS_VERSION_MAJOR  0
#define RACS_VERSION_MINOR  1
#define RACS_VERSION_PATCH  0

#define RACS_VERSION ((RACS_VERSION_MAJOR * 10000) + \
                      (RACS_VERSION_MINOR * 100) +  \
                       RACS_VERSION_PATCH)

RACS_FORCE_INLINE void racs_version(char* buf) {
    sprintf(buf, "racs %d.%d.%d  (%s)", RACS_VERSION_MAJOR, RACS_VERSION_MINOR, RACS_VERSION_PATCH, __DATE__);
}

#define RACS_MAX_VERSIONS (1048576)

typedef struct {
    racs_kvstore *kv;
    pthread_rwlock_t rwlock;
} racs_versions;

racs_uint64 racs_versions_hash(void *key);

int racs_versions_cmp(void *a, void *b);

void racs_versions_destroy_entry(void *key, void *value);

racs_versions *racs_versions_create();

racs_uint64 racs_versions_get(racs_versions *versions, racs_uint64 stream_id);

void racs_versions_put(racs_versions *versions, racs_uint64 stream_id, racs_uint64 version);

void racs_versions_init(racs_versions *versions);

void racs_versions_destroy(racs_versions *versions);

#ifdef __cplusplus
}
#endif

#endif //RACS_VERSION_H
