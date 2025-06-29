#include "context.h"

const char* racs_streaminfo_dir = NULL;
const char* racs_time_dir = NULL;
const char* racs_log_dir = NULL;

void racs_context_init(racs_context *ctx, const char *path) {
    ctx->config = NULL;
    racs_config_load(&ctx->config, path);

    ctx->scache = racs_scache_create(ctx->config->cache.entries);
    ctx->mcache = racs_mcache_create(ctx->config->cache.entries);
    ctx->kv = racs_streamkv_create(ctx->config->cache.entries);
    ctx->mmt = racs_multi_memtable_create(ctx->config->memtable.tables,
                                          ctx->config->memtable.entries);
    racs_streaminfo_dir = ctx->config->data_dir;
    racs_time_dir = ctx->config->data_dir;
    racs_log_dir = ctx->config->log_dir;
}

void racs_context_destroy(racs_context *ctx) {
    racs_config_destroy(ctx->config);
    racs_cache_destroy(ctx->scache);
    racs_cache_destroy(ctx->mcache);
    racs_streamkv_destroy(ctx->kv);
    racs_multi_memtable_destroy(ctx->mmt);
}