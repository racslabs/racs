#include "frame.h"

int rats_frame_parse(rats_uint8 *buf, rats_frame *frame) {
    off_t offset = rats_frame_header_parse(buf, &frame->header);

    frame->pcm_block = buf + offset;
    return 1;
//    rats_uint32 checksum = crc32c(0, frame->pcm_block, frame->header.block_size);
//
//    return checksum == frame->header.checksum;
}

off_t rats_frame_header_parse(rats_uint8 *buf, rats_frame_header *header) {
    memcpy(&header->chunk_id, buf, 4);
    memcpy(&header->mac_addr, buf + 4, 6);
    rats_read_uint32(&header->checksum, buf, 14);
    rats_read_uint16(&header->channels, buf, 18);
    rats_read_uint32(&header->sample_rate, buf, 20);
    rats_read_uint16(&header->bit_depth, buf, 24);
    rats_read_uint16(&header->block_size, buf, 28);
    rats_read_uint32(&header->stream_id_size, buf, 30);
    memcpy(header->stream_id, buf + 34, header->stream_id_size);
    return 34 + header->stream_id_size;
}

int rats_is_frame(const char *id) {
    return memcmp(id, "atsp", 4) == 0;
}
