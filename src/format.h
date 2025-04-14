
#ifndef AUXTS_FORMAT_H
#define AUXTS_FORMAT_H

#include "wav.h"

typedef struct {
    auxts_uint16 channels;
    auxts_uint16 bit_depth;
    auxts_uint32 sample_rate;
    auxts_memory_stream out_stream;
} auxts_format;

void auxts_format_set_channels(auxts_format* fmt, auxts_uint16 channels);
void auxts_format_set_bit_depth(auxts_format* fmt, auxts_uint16 bit_depth);
void auxts_format_set_sample_rate(auxts_format* fmt, auxts_uint32 sample_rate);

int auxts_format_pcm_s16(auxts_format* fmt, const auxts_int16* in, size_t samples, const char* mime_type);

#endif //AUXTS_FORMAT_H
