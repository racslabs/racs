
#ifndef AUXTS_METADATA_H
#define AUXTS_METADATA_H

#include "bytes.h"
#include "murmur3.h"
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>

typedef struct {
    uint32_t channels;
    uint32_t bit_depth;
    uint32_t sample_rate;
    uint64_t bytes;
    uint64_t created_at;
} auxts_metadata;

int auxts_metadata_update(const auxts_metadata* metadata, const char* name);
int auxts_metadata_get(auxts_metadata* metadata, const char* name);

#endif //AUXTS_METADATA_H
