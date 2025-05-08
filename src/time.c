#include "time.h"
#include "types.h"

rats_time rats_time_now() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return rats_time_from_ts(&ts);
}

rats_time rats_time_from_ts(struct timespec *ts) {
    return (ts->tv_sec * 1000000000 + ts->tv_nsec) / 1000000;
}

void rats_time_to_tm(rats_time time, struct tm *info) {
    rats_time sec = time / 1000;
    gmtime_r(&sec, info);
}

void rats_time_to_rfc3339(rats_time time, char *buf) {
    struct tm info;
    rats_time_to_tm(time, &info);
    long rem = time % 1000;

    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03ldZ",
            info.tm_year + 1900, info.tm_mon + 1, info.tm_mday,
            info.tm_hour, info.tm_min, info.tm_sec, rem);
}

rats_time rats_time_from_rfc3339(const char *buf) {
    struct tm info = {0};
    int mill = 0;

    if (sscanf(buf, "%d-%d-%dT%d:%d:%d.%dZ",
               &info.tm_year, &info.tm_mon, &info.tm_mday,
               &info.tm_hour, &info.tm_min, &info.tm_sec, &mill) < 3) {
        fprintf(stderr, "Invalid RFC 3339 format\n");
        return -1;
    }

    info.tm_year -= 1900;
    info.tm_mon -= 1;

    rats_time time = timegm(&info);
    return (time == -1) ? -1 : time * 1000 + mill;
}

rats_time rats_time_from_path(const char *path) {
    struct tm info = {0};
    long mill = 0;

    const char *match = strstr(path, ".data/seg");

    if (sscanf(match, ".data/seg/%4d/%2d/%2d/%2d/%2d/%2d/%3ld",
               &info.tm_year, &info.tm_mon, &info.tm_mday,
               &info.tm_hour, &info.tm_min, &info.tm_sec, &mill) != 7) {
        fprintf(stderr, "Invalid path format\n");
        return -1;
    }

    if (mill < 0 || mill > 999) {
        fprintf(stderr, "Invalid mill value: %ld\n", mill);
        return -1;
    }

    info.tm_year -= 1900;
    info.tm_mon -= 1;

    rats_time time = timegm(&info);
    return (time == -1) ? -1 : (time * 1000) + mill;
}

char *rats_time_range_to_path(rats_time from, rats_time to) {
    char *path1 = NULL;
    char *path2 = NULL;

    rats_time_to_path(from, &path1);
    rats_time_to_path(to, &path2);

    char* shared_path = rats_resolve_shared_path(path1, path2);
    free(path1);
    free(path2);

    return shared_path;
}

void rats_time_to_path(rats_time time, char **path) {
    struct tm info;
    rats_time_to_tm(time, &info);

    long rem = time % 1000;

    asprintf(path, "%s/.data/seg/%d/%02d/%02d/%02d/%02d/%02d/%03ld",
            rats_time_dir,
            info.tm_year + 1900, info.tm_mon + 1,
            info.tm_mday, info.tm_hour,
            info.tm_min, info.tm_sec,
            rem);
}

void rats_time_create_dirs(rats_time time) {
    char *dir = NULL;
    rats_time_to_path(time, &dir);

    char *p = dir;
    while ((p = strchr(p + 1, '/')) != NULL) {
        *p = '\0';
        mkdir(dir, 0777);
        *p = '/';
    }

    free(dir);
}
