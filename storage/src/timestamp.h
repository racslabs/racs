
#ifndef AUXTS_TIMESTAMP_H
#define AUXTS_TIMESTAMP_H

#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUXTS_RFC3339_MAX_SIZE 35

#define AUXTS_MILLISECONDS_PER_SECOND 1000L

#define AUXTS_NANOSECONDS_PER_MILLISECOND 1000000L

#define AUXTS_NANOSECONDS_PER_SECOND 1000000000L

uint64_t AUXTS__get_milliseconds();
void AUXTS__milliseconds_to_tm(uint64_t milliseconds, struct tm* info);
uint64_t AUXTS__ts_to_milliseconds(struct timespec* ts);
void AUXTS__format_rfc3339(uint64_t milliseconds, char* buf);
uint64_t AUXTS__parse_rfc3339(char* buf);

int test_rfc3339();

#ifdef __cplusplus
}
#endif

#endif //AUXTS_TIMESTAMP_H
