
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

extern AUXTS_API const int AUXTS__RFC3339_MAX_SIZE;

extern AUXTS_API const long AUXTS__MILLISECONDS_PER_SECOND;

extern AUXTS_API const long AUXTS__NANOSECONDS_PER_MILLISECOND;

extern AUXTS_API const long AUXTS__NANOSECONDS_PER_SECOND;

AUXTS_API uint64_t AUXTS__get_milliseconds();
AUXTS_API void AUXTS__milliseconds_to_tm(uint64_t milliseconds, struct tm* info);
AUXTS_API uint64_t AUXTS__ts_to_milliseconds(struct timespec* ts);
AUXTS_API void AUXTS__format_rfc3339(uint64_t milliseconds, char* buf);
AUXTS_API uint64_t AUXTS__parse_rfc3339(char* buf);

int test_rfc3339();

#ifdef __cplusplus
}
#endif

#endif //AUXTS_TIMESTAMP_H
