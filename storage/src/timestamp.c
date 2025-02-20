#include "timestamp.h"

uint64_t AUXTS__get_milliseconds() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return AUXTS__ts_to_milliseconds(&ts);
}

uint64_t AUXTS__ts_to_milliseconds(struct timespec* ts) {
    return (ts->tv_sec * AUXTS_NANOSECONDS_PER_SECOND + ts->tv_nsec) / AUXTS_NANOSECONDS_PER_MILLISECOND;
}

void AUXTS__milliseconds_to_tm(uint64_t milliseconds, struct tm* info) {
    time_t seconds = (time_t)(milliseconds / AUXTS_MILLISECONDS_PER_SECOND);
    gmtime_r(&seconds, info);
}

void AUXTS__format_rfc3339(uint64_t milliseconds, char* buf) {
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

uint64_t AUXTS__parse_rfc3339(char* buf) {
    struct tm info;
    struct timespec ts;
    int milliseconds = 0;

    if (strlen(buf) != AUXTS_RFC3339_MAX_SIZE) {
    }

    memset(&info, 0, sizeof(info));

    // Parse the RFC 3339 string
    // Expecting the format YYYY-MM-DDTHH:MM:SS[.sss]Z
    int ret = sscanf(buf, "%d-%d-%dT%d:%d:%d.%dZ",
                     &info.tm_year, &info.tm_mon, &info.tm_mday,
                     &info.tm_hour, &info.tm_min, &info.tm_sec,
                     &milliseconds);

    if (ret < 6) {
        perror("Invalid RFC 3339 format");
        return -1;
    }

    info.tm_year -= 1900;
    info.tm_mon -= 1;

    time_t t = timegm(&info);
    if (t == -1) {
        perror("timegm");
        return -1;
    }

    ts.tv_sec = t;
    ts.tv_nsec = milliseconds * AUXTS_NANOSECONDS_PER_MILLISECOND;

    return AUXTS__ts_to_milliseconds(&ts);
}

int test_rfc3339() {
    uint64_t milliseconds = 1734272943291;

    char buf[55];
    AUXTS__format_rfc3339(milliseconds, buf);
    printf("%s", buf);

    return milliseconds != AUXTS__parse_rfc3339(buf);
}
