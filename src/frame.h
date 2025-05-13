
#ifndef RACS_ATSP_H
#define RACS_ATSP_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "bytes.h"
#include "crc32c.h"

typedef struct {
    char chunk_id[4];
    char mac_addr[6];
    racs_uint64 stream_id;
    racs_uint32 checksum;
    racs_uint16 channels;
    racs_uint32 sample_rate;
    racs_uint16 bit_depth;
    racs_uint16 block_size;
} racs_frame_header;

typedef struct {
    racs_frame_header header;
    racs_uint8 *pcm_block;
} racs_frame;

int racs_frame_parse(racs_uint8 *buf, racs_frame *frame);

off_t racs_frame_header_parse(racs_uint8 *buf, racs_frame_header *header);

int racs_is_frame(const char *id);

#endif //RACS_ATSP_H
