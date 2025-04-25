
#ifndef AUXTS_ATSP_H
#define AUXTS_ATSP_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "bytes.h"
#include "crc32c.h"

typedef struct {
    char     chunk_id[4];
    char     mac_addr[6];
    uint64_t stream_id;
    uint32_t checksum;
    uint16_t channels;
    uint32_t sample_rate;
    uint16_t bit_depth;
    uint16_t block_size;
} rats_sp_header;

typedef struct {
    rats_sp_header header;
    uint8_t* pcm_block;
} rats_sp;

int rats_sp_parse(uint8_t* buf, rats_sp* frame);
void rats_sp_header_parse(uint8_t* buf, rats_sp_header* header);
int rats_is_sp(const char* id);

#endif //AUXTS_ATSP_H
