#include "atsp.h"

int auxts_atsp_frame_read(uint8_t* buf, auxts_atsp_frame* frame) {
    auxts_atsp_header_parse(buf, &frame->header);

    frame->pcm_block = malloc(frame->header.block_size);
    if (!frame->pcm_block) {
        perror("Failed to allocate pcm_block to auxts_atsp_frame");
        return false;
    }

    memcpy(frame->pcm_block, buf, frame->header.block_size);
    uint32_t checksum = crc32c(0, frame->pcm_block, frame->header.block_size);

    return checksum == frame->header.checksum;
}

void auxts_atsp_header_parse(uint8_t* buf, auxts_atsp_header* header) {
    memcpy(&header->chunk_id, buf, 4);
    auxts_read_uint64(&header->hash, buf, 4);
    auxts_read_uint32(&header->checksum, buf, 12);
    auxts_read_uint16(&header->channels, buf, 16);
    auxts_read_uint32(&header->sample_rate, buf, 18);
    auxts_read_uint16(&header->bit_depth, buf, 22);
    auxts_read_uint16(&header->block_size, buf, 24);
}

int auxts_atsp_validate_chunk_id(const char* id) {
    return id[0] == 'a' &&
           id[1] == 't' &&
           id[2] == 's' &&
           id[3] == 'p';
}
