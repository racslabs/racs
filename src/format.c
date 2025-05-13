#include "format.h"

size_t
racs_format_pcm(racs_format *fmt, const void *in, void *out, size_t samples, size_t size, const char *mime_type) {
    if (strcmp(mime_type, "audio/wav") == 0) {
        racs_wav wav;

        racs_wav_set_channels(&wav, fmt->channels);
        racs_wav_set_sample_rate(&wav, fmt->sample_rate);

        return racs_wav_write_s16(&wav, in, out, samples, size);
    }

    if (strcmp(mime_type, "audio/mpeg") == 0 || strcmp(mime_type, "audio/mp3") == 0) {
        racs_mp3 mp3;

        racs_mp3_set_channels(&mp3, fmt->channels);
        racs_mp3_set_sample_rate(&mp3, fmt->sample_rate);
        racs_mp3_init(&mp3);

        size_t bytes = racs_mp3_write_s16(&mp3, in, out, samples, size);
        racs_mp3_destroy(&mp3);

        return bytes;
    }

    if (strcmp(mime_type, "audio/ogg") == 0) {
        racs_ogg ogg;

        racs_ogg_set_channels(&ogg, fmt->channels);
        racs_ogg_set_sample_rate(&ogg, fmt->sample_rate);

        racs_ogg_write_s16(&ogg, in, out, samples, size);
        racs_ogg_destroy(&ogg);

        return ogg.w_pos;
    }

    return 0;
}
