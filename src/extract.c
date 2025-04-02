#include "extract.h"

static void pcm_buffer_append(auxts_pcm_buffer* pbuf, auxts_pcm_block* block);
static void pcm_block_destroy(auxts_pcm_block* block);
static uint8_t* get_data_from_cache_or_sstable(auxts_cache* cache, uint64_t stream_id, int64_t timestamp, const char* file_path);
static auxts_flac_blocks* extract_flac_blocks(auxts_cache* cache, uint64_t stream_id, int64_t from, int64_t to);
static void process_sstable_data(auxts_flac_blocks* blocks, uint64_t stream_id, int64_t from, int64_t to, uint8_t* data);

int auxts_extract_pcm_data(auxts_cache* cache, auxts_pcm_buffer* pbuf, uint64_t stream_id, int64_t from, int64_t to) {
    auxts_flac_blocks* blocks = extract_flac_blocks(cache, stream_id, from, to);
    if (blocks->num_blocks == 0) {
        auxts_flac_blocks_destroy(blocks);
        return AUXTS_EXTRACT_PCM_STATUS_NOT_FOUND;
    }

    auxts_flac_block* flac_block = blocks->blocks[0];
    auxts_pcm_block* pcm_block = auxts_decode_flac_block(flac_block);

    auxts_pcm_buffer_init(pbuf, pcm_block->info.channels, pcm_block->info.sample_rate, pcm_block->info.bit_depth);
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

void auxts_pcm_buffer_destroy(auxts_pcm_buffer* pbuf) {
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
    char* path = auxts_time_range_to_path(from, to);
    auxts_filelist* list = get_sorted_filelist(path);

    auxts_flac_blocks* blocks = auxts_flac_blocks_create();
    if (!blocks) {
        free(path);
        auxts_filelist_destroy(list);
        return NULL;
    }

    for (int i = 0; i < list->num_files; ++i) {
        char* file_path = list->files[i];

        int64_t timestamp = auxts_time_from_path(file_path);
        if (timestamp >= from && timestamp <= to) {
            uint8_t* buffer = get_data_from_cache_or_sstable(cache, stream_id, timestamp, file_path);
            process_sstable_data(blocks, stream_id, from, to, buffer);
        }
    }

    free(path);
    auxts_filelist_destroy(list);

    return blocks;
}

void auxts_pcm_buffer_init(auxts_pcm_buffer* pbuf, uint32_t channels, uint32_t sample_rate, uint32_t bit_depth) {
    pbuf->info.num_samples = 0;
    pbuf->info.channels = channels;
    pbuf->info.sample_rate = sample_rate;
    pbuf->info.bit_depth = bit_depth;
    pbuf->info.max_num_samples = 2;

    pbuf->data = malloc(channels * sizeof(int32_t*));
    if (!pbuf->data) {
        perror("Failed to allocate data to auxts_pcm_buffer");
    }

    for (int channel = 0; channel < channels; ++channel) {
        pbuf->data[channel] = malloc(2 * sizeof(int32_t));
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
        pbuf->info.max_num_samples *= num_samples;

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

int32_t* auxts_flatten_pcm_data(const auxts_pcm_buffer* pbuf) {
    int32_t* samples = malloc(pbuf->info.num_samples * pbuf->info.channels * sizeof(int32_t));
    if (!samples) {
        perror("Failed to allocate samples");
        return NULL;
    }

    for (int channel = 0; channel < pbuf->info.channels; ++channel) {
        memcpy(samples + (channel * pbuf->info.num_samples), pbuf->data[channel], pbuf->info.num_samples * sizeof(int32_t));
    }

    return samples;
}
