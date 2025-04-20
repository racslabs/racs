#include "metadata.h"

static off_t metadata_write(uint8_t buf[], const auxts_metadata* metadata);
static off_t metadata_read(auxts_metadata* metadata, uint8_t buf[]);
static int metadata_exist(const char* path);
static int write_metadata_index(const char* stream_id);
static int get_metadata_path(char* path, const char* stream_id);

int auxts_metadata_attr(const char* stream_id, const char* attr, uint64_t* value) {
    auxts_metadata metadata;
    int rc = auxts_metadata_get(&metadata, stream_id);

    if (rc != AUXTS_METADATA_STATUS_OK)
        return rc;

    if (strcmp(attr, "sample_rate") == 0) {
        *value = metadata.sample_rate;
        return AUXTS_METADATA_STATUS_OK;
    }

    if (strcmp(attr, "channels") == 0) {
        *value = metadata.channels;
        return AUXTS_METADATA_STATUS_OK;
    }

    if (strcmp(attr, "bit_depth") == 0) {
        *value = metadata.bit_depth;
        return AUXTS_METADATA_STATUS_OK;
    }

    if (strcmp(attr, "bytes") == 0) {
        *value = metadata.bytes;
        return AUXTS_METADATA_STATUS_OK;
    }

    return AUXTS_METADATA_STATUS_NOT_FOUND;
}

int auxts_metadata_get(auxts_metadata* metadata, const char* stream_id) {
    char path[55];

    if (!get_metadata_path(path, stream_id))
        return AUXTS_METADATA_STATUS_NOT_FOUND;

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open metadata file");
        return AUXTS_METADATA_STATUS_ERROR;
    }

    uint8_t buf[24];
    read(fd, buf, 24);

    if (metadata_read(metadata, buf) != 24) {
        perror("Failed to read metadata");
        close(fd);
        return AUXTS_METADATA_STATUS_ERROR;
    }

    close(fd);

    return AUXTS_METADATA_STATUS_OK;
}

int auxts_metadata_put(const auxts_metadata* metadata, const char* stream_id) {
    char path[55];

    if (get_metadata_path(path, stream_id))
        return AUXTS_METADATA_STATUS_EXIST;

    mkdir(".data", 0777);
    mkdir(".data/md", 0777);

    int fd = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to open metadata file");
        return AUXTS_METADATA_STATUS_ERROR;
    }

    if (flock(fd, LOCK_EX) == -1) {
        perror("Failed to lock metadata file");
        close(fd);
        return AUXTS_METADATA_STATUS_ERROR;
    }

    uint8_t buf[24];
    if (metadata_write(buf, metadata) != 24) {
        perror("Failed to write metadata");
        close(fd);
        return -1;
    }

    write(fd, buf, 24);

    flock(fd, LOCK_UN);
    close(fd);

    return write_metadata_index(stream_id);
}

int metadata_exist(const char* path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

int get_metadata_path(char* path, const char* stream_id) {
    uint64_t hash = auxts_hash_stream_id(stream_id);

    sprintf(path, ".data/md/%llu", hash);

    if (!metadata_exist(path)) {
        perror("Metadata not found");
        return 0;
    }

    return 1;
}

int auxts_stream_id_exist(uint64_t stream_id) {
    char path[55];
    sprintf(path, ".data/md/%llu", stream_id);
    return metadata_exist(path);
}

int write_metadata_index(const char* stream_id) {
    int fd = open(".data/index", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to open metadata index file");
        return AUXTS_METADATA_STATUS_ERROR;
    }

    if (flock(fd, LOCK_SH) == -1) {
        perror("Failed to lock metadata file");
        close(fd);
        return AUXTS_METADATA_STATUS_ERROR;
    }

    size_t size = strlen(stream_id);
    write(fd, &size, sizeof(size_t));
    write(fd, stream_id, size);

    flock(fd, LOCK_UN);
    close(fd);

    return AUXTS_METADATA_STATUS_OK;
}

off_t metadata_read(auxts_metadata* metadata, uint8_t buf[]) {
    off_t offset = 0;
    offset = auxts_read_uint16(&metadata->channels, buf, offset);
    offset = auxts_read_uint16(&metadata->bit_depth, buf, offset);
    offset = auxts_read_uint32(&metadata->sample_rate, buf, offset);
    offset = auxts_read_uint64(&metadata->bytes, buf, offset);
    offset = auxts_read_uint64(&metadata->ref, buf, offset);
    return offset;
}

off_t metadata_write(uint8_t buf[], const auxts_metadata* metadata) {
    off_t offset = 0;
    offset = auxts_write_uint16(buf, metadata->channels, offset);
    offset = auxts_write_uint16(buf, metadata->bit_depth, offset);
    offset = auxts_write_uint32(buf, metadata->sample_rate, offset);
    offset = auxts_write_uint64(buf, metadata->bytes, offset);
    offset = auxts_write_uint64(buf, metadata->ref, offset);
    return offset;
}

uint64_t auxts_hash_stream_id(const char* stream_id) {
    uint64_t hash[2];
    murmur3_x64_128(stream_id, strlen(stream_id), 0, hash);
    return hash[0];
}
