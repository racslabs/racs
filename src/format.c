#include "format.h"


void auxts_format_init(auxts_format* fmt, auxts_uint16 channels, auxts_uint16 bit_depth, auxts_uint32 sample_rate) {
    memset(fmt, 0, sizeof(auxts_format));

    fmt->channels = channels;
    fmt->bit_depth = bit_depth;
    fmt->sample_rate = sample_rate;
}

int auxts_format_pcm_s32(auxts_format* fmt, const auxts_int32* in, size_t size, const char* mime_type) {
    if (strcmp(mime_type, "audio/wav") == 0) {
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

int auxts_format_pcm_s16(auxts_format* fmt, const auxts_int16* in, size_t size, const char* mime_type) {
    if (strcmp(mime_type, "audio/wav") == 0) {
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
