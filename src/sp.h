
#ifndef RATS_ATSP_H
#define RATS_ATSP_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "bytes.h"
#include "crc32c.h"

typedef struct {
    char     chunk_id[4];
    char     mac_addr[6];
    rats_uint64 stream_id;
    rats_uint32 checksum;
    rats_uint16 channels;
    rats_uint32 sample_rate;
    rats_uint16 bit_depth;
    rats_uint16 block_size;
} rats_sp_header;

typedef struct {
    rats_sp_header header;
    rats_uint8* pcm_block;
} rats_sp;

int rats_sp_parse(rats_uint8* buf, rats_sp* frame);
void rats_sp_header_parse(rats_uint8* buf, rats_sp_header* header);
int rats_is_sp(const char* id);

#endif //RATS_ATSP_H
