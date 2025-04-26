
#ifndef RATS_CREATE_H
#define RATS_CREATE_H

#include "streaminfo.h"
#include "time.h"

int rats_create(rats_cache *mcache, rats_uint64 stream_id, rats_uint32 sample_rate, rats_uint16 channels);

#endif //RATS_CREATE_H
