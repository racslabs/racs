#include "auxts.h"

void auxts_context_init(auxts_context* ctx) {
    ctx->cache = auxts_cache_create(2);
}

void auxts_context_destroy(auxts_context* ctx) {
    auxts_cache_destroy(ctx->cache);
}

void auxts_result_init(auxts_result* result, size_t size) {
    result->size = size;
    result->data = malloc(size);
    if (!result->data) {
        perror("Failed to allocate data to auxts_result");
    }
}

void auxts_result_destroy(auxts_result* result) {
    free(result->data);
}

