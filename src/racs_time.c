// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#include "racs_time.h"


racs_time racs_time_from_ts(struct timespec *ts) {
    return (racs_time)ts->tv_sec * 1000 + ts->tv_nsec / 1000000;
}

racs_time racs_time_now() {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        return 0;
    }
    return racs_time_from_ts(&ts);
}

void racs_time_to_tm(struct tm *info, racs_time time) {
    time_t seconds = (time_t)(time / 1000);
    gmtime_r(&seconds, info);
}

void racs_time_to_rfc3339(char *buf, racs_time time) {
    struct tm info;
    racs_time_to_tm(&info, time);

    size_t len = strftime(buf, 21, "%Y-%m-%dT%H:%M:%S", &info);
    sprintf(buf + len, ".%03ldZ", (long)(time % 1000));
}

racs_time racs_time_from_path(const char *path) {
    struct tm info = {0};
    long milliseconds = 0;
    racs_uint64 stream_id;

    const char *match = strstr(path, ".racs/seg/");
    if (!match) {
        return -1;
    }

    if (sscanf(match + 10, "%llu/%4d/%2d/%2d/%2d/%2d/%2d/%3ld",
               &stream_id, &info.tm_year, &info.tm_mon, &info.tm_mday,
               &info.tm_hour, &info.tm_min, &info.tm_sec, &milliseconds) != 8) {
        return -1;
    }

    info.tm_year -= 1900;
    info.tm_mon -= 1;
    info.tm_isdst = -1;

    time_t t = timegm(&info);
    if (t == -1) {
        return -1;
    }

    return (racs_time)t * 1000 + milliseconds;
}
