
#ifndef AUXTS_FORMAT_H
#define AUXTS_FORMAT_H

#include "wav.h"

typedef struct {
    auxts_uint16 channels;
    auxts_uint16 bit_depth;
    auxts_uint32 sample_rate;
    auxts_memory_stream out_stream;
} auxts_format;

void auxts_format_init(auxts_format* fmt, auxts_uint16 channels, auxts_uint16 bit_depth, auxts_uint32 sample_rate);
int auxts_format_pcm_s32(auxts_format* fmt, const auxts_int32* in, size_t size, const char* mime_type);

#endif //AUXTS_FORMAT_H
