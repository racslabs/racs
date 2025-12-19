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
#include "memtable.h"

#include <pthread.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RACS_WAL_SIZE_1GB (1024 * 1024)

typedef enum {
    RACS_OP_CODE_CREATE,
    RACS_OP_CODE_APPEND,
} racs_op_code;

typedef struct {
    int             fd;
    racs_uint64     lsn;
    size_t          size;
    pthread_mutex_t mutex;
} racs_wal;

static racs_wal *_wal = NULL;

extern const char *racs_wal_dir;

#define RACS_WAL_FSYNC 50

#define racs_wal_append(op_code, size, op) racs_wal_append_(racs_wal_instance(), op_code, size, op)

#define racs_wal_lsn racs_wal_instance()->lsn

racs_wal *racs_wal_create();

racs_wal *racs_wal_instance();

void racs_wal_append_(racs_wal *wal, racs_op_code op_code, size_t size, racs_uint8 *op);

void racs_wal_open(racs_wal *wal);

void racs_wal_rotate(racs_wal *wal);

void racs_wal_close(racs_wal *wal);

void racs_wal_destroy(racs_wal *wal);

void racs_wal_filename(char *buf, size_t buflen, uint64_t segno);

racs_uint64 racs_wal_segment(const char *wal_dir);

void racs_wal_init_lsn(racs_wal *wal);

#ifdef __cplusplus
}
#endif

#endif //RACS_WAL_H