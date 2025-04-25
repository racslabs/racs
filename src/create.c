#include "create.h"

int rats_create(rats_cache* mcache, uint64_t stream_id, uint32_t sample_rate, uint16_t channels) {
    rats_streaminfo streaminfo;
    streaminfo.sample_rate = sample_rate;
    streaminfo.channels = channels;
    streaminfo.bit_depth = 16;
    streaminfo.ref = 0;
    streaminfo.size = 0;

    return rats_streaminfo_put(mcache, &streaminfo, stream_id);
}
