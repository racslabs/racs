
#include "stream.h"


static racs_streams *streams = NULL;

static racs_uint64 racs_streams_hash_cb(const void *key);

static int racs_streams_eq_cb(const void *a, const void *b);

static void racs_streams_destroy_cb(void *key, void *value);


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

int racs_streams_open(racs_streams *streams, const char *name, size_t size) {
    racs_uint64 *hash = malloc(2 * sizeof(racs_uint64));
    racs_mmh3_x64_128(name, size, 0, hash);

    pthread_mutex_lock(&streams->mutex);
    racs_stream *stream = racs_dict_get(streams->dict, hash);
    if (stream) {
        free(hash);
        pthread_mutex_unlock(&streams->mutex);
        return RACS_STREAM_CONFLICT;
    }
    pthread_mutex_unlock(&streams->mutex);

    char path[PATH_MAX];
    sprintf(path, "%s/.racs/md/", racs_config_get()->data_dir);
    strncat(path, name, size);

    stream = racs_stream_create(path);
    if (!stream) {
        free(hash);
        return RACS_STREAM_ALLOC_ERR;
    }

    pthread_mutex_lock(&streams->mutex);
    racs_dict_put(streams->dict, hash, stream);
    pthread_mutex_unlock(&streams->mutex);

    return RACS_STREAM_OK;
}

int racs_streams_put(racs_streams *streams,
                     const char *name,
                     size_t size
                     const char *mime_type,
                     const racs_uint8 *src,
                     racs_uint32 src_size) {

    pthread_mutex_lock(&streams->mutex);
    racs_stream *stream = racs_dict_get(streams->dict, &key[0]);
    if (!stream) {
        pthread_mutex_unlock(&streams->mutex);
        return RACS_STREAM_NOT_FOUND;
    }
    pthread_mutex_unlock(&streams->mutex);

    racs_uint32 decoded_size;
    racs_uint8 *decoded_data = racs_stream_decode(mime_type, src, src_size, &decoded_size);
    if (!decoded_data) {
        return RACS_STREAM_DECODE_ERR;
    }

    pthread_mutex_lock(&streams->mutex);

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
            size_t compressed_size;
            racs_uint8 *compressed_block = racs_zstd_compress(ptr, stream->size, &compressed_size, 3);

            racs_uint32 checksum = crc32c(0, stream->buf, stream->size);
            RACS_MMT_PUT(key, compressed_block, compressed_size, checksum, 0);

            free(compressed_block);
        }
    }

    pthread_mutex_unlock(&streams->mutex);
    free(decoded_data);

    return RACS_STREAM_OK;
}

racs_stream *racs_stream_create(const char *path) {
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

    munmap(info, sizeof(racs_info));
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
