#include "extract0.h"

int auxts_extract_pcm(auxts_cache* cache, auxts_pcm* pcm, const char* stream_id, auxts_time from, auxts_time to) {
    char* path = auxts_time_range_to_path(from, to);
    auxts_filelist* list = get_sorted_filelist(path);

    auxts_metadata metadata;
    auxts_metadata_get(&metadata, stream_id);

    if (metadata.bit_depth == AUXTS_PCM_BIT_DEPTH_16) {
        auxts_pcm_init_s16(pcm, metadata.channels, metadata.channels);
    }

    if (metadata.bit_depth == AUXTS_PCM_BIT_DEPTH_24) {
        auxts_pcm_init_s32(pcm, metadata.channels, metadata.sample_rate);
    }

    auxts_uint64 hash = auxts_hash_stream_id(stream_id);

    for (int i = 0; i < list->num_files; ++i) {
        char* file_path = list->files[i];

        auxts_time time = auxts_time_from_path(file_path);
        if (time >= from && time <= to) {
            auxts_uint8* data = auxts_extract_data_from_cache_or_sstable(cache, hash, time, file_path);
            auxts_extract_process_sstable_data(pcm, data, hash, from, to);
        }
    }

    free(path);
    auxts_filelist_destroy(list);

    return AUXTS_EXTRACT_STATUS_OK;
}

uint8_t* auxts_extract_data_from_cache_or_sstable(auxts_cache* cache, uint64_t stream_id, auxts_time time, const char* path) {
    uint64_t key[2] = {stream_id, time};
    uint8_t* data = auxts_cache_get(cache, key);

    if (!data) {
        auxts_sstable* sstable = auxts_sstable_read(path);
        if (!sstable) {
            return NULL;
        }

        data = sstable->data;
        auxts_sstable_destroy_except_data(sstable);
        auxts_cache_put(cache, key, data);
    }

    return data;
}

void auxts_extract_process_sstable_data(auxts_pcm* pcm, uint8_t* data, uint64_t stream_id, int64_t from, int64_t to) {
    size_t size;
    memcpy(&size, data, sizeof(size_t));

    auxts_sstable* sst = auxts_sstable_read_in_memory(data, size);
    if (!sst) return;

    for (int i = 0; i < sst->num_entries; ++i) {
        size_t offset = sst->index_entries[i].offset;

        auxts_memtable_entry* entry = auxts_memtable_entry_read(data, offset);
        if (!entry) return;

        auxts_time time = (auxts_time)entry->key[1];
        if (entry->key[0] == stream_id && time >= from && time <= to) {

            auxts_flac flac;
            auxts_flac_read_pcm(&flac, entry->block, entry->block_size);

            if (pcm->bit_depth == AUXTS_PCM_BIT_DEPTH_16) {
                auxts_pcm_write_s16i(pcm, (auxts_int16*)flac.out_stream.data, flac.total_samples);
                continue;
            }

            if (pcm->bit_depth == AUXTS_PCM_BIT_DEPTH_24) {
                auxts_pcm_write_s32i(pcm, (auxts_int32*)flac.out_stream.data, flac.total_samples);
                continue;
            }
        } else {
            free(entry->block);
        }

        free(entry);
    }

    auxts_sstable_destroy_except_data(sst);
}