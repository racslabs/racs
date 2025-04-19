#include "context.h"

void auxts_context_init(auxts_context* ctx) {
    ctx->scache = auxts_scache_create(2);
}

void auxts_context_destroy(auxts_context* ctx) {
    auxts_cache_destroy(ctx->scache);
}