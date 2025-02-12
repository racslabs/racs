#include "flac.h"

static FLAC__StreamDecoderReadStatus read_callback(const FLAC__StreamDecoder* decoder, FLAC__byte buffer[], size_t* bytes, void* client_data);
static FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data);
static void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data);
static AUXTS__DecoderContext* DecoderContext_construct(AUXTS__FlacEncodedBlock* block);

FLAC__StreamDecoderReadStatus read_callback(const FLAC__StreamDecoder* decoder, FLAC__byte buffer[], size_t* bytes, void* client_data) {
    (void)decoder;

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
    (void)decoder;

    AUXTS__PcmBlock* block = ((AUXTS__DecoderContext*) client_data)->pcm;

    uint32_t channels = frame->header.channels;
    uint32_t block_size = frame->header.blocksize;

    block->sample_rate = frame->header.sample_rate;
    block->bits_per_sample = frame->header.bits_per_sample;
    block->data = malloc(channels * sizeof(int32_t*));

    for (unsigned channel = 0; channel < channels; channel++) {
        block->data[channels] = malloc(block_size);
        memcpy(block->data[channel], buffer[channel], block_size);
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data) {
    fprintf(stderr, "FLAC decoding error: %d\n", status);
}

AUXTS__DecoderContext* DecoderContext_construct(AUXTS__FlacEncodedBlock* block) {
    AUXTS__DecoderContext* context = malloc(sizeof(AUXTS__DecoderContext));
    if (!context) {
        perror("Error allocating AUXTS__DecoderContext");
        return NULL;
    }

    context->flac = block;
    context->pcm = malloc(sizeof(AUXTS__PcmBlock));
    if (!context->pcm) {
        perror("Error allocating AUXTS__PcmBlock");
        return NULL;
    }

    return context;
}