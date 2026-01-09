// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_DB_H
#define RACS_DB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "exec.h"
#include "context.h"

typedef struct {
    racs_context ctx;
    racs_exec exec;
} racs_db;

static racs_db *_db = NULL;

racs_db *racs_db_instance();

void racs_db_open(racs_db *db, const char *path);

void racs_db_close(racs_db *db);

racs_result racs_db_exec(racs_db *db, const char *cmd);

racs_result racs_db_stream_batch(racs_db *db, racs_uint8 *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif //RACS_DB_H
