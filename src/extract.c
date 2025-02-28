#include "extract.h"

static void pcm_buffer_init(auxts_pcm_buffer* pbuf, uint32_t channels, uint32_t sample_rate, uint32_t bits_per_sample);
static void pcm_buffer_append(auxts_pcm_buffer* pbuf, auxts_pcm_block* block);
static void pcm_block_destroy(auxts_pcm_block* block);
static uint8_t* get_data_from_cache_or_sstable(auxts_cache* cache, uint64_t stream_id, int64_t timestamp, const char* file_path);
static auxts_flac_blocks* extract_flac_blocks(auxts_cache* cache, uint64_t stream_id, int64_t from, int64_t to);
static void process_sstable_data(auxts_flac_blocks* blocks, uint64_t stream_id, int64_t from, int64_t to, uint8_t* data);
static uint8_t* pack_pcm_data(const auxts_pcm_buffer* pbuf);
static auxts_extract_pcm_status extract_pcm_data(auxts_cache* cache, auxts_pcm_buffer* pbuf, uint64_t stream_id, const char* from, const char* to);
static void pcm_buffer_destroy(auxts_pcm_buffer* pbuf);
static void serialize_status_ok(msgpack_packer* pk, const auxts_pcm_buffer* pbuf);
static void serialize_status_not_ok(msgpack_packer* pk, auxts_extract_pcm_status status);

const char* const auxts_extract_pcm_status_message[] = {
        "OK",
        "PCM data not found for the given parameters",
        "Invalid RFC3339 timestamp. Expecting format: YYYY-MM-DDTHH:MM:SS[.sss]Z"
};

const char* const auxts_extract_pcm_status_code[] = {
        "OK",
        "NO_DATA",
        "ERROR"
};

auxts_result auxts_extract(auxts_cache* cache, uint64_t stream_id, const char* from, const char* to) {
    auxts_pcm_buffer pbuf;
    msgpack_sbuffer sbuf;
    msgpack_packer pk;

    msgpack_sbuffer_init(&sbuf);
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    auxts_extract_pcm_status status = extract_pcm_data(cache, &pbuf, stream_id, from, to);
    if (status == AUXTS_EXTRACT_PCM_STATUS_OK) {
        serialize_status_ok(&pk, &pbuf);
        pcm_buffer_destroy(&pbuf);
    } else {
        serialize_status_not_ok(&pk, status);
    }

    auxts_result result;
    auxts_result_init(&result, sbuf.size);
    memcpy(result.data, sbuf.data, sbuf.size);

    msgpack_sbuffer_destroy(&sbuf);

    return result;
}

auxts_extract_pcm_status extract_pcm_data(auxts_cache* cache, auxts_pcm_buffer* pbuf, uint64_t stream_id, const char* from, const char* to) {
    int64_t t_from = auxts_parse_rfc3339(from);
    int64_t t_to = auxts_parse_rfc3339(to);
    if (t_from == -1 || t_to == -1) {
        return AUXTS_EXTRACT_PCM_STATUS_INVALID_TIMESTAMP;
    }

    auxts_flac_blocks* blocks = extract_flac_blocks(cache, stream_id, t_from, t_to);
    if (!blocks->num_blocks) {
        auxts_flac_blocks_destroy(blocks);
        return AUXTS_EXTRACT_PCM_STATUS_NO_DATA;
    }

    auxts_flac_block* flac_block = blocks->blocks[0];
    auxts_pcm_block* pcm_block = auxts_decode_flac_block(flac_block);

    pcm_buffer_init(pbuf, pcm_block->info.channels, pcm_block->info.sample_rate, pcm_block->info.bits_per_sample);
    pcm_buffer_append(pbuf, pcm_block);

    pcm_block_destroy(pcm_block);

    for (int i = 1; i < blocks->num_blocks; ++i) {
        flac_block = blocks->blocks[i];
        pcm_block = auxts_decode_flac_block(flac_block);

        pcm_buffer_append(pbuf, pcm_block);
        pcm_block_destroy(pcm_block);
    }

    auxts_flac_blocks_destroy(blocks);

    return AUXTS_EXTRACT_PCM_STATUS_OK;
}

void pcm_buffer_destroy(auxts_pcm_buffer* pbuf) {
    for (int channel = 0; channel < pbuf->info.channels; ++channel) {
        free(pbuf->data[channel]);
    }

    free(pbuf->data);
}

uint8_t* get_data_from_cache_or_sstable(auxts_cache* cache, uint64_t stream_id, int64_t timestamp, const char* file_path) {
    uint64_t key[2] = {stream_id, timestamp};
    uint8_t* data = auxts_cache_get(cache, key);

    if (!data) {
        auxts_sstable* sstable = auxts_sstable_read(file_path);
        if (!sstable) {
            return NULL;
        }

        data = sstable->data;
        auxts_sstable_destroy_except_data(sstable);
        auxts_cache_put(cache, key, data);
    }

    return data;
}

void process_sstable_data(auxts_flac_blocks* blocks, uint64_t stream_id, int64_t from, int64_t to, uint8_t* data) {
    size_t size;
    memcpy(&size, data, sizeof(size_t));
    size = auxts_swap64_if_big_endian(size);

    auxts_sstable* sstable = auxts_sstable_read_in_memory(data, size);
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
        if (entry->key[0] == stream_id && timestamp >= from && timestamp <= to) {
            auxts_flac_blocks_append(blocks, entry->block, entry->block_size);
        } else {
            free(entry->block);
        }

        free(entry);
    }

    auxts_sstable_destroy_except_data(sstable);
}

