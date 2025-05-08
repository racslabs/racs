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
    if (rats_streaminfo_get(mcache, &streaminfo, stream_id) == 0) return 0;

    if (strcmp(attr, "rate") == 0)
        return streaminfo.sample_rate;

    if (strcmp(attr, "channels") == 0)
        return streaminfo.channels;

    if (strcmp(attr, "bitdepth") == 0)
        return streaminfo.bit_depth;

    return 0;
}

size_t rats_streaminfo_filesize(const char *path) {
    struct stat s;
    if (stat(path, &s) == -1) {
        perror("Failed to get rats_streaminfo file size.");
        return 0;
    }

    return s.st_size;
}

size_t rats_streaminfo_size(rats_streaminfo* streaminfo) {
    return 28 + streaminfo->id_size;
}

int rats_streaminfo_get(rats_cache *mcache, rats_streaminfo *streaminfo, rats_uint64 stream_id) {
    rats_uint64 key[2] = {stream_id, 0};

    rats_uint8 *data = rats_cache_get(mcache, key);
    if (!data) {
        char *path = NULL;

        rats_streaminfo_path(&path, stream_id);
        if (!rats_streaminfo_exits(stream_id)) {
            free(path);
            return 0;
        }

        int fd = open(path, O_RDONLY);
        if (fd == -1) {
            perror("Failed to open rats_streaminfo file");
            free(path);
            return 0;
        }

        size_t len = rats_streaminfo_filesize(path);
        data = malloc(len);
        if (read(fd, data, len) != len) {
            close(fd);
            free(data);
            free(path);
            return 0;
        }

        if (rats_streaminfo_read(streaminfo, data) == 0) {
            close(fd);
            free(data);
            free(path);
            return 0;
        }

        close(fd);
        free(data);
        free(path);
        return -1;
    }

    if (rats_streaminfo_read(streaminfo, data) == 0) return 0;
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

    int rc = rats_streaminfo_get(mcache, streaminfo, stream_id);
    if (rc == -1 || rc == 1) {
        perror("Stream already exist.");
        free(key);
        return 0;
    }

    size_t len = rats_streaminfo_size(streaminfo);
    rats_uint8 *data = malloc(len);
    if (!data) return 0;

    rats_streaminfo_write(data, streaminfo);
    rats_cache_put(mcache, key, data);

    return 1;
}

void rats_streaminfo_load(rats_cache *mcache) {
    char* dir1;
    char* dir2;

    asprintf(&dir1, "%s/.data", rats_streaminfo_dir);
    asprintf(&dir2, "%s/.data/md", rats_streaminfo_dir);

    mkdir(dir1, 0777);
    mkdir(dir2, 0777);

    rats_filelist *list = get_sorted_filelist(dir2);
    free(dir1);
    free(dir2);

    for (int i = 0; i < list->num_files; ++i) {
        int fd = open(list->files[i], O_RDONLY);
        if (fd == -1) {
            perror("Failed to open rats_streaminfo file.");
            continue;
        }

        rats_uint64 stream_id = strtoull(list->files[i], NULL, 10);
        rats_uint64 *key = malloc(2 * sizeof(rats_int64));
        if (!key) {
            close(fd);
            continue;
        }

        key[0] = stream_id;
        key[1] = 0;

        size_t len = rats_streaminfo_filesize(list->files[i]);
        rats_uint8 *data = malloc(len);
        if (!data) {
            close(fd);
            free(key);

            continue;
        }

        if (read(fd, data, len) != len) {
            close(fd);
            free(key);
            free(data);

            continue;
        }

        rats_cache_put(mcache, key, data);
    }

    rats_filelist_destroy(list);
}

void rats_mcache_destroy(void *key, void *value) {
    rats_cache_entry *entry = (rats_cache_entry *) value;
    rats_uint64 stream_id = entry->key[0];

    rats_streaminfo streaminfo;

    off_t size = rats_streaminfo_read(&streaminfo, entry->value);
    rats_streaminfo_flush(entry->value, size, stream_id);

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
    offset = rats_write_uint32(buf, streaminfo->id_size, offset);
    offset = rats_write_bin(buf, streaminfo->id, streaminfo->id_size, offset);
    return offset;
}

