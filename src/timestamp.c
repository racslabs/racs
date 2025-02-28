#include "timestamp.h"

int64_t auxts_get_milliseconds() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return auxts_ts_to_milliseconds(&ts);
}

int64_t auxts_ts_to_milliseconds(struct timespec* ts) {
    return (ts->tv_sec * AUXTS_NANOSECONDS_PER_SECOND + ts->tv_nsec) / AUXTS_NANOSECONDS_PER_MILLISECOND;
}

void auxts_milliseconds_to_tm(int64_t milliseconds, struct tm* info) {
    time_t seconds = (time_t)(milliseconds / AUXTS_MILLISECONDS_PER_SECOND);
    gmtime_r(&seconds, info);
}

void auxts_format_rfc3339(int64_t milliseconds, char* buf) {
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

int64_t auxts_parse_rfc3339(const char* buf) {
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

    return auxts_ts_to_milliseconds(&ts);
}

int64_t auxts_time_partitioned_path_to_timestamp(const char* path) {
    struct tm info = {0};
    struct timespec ts;
    long milliseconds = 0;

    int ret = sscanf(path, ".data/%4d/%2d/%2d/%2d/%2d/%2d/%3ld.df",
                     &info.tm_year, &info.tm_mon, &info.tm_mday,
                     &info.tm_hour, &info.tm_min, &info.tm_sec, &milliseconds);

    if (ret != 7) {
        fprintf(stderr, "Invalid format: expected 7 values, got %d\n", ret);
        return -1;
    }

    if (milliseconds < 0 || milliseconds > 999) {
        fprintf(stderr, "Invalid milliseconds value: %ld\n", milliseconds);
        return -1;
    }

    info.tm_year -= 1900;
    info.tm_mon -= 1;

    time_t t = timegm(&info);
    if (t == -1) {
        perror("timegm failed");
        return -1;
    }

    ts.tv_sec = t;
    ts.tv_nsec = milliseconds * AUXTS_NANOSECONDS_PER_MILLISECOND;

    return auxts_ts_to_milliseconds(&ts);
}

char* auxts_get_path_from_timestamp_range(int64_t from, int64_t to) {
    char path1[255], path2[255];

    auxts_get_time_partitioned_path(from, path1);
    auxts_get_time_partitioned_path(to, path2);

    return auxts_resolve_shared_path(path1, path2);
}

void auxts_get_time_partitioned_path(int64_t milliseconds, char* path) {
    struct tm info = {0};
    auxts_milliseconds_to_tm(milliseconds, &info);

    long remainder = (long)(milliseconds % AUXTS_MILLISECONDS_PER_SECOND);

    sprintf(path, ".data/%d/%02d/%02d/%02d/%02d/%02d/%03ld.df",
            info.tm_year + 1900, info.tm_mon + 1,
            info.tm_mday, info.tm_hour,
            info.tm_min, info.tm_sec,
            remainder);
}

void auxts_create_time_partitioned_dirs(int64_t milliseconds) {
    char dir[32];
    struct tm info;

    auxts_milliseconds_to_tm(milliseconds, &info);

    sprintf(dir, ".data");
    mkdir(dir, 0777);

    sprintf(dir, "%s/%d", dir, info.tm_year + 1900);
    mkdir(dir, 0777);

    sprintf(dir, "%s/%02d", dir, info.tm_mon + 1);
    mkdir(dir, 0777);

    sprintf(dir, "%s/%02d", dir, info.tm_mday);
    mkdir(dir, 0777);

    sprintf(dir, "%s/%02d", dir, info.tm_hour);
    mkdir(dir, 0777);

    sprintf(dir, "%s/%02d", dir, info.tm_min);
    mkdir(dir, 0777);

    sprintf(dir, "%s/%02d", dir, info.tm_sec);
    mkdir(dir, 0777);
}
