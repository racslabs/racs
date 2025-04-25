#include "format.h"

size_t rats_format_pcm(rats_format* fmt, const void* in, void* out, size_t samples, size_t size, const char* mime_type) {
    if (strcmp(mime_type, "audio/wav") == 0) {
        rats_wav wav;

        rats_wav_set_channels(&wav, fmt->channels);
        rats_wav_set_sample_rate(&wav, fmt->sample_rate);

        return rats_wav_write_s16(&wav, in, out, samples, size);
    }

    if (strcmp(mime_type, "audio/mpeg") == 0 || strcmp(mime_type, "audio/mp3") == 0) {
        rats_mp3 mp3;

        rats_mp3_set_channels(&mp3, fmt->channels);
        rats_mp3_set_sample_rate(&mp3, fmt->sample_rate);
        rats_mp3_init(&mp3);

        size_t bytes = rats_mp3_write_s16(&mp3, in, out, samples, size);
        rats_mp3_destroy(&mp3);

        return bytes;
    }

    if (strcmp(mime_type, "audio/ogg") == 0) {
        rats_ogg ogg;

        rats_ogg_set_channels(&ogg, fmt->channels);
        rats_ogg_set_sample_rate(&ogg, fmt->sample_rate);

        rats_ogg_write_s16(&ogg, in, out, samples, size);
        rats_ogg_destroy(&ogg);

        return ogg.w_pos;
    }

    return 0;
}
