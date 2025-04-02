#include "create.h"

int auxts_create(const char* name, uint32_t sample_rate, uint32_t channels, uint32_t bit_depth) {
    auxts_metadata metadata;
    metadata.sample_rate = sample_rate;
    metadata.channels = channels;
    metadata.bit_depth = bit_depth;
    metadata.ref = auxts_time_now();
    metadata.bytes = 0;

    return auxts_metadata_put(&metadata, name);
}
