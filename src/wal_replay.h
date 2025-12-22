// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_WAL_REPLAY_H
#define RACS_WAL_REPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stream.h"
#include "wal.h"

typedef struct {
    racs_uint32  op_code;
    racs_uint32  checksum;
    racs_uint64  size;
    racs_uint8  *op;
    racs_uint64  lsn;
} racs_wal_entry;

typedef struct {
    int         fd;
    size_t      size;
    racs_uint8 *data;
} racs_wal_segment;

void racs_wal_replay(racs_multi_memtable *mmt, racs_offsets *offset);

racs_wal_entry *racs_wal_entry_read(racs_uint8 *buf, off_t *offset);

void racs_wal_entry_destroy(racs_wal_entry *entry);

racs_wal_segment *racs_wal_segment_read(const char *path);

void racs_wal_segment_destroy(racs_wal_segment *segment);

#ifdef __cplusplus
}
#endif

#endif //RACS_WAL_REPLAY_H