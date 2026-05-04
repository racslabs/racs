// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#ifndef RACS_MEMTABLE_H
#define RACS_MEMTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dict.h"
#include "types.h"
#include "sstable.h"
#include <stdio.h>
#include <limits.h>
#include <pthread.h>

typedef struct {
    racs_uint64 key[3];
    racs_uint64 lsn;
    racs_uint32 block_size;
    racs_uint32 checksum;
    racs_uint8 *block;
} racs_memtable_entry;

typedef struct racs_memtable {
    racs_memtable_entry *entries;
    racs_uint16 num_entries;
    racs_uint16 capacity;
    pthread_mutex_t mutex;
    struct racs_memtable *next;
    struct racs_memtable *prev;
} racs_memtable;

racs_memtable *racs_memtable_create(int capacity);

void racs_memtable_append(racs_memtable *mt,
                          const racs_uint64 *key,
                          const racs_uint8 *block,
                          racs_uint16 block_size,
                          racs_uint32 checksum,
                          racs_uint64 lsn);

void racs_memtable_flush(racs_memtable *mt, const char *path);

void racs_memtable_destroy(racs_memtable *mt);

#ifdef __cplusplus
}
#endif

#endif //RACS_MEMTABLE_H
