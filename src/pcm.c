#include "pcm.h"

void auxts_pcm_init_s32(auxts_pcm* pcm, auxts_int32* buf, auxts_uint64 n) {
    memset(pcm, 0, sizeof(auxts_pcm));
    auxts_memory_stream_init(&pcm->memory_stream, buf, n * sizeof(auxts_int32));
}

void auxts_pcm_init_s16(auxts_pcm* pcm, auxts_int16* buf, auxts_uint64 n) {
    memset(pcm, 0, sizeof(auxts_pcm));
    auxts_memory_stream_init(&pcm->memory_stream, buf, n * sizeof(auxts_int16));
}

void auxts_pcm_write_s32(auxts_pcm* pcm, auxts_int32* samples, auxts_uint64 n) {
    auxts_memory_stream_write(&pcm->memory_stream, samples, n * sizeof(auxts_int32));
}

void auxts_pcm_write_s16(auxts_pcm* pcm, auxts_int16* samples, auxts_uint64 n) {
    auxts_memory_stream_write(&pcm->memory_stream, samples, n * sizeof(auxts_int16));
}
