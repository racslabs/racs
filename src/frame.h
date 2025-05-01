
#ifndef RATS_ATSP_H
#define RATS_ATSP_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "bytes.h"
#include "crc32c.h"

typedef struct {
    char chunk_id[4];
    char mac_addr[6];
    rats_uint64 stream_id;
    rats_uint32 checksum;
    rats_uint16 channels;
    rats_uint32 sample_rate;
    rats_uint16 bit_depth;
    rats_uint16 block_size;
} rats_frame_header;

typedef struct {
    rats_frame_header header;
    rats_uint8 *pcm_block;
} rats_frame;

int rats_frame_parse(rats_uint8 *buf, rats_frame *frame);

off_t rats_frame_header_parse(rats_uint8 *buf, rats_frame_header *header);

int rats_is_frame(const char *id);

#endif //RATS_ATSP_H
