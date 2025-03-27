
#ifndef AUXTS_ATSP_H
#define AUXTS_ATSP_H

#include <stdint.h>

typedef struct {
    char chunk_id[4];
    uint64_t hash;
    uint32_t checksum;
    uint16_t channels;
    uint32_t sample_rate;
    uint16_t bit_depth;
    uint16_t block_size;
    uint8_t* pcm_block;
} auxts_atsp_frame;

#endif //AUXTS_ATSP_H
