// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "db.h"

racs_db *racs_db_instance() {
    if (!_db) {
        _db = malloc(sizeof(racs_db));
        if (!_db) {
            perror("Error creating db instance. shutting down");
            exit(-1);
        }
    }
    return _db;
}

void racs_db_open(racs_db *db, const char *path) {
    racs_context_init(&db->ctx, path);
    racs_exec_init(&db->exec);
}

void racs_db_close(racs_db *db) {
    racs_exec_destroy(&db->exec);
    racs_context_destroy(&db->ctx);
}

racs_result racs_db_exec(racs_db *db, const char *cmd) {
    return racs_exec_exec(&db->exec, &db->ctx, cmd);
}

racs_result racs_db_stream(racs_db *db, racs_uint8 *data) {
    return racs_exec_stream(&db->ctx, data);
}
