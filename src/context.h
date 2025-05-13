
#ifndef RACS_CONTEXT_H
#define RACS_CONTEXT_H

#include "config.h"
#include "cache.h"
#include "stream.h"
#include "memtable.h"

typedef struct {
    racs_config *config;
    racs_cache *scache;
    racs_cache *mcache;
    racs_streamkv *kv;
    racs_multi_memtable *mmt;
} racs_context;

void racs_context_init(racs_context *ctx, const char *path);

void racs_context_destroy(racs_context *ctx);

#endif //RACS_CONTEXT_H
