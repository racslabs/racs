#include "extract.h"

static auxts_pcm_buffer* pcm_buffer_create(uint32_t channels, uint32_t sample_rate, uint32_t bits_per_sample);
static void pcm_buffer_append(auxts_pcm_buffer* pbuf, auxts_pcm_block* block);
static void pcm_block_destroy(auxts_pcm_block* block);
static uint64_t time_partitioned_path_to_timestamp(const char* path);
static char* resolve_shared_path(const char* path1, const char* path2);
static char* get_path_from_time_range(uint64_t begin_timestamp, uint64_t end_timestamp);
static uint8_t* get_data_from_cache_or_sstable(auxts_cache* cache, uint64_t stream_id, uint64_t timestamp, const char* file_path);
static auxts_flac_blocks* extract_flac_blocks(auxts_cache* cache, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp);
static void process_sstable_data(auxts_flac_blocks* blocks, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp, uint8_t* data);
static uint8_t* serialize_pcm_data(const auxts_pcm_buffer* pbuf);
static uint64_t next_power_of_two(uint64_t n);

auxts_pcm_buffer* extract_pcm_data(auxts_cache* cache, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp) {
    if (!cache) {
        return NULL;
    }

    auxts_flac_blocks* blocks = extract_flac_blocks(cache, stream_id, begin_timestamp, end_timestamp);
    if (!blocks->num_blocks) {
        auxts_flac_blocks_destroy(blocks);
        return NULL;
    }

    auxts_flac_block* flac_block = blocks->blocks[0];
    auxts_pcm_block* pcm_block = auxts_decode_flac_block(flac_block);
    auxts_pcm_buffer* pbuf = pcm_buffer_create(pcm_block->channels, pcm_block->sample_rate, pcm_block->bits_per_sample);

    pcm_buffer_append(pbuf, pcm_block);
    pcm_block_destroy(pcm_block);

    for (int i = 1; i < blocks->num_blocks; ++i) {
        flac_block = blocks->blocks[i];
        pcm_block = auxts_decode_flac_block(flac_block);

        pcm_buffer_append(pbuf, pcm_block);
        pcm_block_destroy(pcm_block);
    }

    auxts_flac_blocks_destroy(blocks);

    return pbuf;
}

void pcm_buffer_destroy(auxts_pcm_buffer* pbuf) {
    if (!pbuf) {
        return;
    }

    for (int channel = 0; channel < pbuf->channels; ++channel) {
        free(pbuf->data[channel]);
    }

    free(pbuf->data);
    free(pbuf);
}

char* resolve_shared_path(const char* path1, const char* path2) {
    if (!path1 || !path2) {
        perror("Paths cannot be null");
        return NULL;
    }

    size_t len1 = strlen(path1);
    size_t len2 = strlen(path2);
    size_t len = len1 < len2 ? len1 : len2;

    size_t i = 0;
    for ( ; i < len; i++) {
        if (path1[i] != path2[i]) {
            break;
        }
    }

    while (i > 1 && path1[i - 1] != '/') --i;
    --i;

    char* shared_path = malloc(i + 1);
    strncpy(shared_path, path1, i);
    shared_path[i] = '\0';

    return shared_path;
}

uint64_t time_partitioned_path_to_timestamp(const char* path) {
    struct tm info = {0};
    struct timespec ts;
    long milliseconds = 0;

    int ret = sscanf(path, ".data/%4d/%2d/%2d/%2d/%2d/%2d/%3ld.df",
                     &info.tm_year, &info.tm_mon, &info.tm_mday,
                     &info.tm_hour, &info.tm_min, &info.tm_sec, &milliseconds);

    if (ret != 7) {
        fprintf(stderr, "Invalid format: expected 7 values, got %d\n", ret);
        return -1;
    }

    if (milliseconds < 0 || milliseconds > 999) {
        fprintf(stderr, "Invalid milliseconds value: %ld\n", milliseconds);
        return -1;
    }

    info.tm_year -= 1900;
    info.tm_mon -= 1;

    time_t t = timegm(&info);
    if (t == -1) {
        perror("timegm failed");
        return -1;
    }

    ts.tv_sec = t;
    ts.tv_nsec = milliseconds * AUXTS_NANOSECONDS_PER_MILLISECOND;

    return auxts_ts_to_milliseconds(&ts);
}

