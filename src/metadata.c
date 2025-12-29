// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "metadata.h"

racs_int64 racs_metadata_attr(racs_uint64 stream_id, const char *attr) {
    racs_metadata streaminfo;
    if (racs_metadata_get(&streaminfo, stream_id) == 0) return 0;

    if (strcmp(attr, "sample_rate") == 0)
        return streaminfo.sample_rate;

    if (strcmp(attr, "channels") == 0)
        return streaminfo.channels;

    if (strcmp(attr, "bit_depth") == 0)
        return streaminfo.bit_depth;

    if (strcmp(attr, "ref") == 0)
        return streaminfo.ref;

    return 0;
}

size_t racs_metadata_filesize(const char *path) {
    struct stat s;
    if (stat(path, &s) == -1) {
        racs_log_error("Failed to get racs_streaminfo file size.");
        return 0;
    }

    return s.st_size;
}

size_t racs_metadata_size(racs_metadata* streaminfo) {
    return 36 + streaminfo->id_size;
}

int racs_metadata_get(racs_metadata *streaminfo, racs_uint64 stream_id) {
    char *path = NULL;

    racs_metadata_path(&path, stream_id, false);
    if (!racs_metadata_exits(stream_id)) {
        free(path);
        return 0;
    }

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        racs_log_error("Failed to open racs_streaminfo file");
        free(path);
        return 0;
    }

    size_t len = racs_metadata_filesize(path);

    racs_uint8 *data = malloc(len);
    if (read(fd, data, len) != len) {
        close(fd);
        free(data);
        free(path);
        return 0;
    }

    racs_metadata_read(streaminfo, data);
    close(fd);
    free(path);
    free(data);

    return 1;
}

int racs_metadata_put(racs_metadata *streaminfo, racs_uint64 stream_id) {
    racs_metadata s;
    int rc = racs_metadata_get(&s, stream_id);
    if (rc == 1) return 0;

    size_t len = racs_metadata_size(streaminfo);
    racs_uint8 *data = malloc(len);
    if (!data) return 0;

    racs_metadata_write(data, streaminfo);
    racs_metadata_flush(data, len, stream_id);

    free(data);
    return 1;
}

off_t racs_metadata_write(racs_uint8 *buf, racs_metadata *streaminfo) {
    off_t offset = 0;
    offset = racs_write_uint16(buf, streaminfo->channels, offset);
    offset = racs_write_uint16(buf, streaminfo->bit_depth, offset);
    offset = racs_write_uint32(buf, streaminfo->sample_rate, offset);
    offset = racs_write_uint64(buf, streaminfo->ref, offset);
    offset = racs_write_uint64(buf, streaminfo->ttl, offset);
    offset = racs_write_uint32(buf, streaminfo->id_size, offset);
    offset = racs_write_bin(buf, streaminfo->id, streaminfo->id_size, offset);
    return offset;
}

off_t racs_metadata_read(racs_metadata *streaminfo, racs_uint8 *buf) {
    off_t offset = 0;
    offset = racs_read_uint16(&streaminfo->channels, buf, offset);
    offset = racs_read_uint16(&streaminfo->bit_depth, buf, offset);
    offset = racs_read_uint32(&streaminfo->sample_rate, buf, offset);
    offset = racs_read_uint64((racs_uint64 *) &streaminfo->ref, buf, offset);
    offset = racs_read_uint64((racs_uint64 *) &streaminfo->ttl, buf, offset);
    offset = racs_read_uint32(&streaminfo->id_size, buf, offset);

    streaminfo->id = strdup((char *)buf + offset);
    return offset + streaminfo->id_size;
}

racs_time racs_metadata_timestamp(racs_metadata *streaminfo, racs_uint64 offset) {
    double seconds = offset / (double) (streaminfo->channels * streaminfo->sample_rate * (streaminfo->bit_depth / 8));
    return (racs_time) (seconds * 1000) + streaminfo->ref;
}

racs_uint64 racs_hash(const char *stream_id) {
    racs_uint64 hash[2];
    murmur3_x64_128((racs_uint8*)stream_id, strlen(stream_id), 0, hash);
    return hash[0];
}

void racs_metadata_flush(racs_uint8 *data, racs_uint32 len, racs_uint64 stream_id) {
    char* dir1;
    char* dir2;

    asprintf(&dir1, "%s/.racs", racs_metadata_dir);
    asprintf(&dir2, "%s/.racs/md", racs_metadata_dir);

    char *tmp_path = NULL;
    char *final_path = NULL;

    racs_metadata_path(&tmp_path, stream_id, true);

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

    racs_metadata_path(&final_path, stream_id, false);

    if (rename(tmp_path, final_path) < 0)
        racs_log_error("Failed to rename racs_streaminfo file");

    close(fd);
    free(tmp_path);
    free(final_path);
}

int racs_metadata_exits(racs_uint64 stream_id) {
    char* path = NULL;
    racs_metadata_path(&path, stream_id, false);

    struct stat buffer;

    int rc = (stat(path, &buffer) == 0);
    free(path);

    return rc;
}

void racs_metadata_destroy(racs_metadata *streaminfo) {
    free(streaminfo->id);
}

void racs_metadata_path(char **path, racs_uint64 stream_id, int tmp) {
    const char *ext = tmp ? ".tmp" : "";
    asprintf(path, "%s/.racs/md/%llu%s", racs_metadata_dir, stream_id, ext);
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

void racs_streams_list(racs_streams *streams, const char* pattern) {
    char *path = NULL;
    asprintf(&path, "%s/.racs/md", racs_metadata_dir);

    racs_filelist *list = get_sorted_filelist(path);

    for (int i = 0; i < list->num_files; ++i) {
        racs_uint64 stream_id = racs_path_to_stream_id(list->files[i]);

        racs_metadata streaminfo;
        int rc = racs_metadata_get(&streaminfo, stream_id);

        if (rc == 1) {
            rc = fnmatch(pattern, streaminfo.id, FNM_IGNORECASE);
            if (rc == 0) racs_streams_add(streams, streaminfo.id);
            racs_metadata_destroy(&streaminfo);
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

    if (!streams->streams)
        racs_log_error("Failed to allocate racs_streams");
}

void racs_streams_destroy(racs_streams *streams) {
    for (int i = 0; i < streams->num_streams; ++i) {
        free(streams->streams[i]);
    }
    free(streams->streams);
}
