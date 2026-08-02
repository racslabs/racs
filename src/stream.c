
#include "stream.h"


typedef struct {
    racs_uint8 id[16];
    racs_uint32 size;
    racs_uint32 capacity;
    racs_uint8 *buf;
    racs_info *info;
    pthread_mutex_t mutex;
} racs_stream;

const char *const racs_stream_result_string[] = {
    "",
    "not found",
    "conflict",
    "decode error",
    "allocation error",
    "buffer overflow"
};

static racs_streams *streams_ = NULL;

racs_uint64 racs_streams_hash_cb(const void *key);

int racs_streams_eq_cb(const void *a, const void *b);

void racs_streams_destroy_cb(void *key, void *value);

racs_stream *racs_streams_get_stream(racs_streams *streams, racs_uint64 hash);

void racs_streams_put_stream(racs_streams *streams, racs_uint64 hash, racs_stream *stream);

racs_stream *racs_stream_open(const char *path);

void racs_stream_chunk(racs_stream *stream,
                       const racs_uint8 *src,
                       racs_uint32 src_size,
                       racs_uint64 hash);

void racs_stream_destroy(racs_stream *stream);

int racs_stream_decode(const char *mime_type,
                       const racs_uint8 *src,
                       racs_uint32 src_size,
                       racs_uint8 **out,
                       racs_uint32 *out_size);

int racs_stream_decode_pcm(const racs_uint8 *src,
                           racs_uint32 src_size,
                           racs_uint8 **out,
                           racs_uint32 *out_size);


void racs_streams_init(void) {
    if (!streams_) {
        racs_dict_cb cb = {
            .hash = racs_streams_hash_cb,
            .eq = racs_streams_eq_cb,
            .destroy = racs_streams_destroy_cb
        };

        racs_dict *dict = racs_dict_create(8, cb);
        if (!dict) {
            exit(-1);
        }

        streams_ = malloc(sizeof(racs_streams));
        if (!streams_) {
            racs_dict_destroy(dict);
            exit(-1);
        }

        pthread_mutex_init(&streams_->mutex, NULL);
        streams_->dict = dict;
    }
}

racs_streams *racs_streams_get(void) {
    if (!streams_) {
        return NULL;
    }

    return streams_;
}

int racs_streams_create(const char *stream_id,
                        racs_uint32 sample_rate,
                        racs_uint8 channels,
                        racs_uint8 bit_depth) {
    char path[PATH_MAX];
    racs_info_path(path, stream_id);

    if (racs_info_exist(path)) {
        return RACS_STREAM_CONFLICT;
    }

    racs_info *info = racs_info_create(stream_id, sample_rate, channels, bit_depth);
    if (!info) {
        racs_info_destroy(info);
        return RACS_STREAM_ALLOC_ERROR;
    }

    racs_info_flush(info, path);
    racs_info_destroy(info);

    return RACS_STREAM_OK;
}

int racs_streams_open(racs_streams *streams, const char *stream_id) {
    racs_uint64 hash = racs_info_hash(stream_id);

    racs_stream *stream = racs_streams_get_stream(streams, hash);
    if (stream) {
        return RACS_STREAM_CONFLICT;
    }

    char path[PATH_MAX];
    racs_info_path(path, stream_id);

    stream = racs_stream_open(path);
    if (!stream) {
        return RACS_STREAM_NOT_FOUND;
    }

    racs_streams_put_stream(streams, hash, stream);
    return RACS_STREAM_OK;
}

int racs_streams_append(racs_streams *streams,
                        const char *stream_id,
                        const char *mime_type,
                        const racs_uint8 *src,
                        racs_uint32 src_size) {
    racs_uint64 hash = racs_info_hash(stream_id);

    racs_stream *stream = racs_streams_get_stream(streams, hash);
    if (!stream) {
        return RACS_STREAM_NOT_FOUND;
    }

    racs_uint8 *decoded_src = NULL;
    racs_uint32 decoded_src_size = 0;

    int rc = racs_stream_decode(mime_type, src, src_size, &decoded_src, &decoded_src_size);
    if (rc != RACS_STREAM_OK) {
        return RACS_STREAM_DECODE_ERROR;
    }

    racs_stream_chunk(stream, decoded_src, decoded_src_size, hash);
    free(decoded_src);

    return RACS_STREAM_OK;
}

