#include "flac.h"

AUXTS_API const int AUXTS__INITIAL_FLAC_STREAM_CAPACITY = 2;

AUXTS_API const int AUXTS__INITIAL_PCM_BLOCK_CAPACITY = 4096;

static FLAC__StreamDecoderReadStatus read_callback(const FLAC__StreamDecoder* decoder, FLAC__byte buffer[], size_t* bytes, void* client_data);
static FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data);
static void metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data);
static void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data);
static AUXTS__DecoderContext* DecoderContext_construct(AUXTS__FlacEncodedBlock* block);
static void DecoderContext_destroy(AUXTS__DecoderContext* context);

AUXTS_API AUXTS__FlacEncodedBlocks* AUXTS__FlacEncodedBlocks_construct() {
    AUXTS__FlacEncodedBlocks* blocks = malloc(sizeof(AUXTS__FlacEncodedBlocks));
    if (!blocks) {
        perror("Failed to allocate AUXTS__FlacEncodedBlocks");
        return NULL;
    }

    blocks->capacity = AUXTS__INITIAL_FLAC_STREAM_CAPACITY;
    blocks->num_blocks = 0;

    blocks->blocks = malloc(blocks->capacity * sizeof(AUXTS__FlacEncodedBlock));
    if (!blocks->blocks) {
        perror("Failed to allocate blocks to AUXTS__FlacEncodedBlocks");
        return NULL;
    }

    return blocks;
}

AUXTS_API AUXTS__PcmBlock* AUXTS__decode_flac_block(AUXTS__FlacEncodedBlock* block) {
    FLAC__StreamDecoder* decoder = FLAC__stream_decoder_new();
    if (!decoder) {
        perror("Failed to allocate FLAC__StreamDecoder");
        return NULL;
    }

    AUXTS__DecoderContext* context = DecoderContext_construct(block);

    FLAC__stream_decoder_init_stream(decoder, read_callback, NULL, NULL, NULL, NULL, write_callback, metadata_callback, error_callback, context);
    FLAC__stream_decoder_process_until_end_of_stream(decoder);
    FLAC__stream_decoder_delete(decoder);

    AUXTS__PcmBlock* pcm = context->pcm;
    DecoderContext_destroy(context);

    return pcm;
}

AUXTS_API void AUXTS__FlacEncodedBlocks_append(AUXTS__FlacEncodedBlocks* blocks, uint8_t* block_data, uint16_t size) {
    if (blocks->num_blocks == blocks->capacity) {
        blocks->capacity = 1 << blocks->capacity;
        blocks->blocks = realloc(blocks->blocks, blocks->capacity * sizeof(AUXTS__FlacEncodedBlock));

        if (!blocks->blocks) {
            perror("Error reallocating blocks to AUXTS__FlacEncodedBlocks");
            return;
        }
    }

    AUXTS__FlacEncodedBlock* block = malloc(sizeof(AUXTS__FlacEncodedBlock));
    if (!block) {
        perror("Failed to allocate AUXTS__FlacEncodedBlock");
        return;
    }

    block->data = block_data;
    block->size = size;

    blocks->blocks[blocks->num_blocks] = block;
    ++blocks->num_blocks;
}

AUXTS_API void AUXTS__FlacEncodedBlocks_destroy(AUXTS__FlacEncodedBlocks* blocks) {
    for (int i = 0; i < blocks->num_blocks; ++i) {
        AUXTS__FlacEncodedBlock* block = blocks->blocks[i];
        free(block->data);
        free(block);
    }

    free(blocks->blocks);
    free(blocks);
}

void metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data) {
    (void)decoder;

    AUXTS__PcmBlock *block = ((AUXTS__DecoderContext*) client_data)->pcm;

    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
        block->channels = metadata->data.stream_info.channels;
        block->bits_per_sample = metadata->data.stream_info.bits_per_sample;
        block->sample_rate = metadata->data.stream_info.sample_rate;
        block->total_samples = metadata->data.stream_info.total_samples;
        block->data = malloc(block->channels * sizeof(int32_t*));

        for (int channel = 0; channel < block->channels; ++channel) {
            block->data[channel] = malloc(block->total_samples * sizeof(int32_t));
        }
    }
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
    (void) decoder;

    uint32_t block_size = frame->header.blocksize;
    AUXTS__PcmBlock* block = ((AUXTS__DecoderContext*) client_data)->pcm;

    if (block_size + block->num_samples >= block->total_samples) {
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    for (int channel = 0; channel < block->channels; ++channel) {
        memcpy(block->data[channel] + block->num_samples, buffer[channel], block_size * sizeof(int32_t));
    }

    block->num_samples += block_size;

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

    context->pcm->num_samples = 0;

    return context;
}

void DecoderContext_destroy(AUXTS__DecoderContext* context) {
    free(context);
}
