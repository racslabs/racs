#include "extract0.h"

int auxts_extract_pcm(auxts_cache* cache, auxts_pcm* pcm, auxts_uint64 stream_id, auxts_time from, auxts_time to) {
    char* path = auxts_time_range_to_path(from, to);
    auxts_filelist* list = get_sorted_filelist(path);


}

void auxts_extract_process_sstable_data(auxts_pcm* pcm, uint8_t* data, uint64_t stream_id, int64_t from, int64_t to) {
    size_t size;
    memcpy(&size, data, sizeof(size_t));

    auxts_sstable* sst = auxts_sstable_read_in_memory(data, size);
    if (!sst) return;

    for (int j = 0; j < sst->num_entries; ++j) {
        size_t offset = sst->index_entries[j].offset;

        auxts_memtable_entry* entry = auxts_memtable_entry_read(data, offset);
        if (!entry) return;

        auxts_time time = (auxts_time)entry->key[1];
        if (entry->key[0] == stream_id && time >= from && time <= to) {

            auxts_flac flac;
            auxts_flac_open(&flac, entry->block, entry->block_size);

            void* out = malloc(entry->block_size);
            auxts_flac_read_pcm(&flac, out);

            if (pcm->bit_depth == AUXTS_PCM_BIT_DEPTH_16) {
                auxts_pcm_write_s16i(pcm, out, flac.total_samples);
                free(out);
                continue;
            }

            if (pcm->bit_depth == AUXTS_PCM_BIT_DEPTH_24) {
                auxts_pcm_write_s32i(pcm, out, flac.total_samples);
                free(out);
                continue;
            }
        } else {
            free(entry->block);
        }

        free(entry);
    }

    auxts_sstable_destroy_except_data(sst);
}