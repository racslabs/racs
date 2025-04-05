
#ifndef AUXTS_FLAC_H
#define AUXTS_FLAC_H

#include <FLAC/stream_decoder.h>
#include <FLAC/stream_encoder.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pcm.h"

/** this file is a wrapper of libFLAC */

typedef struct {
    auxts_uint8  channels;
    auxts_uint8  bit_depth;
    auxts_uint32 block_size;
    auxts_uint32 samples_rate;
    auxts_uint64 pcm_frame_count;
    auxts_uint64 total_samples;
    FLAC__StreamDecoder* decoder;
    FLAC__StreamEncoder* encoder;
    auxts_memory_stream in_stream;
    auxts_memory_stream out_stream;
} auxts_flac;

auxts_uint64 auxts_flac_read_pcm(auxts_flac* flac, void* in, size_t size);
void auxts_flac_destroy(auxts_flac* flac);

int auxts_flac_decoder_write_pcm_s16(auxts_flac* flac, auxts_pcm* pcm, const auxts_int32** in, size_t n);
int auxts_flac_decoder_write_pcm_s32(auxts_flac* flac, auxts_pcm* pcm, const auxts_int32** in, size_t n);

void auxts_flac_decoder_metadata_callback(const FLAC__StreamDecoder *decoder,
                                          const FLAC__StreamMetadata *metadata,
                                          void *client_data);

FLAC__StreamDecoderReadStatus auxts_flac_decoder_read_callback(const FLAC__StreamDecoder* decoder,
                                                               FLAC__byte buffer[],
                                                               size_t* bytes,
                                                               void* client_data);

FLAC__StreamDecoderWriteStatus auxts_flac_decoder_write_callback(const FLAC__StreamDecoder *decoder,
                                                         const FLAC__Frame *frame,
                                                         const FLAC__int32 *const buffer[],
                                                         void *client_data);

void auxts_flac_decoder_error_callback(const FLAC__StreamDecoder *decoder,
                                       FLAC__StreamDecoderErrorStatus status,
                                       void *client_data);
#endif //AUXTS_FLAC_H
