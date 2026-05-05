#include "path.h"

char *racs_path(const char* path) {
    wordexp_t p;
    if (wordexp(path, &p, 0) != 0) {
        printf("racs: could not expand path: %s", path);
        return NULL;
    }

    char *result = strdup(p.we_wordv[0]);
    wordfree(&p);

    return result;
}


void racs_path_from_time(racs_uint64 stream_id, racs_time time, char *path) {
    struct tm info;
    racs_time_to_tm(time, &info);

    long milliseconds = time % 1000;

    sprintf(path, "%s/.racs/seg/%llu/%d/%02d/%02d/%02d/%02d/%02d/%03ld",
            racs_config_get()->data_dir,
            stream_id,
            info.tm_year + 1900, info.tm_mon + 1,
            info.tm_mday, info.tm_hour,
            info.tm_min, info.tm_sec,
            milliseconds);
}

void racs_path_mkdirs(racs_uint64 stream_id, racs_time time) {
    char dir[PATH_MAX];
    racs_path_from_time(stream_id, time, dir);

    char *p = dir;
    while ((p = strchr(p + 1, '/')) != NULL)
        *p = '\0';
        if (mkdir(dir, 0777) == -1 && errno != EEXIST) {
            *p = '/';
            return;
        }
        *p = '/';
    }
}

