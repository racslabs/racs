
#include "stream.h"


typedef struct {
    racs_uint8       id[16];
    racs_uint32      size;
    racs_uint32      capacity;
    racs_uint8      *buf;
    racs_info       *info;
    pthread_mutex_t  mutex;
} racs_stream;

const char *const racs_stream_result_string[] = {
    "",
    "stream not found",
    "stream conflict",
    "stream decode error",
    "stream allocation error"
};

static racs_streams *streams = NULL;

static racs_uint64 racs_streams_hash_cb(const void *key);

static int racs_streams_eq_cb(const void *a, const void *b);

static void racs_streams_destroy_cb(void *key, void *value);

static racs_stream *racs_streams_get(racs_streams *streams, racs_uint64 hash);

static void racs_streams_put(racs_streams *streams, racs_uint64 hash, racs_stream *stream);

static racs_stream *racs_stream_open(const char *path);

static void racs_stream_chunk(racs_stream *stream,
                              racs_uint8 *decoded_data,
                              racs_uint32 decoded_size,
                              racs_uint64 hash);

static racs_uint64 racs_stream_hash(const char *name, size_t size);

static void racs_stream_destroy(racs_stream *stream);

static racs_uint8 *racs_stream_decode(const char *mime_type,
                                      const racs_uint8 *src,
                                      racs_uint32 src_size,
                                      racs_uint32 *decoded_size);

static racs_uint8 *racs_stream_decode_pcm(const racs_uint8 *src,
                                          racs_uint32 src_size,
                                          racs_uint32 *decoded_size);


void racs_streams_init(void) {
    if (!streams) {
        racs_dict_cb cb = {
            .hash = racs_streams_hash_cb,
            .eq = racs_streams_eq_cb,
            .destroy = racs_streams_destroy_cb
        };

        racs_dict *dict = racs_dict_create(8, cb);
        if (!dict) {
            exit(-1);
        }

        streams = malloc(sizeof(racs_streams));
        if (!streams) {
            racs_dict_destroy(dict);
            exit(-1);
        }

        pthread_mutex_init(&streams->mutex, NULL);
        streams->dict = dict;
    }
}

racs_streams *racs_streams_get_(void) {
    if (!streams) {
        return NULL;
    }

    return streams;
}

int racs_streams_create(const char *name,
                        size_t size,
                        racs_uint32 sample_rate,
                        racs_uint8 channels,
                        racs_uint8 bit_depth) {
    char path[PATH_MAX];
    sprintf(path, "%s/.racs/md/", racs_config_get()->data_dir);
    strncat(path, name, size);

    if (racs_info_exist(path)) {
        return RACS_STREAM_CONFLICT;
    }

    racs_fs_mkdir(path);

    racs_info *info = racs_info_create(sample_rate, channels, bit_depth);
    racs_info_flush(info, path);
    racs_info_destroy(info);

    return RACS_STREAM_OK;
}

int racs_streams_open(racs_streams *streams, const char *name, size_t size) {
    racs_uint64 hash = racs_stream_hash(name, size);

    racs_stream *stream = racs_streams_get(streams, hash);
     if (stream) {
        return RACS_STREAM_CONFLICT;
    }

    char path[PATH_MAX];
    sprintf(path, "%s/.racs/md/", racs_config_get()->data_dir);
    strncat(path, name, size);

    stream = racs_stream_open(path);
    if (!stream) {
        return RACS_STREAM_NOT_FOUND;
    }

    racs_streams_put(streams, hash, stream);
    return RACS_STREAM_OK;
}

int racs_streams_append(racs_streams *streams,
                        const char *name,
                        size_t size,
                        const char *mime_type,
                        const racs_uint8 *src,
                        racs_uint32 src_size) {
    racs_uint64 hash = racs_stream_hash(name, size);

    racs_stream *stream = racs_streams_get(streams, hash);
    if (!stream) {
        return RACS_STREAM_NOT_FOUND;
    }

    racs_uint32 decoded_size;
    racs_uint8 *decoded_data = racs_stream_decode(mime_type, src, src_size, &decoded_size);
    if (!decoded_data) {
        return RACS_STREAM_DECODE_ERR;
    }

    racs_stream_chunk(stream, decoded_data, decoded_size, hash);
    free(decoded_data);

    return RACS_STREAM_OK;
}

int racs_streams_close(racs_streams *streams,
                       const char *name,
                       size_t size) {
    racs_uint64 hash = racs_stream_hash(name, size);

    racs_stream *stream = racs_streams_get(streams, hash);
    if (!stream) {
        return RACS_STREAM_NOT_FOUND;
    }

    if (stream->size > 0) {
        racs_uint32 padded_size = stream->capacity - stream->size;

        racs_uint8 *padded_data = malloc(padded_size);
        if (!padded_data) {
            return RACS_STREAM_ALLOC_ERR;
        }

        memset(padded_data, 0, padded_size);
        racs_stream_chunk(stream, padded_data, padded_size, hash);
        free(padded_data);
    }

    return RACS_STREAM_OK;
}

