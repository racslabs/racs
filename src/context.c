#include "context.h"

void rats_context_init(rats_context* ctx) {
    ctx->scache = rats_scache_create(2);
    ctx->mcache = rats_mcache_create(1);
    ctx->kv = rats_streamkv_create(2);
    ctx->mmt = rats_multi_memtable_create(2, 2);
}

void rats_context_destroy(rats_context* ctx) {
    rats_cache_destroy(ctx->scache);
    rats_cache_destroy(ctx->mcache);
    rats_streamkv_destroy(ctx->kv);
    rats_multi_memtable_destroy(ctx->mmt);
}