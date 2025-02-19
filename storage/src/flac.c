#include "flac.h"

static FLAC__StreamDecoderReadStatus read_callback(const FLAC__StreamDecoder* decoder, FLAC__byte buffer[], size_t* bytes, void* client_data);
static FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data);
static void metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data);
static void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data);
static DecoderContext* decoder_context_create(FlacEncodedBlock* block);

FlacEncodedBlocks* auxts_flac_encoded_blocks_create() {
    FlacEncodedBlocks* blocks = malloc(sizeof(FlacEncodedBlocks));
    if (!blocks) {
        perror("Failed to allocate FlacEncodedBlocks");
        return NULL;
    }

    blocks->capacity = AUXTS_INITIAL_FLAC_STREAM_CAPACITY;
    blocks->num_blocks = 0;

    blocks->blocks = malloc(blocks->capacity * sizeof(FlacEncodedBlock));
    if (!blocks->blocks) {
        perror("Failed to allocate blocks to FlacEncodedBlocks");
        auxts_flac_encoded_blocks_destroy(blocks);
        return NULL;
    }

    return blocks;
}

PcmBlock* auxts_decode_flac_block(FlacEncodedBlock* block) {
    if (!block) {
        return NULL;
    }

    FLAC__StreamDecoder* decoder = FLAC__stream_decoder_new();
    if (!decoder) {
        perror("Failed to allocate FLAC__StreamDecoder");
        return NULL;
    }

    DecoderContext* context = decoder_context_create(block);
    if (!context) {
        return NULL;
    }

    FLAC__stream_decoder_init_stream(decoder, read_callback, NULL, NULL, NULL, NULL, write_callback, metadata_callback, error_callback, context);
    FLAC__stream_decoder_process_until_end_of_stream(decoder);
    FLAC__stream_decoder_delete(decoder);

    PcmBlock* pcm = context->pcm;
    free(context);

    return pcm;
}

void auxts_flac_encoded_blocks_append(FlacEncodedBlocks* blocks, uint8_t* block_data, uint16_t size) {
    if (!blocks) {
        return;
    }

    if (blocks->num_blocks == blocks->capacity) {
        blocks->capacity = 1 << blocks->capacity;

        FlacEncodedBlock** new_blocks = realloc(blocks->blocks, blocks->capacity * sizeof(FlacEncodedBlock));
        if (!new_blocks) {
            perror("Error reallocating blocks to FlacEncodedBlocks");
            return;
        }

        blocks->blocks = new_blocks;
    }

    FlacEncodedBlock* block = malloc(sizeof(FlacEncodedBlock));
    if (!block) {
        perror("Failed to allocate FlacEncodedBlock");
        return;
    }

    block->data = block_data;
    block->size = size;

    blocks->blocks[blocks->num_blocks] = block;
    ++blocks->num_blocks;
}

void auxts_flac_encoded_blocks_destroy(FlacEncodedBlocks* blocks) {
    for (int i = 0; i < blocks->num_blocks; ++i) {
        FlacEncodedBlock* block = blocks->blocks[i];
        free(block->data);
        free(block);
    }

    free(blocks->blocks);
    free(blocks);
}

void metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data) {
    (void)decoder;

    PcmBlock *block = ((DecoderContext*) client_data)->pcm;

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

    FlacEncodedBlock* block = ((DecoderContext*) client_data)->flac;
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
    PcmBlock* block = ((DecoderContext*) client_data)->pcm;

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

DecoderContext* decoder_context_create(FlacEncodedBlock* block) {
    DecoderContext* context = malloc(sizeof(DecoderContext));
    if (!context) {
        perror("Error allocating DecoderContext");
        return NULL;
    }

    context->flac = block;
    context->pcm = malloc(sizeof(PcmBlock));
    if (!context->pcm) {
        perror("Error allocating PcmBlock");
        free(context);
        return NULL;
    }

    context->pcm->num_samples = 0;

    return context;
}
