
#ifndef AUXTS_CONTEXT_H
#define AUXTS_CONTEXT_H

#include "cache.h"

typedef struct {
    auxts_cache* cache;
} auxts_context;

void auxts_context_init(auxts_context* ctx);
void auxts_context_destroy(auxts_context* ctx);

#endif //AUXTS_CONTEXT_H
