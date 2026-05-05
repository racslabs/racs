#include "path.h"

char *racs_path_expand(const char* path) {
    wordexp_t p;
    if (wordexp(path, &p, 0) != 0) {
        printf("racs: could not expand path: %s", path);
        return NULL;
    }

    char *result = strdup(p.we_wordv[0]);
    wordfree(&p);

    return result;
}


void racs_path_from_time(char *path, racs_uint64 stream_id, racs_time time) {
    struct tm info;
    racs_time_to_tm(&info, time);

    long milliseconds = time % 1000;

    sprintf(path, "%s/.racs/seg/%llu/%d/%02d/%02d/%02d/%02d/%02d/%03ld",
            racs_config_get()->data_dir,
            stream_id,
            info.tm_year + 1900, info.tm_mon + 1,
            info.tm_mday, info.tm_hour,
            info.tm_min, info.tm_sec,
            milliseconds);
}

void racs_path_from_time_range(char *path, racs_uint64 stream_id, racs_time_range range) {
    char path1[PATH_MAX];
    char path2[PATH_MAX];

    racs_path_from_time(path1, stream_id, range.from);
    racs_path_from_time(path2, stream_id, range.to);

    racs_path_resolve(path, path1, path2);
}

void racs_path_resolve(char *dest, const char *path1, const char *path2) {
    size_t i = 0;
    size_t slash_idx = 0;

    while (path1[i] && path2[i] && path1[i] == path2[i]) {
        if (path1[i] == '/') {
            slash_idx = i;
        }
        i++;
    }

    if (path1[i] == '\0' && path2[i] == '\0') {
        slash_idx = i;
    }

    memcpy(dest, path1, slash_idx);
    dest[slash_idx] = '\0';
}

void racs_path_mkdirs(racs_uint64 stream_id, racs_time time) {
    char dir[PATH_MAX];
    racs_path_from_time(dir, stream_id, time);

    char *p = dir;
    while ((p = strchr(p + 1, '/')) != NULL) {
        *p = '\0';
        if (mkdir(dir, 0777) == -1 && errno != EEXIST) {
            *p = '/';
            return;
        }
        *p = '/';
    }
}

