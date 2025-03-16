#include "metadata.h"

static off_t metadata_write(uint8_t buf[], const auxts_metadata* metadata);
static off_t metadata_read(auxts_metadata* metadata, uint8_t buf[]);
static int metadata_exist(const char* path);
static int write_metadata_index(const char* name);
static uint64_t hash_stream_name(const char* name);
static int get_metadata_path(char* path, const char* name);

int auxts_metadata_get(auxts_metadata* metadata, const char* name) {
    char path[55];
    if (!get_metadata_path(path, name)) return -1;

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open metadata file");
        return -1;
    }

    uint8_t buf[28];
    read(fd, buf, 28);

    if (metadata_read(metadata, buf) != 28) {
        perror("Failed to read metadata");
        close(fd);
        return -1;
    }

    close(fd);

    return 1;
}

int auxts_metadata_put(const auxts_metadata* metadata, const char* name) {
    char path[55];
    if (get_metadata_path(path, name)) return -1;

    mkdir(".data", 0777);

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
    if (metadata_write(buf, metadata) != 28) {
        perror("Failed to write metadata");
        close(fd);
        return -1;
    }

    write(fd, buf, 28);

    flock(fd, LOCK_UN);
    close(fd);

    return write_metadata_index(name);
}

int metadata_exist(const char* path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

int get_metadata_path(char* path, const char* name) {
    uint64_t stream_id = hash_stream_name(name);

    sprintf(path, ".data/%llu", stream_id);

    if (!metadata_exist(path)) {
        perror("Metadata not found");
        return 0;
    }

    return 1;
}

int auxts_stream_id_exist(uint64_t stream_id) {
    char path[55];
    sprintf(path, ".data/%llu", stream_id);
    return metadata_exist(path);
}

int write_metadata_index(const char* name) {
    int fd = open(".data/index", O_WRONLY | O_APPEND | O_CREAT, 0644);
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

off_t metadata_read(auxts_metadata* metadata, uint8_t buf[]) {
    off_t offset = 0;
    offset = auxts_read_uint32(&metadata->channels, buf, offset);
    offset = auxts_read_uint32(&metadata->sample_rate, buf, offset);
    offset = auxts_read_uint32(&metadata->bit_depth, buf, offset);
    offset = auxts_read_uint64(&metadata->bytes, buf, offset);
    offset = auxts_read_uint64(&metadata->created_at, buf, offset);
    return offset;
}

off_t metadata_write(uint8_t buf[], const auxts_metadata* metadata) {
    off_t offset = 0;
    offset = auxts_write_uint32(buf, metadata->channels, offset);
    offset = auxts_write_uint32(buf, metadata->sample_rate, offset);
    offset = auxts_write_uint32(buf, metadata->bit_depth, offset);
    offset = auxts_write_uint64(buf, metadata->bytes, offset);
    offset = auxts_write_uint64(buf, metadata->created_at, offset);
    return offset;
}

uint64_t hash_stream_name(const char* name) {
    uint64_t hash[2];
    murmur3_x64_128(name, strlen(name), 0, hash);
    return hash[0];
}
