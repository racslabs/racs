
#ifndef RATS_WAV_H
#define RATS_WAV_H

#include <stdlib.h>
#include "bytes.h"
#include "simd/pcm_simd.h"

typedef struct {
    char         chunk_id[4];
    rats_uint32 chunk_size;
    char         format[4];
} rats_wav_header;

typedef struct {
    char         sub_chunk1_id[4];
    rats_uint32 sub_chunk1_size;
    rats_uint16 audio_format;
    rats_uint16 channels;
    rats_uint32 sample_rate;
    rats_uint32 byte_rate;
    rats_uint16 block_align;
    rats_uint16 bit_depth;
} rats_wav_format;

typedef struct {
    char         sub_chunk2_id[4];
    rats_uint32 sub_chunk2_size;
} rats_wav_data;

typedef struct {
    rats_wav_header    header;
    rats_wav_format    format;
    rats_wav_data      data;
    off_t               pos;
} rats_wav;

void rats_wav_set_channels(rats_wav* wav, rats_uint16 channels);
void rats_wav_set_sample_rate(rats_wav* wav, rats_uint32 sample_rate);

size_t rats_wav_write(rats_wav* wav, const void* in, void* out, size_t samples, size_t size);
size_t rats_wav_write_s16(rats_wav* wav, const rats_int16* in, void* out, size_t samples, size_t size);

void rats_wav_encode_header(void* out, rats_wav* wav, rats_uint32 samples);
void rats_wav_encode_format(void* out, rats_wav* wav);
void rats_wav_encode_data(void* out, rats_wav* wav, const void* data, rats_uint32 samples);

#endif //RATS_WAV_H
