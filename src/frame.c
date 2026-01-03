// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "frame.h"

int racs_frame_parse(racs_uint8 *buf, racs_frame *frame) {
    off_t offset = racs_frame_header_parse(buf, &frame->header);
    frame->pcm_block = buf + offset;

    racs_uint32 checksum = crc32c(0, frame->pcm_block, frame->header.block_size);
    return checksum == frame->header.checksum;
}

off_t racs_frame_header_parse(racs_uint8 *buf, racs_frame_header *header) {
    memcpy(&header->chunk_id, buf, 3);
    memcpy(&header->session_id, buf + 3, 16);
    racs_read_uint64(&header->stream_id, buf, 19);
    racs_read_uint32(&header->checksum, buf, 27);
    racs_read_uint16(&header->block_size, buf, 31);

    header->flags = buf[33];
    return 34;
}

int racs_is_frame(const char *id) {
    if (!id) return 0;
    return memcmp(id, "rsp", 3) == 0;
}
