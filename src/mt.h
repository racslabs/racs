// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#ifndef RACS_MT_H
#define RACS_MT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dict.h"
#include "types.h"
#include "path.h"
#include "fs.h"
#include "sst.h"
#include <stdio.h>
#include <pthread.h>

typedef enum {
    RACS_MT_STATE_ACTIVE,
    RACS_MT_STATE_IMMUTABLE,
    RACS_MT_STATE_FLUSHING
} racs_mt_state;

typedef struct {
    racs_uint64 key[3];
    racs_uint64 lsn;
    racs_uint32 block_size;
    racs_uint32 checksum;
    racs_uint8 *block;
} racs_mt_entry;

typedef struct {
    racs_uint16 capacity;
    racs_uint16 num_entries;
    pthread_mutex_t mutex;
    racs_mt_entry *entries;
} racs_mt;

typedef struct {
    int capacity;
    racs_dict *dict;
} racs_mt_parts;

typedef struct racs_mt_node {
    racs_mt *mt;
    racs_mt_state state;
    struct racs_mt_node *next;
    struct racs_mt_node *prev;
} racs_mt_node;

typedef struct {
    int capacity;
    int num_tables;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    racs_mt_node *head;
    racs_mt_node *tail;
} racs_mmt;


racs_mt *racs_mt_create(int capacity);

void racs_mt_put(racs_mt *mt,
                 const racs_uint64 *key,
                 const racs_uint8 *block,
                 racs_uint16 block_size,
                 racs_uint32 checksum,
                 racs_uint64 lsn);

void racs_mt_flush(racs_mt *mt, const char *path);

void racs_mt_destroy(racs_mt *mt);

racs_mt_parts *racs_mt_parts_create(int capacity);

void racs_mt_parts_put(racs_mt_parts *parts,
                       const racs_uint64 *key,
                       const racs_uint8 *block,
                       racs_uint16 block_size,
                       racs_uint32 checksum,
                       racs_uint64 lsn);

void racs_mt_parts_destroy(racs_mt_parts *parts);

void racs_mt_split_and_flush(racs_mt *mt);

#ifdef __cplusplus
}
#endif

#endif //RACS_MT_H
