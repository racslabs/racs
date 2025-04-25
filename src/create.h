
#ifndef AUXTS_CREATE_H
#define AUXTS_CREATE_H

#include "streaminfo.h"
#include "time.h"

int rats_create(rats_cache* mcache, uint64_t stream_id, uint32_t sample_rate, uint16_t channels);

#endif //AUXTS_CREATE_H
