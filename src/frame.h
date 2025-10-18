// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_FRAME_H
#define RACS_FRAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "bytes.h"
#include "crc32c.h"

#define RACS_FLAG_COMPRESSED 0x01

#define RACS_IS_COMPRESSED(h) ((h).flags & RACS_FLAG_COMPRESSED)

#define RACS_IS_FRAME(d) racs_is_frame(d)


typedef struct {
    char chunk_id[3];
    racs_uint8  session_id[16];
    racs_uint64 stream_id;
    racs_uint32 checksum;
    racs_uint16 channels;
    racs_uint32 sample_rate;
    racs_uint16 bit_depth;
    racs_uint16 block_size;
    racs_uint8  flags;
} racs_frame_header;

typedef struct {
    racs_frame_header header;
    racs_uint8 *pcm_block;
} racs_frame;

int racs_frame_parse(racs_uint8 *buf, racs_frame *frame);

off_t racs_frame_header_parse(racs_uint8 *buf, racs_frame_header *header);

int racs_is_frame(const char *id);

#ifdef __cplusplus
}
#endif

#endif //RACS_FRAME_H
