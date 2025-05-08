
#ifndef RATS_CONTEXT_H
#define RATS_CONTEXT_H

#include "config.h"
#include "cache.h"
#include "stream.h"
#include "memtable.h"

typedef struct {
    rats_config *config;
    rats_cache *scache;
    rats_cache *mcache;
    rats_streamkv *kv;
    rats_multi_memtable *mmt;
} rats_context;

void rats_context_init(rats_context *ctx, const char *path);

void rats_context_destroy(rats_context *ctx);

#endif //RATS_CONTEXT_H
