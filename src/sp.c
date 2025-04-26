#include "sp.h"

int rats_sp_parse(rats_uint8* buf, rats_sp* frame) {
    rats_sp_header_parse(buf, &frame->header);

    frame->pcm_block = buf + sizeof(rats_sp_header);
    return 1;
//    rats_uint32 checksum = crc32c(0, frame->pcm_block, frame->header.block_size);
//
//    return checksum == frame->header.checksum;
}

void rats_sp_header_parse(rats_uint8* buf, rats_sp_header* header) {
    memcpy(&header->chunk_id, buf, 4);
    memcpy(&header->mac_addr, buf + 4, 6);
    rats_read_uint64(&header->stream_id, buf, 10);
    rats_read_uint32(&header->checksum, buf, 18);
    rats_read_uint16(&header->channels, buf, 22);
    rats_read_uint32(&header->sample_rate, buf, 24);
    rats_read_uint16(&header->bit_depth, buf, 28);
    rats_read_uint16(&header->block_size, buf, 30);
}

int rats_is_sp(const char* id) {
    return (memcmp(id, "atsp", 4) == 0);
}
