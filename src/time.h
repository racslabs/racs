
#ifndef RATS_TIME_H
#define RATS_TIME_H

#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "export.h"
#include "filelist.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char *rats_time_dir;

rats_time rats_time_now();

void rats_time_to_tm(rats_time time, struct tm *info);

rats_time rats_time_from_ts(struct timespec *ts);

void rats_time_to_rfc3339(rats_time time, char *buf);

rats_time rats_time_from_rfc3339(const char *buf);

rats_time rats_time_from_path(const char *path);

void rats_time_to_path(rats_time time, char **path);

char *rats_time_range_to_path(rats_time from, rats_time to);

void rats_time_create_dirs(rats_time time);

#ifdef __cplusplus
}
#endif

#endif //RATS_TIME_H