auxts_flac_blocks* extract_flac_blocks(auxts_cache* cache, uint64_t stream_id, int64_t from, int64_t to) {
    char* path = auxts_get_path_from_timestamp_range(from, to);
    auxts_filelist* list = get_sorted_filelist(path);

    auxts_flac_blocks* blocks = auxts_flac_blocks_create();
    if (!blocks) {
        free(path);
        auxts_filelist_destroy(list);
        return NULL;
    }

    for (int i = 0; i < list->num_files; ++i) {
        char* file_path = list->files[i];

        uint64_t timestamp = auxts_time_partitioned_path_to_timestamp(file_path);
        if (timestamp >= from && timestamp <= to) {
            uint8_t* buffer = get_data_from_cache_or_sstable(cache, stream_id, timestamp, file_path);
            process_sstable_data(blocks, stream_id, from, to, buffer);
        }
    }

    free(path);
    auxts_filelist_destroy(list);

    return blocks;
}

void pcm_buffer_init(auxts_pcm_buffer* pbuf, uint32_t channels, uint32_t sample_rate, uint32_t bits_per_sample) {
    pbuf->info.num_samples = 0;
    pbuf->info.channels = channels;
    pbuf->info.sample_rate = sample_rate;
    pbuf->info.bits_per_sample = bits_per_sample;
    pbuf->info.max_num_samples = AUXTS_INITIAL_PCM_BUFFER_CAPACITY;

    pbuf->data = malloc(channels * sizeof(int32_t*));
    if (!pbuf->data) {
        perror("Failed to allocate data to auxts_pcm_buffer");
    }

    for (int channel = 0; channel < channels; ++channel) {
        pbuf->data[channel] = malloc(AUXTS_INITIAL_PCM_BUFFER_CAPACITY * sizeof(int32_t));
    }
}

void pcm_block_destroy(auxts_pcm_block* block) {
    for (int channel = 0; channel < block->info.channels; ++channel) {
        free(block->data[channel]);
    }

    free(block->data);
    free(block);
}

void pcm_buffer_append(auxts_pcm_buffer* pbuf, auxts_pcm_block* block) {
    if (!pbuf || !block) {
        return;
    }

    size_t num_samples = block->info.total_samples + pbuf->info.num_samples;

    if (num_samples > pbuf->info.max_num_samples) {
        pbuf->info.max_num_samples = auxts_next_power_of_two(num_samples);

        for (int channel = 0; channel < pbuf->info.channels; ++channel) {
            int32_t* audio_data = realloc(pbuf->data[channel], pbuf->info.max_num_samples * sizeof(int32_t));
            if (!audio_data) {
                return;
            }

            pbuf->data[channel] = audio_data;
        }
    }

    for (int channel = 0; channel < pbuf->info.channels; ++channel) {
        memcpy(pbuf->data[channel] + pbuf->info.num_samples, block->data[channel], block->info.total_samples * sizeof(int32_t));
    }

    pbuf->info.num_samples = num_samples;
}

uint8_t* pack_pcm_data(const auxts_pcm_buffer* pbuf) {
    uint8_t* samples = malloc(pbuf->info.num_samples * pbuf->info.channels * sizeof(int32_t));
    if (!samples) {
        perror("Failed to allocate samples");
        return NULL;
    }

    for (int channel = 0; channel < pbuf->info.channels; ++channel) {
        #ifdef __ARM_NEON__
            auxts_bulk_swap32_if_big_endian(pbuf->data[channel], pbuf->info.num_samples);
        #endif
        memcpy(samples + (channel * pbuf->info.num_samples), pbuf->data[channel], pbuf->info.num_samples * sizeof(int32_t));
    }

    return samples;
}

void serialize_status_ok(msgpack_packer* pk, const auxts_pcm_buffer* pbuf) {
    msgpack_pack_array(pk, 12);

    msgpack_pack_str(pk, 6);
    msgpack_pack_str_body(pk, "status", 6);

    msgpack_pack_str(pk, 2);
    msgpack_pack_str_body(pk, "OK", 2);

    msgpack_pack_str(pk, 7);
    msgpack_pack_str_body(pk, "samples", 7);
    msgpack_pack_uint64(pk, pbuf->info.num_samples);

    msgpack_pack_str(pk, 8);
    msgpack_pack_str_body(pk, "channels", 8);
    msgpack_pack_uint32(pk, pbuf->info.channels);

    msgpack_pack_str(pk, 11);
    msgpack_pack_str_body(pk, "sample_rate", 11);
    msgpack_pack_uint32(pk, pbuf->info.sample_rate);

    msgpack_pack_str(pk, 15);
    msgpack_pack_str_body(pk, "bits_per_sample", 15);
    msgpack_pack_uint32(pk, pbuf->info.bits_per_sample);

    msgpack_pack_str(pk, 8);
    msgpack_pack_str_body(pk, "pcm_data", 8);

    uint8_t* data = pack_pcm_data(pbuf);
    size_t size = pbuf->info.num_samples * pbuf->info.channels * sizeof(int32_t);

    msgpack_pack_bin(pk, size);
    msgpack_pack_bin_body(pk, data, size);

    free(data);
}

void serialize_status_not_ok(msgpack_packer* pk, auxts_extract_pcm_status status) {
    msgpack_pack_array(pk, 4);

    msgpack_pack_str(pk, 6);
    msgpack_pack_str_body(pk, "status", 6);

    const char* status_code = auxts_extract_pcm_status_code[status];

    msgpack_pack_str(pk, strlen(status_code));
    msgpack_pack_str_body(pk, status_code, strlen(status_code));

    const char* message = auxts_extract_pcm_status_message[status];
    msgpack_pack_str(pk, 7);
    msgpack_pack_str_body(pk, "message", 7);

    msgpack_pack_str(pk, strlen(message));
    msgpack_pack_str_body(pk, status_code, strlen(message));
}