racs_stream *racs_streams_get(racs_streams *streams, racs_uint64 hash) {
    pthread_mutex_lock(&streams->mutex);
    racs_stream *stream = racs_dict_get(streams->dict, &hash);
    if (!stream) {
        pthread_mutex_unlock(&streams->mutex);
        return NULL;
    }
    pthread_mutex_unlock(&streams->mutex);
}

void racs_streams_put(racs_streams *streams, racs_uint64 hash, racs_stream *stream) {
    racs_uint64 *_hash = malloc(sizeof(racs_uint64));
    if (!_hash) {
        return;
    }

    *_hash = hash;

    pthread_mutex_lock(&streams->mutex);
    racs_dict_put(streams->dict, _hash, stream);
    pthread_mutex_unlock(&streams->mutex);
}

void racs_streams_destroy(void) {
    if (!streams) {
        return;
    }

    if (streams->dict) {
        racs_dict_destroy(streams->dict);
    }

    pthread_mutex_destroy(&streams->mutex);
    free(streams);
}

void racs_stream_chunk(racs_stream *stream,
                       racs_uint8 *decoded_data,
                       racs_uint32 decoded_size,
                       racs_uint64 hash) {
    pthread_mutex_lock(&stream->mutex);

    racs_uint8 *ptr = decoded_data;
    while (ptr) {
        racs_uint32 ptr_diff = ptr - decoded_data;

        racs_uint32 b_bytes = stream->capacity - stream->size;
        racs_uint32 r_bytes = decoded_size - ptr_diff;
        racs_uint32 w_bytes = (r_bytes > b_bytes) ? b_bytes : r_bytes;

        memcpy(stream->buf, ptr, w_bytes);

        ptr += w_bytes;
        stream->size += w_bytes;

        if (stream->size >= stream->capacity) {
            racs_uint64 offset = RACS_OFFSETS_GET(hash);
            racs_time time = racs_info_to_time(stream->info, offset);
            racs_uint64 key[3] = { hash, time, 0 };

            size_t compressed_size;
            racs_uint8 *compressed_block = racs_zstd_compress(ptr, stream->size, &compressed_size, 3);

            racs_uint32 checksum = crc32c(0, stream->buf, stream->size);
            RACS_MMT_PUT(key, compressed_block, compressed_size, checksum, 0);

            free(compressed_block);

            offset += stream->size;
            RACS_OFFSETS_PUT(hash, offset);
        }
    }

    pthread_mutex_unlock(&stream->mutex);
}

racs_stream *racs_stream_open(const char *path) {
    racs_info *info = racs_info_open(path);
    if (!info) {
        return NULL;
    }

    racs_stream *stream = malloc(sizeof(racs_stream));
    if (!stream) {
        munmap(info, sizeof(racs_info));
        return NULL;
    }

    racs_uint32 samples_per_block = racs_config_get()->memtable.samples_per_block;
    racs_uint32 capacity = samples_per_block * info->channels * (info->bit_depth / 8);

    stream->size = 0;
    stream->info = info;
    stream->capacity = capacity;

    stream->buf = malloc(capacity);
    if (!stream->buf) {
        munmap(info, sizeof(racs_info));
        free(stream);
        return NULL;
    }

    pthread_mutex_init(&stream->mutex, NULL);
    return stream;
}

void racs_stream_destroy(racs_stream *stream) {
    if (!stream) {
        return;
    }

    if (stream->buf) {
        free(stream->buf);
    }

    if (stream->info) {
         munmap(stream->info, sizeof(racs_info));
    }

    pthread_mutex_destroy(&stream->mutex);
    free(stream);
}

racs_uint8 *racs_stream_decode(const char *mime_type,
                               const racs_uint8 *src,
                               racs_uint32 src_size,
                               racs_uint32 *decoded_size) {
    if (strcmp(mime_type, "audio/pcm") == 0) {
        return racs_stream_decode_pcm(src, src_size, decoded_size);
    }

    return NULL;
}

racs_uint8 *racs_stream_decode_pcm(const racs_uint8 *src,
                                   racs_uint32 src_size,
                                   racs_uint32 *decoded_size) {
    racs_uint8 *out = malloc(src_size);
    if (!out) {
        *decoded_size = 0;
        return NULL;
    }

    memcpy(out, src, src_size);
    *decoded_size = src_size;

    return out;
}

racs_uint64 racs_stream_hash(const char *name, size_t size) {
    racs_uint64 hash[2];
    racs_mmh3_x64_128(name, size, 0, hash);
    return hash[0];
}

racs_uint64 racs_streams_hash_cb(const void *key) {
    racs_uint64 hash[2];
    racs_mmh3_x64_128(key, sizeof(racs_uint64), 0, hash);
    return hash[0];
}

int racs_streams_eq_cb(const void *a, const void *b) {
    racs_uint64 *x = (racs_uint64 *) a;
    racs_uint64 *y = (racs_uint64 *) b;
    return x[0] == y[0];
}

void racs_streams_destroy_cb(void *key, void *value) {
    free(key);
    racs_stream_destroy(value);
}
