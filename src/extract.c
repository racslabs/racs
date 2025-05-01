#include "extract.h"

int rats_extract_pcm(rats_context *ctx, rats_pcm *pcm, const char *stream_id, rats_time from, rats_time to) {
    char *path = rats_time_range_to_path(from, to);
    rats_filelist *list = get_sorted_filelist(path);

    rats_streaminfo streaminfo;
    rats_uint64 hash = rats_hash(stream_id);

    int rc = rats_streaminfo_get(ctx->mcache, &streaminfo, hash);
    if (rc == -1) rats_streaminfo_put(ctx->mcache, &streaminfo, hash);
    if (rc == 0) return RATS_EXTRACT_STATUS_NOT_FOUND;

    rats_pcm_set_bit_depth(pcm, streaminfo.bit_depth);
    rats_pcm_set_channels(pcm, streaminfo.channels);
    rats_pcm_set_sample_rate(pcm, streaminfo.sample_rate);

    rats_pcm_init(pcm);

    for (int i = 0; i < list->num_files; ++i) {
        char *file_path = list->files[i];

        rats_time time = rats_time_from_path(file_path);
        if (time >= from && time <= to) {
            rats_uint8 *data = rats_extract_from_cache_or_sstable(ctx->scache, hash, time, file_path);
            rats_extract_process_sstable(pcm, data, hash, from, to);
        }
    }

    free(path);
    rats_filelist_destroy(list);

    return RATS_EXTRACT_STATUS_OK;
}

rats_uint8 *
rats_extract_from_cache_or_sstable(rats_cache *cache, rats_uint64 stream_id, rats_time time, const char *path) {
    rats_uint64 key[2] = {stream_id, time};
    rats_uint8 *data = rats_cache_get(cache, key);

    if (!data) {
        rats_sstable *sstable = rats_sstable_read(path);
        if (!sstable) return NULL;

        data = sstable->data;
        rats_sstable_destroy_except_data(sstable);
        rats_cache_put(cache, key, data);
    }

    return data;
}

void
rats_extract_process_sstable(rats_pcm *pcm, rats_uint8 *data, rats_uint64 stream_id, rats_int64 from, rats_int64 to) {
    size_t size;
    memcpy(&size, data, sizeof(size_t));

    rats_sstable *sst = rats_sstable_read_in_memory(data, size);
    if (!sst) return;

    for (int i = 0; i < sst->num_entries; ++i) {
        size_t offset = sst->index_entries[i].offset;

        rats_memtable_entry *entry = rats_memtable_entry_read(data, offset);
        if (!entry) return;

        rats_time time = (rats_time) entry->key[1];
        if (entry->key[0] == stream_id && time >= from && time <= to) {
            size_t samples = entry->block_size / (pcm->channels * pcm->bit_depth / 8);
            rats_pcm_write_s16(pcm, (rats_int16 *) entry->block, samples);
        } else {
            free(entry->block);
        }

        free(entry);
    }

    rats_sstable_destroy_except_data(sst);
}