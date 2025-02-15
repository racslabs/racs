#include "flac.h"

AUXTS_API const int AUXTS__INITIAL_FLAC_STREAM_CAPACITY = 2;

static FLAC__StreamDecoderReadStatus read_callback(const FLAC__StreamDecoder* decoder, FLAC__byte buffer[], size_t* bytes, void* client_data);
static FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data);
static void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data);
static AUXTS__DecoderContext* DecoderContext_construct(AUXTS__FlacEncodedBlock* block);
static void DecoderContext_destroy(AUXTS__DecoderContext* context);

AUXTS_API AUXTS__FlacEncodedBlocks* AUXTS__FlacEncodedBlocks_construct() {
    AUXTS__FlacEncodedBlocks* stream = malloc(sizeof(AUXTS__FlacEncodedBlocks));
    if (!stream) {
        perror("Failed to allocate AUXTS__FlacEncodedBlocks");
        exit(EXIT_FAILURE);
    }

    stream->capacity = AUXTS__INITIAL_FLAC_STREAM_CAPACITY;
    stream->size = 0;

    stream->blocks = malloc(stream->capacity * sizeof(AUXTS__FlacEncodedBlock));
    if (!stream->blocks) {
        perror("Failed to allocate blocks to AUXTS__FlacEncodedBlocks");
        exit(EXIT_FAILURE);
    }

    return stream;
}

AUXTS_API AUXTS__PcmBlock* AUXTS__decode_flac_block(AUXTS__FlacEncodedBlock* block) {
    FLAC__StreamDecoder* decoder = FLAC__stream_decoder_new();
    if (!decoder) {
        perror("Failed to allocate FLAC__StreamDecoder");
        return NULL;
    }

    AUXTS__DecoderContext* context = DecoderContext_construct(block);

    FLAC__stream_decoder_init_stream(decoder, read_callback, NULL, NULL, NULL, NULL, write_callback, NULL, error_callback, context);
    FLAC__stream_decoder_process_until_end_of_stream(decoder);
    FLAC__stream_decoder_delete(decoder);

    AUXTS__PcmBlock* pcm = context->pcm;
    DecoderContext_destroy(context);

    return pcm;
}

AUXTS_API void AUXTS__FlacEncodedBlocks_append(AUXTS__FlacEncodedBlocks* blocks, uint8_t* block_data, uint16_t size) {
    if (blocks->size == blocks->capacity) {
        blocks->capacity = 1 << blocks->capacity;
        blocks->blocks = realloc(blocks->blocks, blocks->capacity * sizeof(AUXTS__FlacEncodedBlock));

        if (!blocks->blocks) {
            perror("Error reallocating blocks to AUXTS__FlacEncodedBlocks");
            exit(EXIT_FAILURE);
        }
    }

    AUXTS__FlacEncodedBlock* block = malloc(sizeof(AUXTS__FlacEncodedBlock));
    if (!block) {
        perror("Failed to allocate AUXTS__FlacEncodedBlock");
        exit(EXIT_FAILURE);
    }

    block->data = block_data;
    block->size = size;

    blocks->blocks[blocks->size] = block;
    ++blocks->size;
}

AUXTS_API void AUXTS__FlacEncodedBlocks_destroy(AUXTS__FlacEncodedBlocks* blocks) {
    for (int i = 0; i < blocks->size; ++i) {
        AUXTS__FlacEncodedBlock* block = blocks->blocks[i];
        free(block->data);
        free(block);
    }

    free(blocks->blocks);
    free(blocks);
}

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
    (void)decoder;
    (void)client_data;

    fprintf(stderr, "FLAC decoding error: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
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

void DecoderContext_destroy(AUXTS__DecoderContext* context) {
    free(context);
}
