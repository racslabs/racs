
#ifndef AUXTS_ATSP_H
#define AUXTS_ATSP_H

#include <sys/socket.h>
#include <stdbool.h>
#include "bytes.h"

typedef struct {
    char chunk_id[4];
    uint64_t hash;
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

int auxts_atsp_header_parse(int socket_fd, auxts_atsp_header* header);

#endif //AUXTS_ATSP_H
