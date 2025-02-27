#include "extract.h"

static auxts_pcm_buffer* pcm_buffer_create(uint32_t channels, uint32_t sample_rate, uint32_t bits_per_sample);
static void pcm_buffer_append(auxts_pcm_buffer* pbuf, auxts_pcm_block* block);
static void pcm_block_destroy(auxts_pcm_block* block);
static uint8_t* get_data_from_cache_or_sstable(auxts_cache* cache, uint64_t stream_id, uint64_t timestamp, const char* file_path);
static auxts_flac_blocks* extract_flac_blocks(auxts_cache* cache, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp);
static void process_sstable_data(auxts_flac_blocks* blocks, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp, uint8_t* data);
static uint8_t* pack_pcm_data(const auxts_pcm_buffer* pbuf);
static void pcm_buffer_serialize(msgpack_packer* pk, const auxts_pcm_buffer* pbuf);
static auxts_pcm_buffer* extract_pcm_data(auxts_cache* cache, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp);
static void pcm_buffer_destroy(auxts_pcm_buffer* pbuf);

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
    auxts_pcm_buffer* pbuf = pcm_buffer_create(pcm_block->info.channels, pcm_block->info.sample_rate, pcm_block->info.bits_per_sample);

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

    for (int channel = 0; channel < pbuf->info.channels; ++channel) {
        free(pbuf->data[channel]);
    }

    free(pbuf->data);
    free(pbuf);
}

uint8_t* get_data_from_cache_or_sstable(auxts_cache* cache, uint64_t stream_id, uint64_t timestamp, const char* file_path) {
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

void process_sstable_data(auxts_flac_blocks* blocks, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp, uint8_t* data) {
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
    char* path = auxts_get_path_from_timestamp_range(begin_timestamp, end_timestamp);
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

    pbuf->info.num_samples = 0;
    pbuf->info.channels = channels;
    pbuf->info.sample_rate = sample_rate;
    pbuf->info.bits_per_sample = bits_per_sample;
    pbuf->info.max_num_samples = AUXTS_INITIAL_PCM_BUFFER_CAPACITY;

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

void pcm_buffer_serialize(msgpack_packer* pk, const auxts_pcm_buffer* pbuf) {
    msgpack_pack_array(pk, 10);

    msgpack_pack_str(pk, 6);
    msgpack_pack_str_body(pk, "status", 6);

    msgpack_pack_str(pk, 2);
    msgpack_pack_str_body(pk, "OK", 2);

    msgpack_pack_str(pk, 11);
    msgpack_pack_str_body(pk, "num_samples", 11);
    msgpack_pack_uint64(pk, pbuf->info.num_samples);

    msgpack_pack_str(pk, 8);
    msgpack_pack_str_body(pk, "channels", 8);
    msgpack_pack_uint32(pk, pbuf->info.channels);

    msgpack_pack_str(pk, 11);
    msgpack_pack_str_body(pk, "sample_rate", 11);
    msgpack_pack_uint32(pk, pbuf->info.num_samples);

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

auxts_result auxts_extract(auxts_cache* cache, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp) {
    msgpack_sbuffer sbuf;
    msgpack_packer pk;

    msgpack_sbuffer_init(&sbuf);
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    auxts_pcm_buffer* pbuf = extract_pcm_data(cache, stream_id, begin_timestamp, end_timestamp);
    pcm_buffer_serialize(&pk, pbuf);

    auxts_result result;
    auxts_result_init(&result, sbuf.size);
    memcpy(result.data, sbuf.data, sbuf.size);
    printf("Packed data size: %zu bytes\n", sbuf.size);

    msgpack_sbuffer_destroy(&sbuf);

    return result;
}
