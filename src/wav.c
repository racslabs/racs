#include "wav.h"
#include <stdio.h>

void racs_wav_set_channels(racs_wav *wav, racs_uint16 channels) {
    wav->format.channels = channels;
}

void racs_wav_set_sample_rate(racs_wav *wav, racs_uint32 sample_rate) {
    wav->format.sample_rate = sample_rate;
}

size_t racs_wav_write_s16(racs_wav *wav, const racs_int16 *in, void *out, size_t samples, size_t size) {
    if (wav->format.channels == 2) {
        racs_int16 *_in = malloc(wav->format.channels * samples * sizeof(racs_int16));
        racs_simd_planar_s16(in, _in, wav->format.channels * samples);

        size_t _size = racs_wav_write(wav, _in, out, samples, size);

        free(_in);
        return _size;
    }

    return racs_wav_write(wav, in, out, samples, size);
}

/* For internal use ONLY */
size_t racs_wav_write(racs_wav *wav, const void *in, void *out, size_t samples, size_t size) {
    wav->pos = 0;

    racs_wav_encode_header(out, wav, samples);
    racs_wav_encode_format(out, wav);
    racs_wav_encode_data(out, wav, in, samples);

    return wav->pos;
}

void racs_wav_encode_header(void *out, racs_wav *wav, racs_uint32 samples) {
    wav->format.bit_depth = 16;

    memcpy(wav->header.chunk_id, "RIFF", 4);
    wav->header.chunk_size = (samples * wav->format.channels * wav->format.bit_depth / 8) + 36;
    memcpy(wav->header.format, "WAVE", 4);

    wav->pos = racs_write_bin(out, wav->header.chunk_id, 4, wav->pos);
    wav->pos = racs_write_uint32(out, wav->header.chunk_size, wav->pos);
    wav->pos = racs_write_bin(out, wav->header.format, 4, wav->pos);
}

void racs_wav_encode_format(void *out, racs_wav *wav) {
    wav->format.block_align = wav->format.channels * wav->format.bit_depth / 8;
    wav->format.byte_rate = wav->format.sample_rate * wav->format.block_align;

    wav->format.sub_chunk1_size = 16;
    wav->format.audio_format = 1;

    memcpy(wav->format.sub_chunk1_id, "fmt ", 4);

    wav->pos = racs_write_bin(out, &wav->format.sub_chunk1_id, 4, wav->pos);
    wav->pos = racs_write_uint32(out, wav->format.sub_chunk1_size, wav->pos);
    wav->pos = racs_write_uint16(out, wav->format.audio_format, wav->pos);
    wav->pos = racs_write_uint16(out, wav->format.channels, wav->pos);
    wav->pos = racs_write_uint32(out, wav->format.sample_rate, wav->pos);
    wav->pos = racs_write_uint32(out, wav->format.byte_rate, wav->pos);
    wav->pos = racs_write_uint16(out, wav->format.block_align, wav->pos);
    wav->pos = racs_write_uint16(out, wav->format.bit_depth, wav->pos);
}

void racs_wav_encode_data(void *out, racs_wav *wav, const void *data, racs_uint32 samples) {
    memcpy(wav->data.sub_chunk2_id, "data", 4);
    wav->data.sub_chunk2_size = samples * wav->format.channels * wav->format.bit_depth / 8;

    wav->pos = racs_write_bin(out, wav->data.sub_chunk2_id, 4, wav->pos);
    wav->pos = racs_write_uint32(out, wav->data.sub_chunk2_size, wav->pos);
    wav->pos = racs_write_bin(out, data, wav->data.sub_chunk2_size, wav->pos);
}
