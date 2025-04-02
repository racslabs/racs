
#ifndef AUXTS_TIME_H
#define AUXTS_TIME_H

#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "export.h"
#include "filelist.h"
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

auxts_time auxts_time_now();
void auxts_time_to_tm(auxts_time time, struct tm* info);
auxts_time auxts_time_ts_to_time(struct timespec* ts);
void auxts_time_format_rfc3339(auxts_time time, char* buf);
auxts_time auxts_time_parse_rfc3339(const char* buf);
auxts_time auxts_time_path_to_time(const char* path);
void auxts_time_to_path(auxts_time time, char* path);
char* auxts_time_range_to_path(auxts_time from, auxts_time to);
void auxts_time_create_dirs(auxts_time time);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_TIME_H
