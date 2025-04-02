
#ifndef AUXTS_TIMESTAMP_H
#define AUXTS_TIMESTAMP_H

#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "export.h"
#include "filelist.h"

#ifdef __cplusplus
extern "C" {
#endif

int64_t auxts_time_now();
void auxts_time_to_tm(int64_t time, struct tm* info);
int64_t auxts_time_ts_to_milliseconds(struct timespec* ts);
void auxts_time_format_rfc3339(int64_t time, char* buf);
int64_t auxts_time_parse_rfc3339(const char* buf);
int64_t auxts_time_path_to_time(const char* path);
void auxts_time_to_path(int64_t time, char* path);
char* auxts_time_range_to_path(int64_t from, int64_t to);
void auxts_time_create_dirs(int64_t time);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_TIMESTAMP_H
