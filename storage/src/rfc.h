
#ifndef AUXTS_RFC_H
#define AUXTS_RFC_H

#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define AUXTS__RFC3339_MAX_SIZE 35

#define AUXTS__MILLISECONDS_PER_SECOND 1000L

#define AUXTS__NANOSECONDS_PER_MILLISECOND 1000000L

#define AUXTS__NANOSECONDS_PER_SECOND 1000000000L

uint64_t AUXTS__get_milliseconds();
void AUXTS__milliseconds_to_tm(uint64_t milliseconds, struct tm* info);
uint64_t AUXTS__ts_to_milliseconds(struct timespec* ts);
void AUXTS__format_rfc3339(uint64_t milliseconds, char* buf);
uint64_t AUXTS__parse_rfc3339(char* buf);

int test_rfc3339();

#endif //AUXTS_RFC_H
