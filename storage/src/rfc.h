
#ifndef AUXTS_RFC_H
#define AUXTS_RFC_H

#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define AUXTS_RFC3339_MAX_SIZE 35

#define AUXTS_MILLISECONDS_PER_SECOND 1000L

#define AUXTS_NANOSECONDS_PER_MILLISECOND 1000000L

#define AUXTS_NANOSECONDS_PER_SECOND 1000000000L

uint64_t get_milliseconds();

void milliseconds_to_tm(uint64_t milliseconds, struct tm* info);

uint64_t ts_to_milliseconds(struct timespec* ts);

void format_rfc3339(uint64_t milliseconds, char* buf);

uint64_t parse_rfc3339(char* buf);

int test_rfc3339();

#endif //AUXTS_RFC_H
