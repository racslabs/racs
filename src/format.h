
#ifndef AUXTS_FORMAT_H
#define AUXTS_FORMAT_H

#include "mp3.h"
#include "wav.h"
#include "ogg.h"

typedef struct {
    auxts_uint16 channels;
    auxts_uint32 sample_rate;
} auxts_format;

size_t auxts_format_pcm(auxts_format* fmt, const void* in, void* out, size_t samples, size_t size, const char* mime_type);

#endif //AUXTS_FORMAT_H
