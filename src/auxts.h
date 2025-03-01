
#ifndef AUXTS_AUXTS_H
#define AUXTS_AUXTS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "cache.h"

typedef struct {
    uint8_t* data;
    size_t size;
} auxts_result;

typedef struct {
    auxts_cache* cache;
} auxts_context;

void auxts_result_init(auxts_result* result, size_t size);
void auxts_result_destroy(auxts_result* result);
void auxts_context_init(auxts_context* ctx);
void auxts_context_destroy(auxts_context* ctx);


#endif //AUXTS_AUXTS_H
