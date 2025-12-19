// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "streaminfo.h"

racs_cache *racs_mcache_create(size_t capacity) {
    racs_cache *cache = malloc(sizeof(racs_cache));
    if (!cache) {
        racs_log_error("Failed to allocate racs_mcache");
        return NULL;
    }

    pthread_rwlock_init(&cache->rwlock, NULL);

    cache->size = 0;
    cache->capacity = capacity;
    cache->head = NULL;
    cache->tail = NULL;
    cache->kv = racs_kvstore_create(capacity, racs_cache_hash, racs_mcache_cmp, racs_mcache_destroy);

    return cache;
}

racs_int64 racs_streaminfo_attr(racs_cache *mcache, racs_uint64 stream_id, const char *attr) {
    racs_streaminfo streaminfo;
    if (racs_streaminfo_get(mcache, &streaminfo, stream_id) == 0) return 0;

    if (strcmp(attr, "sample_rate") == 0)
        return streaminfo.sample_rate;

    if (strcmp(attr, "channels") == 0)
        return streaminfo.channels;

    if (strcmp(attr, "bit_depth") == 0)
        return streaminfo.bit_depth;

    if (strcmp(attr, "size") == 0)
        return streaminfo.size;

    if (strcmp(attr, "ref") == 0)
        return streaminfo.ref;

    return 0;
}

size_t racs_streaminfo_filesize(const char *path) {
    struct stat s;
    if (stat(path, &s) == -1) {
        racs_log_error("Failed to get racs_streaminfo file size.");
        return 0;
    }

    return s.st_size;
}

size_t racs_streaminfo_size(racs_streaminfo* streaminfo) {
    return 36 + streaminfo->id_size;
}

int racs_streaminfo_get(racs_cache *mcache, racs_streaminfo *streaminfo, racs_uint64 stream_id) {
    racs_uint64 key[2] = {stream_id, 0};

    racs_uint8 *data = racs_cache_get(mcache, key);
    if (!data) {
        char *path = NULL;

        racs_streaminfo_path(&path, stream_id, false);
        if (!racs_streaminfo_exits(stream_id)) {
            free(path);
            return 0;
        }

        int fd = open(path, O_RDONLY);
        if (fd < 0) {
            racs_log_error("Failed to open racs_streaminfo file");
            free(path);
            return 0;
        }

        size_t len = racs_streaminfo_filesize(path);
        data = malloc(len);
        if (read(fd, data, len) != len) {
            close(fd);
            free(data);
            free(path);
            return 0;
        }

        if (racs_streaminfo_read(streaminfo, data) == 0) {
            close(fd);
            free(data);
            free(path);
            return 0;
        }

        close(fd);
        free(path);

        racs_uint64 *_key = malloc(2 * sizeof(racs_uint64));
        _key[0] = stream_id;
        _key[1] = 0;
        racs_cache_put(mcache, _key, data);

        return -1;
    }

    if (racs_streaminfo_read(streaminfo, data) == 0) return 0;
    return 1;
}

int racs_streaminfo_put(racs_cache *mcache, racs_streaminfo *streaminfo, racs_uint64 stream_id) {
    racs_uint64 *key = malloc(2 * sizeof(racs_uint64));
    if (!key) {
        racs_log_error("Failed to allocate key.");
        return 0;
    }

    key[0] = stream_id;
    key[1] = 0;

    racs_streaminfo s;
    int rc = racs_streaminfo_get(mcache, &s, stream_id);

    if (rc == -1 || rc == 1) {
        racs_uint8 *data = racs_cache_get(mcache, key);
        racs_streaminfo_write(data, streaminfo);
        free(key);
        return 0;
    }

    size_t len = racs_streaminfo_size(streaminfo);
    racs_uint8 *data = malloc(len);
    if (!data) {
        free(key);
        return 0;
    }

    racs_streaminfo_write(data, streaminfo);
    racs_cache_put(mcache, key, data);

    return 1;
}

void racs_mcache_destroy(void *key, void *value) {
    racs_cache_node *node = (racs_cache_node *)value;
    racs_uint64 stream_id = node->entry.key[0];

    racs_streaminfo streaminfo;

    off_t size = racs_streaminfo_read(&streaminfo, node->entry.value);
    racs_streaminfo_flush(node->entry.value, size, stream_id);

    free(node->entry.value);
    free(node);
    free(key);
}

off_t racs_streaminfo_write(racs_uint8 *buf, racs_streaminfo *streaminfo) {
    off_t offset = 0;
    offset = racs_write_uint16(buf, streaminfo->channels, offset);
    offset = racs_write_uint16(buf, streaminfo->bit_depth, offset);
    offset = racs_write_uint32(buf, streaminfo->sample_rate, offset);
    offset = racs_write_uint64(buf, streaminfo->size, offset);
    offset = racs_write_uint64(buf, streaminfo->lsn, offset);
    offset = racs_write_uint64(buf, streaminfo->ref, offset);
    offset = racs_write_uint64(buf, streaminfo->ttl, offset);
    offset = racs_write_uint32(buf, streaminfo->id_size, offset);
    offset = racs_write_bin(buf, streaminfo->id, streaminfo->id_size, offset);
    return offset;
}

