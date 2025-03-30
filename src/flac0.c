#include "flac0.h"

void auxts_flac_open(auxts_flac* flac, void* data, size_t size) {
    drflac* dr = drflac_open_memory(data, size, NULL);
    if (!dr) {
        perror("Failed to open drflac");
        return;
    }

    memset(flac, 0, sizeof(auxts_flac));

    flac->channels = dr->channels;
    flac->bit_depth = dr->bitsPerSample;
    flac->samples_rate = dr->sampleRate;
    flac->pcm_frame_count = dr->totalPCMFrameCount;

    flac->dr = dr;
}

void auxts_flac_close(auxts_flac* flac) {
    drflac_close(flac->dr);
}

auxts_uint64 auxts_flac_read_pcm_s32(auxts_flac* flac, auxts_int32* data) {
    return drflac_read_pcm_frames_s32(flac->dr, flac->pcm_frame_count, data);
}

auxts_uint64 auxts_flac_read_pcm_s16(auxts_flac* flac, auxts_int16* data) {
    return drflac_read_pcm_frames_s16(flac->dr, flac->pcm_frame_count, data);
}
