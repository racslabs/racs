#include "frame.h"

int racs_frame_parse(racs_uint8 *buf, racs_frame *frame) {
    off_t offset = racs_frame_header_parse(buf, &frame->header);
    frame->pcm_block = buf + offset;

    racs_uint32 checksum = crc32c(0, frame->pcm_block, frame->header.block_size);
    return checksum == frame->header.checksum;
}

off_t racs_frame_header_parse(racs_uint8 *buf, racs_frame_header *header) {
    memcpy(&header->chunk_id, buf, 4);
    memcpy(&header->session_id, buf + 4, 16);
    racs_read_uint64(&header->stream_id, buf, 20);
    racs_read_uint32(&header->checksum, buf, 28);
    racs_read_uint16(&header->channels, buf, 32);
    racs_read_uint32(&header->sample_rate, buf, 34);
    racs_read_uint16(&header->bit_depth, buf, 38);
    racs_read_uint16(&header->block_size, buf, 40);
    return 42;
}

int racs_is_frame(const char *id) {
    if (!id) return 0;
    return memcmp(id, "rspt", 4) == 0;
}