char* get_path_from_time_range(uint64_t begin_timestamp, uint64_t end_timestamp) {
    char path1[255], path2[255];

    auxts_get_time_partitioned_path(begin_timestamp, path1);
    auxts_get_time_partitioned_path(end_timestamp, path2);

    return resolve_shared_path(path1, path2);
}

uint8_t* get_data_from_cache_or_sstable(auxts_cache* cache, uint64_t stream_id, uint64_t timestamp, const char* file_path) {
    uint64_t key[2] = {stream_id, timestamp};
    uint8_t* data = auxts_cache_get(cache, key);

    if (!data) {
        auxts_sstable* sstable = auxts_sstable_read_index_entries(file_path);
        if (!sstable) {
            return NULL;
        }

        data = sstable->data;
        auxts_sstable_destroy_except_data(sstable);

        auxts_cache_put(cache, key, data);
    }

    return data;
}

void process_sstable_data(auxts_flac_blocks* blocks, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp, uint8_t* data) {
    size_t size;
    memcpy(&size, data, sizeof(size_t));
    size = auxts_swap64_if_big_endian(size);

    auxts_sstable* sstable = auxts_sstable_read_index_entries_in_memory(data, size);
    if (!sstable) {
        return;
    }

    for (int j = 0; j < sstable->num_entries; ++j) {
        size_t offset = sstable->index_entries[j].offset;

        auxts_memtable_entry* entry = auxts_memtable_read_entry(data, offset);
        if (!entry) {
            return;
        }

        uint64_t timestamp = entry->key[1];

        if (entry->key[0] == stream_id && timestamp >= begin_timestamp && timestamp <= end_timestamp) {
            auxts_flac_blocks_append(blocks, entry->block, entry->block_size);
        } else {
            free(entry->block);
        }

        free(entry);
    }

    auxts_sstable_destroy_except_data(sstable);
}

auxts_flac_blocks* extract_flac_blocks(auxts_cache* cache, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp) {
    char* path = get_path_from_time_range(begin_timestamp, end_timestamp);
    auxts_filelist* list = get_sorted_filelist(path);

    auxts_flac_blocks* blocks = auxts_flac_blocks_create();
    if (!blocks) {
        free(path);
        auxts_filelist_destroy(list);
        return NULL;
    }

    for (int i = 0; i < list->num_files; ++i) {
        char* file_path = list->files[i];

        uint64_t timestamp = time_partitioned_path_to_timestamp(file_path);
        if (timestamp >= begin_timestamp && timestamp <= end_timestamp) {
            uint8_t* buffer = get_data_from_cache_or_sstable(cache, stream_id, timestamp, file_path);
            process_sstable_data(blocks, stream_id, begin_timestamp, end_timestamp, buffer);
        }
    }

    free(path);
    auxts_filelist_destroy(list);

    return blocks;
}

auxts_pcm_buffer* pcm_buffer_create(uint32_t channels, uint32_t sample_rate, uint32_t bits_per_sample) {
    auxts_pcm_buffer* pbuf = malloc(sizeof(auxts_pcm_buffer));
    if (!pbuf) {
        perror("Failed to allocate auxts_pcm_buffer");
        return NULL;
    }

    pbuf->num_samples = 0;
    pbuf->channels = channels;
    pbuf->sample_rate = sample_rate;
    pbuf->bits_per_sample = bits_per_sample;
    pbuf->max_num_samples = AUXTS_INITIAL_PCM_BUFFER_CAPACITY;

    pbuf->data = malloc(channels * sizeof(int32_t*));
    if (!pbuf->data) {
        perror("Failed to allocate data to auxts_pcm_buffer");
        free(pbuf);
        return NULL;
    }

    for (int channel = 0; channel < channels; ++channel) {
        pbuf->data[channel] = malloc(AUXTS_INITIAL_PCM_BUFFER_CAPACITY * sizeof(int32_t));
    }

    return pbuf;
}

