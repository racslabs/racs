#include "extract.h"

int racs_extract_pcm(racs_context *ctx, racs_pcm *pcm, const char *stream_id, racs_time from, racs_time to) {
    char *path = racs_time_range_to_path(from, to);
    racs_filelist *list = get_sorted_filelist(path);

    racs_streaminfo streaminfo;
    racs_uint64 hash = racs_hash(stream_id);

    int rc = racs_streaminfo_get(ctx->mcache, &streaminfo, hash);
    if (rc == -1) racs_streaminfo_put(ctx->mcache, &streaminfo, hash);
    if (rc == 0) return RACS_EXTRACT_STATUS_NOT_FOUND;

    racs_pcm_set_bit_depth(pcm, streaminfo.bit_depth);
    racs_pcm_set_channels(pcm, streaminfo.channels);
    racs_pcm_set_sample_rate(pcm, streaminfo.sample_rate);

    racs_pcm_init(pcm);

    for (int i = 0; i < list->num_files; ++i) {
        char *file_path = list->files[i];

        racs_time time = racs_time_from_path(file_path);
        if (time >= from && time <= to) {
            racs_uint8 *data = racs_extract_from_cache_or_sstable(ctx->scache, hash, time, file_path);
            racs_extract_process_sstable(pcm, data, hash, from, to);
        }
    }

    racs_memtable *mt = ctx->mmt->head;
    while (mt) {
        for (int i = 0; i < mt->num_entries; ++i) {
            racs_memtable_entry *entry = &mt->entries[i];
            racs_uint64 _stream_id = entry->key[0];
            racs_time time = (racs_time)entry->key[1];

            if (hash == _stream_id && time >= from && time <= to) {
                size_t samples = entry->block_size / (pcm->channels * pcm->bit_depth / 8);
                racs_pcm_write_s16(pcm, (racs_int16 *) entry->block, samples);
            }
        }
        mt = (racs_memtable *) mt->next;
    }

    free(path);
    racs_filelist_destroy(list);

    return RACS_EXTRACT_STATUS_OK;
}

racs_uint8 *
racs_extract_from_cache_or_sstable(racs_cache *cache, racs_uint64 stream_id, racs_time time, const char *path) {
    racs_uint64 key[2] = {stream_id, time};
    racs_uint8 *data = racs_cache_get(cache, key);

    if (!data) {
        racs_sstable *sstable = racs_sstable_read(path);
        if (!sstable) return NULL;

        data = sstable->data;
        racs_sstable_destroy_except_data(sstable);
        racs_cache_put(cache, key, data);
    }

    return data;
}

void
racs_extract_process_sstable(racs_pcm *pcm, racs_uint8 *data, racs_uint64 stream_id, racs_int64 from, racs_int64 to) {
    size_t size;
    memcpy(&size, data, sizeof(size_t));

    racs_sstable *sst = racs_sstable_read_in_memory(data, size);
    if (!sst) return;

    for (int i = 0; i < sst->num_entries; ++i) {
        size_t offset = sst->index_entries[i].offset;

        racs_memtable_entry *entry = racs_memtable_entry_read(data, offset);
        if (!entry) return;

        racs_time time = (racs_time) entry->key[1];
        if (entry->key[0] == stream_id && time >= from && time <= to) {
            size_t samples = entry->block_size / (pcm->channels * pcm->bit_depth / 8);
            racs_pcm_write_s16(pcm, (racs_int16 *) entry->block, samples);
        } else {
            free(entry->block);
        }

        free(entry);
    }

    racs_sstable_destroy_except_data(sst);
}
