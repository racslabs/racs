
#ifndef RACS_FORMAT_H
#define RACS_FORMAT_H

#include "mp3.h"
#include "wav.h"
#include "ogg.h"

typedef struct {
    racs_uint16 channels;
    racs_uint32 sample_rate;
    racs_uint16 bit_depth;
} racs_format;

size_t racs_format_pcm(racs_format *fmt, const void *in, void *out, size_t samples, size_t size, const char *mime_type);

#endif //RACS_FORMAT_H
