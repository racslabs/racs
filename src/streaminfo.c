#include "streaminfo.h"

rats_cache *rats_mcache_create(size_t capacity) {
    rats_cache *cache = malloc(sizeof(rats_cache));
    if (!cache) {
        perror("Failed to allocate rats_mcache");
        return NULL;
    }

    pthread_rwlock_init(&cache->rwlock, NULL);

    cache->size = 0;
    cache->capacity = capacity;
    cache->head = NULL;
    cache->tail = NULL;
    cache->kv = rats_kvstore_create(capacity, rats_cache_hash, rats_cache_cmp, rats_mcache_destroy);

    return cache;
}

rats_uint64 rats_streaminfo_attr(rats_cache *mcache, rats_uint64 stream_id, const char *attr) {
    rats_streaminfo streaminfo;
    if (!rats_streaminfo_get(mcache, &streaminfo, stream_id)) return 0;

    if (strcmp(attr, "rate") == 0)
        return streaminfo.sample_rate;

    if (strcmp(attr, "channels") == 0)
        return streaminfo.channels;

    if (strcmp(attr, "bitdepth") == 0)
        return streaminfo.bit_depth;

    return 0;
}

int rats_streaminfo_get(rats_cache *mcache, rats_streaminfo *streaminfo, rats_uint64 stream_id) {
    rats_uint64 key[2] = {stream_id, 0};

    rats_uint8 *data = rats_cache_get(mcache, key);
    if (!data) {
        char path[55];
        rats_streaminfo_path(path, stream_id);
        if (!rats_streaminfo_exits(stream_id)) return 0;

        int fd = open(path, O_RDONLY);
        if (fd == -1) {
            perror("Failed to open rats_streaminfo file");
            return 0;
        }

        rats_uint8 buf[24];
        if (read(fd, buf, 24) != 24) {
            close(fd);
            return 0;
        }

        if (rats_streaminfo_read(streaminfo, buf) != 24) return 0;

        close(fd);
        return 1;
    }

    if (rats_streaminfo_read(streaminfo, data) != 24) return 0;
    return 1;
}

int rats_streaminfo_put(rats_cache *mcache, rats_streaminfo *streaminfo, rats_uint64 stream_id) {
    rats_uint64 *key = malloc(2 * sizeof(rats_int64));
    if (!key) {
        perror("Failed to allocate key.");
        return 0;
    }

    key[0] = stream_id;
    key[1] = 0;

    if (rats_streaminfo_get(mcache, streaminfo, stream_id)) {
        perror("Stream already exist.");
        free(key);
        return 0;
    }

    rats_uint8 *data = malloc(24);
    if (!data) return 0;

    rats_streaminfo_write(data, streaminfo);
    rats_cache_put(mcache, key, data);

    return 1;
}

void rats_mcache_destroy(void *key, void *value) {
    rats_cache_entry *entry = (rats_cache_entry *) value;
    rats_uint64 stream_id = entry->key[0];

    rats_streaminfo_flush(entry->value, stream_id);
    free(entry->value);
    free(key);
}

off_t rats_streaminfo_write(rats_uint8 *buf, rats_streaminfo *streaminfo) {
    off_t offset = 0;
    offset = rats_write_uint16(buf, streaminfo->channels, offset);
    offset = rats_write_uint16(buf, streaminfo->bit_depth, offset);
    offset = rats_write_uint32(buf, streaminfo->sample_rate, offset);
    offset = rats_write_uint64(buf, streaminfo->size, offset);
    offset = rats_write_uint64(buf, streaminfo->ref, offset);
    return offset;
}

off_t rats_streaminfo_read(rats_streaminfo *streaminfo, rats_uint8 *buf) {
    off_t offset = 0;
    offset = rats_read_uint16(&streaminfo->channels, buf, offset);
    offset = rats_read_uint16(&streaminfo->bit_depth, buf, offset);
    offset = rats_read_uint32(&streaminfo->sample_rate, buf, offset);
    offset = rats_read_uint64(&streaminfo->size, buf, offset);
    offset = rats_read_uint64(&streaminfo->ref, buf, offset);
    return offset;
}

rats_time rats_streaminfo_offset(rats_streaminfo *streaminfo) {
    double seconds = (streaminfo->size / (double) (streaminfo->channels * streaminfo->sample_rate * 2));
    return (rats_time) (seconds * 1000) + streaminfo->ref;
}

rats_uint64 rats_hash(const char *stream_id) {
    rats_uint64 hash[2];
    murmur3_x64_128((void *) stream_id, strlen(stream_id), 0, hash);
    return hash[0];
}

void rats_streaminfo_flush(rats_uint8 *data, rats_uint64 stream_id) {
    char path[55];
    rats_streaminfo_path(path, stream_id);

    mkdir(".data", 0777);
    mkdir(".data/md", 0777);

    int fd = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to open rats_streaminfo file");
        return;
    }

    if (flock(fd, LOCK_EX) == -1) {
        perror("Failed to lock rats_streaminfo file");
        close(fd);
        return;
    }

    write(fd, data, 24);
    flock(fd, LOCK_UN);
    close(fd);
}

int rats_streaminfo_exits(rats_uint64 stream_id) {
    char path[55];
    rats_streaminfo_path(path, stream_id);

    struct stat buffer;
    return stat(path, &buffer) == 0;
}

void rats_streaminfo_path(char *path, rats_uint64 stream_id) {
    sprintf(path, ".data/md/%llu", stream_id);
}