off_t racs_streaminfo_read(racs_streaminfo *streaminfo, racs_uint8 *buf) {
    off_t offset = 0;
    offset = racs_read_uint16(&streaminfo->channels, buf, offset);
    offset = racs_read_uint16(&streaminfo->bit_depth, buf, offset);
    offset = racs_read_uint32(&streaminfo->sample_rate, buf, offset);
    offset = racs_read_uint64(&streaminfo->size, buf, offset);
    offset = racs_read_uint64(&streaminfo->lsn, buf, offset);
    offset = racs_read_uint64((racs_uint64 *) &streaminfo->ref, buf, offset);
    offset = racs_read_uint64((racs_uint64 *) &streaminfo->ttl, buf, offset);
    offset = racs_read_uint32(&streaminfo->id_size, buf, offset);

    streaminfo->id = (char*)buf + offset;
    return offset + streaminfo->id_size;
}

racs_time racs_streaminfo_offset(racs_streaminfo *streaminfo) {
    double seconds = streaminfo->size / (double) (streaminfo->channels * streaminfo->sample_rate * (streaminfo->bit_depth / 8));
    return (racs_time) (seconds * 1000) + streaminfo->ref;
}

racs_uint64 racs_hash(const char *stream_id) {
    racs_uint64 hash[2];
    murmur3_x64_128((racs_uint8*)stream_id, strlen(stream_id), 0, hash);
    return hash[0];
}

void racs_streaminfo_flush(racs_uint8 *data, racs_uint32 len, racs_uint64 stream_id) {
    char* dir1;
    char* dir2;

    asprintf(&dir1, "%s/.racs", racs_streaminfo_dir);
    asprintf(&dir2, "%s/.racs/md", racs_streaminfo_dir);

    char *tmp_path = NULL;
    char *final_path = NULL;

    racs_streaminfo_path(&tmp_path, stream_id, true);

    mkdir(dir1, 0777);
    mkdir(dir2, 0777);

    free(dir1);
    free(dir2);

    int fd = open(tmp_path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd < 0) {
        racs_log_error("Failed to open racs_streaminfo file");
        free(tmp_path);
        return;
    }

    if (flock(fd, LOCK_EX) < 0) {
        racs_log_error("Failed to lock racs_streaminfo file");
        close(fd);
        free(tmp_path);
        return;
    }

    write(fd, data, len);

    if (fsync(fd) < 0)
        racs_log_error("fsync failed on racs_streaminfo file");

    if (flock(fd, LOCK_UN) < 0)
        racs_log_error("Failed to unlock racs_streaminfo file");

    racs_streaminfo_path(&final_path, stream_id, false);

    if (rename(tmp_path, final_path) < 0)
        racs_log_error("Failed to rename racs_streaminfo file");

    close(fd);
    free(tmp_path);
    free(final_path);
}

int racs_streaminfo_exits(racs_uint64 stream_id) {
    char* path = NULL;
    racs_streaminfo_path(&path, stream_id, false);

    struct stat buffer;

    int rc = (stat(path, &buffer) == 0);
    free(path);

    return rc;
}

void racs_streaminfo_path(char **path, racs_uint64 stream_id, int tmp) {
    const char *ext = tmp ? ".tmp" : "";
    asprintf(path, "%s/.racs/md/%llu%s", racs_streaminfo_dir, stream_id, ext);
}

racs_uint64 racs_path_to_stream_id(char *path) {
    char *curr;
    char *prev = NULL;

    curr = strtok(path, "/");
    while (curr != NULL) {
        prev = curr;
        curr = strtok(NULL, "/");
    }

    if (!prev) return 0;

    return strtoull(prev, NULL, 10);
}

void racs_streaminfo_list(racs_cache *mcache, racs_streams *streams, const char* pattern) {
    char *path = NULL;
    asprintf(&path, "%s/.racs/md", racs_streaminfo_dir);

    racs_filelist *list = get_sorted_filelist(path);

    for (int i = 0; i < list->num_files; ++i) {
        racs_uint64 stream_id = racs_path_to_stream_id(list->files[i]);

        racs_streaminfo streaminfo;
        int rc = racs_streaminfo_get(mcache, &streaminfo, stream_id);

        if (rc == -1 || rc == 1) {
            rc = fnmatch(pattern, streaminfo.id, FNM_IGNORECASE);
            if (rc == 0) racs_streams_add(streams, streaminfo.id);
        }
    }

    racs_filelist_destroy(list);
    free(path);
}

void racs_streams_add(racs_streams *streams, const char *stream) {
    if (streams->num_streams == streams->max_streams) {
        streams->max_streams *= 2;

        char **_streams = realloc(streams->streams, streams->max_streams * sizeof(char *));
        if (!_streams) {
            racs_log_error("Error reallocating file paths to racs_filelist");
            return;
        }

        streams->streams = _streams;
    }

    streams->streams[streams->num_streams] = strdup(stream);
    ++streams->num_streams;
}

void racs_streams_init(racs_streams *streams) {
    streams->num_streams = 0;
    streams->max_streams = 2;
    streams->streams = malloc(2 * sizeof(char *));
    if (!streams->streams) {
        racs_log_error("Failed to allocate racs_streams");
        return;
    }
}

void racs_streams_destroy(racs_streams *streams) {
    for (int i = 0; i < streams->num_streams; ++i) {
        free(streams->streams[i]);
    }
    free(streams->streams);
}

int racs_mcache_cmp(void *a, void *b) {
    racs_uint64 *x = (racs_uint64 *) a;
    racs_uint64 *y = (racs_uint64 *) b;
    return x[0] == y[0];
}
