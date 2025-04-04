
#ifndef AUXTS_METADATA_H
#define AUXTS_METADATA_H

#include "bytes.h"
#include "murmur3.h"
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

typedef enum {
    AUXTS_METADATA_STATUS_OK,
    AUXTS_METADATA_STATUS_EXIST,
    AUXTS_METADATA_STATUS_NOT_FOUND,
    AUXTS_METADATA_STATUS_ERROR
} auxts_metadata_status;

typedef struct {
    uint32_t channels;
    uint32_t bit_depth;
    uint32_t sample_rate;
    uint64_t bytes;
    uint64_t ref;
} auxts_metadata;

int auxts_metadata_put(const auxts_metadata* metadata, const char* stream_id);
int auxts_metadata_get(auxts_metadata* metadata, const char* stream_id);
int auxts_metadata_attr(const char* stream_id, const char* attr, uint64_t* value);
int auxts_stream_id_exist(uint64_t stream_id);
uint64_t auxts_hash_stream_id(const char* stream_id);

#endif //AUXTS_METADATA_H
