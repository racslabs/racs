
#ifndef AUXTS_ATSP_H
#define AUXTS_ATSP_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "bytes.h"
#include "crc32c.h"

typedef struct {
    char chunk_id[4];
    char mac_addr[6];
    uint64_t stream_id;
    uint32_t checksum;
    uint16_t channels;
    uint32_t sample_rate;
    uint16_t bit_depth;
    uint16_t block_size;
} auxts_atsp_header;

typedef struct {
    auxts_atsp_header header;
    uint8_t* pcm_block;
} auxts_atsp_frame;

int auxts_atsp_frame_read(uint8_t* buf, auxts_atsp_frame* frame);
void auxts_atsp_header_parse(uint8_t* buf, auxts_atsp_header* header);
int auxts_is_atsp(const char* id);

#endif //AUXTS_ATSP_H
