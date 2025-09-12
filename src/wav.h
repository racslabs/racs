
#ifndef RACS_WAV_H
#define RACS_WAV_H

#include <stdlib.h>
#include "bytes.h"

typedef struct {
    char chunk_id[4];
    racs_uint32 chunk_size;
    char format[4];
} racs_wav_header;

typedef struct {
    char sub_chunk1_id[4];
    racs_uint32 sub_chunk1_size;
    racs_uint16 audio_format;
    racs_uint16 channels;
    racs_uint32 sample_rate;
    racs_uint32 byte_rate;
    racs_uint16 block_align;
    racs_uint16 bit_depth;
} racs_wav_format;

typedef struct {
    char sub_chunk2_id[4];
    racs_uint32 sub_chunk2_size;
} racs_wav_data;

typedef struct {
    racs_wav_header header;
    racs_wav_format format;
    racs_wav_data data;
    off_t pos;
} racs_wav;

void racs_wav_set_channels(racs_wav *wav, racs_uint16 channels);

void racs_wav_set_sample_rate(racs_wav *wav, racs_uint32 sample_rate);

void racs_wav_set_bit_depth(racs_wav *wav, racs_uint16 bit_depth);

size_t racs_wav_write(racs_wav *wav, const void *in, void *out, size_t samples, size_t size);

size_t racs_wav_write_s16(racs_wav *wav, const racs_int16 *in, void *out, size_t samples, size_t size);

size_t racs_wav_write_s24(racs_wav *wav, const racs_int24 *in, void *out, size_t samples, size_t size);

void racs_wav_encode_header(void *out, racs_wav *wav, racs_uint32 samples);

void racs_wav_encode_format(void *out, racs_wav *wav);

void racs_wav_encode_data(void *out, racs_wav *wav, const void *data, racs_uint32 samples);

#endif //RACS_WAV_H
