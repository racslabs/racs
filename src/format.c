#include "format.h"

size_t
racs_format_pcm(racs_format *fmt, const void *in, void *out, size_t samples, size_t size, const char *mime_type) {
    if (strcmp(mime_type, "audio/wav") == 0) {
        racs_wav wav;

        racs_wav_set_channels(&wav, fmt->channels);
        racs_wav_set_sample_rate(&wav, fmt->sample_rate);
        racs_wav_set_bit_depth(&wav, fmt->bit_depth);

        size_t bytes = 0;

        if (fmt->bit_depth == 16) {
            racs_int16 *_in = racs_s32_s16(in, samples * fmt->channels);
            bytes = racs_wav_write_s16(&wav, _in, out, samples, size);
            free(_in);
        }

        if (fmt->bit_depth == 24) {
            racs_int24 *_in = racs_s32_s24(in, samples * fmt->channels);
            bytes = racs_wav_write_s24(&wav, _in, out, samples, size);
            free(_in);
        }

        return bytes;
    }

    if (strcmp(mime_type, "audio/mpeg") == 0 || strcmp(mime_type, "audio/mp3") == 0) {
        racs_mp3 mp3;

        racs_mp3_set_channels(&mp3, fmt->channels);
        racs_mp3_set_sample_rate(&mp3, fmt->sample_rate);
        racs_mp3_init(&mp3);

        size_t bytes = 0;

        if (fmt->bit_depth == 16) {
            racs_int16 *_in = racs_s32_s16(in, samples * fmt->channels);
            bytes = racs_mp3_write_s16(&mp3, _in, out, samples, size);
            free(_in);
        }

        if (fmt->bit_depth == 24) {
            racs_int16 *_in = racs_s32_s16_d8(in, fmt->channels * samples);
            bytes = racs_mp3_write_s16(&mp3, _in, out, samples, size);
            free(_in);
        }

        racs_mp3_destroy(&mp3);
        return bytes;
    }

    if (strcmp(mime_type, "audio/ogg") == 0) {
        racs_ogg ogg;

        racs_ogg_set_channels(&ogg, fmt->channels);
        racs_ogg_set_sample_rate(&ogg, fmt->sample_rate);

        size_t bytes = 0;

        if (fmt->bit_depth == 16) {
            racs_int16 *_in = racs_s32_s16(in, samples * fmt->channels);
            bytes = racs_ogg_write_s16(&ogg, _in, out, samples);
            free(_in);
        }

        if (fmt->bit_depth == 24) {
            racs_int16 *_in = racs_s32_s16_d8(in, fmt->channels * samples);
            bytes = racs_ogg_write_s16(&ogg, _in, out, samples);
            free(_in);
        }

        racs_ogg_destroy(&ogg);
        return bytes;
    }

    return 0;
}
