// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "context.h"

#include "wal.h"

const char* racs_streaminfo_dir = NULL;
const char* racs_time_dir = NULL;
const char* racs_log_dir = NULL;
const char* racs_wal_dir = NULL;

void racs_context_init(racs_context *ctx, const char *path) {
    ctx->config = NULL;
    racs_config_load(&ctx->config, path);

    ctx->scache = racs_scache_create(ctx->config->cache.entries);
    ctx->mcache = racs_mcache_create(ctx->config->cache.entries);
    ctx->kv = racs_streamkv_create(ctx->config->cache.entries);
    ctx->mmt = racs_multi_memtable_create(ctx->config->memtable.tables, ctx->config->memtable.entries);

    racs_streaminfo_dir = ctx->config->data_dir;
    racs_time_dir = ctx->config->data_dir;
    racs_wal_dir = ctx->config->data_dir;
    racs_log_dir = ctx->config->log_dir;
}

void racs_context_destroy(racs_context *ctx) {
    racs_multi_memtable_flush(ctx->mmt);
    racs_streamkv_destroy(ctx->kv);
    racs_cache_destroy(ctx->scache);
    racs_cache_destroy(ctx->mcache);
    racs_config_destroy(ctx->config);

    racs_log_destroy(_log);
    racs_wal_destroy(_wal);
}