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
    racs_uint8  id[16];
    racs_uint16 channels;
    racs_uint16 bit_depth;
    racs_uint32 sample_rate;
    racs_time   ref;
    racs_uint64 stream_id;
} racs_session;

typedef struct {
    racs_kvstore *kv;
    pthread_rwlock_t rwlock;
} racs_sessions;

extern const char *const racs_stream_status_string[];

int racs_stream_create(racs_versions *versions, const char* stream_id, racs_uint32 sample_rate, racs_uint16 channels, racs_uint16 bit_depth);

int racs_stream_append(racs_multi_memtable *mmt, racs_offsets *offsets, racs_versions *versions, racs_sessions *sessions, racs_uint8 *data);

void racs_stream_batch_append(racs_multi_memtable *mmt, racs_offsets *offsets, racs_versions *versions, racs_sessions *kv, racs_uint8 *data, size_t size);

int racs_stream_open(racs_sessions *sessions, racs_uint64 stream_id, racs_uint8 *session_id);

int racs_stream_close(racs_sessions *sessions, racs_uint64 stream_id);

racs_uint8 *racs_sessions_get(racs_sessions *sessions, racs_uint64 stream_id);

void racs_sessions_delete(racs_sessions *sessions, racs_uint64 stream_id);

void racs_sessions_put(racs_sessions *sessions, racs_uint64 stream_id, racs_uint8 *session);

racs_sessions *racs_sessions_create(int capacity);

void racs_sessions_destroy(racs_sessions *sessions);

racs_uint64 racs_sessions_hash(void *key);

int racs_sessions_cmp(void *a, void *b);

void racs_sessions_destroy_entry(void *key, void *value);

int racs_sessions_id_cmp(const racs_uint8 *src, const racs_uint8 *dest);

off_t racs_session_read(racs_session *session, racs_uint8 *buf);

off_t racs_session_write(racs_uint8 *buf, racs_session *session);

#ifdef __cplusplus
}
#endif

#endif //RACS_STREAM_H