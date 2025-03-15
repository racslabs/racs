#include "metadata.h"

static off_t metadata_write(uint8_t buf[], const auxts_metadata* metadata);
static int write_metadata_index(const char* name);
static uint64_t hash_stream_name(const char* name);

int auxts_metadata_update(const auxts_metadata* metadata, const char* name) {
    uint64_t hash = hash_stream_name(name);

    char path[55];
    sprintf(path, ".data/%llu", hash);

    int fd = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to open metadata file");
        return -1;
    }

    if (flock(fd, LOCK_EX) == -1) {
        perror("Failed to lock metadata file");
        close(fd);
        return -1;
    }

    uint8_t buf[28];
    if (metadata_write(buf, metadata) != 28 || write(fd, buf, 28) == -1) {
        perror("Failed to write metadata");
        close(fd);
        return -1;
    }

    flock(fd, LOCK_UN);
    close(fd);

    return write_metadata_index(name);
}

int write_metadata_index(const char* name) {
    int fd = open("./data/index", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to open metadata index file");
        return -1;
    }

    if (flock(fd, LOCK_SH) == -1) {
        perror("Failed to lock metadata file");
        close(fd);
        return -1;
    }

    size_t size = strlen(name);
    write(fd, &size, sizeof(size_t));
    write(fd, name, size);

    flock(fd, LOCK_UN);
    close(fd);

    return 1;
}

off_t metadata_write(uint8_t buf[], const auxts_metadata* metadata) {
    off_t offset = 0;
    offset = auxts_write_uint32(buf, metadata->channels, offset);
    offset = auxts_write_uint32(buf, metadata->sample_rate, offset);
    offset = auxts_write_uint32(buf, metadata->bit_depth, offset);
    offset = auxts_write_uint64(buf, metadata->bytes, offset);
    offset = auxts_write_uint64(buf, metadata->ref, offset);
    return offset;
}

uint64_t hash_stream_name(const char* name) {
    uint64_t hash[2];
    murmur3_x64_128(name, strlen(name), 0, hash);
    return hash[0];
}
