#include "context.h"

const char* rats_streaminfo_dir = NULL;
const char* rats_time_dir = NULL;

void rats_context_init(rats_context *ctx, const char *path) {
    ctx->config = NULL;
    rats_config_load(&ctx->config, path);

    ctx->scache = rats_scache_create(ctx->config->cache.entries);
    ctx->mcache = rats_mcache_create(ctx->config->cache.entries);
    ctx->kv = rats_streamkv_create(ctx->config->cache.entries);
    ctx->mmt = rats_multi_memtable_create(ctx->config->memtable.tables,
                                          ctx->config->memtable.entries);
    rats_streaminfo_dir = ctx->config->data_dir;
    rats_time_dir = ctx->config->data_dir;

    rats_streaminfo_load(ctx->mcache);
}

void rats_context_destroy(rats_context *ctx) {
    rats_config_destroy(ctx->config);
    rats_cache_destroy(ctx->scache);
    rats_cache_destroy(ctx->mcache);
    rats_streamkv_destroy(ctx->kv);
    rats_multi_memtable_destroy(ctx->mmt);
}