#include "rfc.h"

uint64_t get_milliseconds() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts_to_milliseconds(&ts);
}

uint64_t ts_to_milliseconds(struct timespec* ts) {
    return (ts->tv_sec * AUXTS_NANOSECONDS_PER_SECOND + ts->tv_nsec) / AUXTS_NANOSECONDS_PER_MILLISECOND;
}

void milliseconds_to_tm(uint64_t milliseconds, struct tm* info) {
    time_t seconds = (time_t)(milliseconds / AUXTS_MILLISECONDS_PER_SECOND);
    gmtime_r(&seconds, info);
}

void format_rfc3339(uint64_t milliseconds, char* buf) {
    if (strlen(buf) < AUXTS_RFC3339_MAX_SIZE) {
    }

    time_t seconds = (time_t)(milliseconds / AUXTS_MILLISECONDS_PER_SECOND);
    long remainder = (long)(milliseconds % AUXTS_MILLISECONDS_PER_SECOND);

    struct tm info;
    gmtime_r(&seconds, &info);

    // Manually format into RFC 3339
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03ldZ",
            info.tm_year + 1900,  // Year
            info.tm_mon + 1,      // Month (0-based in info structure)
            info.tm_mday,         // Day
            info.tm_hour,         // Hour
            info.tm_min,          // Minute
            info.tm_sec,          // Second
            remainder);           // Milliseconds
}
