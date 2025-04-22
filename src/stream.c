#include "stream.h"

int auxts_stream(auxts_cache* mcache, auxts_uint64 stream_id, auxts_uint32 sample_rate, auxts_uint16 channels) {
    auxts_streaminfo streaminfo;
    streaminfo.sample_rate = sample_rate;
    streaminfo.channels = channels;
    streaminfo.bit_depth = 16;
    streaminfo.ref = 0;
    streaminfo.size = 0;

    return auxts_streaminfo_put(mcache, &streaminfo, stream_id);
}
