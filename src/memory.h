// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_MEMORY_H
#define RACS_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "log.h"

typedef enum {
    RACS_MEMSTREAM_CONTINUE,
    RACS_MEMSTREAM_ABORT
} racs_memstream_status;

typedef struct {
    size_t size;
    size_t pos;
    racs_uint8 *data;
} racs_memstream;

void racs_memstream_init(racs_memstream *stream);

int racs_memstream_write(racs_memstream *stream, const void *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif //RACS_MEMORY_H
