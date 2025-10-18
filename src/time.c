// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "time.h"

racs_time racs_time_now() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return racs_time_from_ts(&ts);
}

racs_time racs_time_from_ts(struct timespec *ts) {
    return (ts->tv_sec * 1000000000 + ts->tv_nsec) / 1000000;
}

void racs_time_to_tm(racs_time time, struct tm *info) {
    racs_time sec = time / 1000;
    gmtime_r(&sec, info);
}

void racs_time_to_rfc3339(racs_time time, char *buf) {
    struct tm info;
    racs_time_to_tm(time, &info);
    long rem = time % 1000;

    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03ldZ",
            info.tm_year + 1900, info.tm_mon + 1, info.tm_mday,
            info.tm_hour, info.tm_min, info.tm_sec, rem);
}

racs_time racs_time_from_rfc3339(const char *buf) {
    struct tm info = {0};
    int mill = 0;

    if (sscanf(buf, "%d-%d-%dT%d:%d:%d.%dZ",
               &info.tm_year, &info.tm_mon, &info.tm_mday,
               &info.tm_hour, &info.tm_min, &info.tm_sec, &mill) < 3) {
        racs_log_error("Invalid RFC 3339 format");
        return -1;
    }

    info.tm_year -= 1900;
    info.tm_mon -= 1;

    racs_time time = timegm(&info);
    return (time == -1) ? -1 : time * 1000 + mill;
}

racs_time racs_time_from_path(const char *path) {
    struct tm info = {0};
    long mill = 0;

    const char *match = strstr(path, ".racs/seg");

    if (sscanf(match, ".racs/seg/%4d/%2d/%2d/%2d/%2d/%2d/%3ld",
               &info.tm_year, &info.tm_mon, &info.tm_mday,
               &info.tm_hour, &info.tm_min, &info.tm_sec, &mill) != 7) {
        racs_log_error("Invalid path format");
        return -1;
    }

    if (mill < 0 || mill > 999) {
        racs_log_error("Invalid mill value: %ld", mill);
        return -1;
    }

    info.tm_year -= 1900;
    info.tm_mon -= 1;

    racs_time time = timegm(&info);
    return (time == -1) ? -1 : (time * 1000) + mill;
}

char *racs_time_range_to_path(racs_time from, racs_time to) {
    char *path1 = NULL;
    char *path2 = NULL;

    racs_time_to_path(from, &path1, false);
    racs_time_to_path(to, &path2, false);

    char* shared_path = racs_resolve_shared_path(path1, path2);
    free(path1);
    free(path2);

    return shared_path;
}

void racs_time_to_path(racs_time time, char **path, int tmp) {
    struct tm info;
    racs_time_to_tm(time, &info);

    long rem = time % 1000;
    const char *ext = tmp ? ".tmp" : "";

    asprintf(path, "%s/.racs/seg/%d/%02d/%02d/%02d/%02d/%02d/%03ld%s",
            racs_time_dir,
            info.tm_year + 1900, info.tm_mon + 1,
            info.tm_mday, info.tm_hour,
            info.tm_min, info.tm_sec,
            rem, ext);
}

void racs_time_create_dirs(racs_time time) {
    char *dir = NULL;
    racs_time_to_path(time, &dir, true);

    char *p = dir;
    while ((p = strchr(p + 1, '/')) != NULL) {
        *p = '\0';
        mkdir(dir, 0777);
        *p = '/';
    }

    free(dir);
}
