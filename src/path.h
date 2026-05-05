// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#ifndef RACS_PATH_H
#define RACS_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "racs_time.h"
#include "config.h"
#include <wordexp.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>

int racs_path_expand(char *dest, const char *path);

void racs_path_from_time(char *path, racs_uint64 stream_id, racs_time time);

void racs_path_from_range(char *path, racs_uint64 stream_id, racs_time_range range);

void racs_path_mkdirs(racs_uint64 stream_id, racs_time time);

void racs_path_resolve(char *dest, const char *path1, const char *path2);

#ifdef __cplusplus
}
#endif

#endif //RACS_PATH_H
