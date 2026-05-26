#include "info.h"


racs_info *racs_info_create(racs_uint32 sample_rate,
                            racs_uint8  channels,
                            racs_uint8  bit_depth) {
    racs_info *info = malloc(sizeof(racs_info));
    if (!info) {
        return NULL;
    }

    info->ttl = 0;
    info->ref = racs_time_now();
    info->sample_rate = sample_rate;
    info->channels = channels;
    info->bit_depth = bit_depth;

    return info;
}

int racs_info_flush(racs_info *info, const char *path) {
    if (!info || !path) {
        return -1;
    }

    char tmp_path[PATH_MAX];
    snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", path);

    int fd = open(tmp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd != -1) {
        if (write(fd, info, sizeof(racs_info)) == sizeof(racs_info)) {
            fsync(fd);
            close(fd);

            if (rename(tmp_path, path) != 0) {
                unlink(tmp_path);
            }
        } else {
            close(fd);
            unlink(tmp_path);
        }
    }
}

racs_info *racs_info_open(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return NULL;
    }

    racs_uint8 *data = mmap(NULL, sizeof(racs_info), PROT_READ, MAP_PRIVATE, fd, 0);
    madvise(data, sizeof(racs_info), MADV_WILLNEED | MADV_SEQUENTIAL);

    close(fd);

    if (data == MAP_FAILED) {
        return NULL;
    }

    return (racs_info *)data;
}

int racs_info_exist(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

racs_time racs_info_to_time(racs_info *info, racs_uint64 offset) {
    racs_uint32 bytes_per_sample = (info->bit_depth / 8);

    double bytes_per_second = info->channels * info->sample_rate * bytes_per_sample;
    double seconds = (double)offset / bytes_per_second;

    return (racs_time) (seconds * 1000) + info->ref;
}

void racs_info_destroy(racs_info *info) {
    free(info);
}
