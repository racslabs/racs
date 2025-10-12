// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RSAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "memory.h"
#include <stdlib.h>
#include <string.h>

#define racs_max(a, b) ((a) > (b) ? (a) : (b))

void racs_memstream_init(racs_memstream *stream) {
    memset(stream, 0, sizeof(racs_memstream));
}

int racs_memstream_write(racs_memstream *stream, const void *data, size_t size) {
    size_t pos = stream->current_pos + size;

    if (pos > stream->size) {
        size_t _size = racs_max(pos, stream->size * 2);

        racs_uint8 *_data = realloc(stream->data, _size);
        if (!_data) {
            racs_log_error("Failed to re-allocate racs_memstream data");
            return RACS_MEMSTREAM_ABORT;
        }

        stream->data = _data;
        stream->size = _size;
    }

    memcpy(stream->data + stream->current_pos, data, size);
    stream->current_pos += size;

    return RACS_MEMSTREAM_CONTINUE;
}

