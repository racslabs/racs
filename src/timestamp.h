
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

uint64_t auxts_get_milliseconds();
void auxts_milliseconds_to_tm(uint64_t milliseconds, struct tm* info);
uint64_t auxts_ts_to_milliseconds(struct timespec* ts);
void auxts_format_rfc3339(uint64_t milliseconds, char* buf);
uint64_t auxts_parse_rfc3339(char* buf);
uint64_t auxts_time_partitioned_path_to_timestamp(const char* path);
void auxts_get_time_partitioned_path(uint64_t milliseconds, char* path);
char* auxts_get_path_from_timestamp_range(uint64_t begin_timestamp, uint64_t end_timestamp);
void auxts_create_time_partitioned_dirs(uint64_t milliseconds);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_TIMESTAMP_H
