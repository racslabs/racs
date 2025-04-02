#include "time.h"
#include "type.h"

auxts_time auxts_time_now() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return auxts_time_ts_to_time(&ts);
}

auxts_time auxts_time_ts_to_time(struct timespec* ts) {
    return (ts->tv_sec * 1000000000 + ts->tv_nsec) / 1000000;
}

void auxts_time_to_tm(auxts_time time, struct tm* info) {
    time_t seconds = (time_t)(time / 1000);
    gmtime_r(&seconds, info);
}

void auxts_time_format_rfc3339(auxts_time time, char* buf) {
    struct tm info;
    auxts_time_to_tm(time, &info);
    long remainder = time % 1000;

    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03ldZ",
            info.tm_year + 1900, info.tm_mon + 1, info.tm_mday,
            info.tm_hour, info.tm_min, info.tm_sec, remainder);
}

auxts_time auxts_time_parse_rfc3339(const char* buf) {
    struct tm info = {0};
    int milliseconds = 0;

    if (sscanf(buf, "%d-%d-%dT%d:%d:%d.%dZ",
               &info.tm_year, &info.tm_mon, &info.tm_mday,
               &info.tm_hour, &info.tm_min, &info.tm_sec, &milliseconds) < 3) {
        fprintf(stderr, "Invalid RFC 3339 format\n");
        return -1;
    }

    info.tm_year -= 1900;
    info.tm_mon -= 1;
    time_t t = timegm(&info);
    return (t == -1) ? -1 : (int64_t)t * 1000 + milliseconds;
}

auxts_time auxts_time_path_to_time(const char* path) {
    struct tm info = {0};
    long milliseconds = 0;

    if (sscanf(path, ".data/seg/%4d/%2d/%2d/%2d/%2d/%2d/%3ld",
               &info.tm_year, &info.tm_mon, &info.tm_mday,
               &info.tm_hour, &info.tm_min, &info.tm_sec, &milliseconds) != 7) {
        fprintf(stderr, "Invalid path format\n");
        return -1;
    }

    if (milliseconds < 0 || milliseconds > 999) {
        fprintf(stderr, "Invalid milliseconds value: %ld\n", milliseconds);
        return -1;
    }

    info.tm_year -= 1900;
    info.tm_mon -= 1;
    time_t t = timegm(&info);
    return (t == -1) ? -1 : (int64_t)t * 1000 + milliseconds;
}

char* auxts_time_range_to_path(auxts_time from, auxts_time to) {
    char path1[255], path2[255];

    auxts_time_to_path(from, path1);
    auxts_time_to_path(to, path2);

    return auxts_resolve_shared_path(path1, path2);
}

void auxts_time_to_path(auxts_time time, char* path) {
    struct tm info;
    auxts_time_to_tm(time, &info);
    long remainder = time % 1000;

    sprintf(path, ".data/seg/%d/%02d/%02d/%02d/%02d/%02d/%03ld",
            info.tm_year + 1900, info.tm_mon + 1,
            info.tm_mday, info.tm_hour,
            info.tm_min, info.tm_sec,
            remainder);
}

void auxts_time_create_dirs(auxts_time time) {
    struct tm info;
    auxts_time_to_tm(time, &info);

    char dir[128];
    snprintf(dir, sizeof(dir), ".data/seg/%d/%02d/%02d/%02d/%02d/%02d",
             info.tm_year + 1900, info.tm_mon + 1,
             info.tm_mday, info.tm_hour,
             info.tm_min, info.tm_sec);

    mkdir(".data", 0777);
    mkdir(".data/seg", 0777);
    char* p = dir;
    while ((p = strchr(p + 1, '/')) != NULL) {
        *p = '\0';
        mkdir(dir, 0777);
        *p = '/';
    }
}
