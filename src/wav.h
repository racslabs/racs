
#ifndef AUXTS_WAV_H
#define AUXTS_WAV_H

#include <stdlib.h>
#include "bytes.h"
#include "simd/pcm_simd.h"

typedef struct {
    char         chunk_id[4];
    auxts_uint32 chunk_size;
    char         format[4];
} auxts_wav_header;

typedef struct {
    char         sub_chunk1_id[4];
    auxts_uint32 sub_chunk1_size;
    auxts_uint16 audio_format;
    auxts_uint16 channels;
    auxts_uint32 sample_rate;
    auxts_uint32 byte_rate;
    auxts_uint16 block_align;
    auxts_uint16 bit_depth;
} auxts_wav_format;

typedef struct {
    char         sub_chunk2_id[4];
    auxts_uint32 sub_chunk2_size;
} auxts_wav_data;

typedef struct {
    auxts_wav_header    header;
    auxts_wav_format    format;
    auxts_wav_data      data;
    off_t               pos;
} auxts_wav;

void auxts_wav_set_channels(auxts_wav* wav, auxts_uint16 channels);
void auxts_wav_set_sample_rate(auxts_wav* wav, auxts_uint32 sample_rate);

size_t auxts_wav_write(auxts_wav* wav, const void* in, void* out, size_t samples, size_t size);
size_t auxts_wav_write_s16(auxts_wav* wav, const auxts_int16* in, void* out, size_t samples, size_t size);

void auxts_wav_encode_header(void* out, auxts_wav* wav, auxts_uint32 samples);
void auxts_wav_encode_format(void* out, auxts_wav* wav);
void auxts_wav_encode_data(void* out, auxts_wav* wav, const void* data, auxts_uint32 samples);

#endif //AUXTS_WAV_H