int racs_streams_close(racs_streams *streams, const char *stream_id) {
    racs_uint64 hash = racs_info_hash(stream_id);

    racs_stream *stream = racs_streams_get_stream(streams, hash);
    if (!stream) {
        return RACS_STREAM_NOT_FOUND;
    }

    if (stream->size > 0) {
        racs_uint32 padded_src_size = stream->capacity - stream->size;
        racs_uint8 *padded_src = malloc(padded_src_size);
        if (!padded_src) {
            return RACS_STREAM_ALLOC_ERROR;
        }

        memset(padded_src, 0, padded_src_size);
        racs_stream_chunk(stream, padded_src, padded_src_size, hash);
        free(padded_src);
    }

    return RACS_STREAM_OK;
}

racs_stream *racs_streams_get_stream(racs_streams *streams, racs_uint64 hash) {
    pthread_mutex_lock(&streams->mutex);

    racs_stream *stream = racs_dict_get(streams->dict, &hash);
    if (!stream) {
        pthread_mutex_unlock(&streams->mutex);
        return NULL;
    }

    pthread_mutex_unlock(&streams->mutex);
    return stream;
}

void racs_streams_put_stream(racs_streams *streams, racs_uint64 hash, racs_stream *stream) {
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
    if (!streams_) {
        return;
    }

    if (streams_->dict) {
        racs_dict_destroy(streams_->dict);
    }

    pthread_mutex_destroy(&streams_->mutex);
    free(streams_);
}

void racs_stream_chunk(racs_stream *stream,
                       const racs_uint8 *src,
                       racs_uint32 src_size,
                       racs_uint64 hash) {
    pthread_mutex_lock(&stream->mutex);

    racs_uint32 bytes_read = 0;
    while (bytes_read < src_size) {
        racs_uint32 b_bytes = stream->capacity - stream->size;
        racs_uint32 r_bytes = src_size - bytes_read;
        racs_uint32 w_bytes = (r_bytes > b_bytes) ? b_bytes : r_bytes;

        memcpy(stream->buf + stream->size, src + bytes_read, w_bytes);

        bytes_read += w_bytes;
        stream->size += w_bytes;

        if (stream->size >= stream->capacity) {
            racs_offsets *offsets = racs_offsets_get();
            racs_uint64 offset = racs_offsets_get_offset(offsets, hash);

            racs_time time = racs_info_to_time(stream->info, offset);
            racs_uint64 key[3] = {hash, time, 0};

            void *compressed_block = NULL;
            size_t compressed_size = 0;

            int rc = racs_zstd_compress(src + bytes_read, stream->size, &compressed_block, &compressed_size, 3);
            if (rc != RACS_ZSTD_OK) {
                continue;
            }

            racs_uint32 checksum = crc32c(0, stream->buf, stream->size);

            racs_mmt *mmt = racs_mmt_get();
            racs_mmt_put(mmt, key, compressed_block, compressed_size, checksum, 0);

            free(compressed_block);

            offset += stream->size;
            racs_offsets_put_offset(offsets, hash, offset);

            stream->size = 0;
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
        racs_info_munmap(info);
        return NULL;
    }

    racs_uint32 samples_per_block = racs_config_get()->memtable.samples_per_block;
    racs_uint32 capacity = samples_per_block * info->channels * (info->bit_depth / 8);

    stream->size = 0;
    stream->info = info;
    stream->capacity = capacity;

    stream->buf = malloc(capacity);
    if (!stream->buf) {
        racs_info_munmap(info);
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
        racs_info_munmap(stream->info);
    }

    pthread_mutex_destroy(&stream->mutex);
    free(stream);
}

int racs_stream_decode(const char *mime_type,
                       const racs_uint8 *src,
                       racs_uint32 src_size,
                       racs_uint8 **out,
                       racs_uint32 *out_size) {
    if (strcasecmp(racs_trim(mime_type), "audio/pcm") == 0) {
        return racs_stream_decode_pcm(src, src_size, out, out_size);
    }

    return RACS_STREAM_DECODE_ERROR;
}

int racs_stream_decode_pcm(const racs_uint8 *src,
                           racs_uint32 src_size,
                           racs_uint8 **out,
                           racs_uint32 *out_size) {
    *out = NULL;
    *out_size = 0;

    racs_uint8 *buf = malloc(src_size);
    if (!buf) {
        return RACS_STREAM_DECODE_ERROR;
    }

    memcpy(buf, src, src_size);

    *out = buf;
    *out_size = src_size;

    return RACS_STREAM_OK;
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
