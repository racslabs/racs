#include "streaminfo.h"

auxts_uint64 auxts_streaminfo_attr(auxts_cache* mcache, auxts_uint64 stream_id, const char* attr) {
    auxts_streaminfo streaminfo;
    if (!auxts_streaminfo_get(mcache, &streaminfo, stream_id)) return 0;

    if (strcmp(attr, "rate") == 0)
        return streaminfo.sample_rate;

    if (strcmp(attr, "channels") == 0)
        return streaminfo.channels;

    if (strcmp(attr, "bitdepth") == 0)
        return streaminfo.bit_depth;

    if (strcmp(attr, "size") == 0)
        return streaminfo.size;

    return 0;
}

int auxts_streaminfo_get(auxts_cache* mcache, auxts_streaminfo* streaminfo, auxts_uint64 stream_id) {
    auxts_uint64 key[2] = {stream_id, 0};

    auxts_uint8* data = auxts_cache_get(mcache, key);
    if (!data) return 0;

    off_t bytes = auxts_streaminfo_read(streaminfo, data);
    if (bytes != sizeof(auxts_streaminfo)) return 0;

    return 1;
}

int auxts_streaminfo_put(auxts_cache* mcache, auxts_streaminfo* streaminfo, auxts_uint64 stream_id) {
    auxts_uint8* data = malloc(sizeof(auxts_streaminfo));
    if (!data) return 0;

    auxts_uint64 key[2] = {stream_id, 0};

    auxts_streaminfo_write(data, streaminfo);
    auxts_cache_put(mcache, key, data);

    return 1;
}

void auxts_mcache_destroy(void* key, void* value) {
    auxts_cache_entry* entry = (auxts_cache_entry*)value;
    auxts_uint64 stream_id = entry->key[0];

    auxts_streaminfo streaminfo;
    auxts_streaminfo_read(&streaminfo, entry->value);
    auxts_streaminfo_flush(&streaminfo, stream_id);

    free(entry->value);
}

off_t auxts_streaminfo_write(uint8_t* buf, auxts_streaminfo* streaminfo) {
    off_t offset = 0;
    offset = auxts_write_uint16(buf, streaminfo->channels, offset);
    offset = auxts_write_uint16(buf, streaminfo->bit_depth, offset);
    offset = auxts_write_uint32(buf, streaminfo->sample_rate, offset);
    offset = auxts_write_uint64(buf, streaminfo->size, offset);
    offset = auxts_write_uint64(buf, streaminfo->ref, offset);
    return offset;
}

off_t auxts_streaminfo_read(auxts_streaminfo* streaminfo, auxts_uint8* buf) {
    off_t offset = 0;
    offset = auxts_read_uint16(&streaminfo->channels, buf, offset);
    offset = auxts_read_uint16(&streaminfo->bit_depth, buf, offset);
    offset = auxts_read_uint32(&streaminfo->sample_rate, buf, offset);
    offset = auxts_read_uint64(&streaminfo->size, buf, offset);
    offset = auxts_read_uint64((auxts_uint64*)&streaminfo->ref, buf, offset);
    return offset;
}

void auxts_streaminfo_flush(auxts_streaminfo* streaminfo, auxts_uint64 stream_id) {
    char path[55];
    auxts_streaminfo_path(path, stream_id);

    mkdir(".data", 0777);
    mkdir(".data/md", 0777);

    int fd = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to open auxts_streaminfo file");
        return;
    }

    if (flock(fd, LOCK_EX) == -1) {
        perror("Failed to lock auxts_streaminfo file");
        close(fd);
        return;
    }

    uint8_t buf[24];
    if (auxts_streaminfo_write(buf, streaminfo) != 24) {
        perror("Failed to write auxts_streaminfo");
        close(fd);
        return;
    }

    write(fd, buf, 24);
    flock(fd, LOCK_UN);
    close(fd);
}

int auxts_streaminfo_exits(auxts_uint64 stream_id) {
    char path[55];
    auxts_streaminfo_path(path, stream_id);

    struct stat buffer;
    return stat(path, &buffer) == 0;
}

void auxts_streaminfo_path(char* path, auxts_uint64 stream_id) {
    sprintf(path, ".data/md/%llu", stream_id);
}
