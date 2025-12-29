// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_METADATA_H
#define RACS_METADATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fnmatch.h>
#include "types.h"
#include "bytes.h"
#include "cache.h"
#include "filelist.h"
#include "log.h"

#ifndef FNM_IGNORECASE
#define FNM_IGNORECASE 0x01
#endif

typedef struct {
    racs_uint16 channels;
    racs_uint16 bit_depth;
    racs_uint32 sample_rate;
    racs_time   ref;
    racs_time   ttl;
    racs_uint32 id_size;
    char*       id;
} racs_metadata;

typedef struct {
    char **streams;
    size_t num_streams;
    size_t max_streams;
} racs_streams;

extern const char* racs_metadata_dir;

racs_int64 racs_metadata_attr(racs_uint64 stream_id, const char *attr);

int racs_metadata_get(racs_metadata *streaminfo, racs_uint64 stream_id);

int racs_metadata_put(racs_metadata *streaminfo, racs_uint64 stream_id);

void racs_streams_list(racs_streams *streams, const char* pattern);

size_t racs_metadata_size(racs_metadata* streaminfo);

off_t racs_metadata_write(racs_uint8 *buf, racs_metadata *streaminfo);

off_t racs_metadata_read(racs_metadata *streaminfo, racs_uint8 *buf);

size_t racs_metadata_filesize(const char *path);

void racs_metadata_flush(racs_uint8 *data, racs_uint32 len, racs_uint64 stream_id);

void racs_metadata_path(char **path, racs_uint64 stream_id, int tmp);

int racs_metadata_exits(racs_uint64 stream_id);

void racs_metadata_destroy(racs_metadata *streaminfo);

racs_time racs_metadata_timestamp(racs_metadata *streaminfo, racs_uint64 offset);

void racs_streams_add(racs_streams *streams, const char *stream);

void racs_streams_init(racs_streams *streams);

void racs_streams_destroy(racs_streams *streams);

racs_uint64 racs_hash(const char *stream_id);

racs_uint64 racs_path_to_stream_id(char *path);

#ifdef __cplusplus
}
#endif

#endif //RACS_METADATA_H
