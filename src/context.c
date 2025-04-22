#include "context.h"

void auxts_context_init(auxts_context* ctx) {
    ctx->scache = auxts_scache_create(2);
    ctx->mcache = auxts_mcache_create(1);
    ctx->kv = auxts_streamkv_create(2);
}

void auxts_context_destroy(auxts_context* ctx) {
    auxts_cache_destroy(ctx->scache);
    auxts_cache_destroy(ctx->mcache);
    auxts_streamkv_destroy(ctx->kv);
}