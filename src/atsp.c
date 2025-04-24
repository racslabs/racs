#include "atsp.h"

int auxts_atsp_parse(uint8_t* buf, auxts_atsp_frame* frame) {
    auxts_atsp_header_parse(buf, &frame->header);

    frame->pcm_block = buf + sizeof(auxts_atsp);
    return 1;
//    uint32_t checksum = crc32c(0, frame->pcm_block, frame->header.block_size);
//
//    return checksum == frame->header.checksum;
}

void auxts_atsp_header_parse(uint8_t* buf, auxts_atsp* header) {
    memcpy(&header->chunk_id, buf, 4);
    memcpy(&header->mac_addr, buf + 4, 6);
    auxts_read_uint64(&header->stream_id, buf, 10);
    auxts_read_uint32(&header->checksum, buf, 18);
    auxts_read_uint16(&header->channels, buf, 22);
    auxts_read_uint32(&header->sample_rate, buf, 24);
    auxts_read_uint16(&header->bit_depth, buf, 28);
    auxts_read_uint16(&header->block_size, buf, 30);
}

int auxts_is_atsp(const char* id) {
    return (memcmp(id, "atsp", 4) == 0);
}
