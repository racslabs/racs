#include "create.h"

int auxts_create(const char* name, uint32_t sample_rate, uint16_t channels, uint16_t bit_depth) {
    auxts_metadata metadata;
    metadata.sample_rate = sample_rate;
    metadata.channels = channels;
    metadata.bit_depth = bit_depth;
    metadata.ref = 0;
    metadata.bytes = 0;

    return auxts_metadata_put(&metadata, name);
}
