#include "format.h"

size_t auxts_format_pcm(auxts_format* fmt, const void* in, void* out, size_t samples, size_t size, const char* mime_type) {
    if (strcmp(mime_type, "audio/wav") == 0) {
        auxts_wav wav;

        auxts_wav_set_channels(&wav, fmt->channels);
        auxts_wav_set_sample_rate(&wav, fmt->sample_rate);

        return auxts_wav_write_s16(&wav, in, out, samples, size);
    }

    if (strcmp(mime_type, "audio/mpeg") == 0 || strcmp(mime_type, "audio/mp3") == 0) {
        auxts_mp3 mp3;

        auxts_mp3_set_channels(&mp3, fmt->channels);
        auxts_mp3_set_sample_rate(&mp3, fmt->sample_rate);
        auxts_mp3_init(&mp3);

        size_t bytes = auxts_mp3_write_s16(&mp3, in, out, samples, size);
        auxts_mp3_destroy(&mp3);

        return bytes;
    }

    return 0;
}
