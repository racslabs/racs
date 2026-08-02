#include "info.h"


size_t racs_info_size(racs_info *info);

racs_info *racs_info_create(const char *stream_id,
                            racs_uint32 sample_rate,
                            racs_uint8 channels,
                            racs_uint8 bit_depth) {
    if (!stream_id) {
        return NULL;
    }

    racs_uint32 stream_id_size = (racs_uint32) (strlen(stream_id) + 1);

    racs_info *info = malloc(sizeof(racs_info) + stream_id_size);
    if (!info) {
        return NULL;
    }

    info->ttl = 0;
    info->ref = racs_time_now();
    info->sample_rate = sample_rate;
    info->channels = channels;
    info->bit_depth = bit_depth;
    info->stream_id_size = stream_id_size;

    strcpy(info->stream_id, stream_id);
    return info;
}

racs_uint64 racs_info_hash(const char *stream_id) {
    if (!stream_id) {
        return 0;
    }

    racs_uint64 hash[2];
    racs_mmh3_x64_128(stream_id, strlen(stream_id), 0, hash);
    return hash[0];
}

size_t racs_info_size(racs_info *info) {
    if (!info) {
        return 0;
    }

    return sizeof(*info) + info->stream_id_size;
}

void racs_info_path(char *path, const char *stream_id) {
    if (!stream_id || !path) {
        return;
    }

    racs_uint64 hash = racs_info_hash(stream_id);
    snprintf(path, PATH_MAX, "%s/.racs/md/%llu", racs_config_get()->data_dir, hash);
}

int racs_info_flush(racs_info *info, const char *path) {
    if (!info || !path) {
        return -1;
    }

    size_t size = racs_info_size(info);
    if (size == 0) {
        return -1;
    }

    racs_uint8 *buf = malloc(size);
    if (!buf) {
        return -1;
    }

    racs_fs_mkdir(path);

    int rc = racs_fs_write(path, info, size);
    if (rc == -1) {
        free(buf);
        return -1;
    }

    free(buf);
    return 0;
}

racs_info *racs_info_open(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return NULL;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        return NULL;
    }

    size_t size = st.st_size;
    racs_uint8 *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    madvise(data, size, MADV_WILLNEED | MADV_SEQUENTIAL);

    close(fd);

    if (data == MAP_FAILED) {
        return NULL;
    }

    return (racs_info *) data;
}

int racs_info_exist(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

racs_time racs_info_to_time(racs_info *info, racs_uint64 offset) {
    racs_uint32 bytes_per_sample = (info->bit_depth / 8);

    double bytes_per_second = info->channels * info->sample_rate * bytes_per_sample;
    double seconds = (double) offset / bytes_per_second;

    return (racs_time) (seconds * 1000) + info->ref;
}

void racs_info_destroy(racs_info *info) {
    if (!info) {
        return;
    }

    free(info);
}

void racs_info_munmap(racs_info *info) {
    if (!info) {
        return;
    }

    munmap(info, racs_info_size(info));
}
