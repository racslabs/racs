#include "metadata_test.h"

void test_metadata() {
    auxts_metadata metadata;
    metadata.channels = 2;
    metadata.bit_depth = 16;
    metadata.sample_rate = 44100;
    metadata.created_at = 1739141512213;
    metadata.bytes = 44100*2;

    auxts_metadata_put(&metadata, "test");
    auxts_metadata_get(&metadata, "test");

    assert(metadata.channels == 2);
    assert(metadata.bit_depth == 16);
    assert(metadata.sample_rate == 44100);
    assert(metadata.created_at == 1739141512213);
    assert(metadata.bytes == 44100*2);
}