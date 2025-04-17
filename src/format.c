#include "format.h"

size_t auxts_format_pcm(auxts_format* fmt, const void* in, void* out, size_t samples, size_t size, const char* mime_type) {
    if (strcmp(mime_type, "audio/wav") == 0) {
        auxts_wav wav;

        auxts_wav_set_channels(&wav, fmt->channels);
        auxts_wav_set_sample_rate(&wav, fmt->sample_rate);

        return auxts_wav_write(&wav, in, out, samples, size);
    }

    if (strcmp(mime_type, "audio/mpeg") == 0 || strcmp(mime_type, "audio/mp3") == 0) {
        lame_t lame = lame_init();
        lame_set_in_samplerate(lame, (int)fmt->sample_rate);
        lame_set_num_channels(lame, fmt->channels);
        lame_set_VBR(lame, vbr_default);
        lame_init_params(lame);

        size_t bytes = lame_encode_buffer_interleaved(lame, (auxts_int16*)in, (int)(size / fmt->channels), out, (int)size);
        lame_close(lame);

        return bytes;
    }

    return 0;
}
