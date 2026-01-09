// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_RANGE_H
#define RACS_RANGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <msgpack.h>
#include "memtable.h"
#include "filelist.h"
#include "result.h"
#include "context.h"
#include "pcm.h"
#include "metadata.h"
#include "zstd.h"

typedef enum {
    RACS_RANGE_STATUS_OK,
    RACS_RANGE_STATUS_NOT_FOUND
} racs_range_status;

int racs_range(racs_context *ctx, racs_pcm *pcm, const char *stream_id, double start, double duration);

int racs_range_as_timestamp(racs_context *ctx, racs_pcm *pcm, racs_uint64 hash, racs_time from, racs_time to);

racs_uint8 *
racs_range_from_cache_or_sstable(racs_cache *cache, racs_uint64 stream_id, racs_time time, const char *path);

void
racs_range_process_sstable(racs_pcm *pcm, racs_uint8 *data, racs_uint64 stream_id, racs_int64 from, racs_int64 to);

#ifdef __cplusplus
}
#endif

#endif //RACS_RANGE_H
