#include "context.h"

void auxts_context_init(auxts_context* ctx) {
    ctx->cache = auxts_cache_create(2);
}

void auxts_context_destroy(auxts_context* ctx) {
    auxts_cache_destroy(ctx->cache);
}