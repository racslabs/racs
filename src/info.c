#include "info.h"


void racs_info_init(racs_info  *info,
                    const char *name,
                    racs_uint32 sample_rate,
                    racs_uint8  channels,
                    racs_uint8  bit_depth) {
    info->ttl = 0;
    info->ref = racs_time_now();
    info->sample_rate = sample_rate;
    info->channels = channels;
    info->bit_depth = bit_depth;

    strcpy(info->name, name);
}

int racs_info_flush(racs_info *info, const char *path) {
    if (!info || !path) {
        return -1;
    }

    char tmp_path[PATH_MAX];
    snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", path);

    int fd = open(tmp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd != -1) {
        if (write(fd, info, 22) == 22) {
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