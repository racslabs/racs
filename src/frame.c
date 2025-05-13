#include "frame.h"

int racs_frame_parse(racs_uint8 *buf, racs_frame *frame) {
    off_t offset = racs_frame_header_parse(buf, &frame->header);

    frame->pcm_block = buf + offset;
    return 1;
//    racs_uint32 checksum = crc32c(0, frame->pcm_block, frame->header.block_size);
//
//    return checksum == frame->header.checksum;
}

off_t racs_frame_header_parse(racs_uint8 *buf, racs_frame_header *header) {
    memcpy(&header->chunk_id, buf, 4);
    memcpy(&header->mac_addr, buf + 4, 6);
    racs_read_uint64(&header->stream_id, buf, 10);
    racs_read_uint32(&header->checksum, buf, 18);
    racs_read_uint16(&header->channels, buf, 22);
    racs_read_uint32(&header->sample_rate, buf, 24);
    racs_read_uint16(&header->bit_depth, buf, 28);
    racs_read_uint16(&header->block_size, buf, 30);
    return 32;
}

int racs_is_frame(const char *id) {
    return memcmp(id, "atsp", 4) == 0;
}
