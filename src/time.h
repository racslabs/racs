// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_TIME_H
#define RACS_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "export.h"
#include "filelist.h"
#include "types.h"
#include "log.h"

extern const char *racs_time_dir;

racs_time racs_time_now();

void racs_time_to_tm(racs_time time, struct tm *info);

racs_time racs_time_from_ts(struct timespec *ts);

void racs_time_to_rfc3339(racs_time time, char *buf);

racs_time racs_time_from_rfc3339(const char *buf);

racs_time racs_time_from_path(const char *path);

void racs_time_to_path(racs_time time, char **path, int tmp);

char *racs_time_range_to_path(racs_time from, racs_time to);

void racs_time_create_dirs(racs_time time);

#ifdef __cplusplus
}
#endif

#endif //RACS_TIME_H