off_t rats_streaminfo_read(rats_streaminfo *streaminfo, rats_uint8 *buf) {
    off_t offset = 0;
    offset = rats_read_uint16(&streaminfo->channels, buf, offset);
    offset = rats_read_uint16(&streaminfo->bit_depth, buf, offset);
    offset = rats_read_uint32(&streaminfo->sample_rate, buf, offset);
    offset = rats_read_uint64(&streaminfo->size, buf, offset);
    offset = rats_read_uint64(&streaminfo->ref, buf, offset);
    offset = rats_read_uint32(&streaminfo->id_size, buf, offset);

    streaminfo->id = (char*)buf + offset;
    return offset + streaminfo->id_size;
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

void rats_streaminfo_flush(rats_uint8 *data, rats_uint32 len, rats_uint64 stream_id) {
    char* dir1;
    char* dir2;

    asprintf(&dir1, "%s/.data", rats_streaminfo_dir);
    asprintf(&dir2, "%s/.data/md", rats_streaminfo_dir);

    char *path = NULL;
    rats_streaminfo_path(&path, stream_id);

    mkdir(dir1, 0777);
    mkdir(dir2, 0777);

    free(dir1);
    free(dir2);

    int fd = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd == -1) {
        perror("Failed to open rats_streaminfo file");
        free(path);
        return;
    }

    if (flock(fd, LOCK_EX) == -1) {
        perror("Failed to lock rats_streaminfo file");
        close(fd);
        free(path);
        return;
    }

    write(fd, data, len);
    flock(fd, LOCK_UN);
    close(fd);
    free(path);
}

int rats_streaminfo_exits(rats_uint64 stream_id) {
    char* path = NULL;
    rats_streaminfo_path(&path, stream_id);

    struct stat buffer;

    int rc = (stat(path, &buffer) == 0);
    free(path);

    return rc;
}

void rats_streaminfo_path(char **path, rats_uint64 stream_id) {
    asprintf(path, "%s/.data/md/%llu", rats_streaminfo_dir, stream_id);
}

void rats_streaminfo_list(rats_cache *mcache, rats_streams *streams, const char* pattern) {
    char *path = NULL;
    asprintf(&path, "%s/.data/md", rats_streaminfo_dir);

    rats_filelist *list = rats_filelist_create();
    rats_filelist_add(list, path);

    for (int i = 0; i < list->num_files; ++i) {
        rats_uint64 stream_id = strtoull(list->files[i], NULL, 10);

        rats_streaminfo streaminfo;
        int rc = rats_streaminfo_get(mcache, &streaminfo, stream_id);
        if (rc == -1 || rc == 1) {
            rc = fnmatch(pattern, streaminfo.id, FNM_IGNORECASE);
            if (rc == 0) rats_streams_add(streams, streaminfo.id);
        }
    }

    rats_filelist_destroy(list);
    free(path);

    pthread_rwlock_rdlock(&mcache->rwlock);

    rats_cache_node *node = mcache->head;
    while (node) {
        rats_cache_node *next = (rats_cache_node *) node->next;

        rats_streaminfo streaminfo;
        rats_streaminfo_read(&streaminfo, node->entry.value);
        int rc = fnmatch(pattern, streaminfo.id, FNM_IGNORECASE);
        if (rc == 0) rats_streams_add(streams, streaminfo.id);

        node = next;
    }

    pthread_rwlock_unlock(&mcache->rwlock);
}

void rats_streams_add(rats_streams *streams, const char *stream) {
    if (streams->num_streams == streams->max_streams) {
        streams->max_streams *= 2;

        char **_streams = realloc(streams->streams, streams->max_streams * sizeof(char *));
        if (!_streams) {
            perror("Error reallocating file paths to rats_filelist");
            return;
        }

        streams->streams = _streams;
    }

    streams->streams[streams->num_streams] = strdup(stream);
    ++streams->num_streams;
}

void rats_streams_init(rats_streams *streams) {
    streams->num_streams = 0;
    streams->max_streams = 2;
    streams->streams = malloc(2 * sizeof(char *));
    if (!streams->streams) {
        perror("Failed to allocate rats_streams");
        return;
    }
}

void rats_streams_destroy(rats_streams *streams) {
    for (int i = 0; i < streams->num_streams; ++i) {
        free(streams->streams[i]);
    }
    free(streams->streams);
}