void pcm_block_destroy(auxts_pcm_block* block) {
    for (int channel = 0; channel < block->channels; ++channel) {
        free(block->data[channel]);
    }

    free(block->data);
    free(block);
}

void pcm_buffer_append(auxts_pcm_buffer* pbuf, auxts_pcm_block* block) {
    if (!pbuf || !block) {
        return;
    }

    size_t num_samples = block->total_samples + pbuf->num_samples;

    if (num_samples > pbuf->max_num_samples) {
        pbuf->max_num_samples = next_power_of_two(num_samples);

        for (int channel = 0; channel < pbuf->channels; ++channel) {
            int32_t* audio_data = realloc(pbuf->data[channel], pbuf->max_num_samples * sizeof(int32_t));
            if (!audio_data) {
                return;
            }

            pbuf->data[channel] = audio_data;
        }
    }

    for (int channel = 0; channel < pbuf->channels; ++channel) {
        memcpy(pbuf->data[channel] + pbuf->num_samples, block->data[channel], block->total_samples * sizeof(int32_t));
    }

    pbuf->num_samples = num_samples;
}

uint8_t* serialize_pcm_data(const auxts_pcm_buffer* pbuf) {
    uint8_t* samples = malloc(pbuf->num_samples * pbuf->channels * sizeof(int32_t));
    if (!samples) {
        perror("Failed to allocate samples");
        return NULL;
    }

    for (int channel = 0; channel < pbuf->channels; ++channel) {
        #ifdef __ARM_NEON__
            auxts_bulk_swap32_if_big_endian(pbuf->data[channel], pbuf->num_samples);
        #endif
        memcpy(samples + (channel * pbuf->num_samples), pbuf->data[channel], pbuf->num_samples * sizeof(int32_t));
    }

    return samples;
}

auxts_result* serialize_pcm_buffer(const auxts_pcm_buffer* pbuf) {
    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    msgpack_pack_map(&pk, 5);

    msgpack_pack_str(&pk, 6);
    msgpack_pack_str_body(&pk, "status", 6);

    msgpack_pack_str(&pk, 2);
    msgpack_pack_str_body(&pk, "OK", 2);

    msgpack_pack_str(&pk, 8);
    msgpack_pack_str_body(&pk, "channels", 8);
    msgpack_pack_uint32(&pk, pbuf->channels);

    msgpack_pack_str(&pk, 11);
    msgpack_pack_str_body(&pk, "sample_rate", 11);
    msgpack_pack_uint32(&pk, pbuf->sample_rate);

    msgpack_pack_str(&pk, 15);
    msgpack_pack_str_body(&pk, "bits_per_sample", 15);
    msgpack_pack_uint32(&pk, pbuf->bits_per_sample);

    msgpack_pack_str(&pk, 15);
    msgpack_pack_str_body(&pk, "num_samples", 15);
    msgpack_pack_uint32(&pk, pbuf->bits_per_sample);

    msgpack_pack_str(&pk, 8);
    msgpack_pack_str_body(&pk, "pcm_data", 8);

    uint8_t* data = serialize_pcm_data(pbuf);
    if (!data) {
        msgpack_pack_nil(&pk);
    } else {
        size_t size = pbuf->num_samples * pbuf->channels * sizeof(int32_t);
        msgpack_pack_bin(&pk, size);
        msgpack_pack_bin_body(&pk, pbuf->data, size);
    }

    auxts_result* result = malloc(sizeof(auxts_result));
    if (!result) {
        perror("Failed to allocate auxts_result");
        msgpack_sbuffer_destroy(&sbuf);
        msgpack_packer_free(&pk);
        return NULL;
    }

    result->data = malloc(sbuf.size);
    if (!result->data) {
        perror("Failed to allocate data to auxts_result");
        msgpack_sbuffer_destroy(&sbuf);
        msgpack_packer_free(&pk);
        free(result);
        return NULL;
    }

    memcpy(result->data, sbuf.data, sbuf.size);
    result->size = sbuf.size;

    return result;
}

uint64_t next_power_of_two(uint64_t n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;

    return n;
}
