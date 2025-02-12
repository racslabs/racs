#include "flac.h"

static FLAC__StreamDecoderReadStatus read_callback(const FLAC__StreamDecoder* decoder, FLAC__byte buffer[], size_t* bytes, void* client_data);
static FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data);

FLAC__StreamDecoderReadStatus read_callback(const FLAC__StreamDecoder* decoder, FLAC__byte buffer[], size_t* bytes, void* client_data) {
    AUXTS__FlacEncodedBlock* block = ((AUXTS__DecoderContext*) client_data)->flac;
    if (*bytes > block->size - block->offset) {
        *bytes = block->size - block->offset;
    }

    if (*bytes == 0) {
        return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
    }

    memcpy(buffer, block->data + block->offset, *bytes);
    block->offset += *bytes;

    return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data) {
    AUXTS__PcmBlock* block = ((AUXTS__DecoderContext*) client_data)->pcm;

    (void)decoder;

    unsigned channels = frame->header.channels;
    unsigned block_size = frame->header.blocksize;

    for (unsigned sample = 0; sample < block_size; sample++) {
        for (unsigned channel = 0; channel < channels; channel++) {
            block->data[channel][sample] = buffer[channel][sample];
        }
    }
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}