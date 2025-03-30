
#ifndef AUXTS_FLAC0_H
#define AUXTS_FLAC0_H

#include <dr_flac.h>
#include <tflac.h>
#include <stdio.h>
#include <string.h>
#include "type.h"

/** this file is a wrapper of drflac and tflac. */

typedef struct {
    auxts_uint8  channels;
    auxts_uint8  bit_depth;
    auxts_uint32 samples_rate;
    auxts_uint64 pcm_frame_count;
    drflac*      dr;
    tflac*       tf;
} auxts_flac;

void auxts_flac_open(auxts_flac* flac, void* data, size_t size);
void auxts_flac_close(auxts_flac* flac);

auxts_uint64 auxts_flac_read_pcm_s32(auxts_flac* flac, auxts_int32* data);
auxts_uint64 auxts_flac_read_pcm_s16(auxts_flac* flac, auxts_int16* data);

#endif //AUXTS_FLAC0_H
