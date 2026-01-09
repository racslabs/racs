// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_STREAM_H
#define RACS_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "metadata.h"
#include "memtable.h"
#include "frame.h"
#include "offsets.h"
#include "result.h"
#include "log.h"
#include "wal.h"
#include "zstd.h"
#include <msgpack.h>

typedef enum {
    RACS_STREAM_OK,
    RACS_STREAM_MALFORMED,
    RACS_STREAM_CONFLICT,
    RACS_STREAM_NOT_FOUND
} racs_stream_status;

typedef struct {
    racs_kvstore *kv;
    pthread_rwlock_t rwlock;
} racs_streamkv;

extern const char *const racs_stream_status_string[];

int racs_stream_create(const char* stream_id, racs_uint32 sample_rate, racs_uint16 channels, racs_uint16 bit_depth);

int racs_stream_append(racs_multi_memtable *mmt, racs_offsets *offsets, racs_streamkv *kv, racs_uint8 *data);

void racs_stream_batch_append(racs_multi_memtable *mmt, racs_offsets *offsets, racs_streamkv *kv, racs_uint8 *data, size_t size);

int racs_stream_open(racs_streamkv *kv, racs_uint64 stream_id);

int racs_stream_close(racs_streamkv *kv, racs_uint64 stream_id);

racs_uint8 *racs_streamkv_get(racs_streamkv *kv, racs_uint64 stream_id);

void racs_streamkv_delete(racs_streamkv *kv, racs_uint64 stream_id);

void racs_streamkv_put(racs_streamkv *kv, racs_uint64 stream_id, racs_uint8 *session_id);

racs_streamkv *racs_streamkv_create(int capacity);

void racs_streamkv_destroy(racs_streamkv *kv);

racs_uint64 racs_streamkv_hash(void *key);

int racs_streamkv_cmp(void *a, void *b);

void racs_streamkv_destroy_entry(void *key, void *value);

int racs_session_cmp(const racs_uint8 *src, const racs_uint8 *dest);

#ifdef __cplusplus
}
#endif

#endif //RACS_STREAM_H