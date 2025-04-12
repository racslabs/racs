#include "format.h"


void auxts_format_init(auxts_format* fmt, auxts_uint16 channels, auxts_uint16 bit_depth, auxts_uint32 sample_rate) {
    fmt->channels = channels;
    fmt->bit_depth = bit_depth;
    fmt->sample_rate = sample_rate;
}

int auxts_format_pcm_s32(auxts_format* fmt, const auxts_int32* in, size_t size, const char* mime_type) {
    if (strcmp(mime_type, "audio/wav") == 0) {
        auxts_wav wav;
        auxts_wav_init(&wav, fmt->channels, fmt->bit_depth, fmt->sample_rate);

        size_t samples = size / (fmt->channels * sizeof(auxts_int32));
        auxts_int24* out = malloc(samples * fmt->channels * sizeof(auxts_int24));

        auxts_simd_s32_s24(in, out, samples * fmt->channels);
        auxts_wav_write_s24(&wav, out, samples);

        fmt->out_stream = wav.out_stream;

        return 0;
    }

    if (strcmp(mime_type, "audio/mp3") == 0 || strcmp(mime_type, "audio/mpeg") == 0) {
        // add specific error code
        return 1;
    }

    if (strcmp(mime_type, "audio/ogg") == 0) {
        // add specific error code
        return 1;
    }

    return 1;
}
