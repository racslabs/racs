
#ifndef RATS_FORMAT_H
#define RATS_FORMAT_H

#include "mp3.h"
#include "wav.h"
#include "ogg.h"

typedef struct {
    rats_uint16 channels;
    rats_uint32 sample_rate;
} rats_format;

size_t rats_format_pcm(rats_format *fmt, const void *in, void *out, size_t samples, size_t size, const char *mime_type);

#endif //RATS_FORMAT_H
