// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "context.h"

const char* racs_metadata_dir = NULL;
const char* racs_time_dir = NULL;
const char* racs_log_dir = NULL;
const char* racs_wal_dir = NULL;

int racs_wal_fsync;

void racs_context_init(racs_context *ctx, const char *path) {
    ctx->config = NULL;
    racs_config_load(&ctx->config, path);

    ctx->scache = racs_scache_create(ctx->config->cache.entries);
    ctx->sessions = racs_sessions_create(ctx->config->cache.entries);
    ctx->offsets = racs_offsets_create();
    ctx->versions = racs_versions_create();
    ctx->mmt = racs_multi_memtable_create(ctx->config->memtable.tables, ctx->config->memtable.entries);

    racs_metadata_dir = ctx->config->data_dir;
    racs_time_dir = ctx->config->data_dir;
    racs_wal_dir = ctx->config->data_dir;
    racs_log_dir = ctx->config->log_dir;
    racs_wal_fsync = ctx->config->wal.fsync;

    racs_offsets_init(ctx->offsets);
    racs_versions_init(ctx->versions);
}

void racs_context_destroy(racs_context *ctx) {
    racs_multi_memtable_flush(ctx->mmt, ctx->versions);
    racs_sessions_destroy(ctx->sessions);
    racs_offsets_destroy(ctx->offsets);
    racs_versions_destroy(ctx->versions);
    racs_cache_destroy(ctx->scache);
    racs_config_destroy(ctx->config);

    racs_log_destroy(_log);
    racs_wal_destroy(_wal);
}