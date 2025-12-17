// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_WAL_H
#define RACS_WAL_H

#include "types.h"
#include "bytes.h"
#include "crc32c.h"
#include "log.h"
#include "streaminfo.h"

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RACS_OP_CODE_CREATE,
    RACS_OP_CODE_APPEND,
} racs_op_code;

typedef struct {
    racs_op_code op_code;
    racs_uint32  checksum;
    size_t       size;
    racs_uint8  *op;
    racs_uint64  seq;
} racs_wal_entry;

typedef struct {
    int             fd;
    int             mf0_fd;
    int             mf1_fd;
    racs_uint64     seq;
    size_t          size;
    pthread_mutex_t mutex;
} racs_wal;

static racs_wal *_wal = NULL;

extern const char *racs_wal_dir;

#define RACS_WAL_FSYNC 50

#define racs_wal_mf0(seq) racs_wal_mf0_(racs_wal_instance(), seq)

#define racs_wal_mf1(seq) racs_wal_mf1_(racs_wal_instance(), seq)

#define racs_wal_append(op_code, size, op) racs_wal_append_(racs_wal_instance(), op_code, size, op)

#define racs_wal_seq _wal->seq

racs_wal *racs_wal_create();

racs_wal *racs_wal_instance();

void racs_wal_mf0_(racs_wal *wal, racs_uint64 seq);

void racs_wal_mf1_(racs_wal *wal, racs_uint64 seq);

void racs_wal_append_(racs_wal *wal, racs_op_code op_code, size_t size, racs_uint8 *op);

void racs_wal_open(racs_wal *wal);

void racs_wal_close(racs_wal *wal);

void racs_wal_destroy(racs_wal *wal);

#ifdef __cplusplus
}
#endif

#endif //RACS_WAL_H