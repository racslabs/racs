
#ifndef AUXTS_CONTEXT_H
#define AUXTS_CONTEXT_H

#include "cache.h"
#include "stream.h"

typedef struct {
    auxts_cache*    scache;
    auxts_cache*    mcache;
    auxts_streamkv* kv;
} auxts_context;

void auxts_context_init(auxts_context* ctx);
void auxts_context_destroy(auxts_context* ctx);

#endif //AUXTS_CONTEXT_H
