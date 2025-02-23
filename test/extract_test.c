#include "extract_test.h"

void test_extract() {
    uint64_t stream_id = 8185897970534249969ULL;
    uint64_t begin_timestamp = 1739141512213ULL;
    uint64_t end_timestamp = 1739141512214ULL;

    lru_cache* cache = auxts_lru_cache_create(2);
    pcm_buffer* buffer = auxts_extract_pcm_data(cache, stream_id, begin_timestamp, end_timestamp);

    assert(buffer->channels == 2);
    assert(buffer->bits_per_sample == 16);
    assert(buffer->sample_rate == 44100);
    assert(buffer->num_samples == 44313);

    auxts_pcm_buffer_destroy(buffer);
    auxts_lru_cache_destroy(cache);
}
