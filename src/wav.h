
#ifndef AUXTS_WAV_H
#define AUXTS_WAV_H

#include "stream.h"
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
    auxts_memory_stream out_stream;
} auxts_wav;

void auxts_wav_init(auxts_wav* wav, auxts_uint16 channels, auxts_uint16 bit_depth, auxts_uint32 sample_rate);

size_t auxts_wav_write_s24(auxts_wav* wav, auxts_int24* in, size_t samples);
size_t auxts_wav_write_s16(auxts_wav* wav, auxts_int16* in, size_t samples);
size_t auxts_wav_write(auxts_wav* wav, void* in, size_t samples);

void auxts_wav_encode_header(auxts_wav* wav, auxts_uint32 samples);
void auxts_wav_encode_format(auxts_wav* wav);
void auxts_wav_encode_data(auxts_wav* wav, void* data, auxts_uint32 samples);

void auxts_wav_destroy(auxts_wav* wav);

#endif //AUXTS_WAV_H
