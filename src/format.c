#include "format.h"

void auxts_format_init(auxts_format* fmt, void* out, size_t size) {
    auxts_memory_stream_init(&fmt->out_stream, out, size);
}

void auxts_format_set_channels(auxts_format* fmt, auxts_uint16 channels) {
    fmt->channels = channels;
}

void auxts_format_set_bit_depth(auxts_format* fmt, auxts_uint16 bit_depth) {
    fmt->bit_depth = bit_depth;
}

void auxts_format_set_sample_rate(auxts_format* fmt, auxts_uint32 sample_rate) {
    fmt->sample_rate = sample_rate;
}

int auxts_format_pcm_s16(auxts_format* fmt, const auxts_int16* in, size_t samples, const char* mime_type) {
    if (strcmp(mime_type, "audio/wav") == 0) {
        auxts_wav wav;
        memset(&wav, 0, sizeof(auxts_wav));

        auxts_wav_set_channels(&wav, fmt->channels);
        auxts_wav_set_bit_depth(&wav, fmt->bit_depth);
        auxts_wav_set_sample_rate(&wav, fmt->sample_rate);

        auxts_wav_write_s16(&wav, in, fmt->out_stream.data, samples, fmt->out_stream.size);
        fmt->out_stream = wav.out_stream;

        return 0;
    }

    if (strcmp(mime_type, "audio/mp3") == 0 || strcmp(mime_type, "audio/mpeg") == 0) {
        // add specific error code
        return 0;
    }

    if (strcmp(mime_type, "audio/ogg") == 0) {
        // add specific error code
        return 0;
    }

    return 1;
}
