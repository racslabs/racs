// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#ifndef RACS_TIME_H
#define RACS_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

typedef struct {
    racs_time from;
    racs_time to;
} racs_time_range;

static inline int racs_time_in_range(racs_time t, racs_time_range range) {
    return t >= range.from && t <= range.to;
}

racs_time racs_time_from_ts(struct timespec *ts);

racs_time racs_time_now();

void racs_time_to_tm(struct tm *info, racs_time time);

void racs_time_to_rfc3339(char *buf, racs_time time);

racs_time racs_time_from_path(const char *path);

#ifdef __cplusplus
}
#endif

#endif //RACS_TIME_H
