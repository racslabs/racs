
#include "stream.h"


const char *const racs_stream_result_string[] = {
    [RACS_STREAM_OK]                  = "ok",
    [RACS_STREAM_NOT_FOUND]           = "not found",
    [RACS_STREAM_CONFLICT]            = "conflict",
    [RACS_STREAM_DECODE_ERROR]        = "decode error",
    [RACS_STREAM_INTERNAL_ERROR]      = "internal error",
    [RACS_STREAM_UNKNOWN_CODEC]       = "unknown codec",
    [RACS_STREAM_INVALID_BITDEPTH]    = "invalid bit depth",
    [RACS_STREAM_INVALID_SAMPLE_RATE] = "invalid sample rate",
    [RACS_STREAM_INVALID_CHANNELS]    = "invalid channels",
    [RACS_STREAM_INGESTION_ERROR]     = "ingestion error"
};


ssize_t racs_stream_chunk(const racs_info *info, 
                          const racs_uint8 *src, 
                          size_t src_size,
                          racs_uint64 hash);

int racs_stream_decode(const char *s_codec,
                       const racs_info *info,
                       const racs_uint8 *src,
                       size_t src_size,
                       racs_uint8 **out,
                       size_t *out_size);


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
        return RACS_STREAM_INTERNAL_ERROR;
    }

    racs_info_flush(info, path);
    racs_info_destroy(info);

    return RACS_STREAM_OK;
}

int racs_stream(const char *stream_id, 
                const char *s_codec, 
                const racs_uint8 *src,
                size_t src_size) {

    char path[PATH_MAX];
    racs_info_path(path, stream_id);

    racs_info *info = racs_info_open(path);
    if (!info) {
        return RACS_STREAM_NOT_FOUND;
    }
    
    size_t out_size;
    racs_uint8 *out = NULL;

    int status = racs_stream_decode(s_codec, info, src, src_size, &out, &out_size);
    if (status != RACS_STREAM_OK) {
        racs_info_munmap(info);
        return status;
    }

    racs_uint64 hash = racs_info_hash(stream_id);
    ssize_t n = racs_stream_chunk(info, out, out_size, hash);
    if (n < 0) {
        racs_info_munmap(info);
        free(out);

        return RACS_STREAM_INGESTION_ERROR;
    }

    racs_info_munmap(info);
    free(out);

    return status;
}

ssize_t racs_stream_chunk(const racs_info *info, 
                          const racs_uint8 *src, 
                          size_t src_size,
                          racs_uint64 hash) {             
    if (!info || !src) {
        return -1; 
    }

    if (src_size == 0) {
        return 0;  
    }

    racs_config *cfg = racs_config_get();
    racs_uint32 samples_per_block = cfg->memtable.samples_per_block;
    size_t bytes_per_sample = info->bit_depth / 8;
    size_t capacity = samples_per_block * info->channels * bytes_per_sample;

    if (capacity == 0) {
        return -1; 
    }

    racs_offsets *offsets = racs_offsets_get();
    racs_mt_list *list = racs_mt_list_get();

    size_t bytes_read = 0;

    while (bytes_read < src_size) {
        size_t remaining = src_size - bytes_read;
        const racs_uint8 *src_ptr = src + bytes_read;
        racs_uint8 *padded_block = NULL;

        if (remaining < capacity) {
            padded_block = (racs_uint8 *)calloc(1, capacity);
            if (!padded_block) {
                return (bytes_read > 0) ? (ssize_t)bytes_read : -1;
            }

            memcpy(padded_block, src_ptr, remaining);
            src_ptr = padded_block;
        }

        racs_uint64 offset = racs_offsets_get_offset(offsets, hash);
        racs_time time = racs_info_to_time(info, offset); 
        racs_uint64 key[3] = { hash, time, 0 };
        racs_uint32 checksum = crc32c(0, src_ptr, capacity); 

        int rc = racs_mt_list_put(list, key, src_ptr, capacity, checksum, 0);

        if (padded_block) {
            free(padded_block);
        }

        if (rc < 0) {
            return (bytes_read > 0) ? (ssize_t)bytes_read : -1; 
        }

        bytes_read += (remaining < capacity) ? remaining : capacity;
        racs_offsets_put_offset(offsets, hash, offset + capacity);
    }

    return (ssize_t)bytes_read;
}

int racs_stream_decode(const char *s_codec,
                       const racs_info *info,
                       const racs_uint8 *src,
                       size_t src_size,
                       racs_uint8 **out,
                       size_t *out_size) {
    if (!s_codec || !info || !src || !out || !out_size) {
        return RACS_STREAM_DECODE_ERROR;
    }     
    
    if (src_size == 0) {
        *out = NULL;
        *out_size = 0;
        return RACS_STREAM_OK;
    }
                        
    int codec = racs_codec_from_string(s_codec);
    if (codec == RACS_CODEC_UNKNOWN) {
        return RACS_STREAM_UNKNOWN_CODEC;
    }

    if (codec == RACS_CODEC_OPUS) {
        if (info->sample_rate != RACS_OPUS_DEFAULT_SAMPLE_RATE) {
            return RACS_STREAM_INVALID_SAMPLE_RATE;
        }
    }

    if (codec == RACS_CODEC_MP3 || codec == RACS_CODEC_AAC || codec == RACS_CODEC_OPUS) {
        if (info->bit_depth != 16) {
            return RACS_STREAM_INVALID_BITDEPTH;
        }
    }

    racs_codec_format fmt;
    int status = racs_codec_decode(codec, &fmt, src, src_size, out, out_size);

    switch (status) {
        case RACS_CODEC_OK:
            break;
        case RACS_CODEC_UNSUPPORTED_CHANNELS:
            return RACS_STREAM_INVALID_CHANNELS;
        case RACS_CODEC_UNSUPPORTED_SAMPLE_RATE:
            return RACS_STREAM_INVALID_SAMPLE_RATE;
        case RACS_CODEC_UNSUPPORTED_BITDEPTH:
            return RACS_STREAM_INVALID_BITDEPTH;
        default:
            return RACS_STREAM_DECODE_ERROR;
    }

    if (fmt.channels != info->channels) {
        free(*out);
        *out = NULL;
        return RACS_STREAM_INVALID_CHANNELS;
    }

    if (fmt.bit_depth != info->bit_depth) {
        free(*out);
        *out = NULL;
        return RACS_STREAM_INVALID_BITDEPTH;
    }

    if (fmt.sample_rate != info->sample_rate) {
        free(*out);
        *out = NULL;
        return RACS_STREAM_INVALID_SAMPLE_RATE;
    }

    return RACS_STREAM_OK;
}