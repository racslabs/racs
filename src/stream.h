
#ifndef AUXTS_STREAM_H
#define AUXTS_STREAM_H

#include "streaminfo.h"

int auxts_stream(auxts_cache* mcache, auxts_uint64 stream_id, auxts_uint32 sample_rate, auxts_uint16 channels);

#endif //AUXTS_STREAM_H
