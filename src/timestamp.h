
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

#define AUXTS_RFC3339_MAX_SIZE 35

#define AUXTS_MILLISECONDS_PER_SECOND 1000L

#define AUXTS_NANOSECONDS_PER_MILLISECOND 1000000L

#define AUXTS_NANOSECONDS_PER_SECOND 1000000000L

int64_t auxts_milliseconds();
void auxts_milliseconds_to_tm(int64_t milliseconds, struct tm* info);
int64_t auxts_ts_to_milliseconds(struct timespec* ts);
void auxts_format_rfc3339(int64_t milliseconds, char* buf);
int64_t auxts_parse_rfc3339(const char* buf);
int64_t auxts_time_partitioned_path_to_timestamp(const char* path);
void auxts_get_time_partitioned_path(int64_t milliseconds, char* path);
char* auxts_get_path_from_timestamp_range(int64_t from, int64_t to);
void auxts_create_time_partitioned_dirs(int64_t milliseconds);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_TIMESTAMP_H
