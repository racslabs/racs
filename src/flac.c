#include "flac.h"

static FLAC__StreamDecoderReadStatus read_callback(const FLAC__StreamDecoder* decoder, FLAC__byte buffer[], size_t* bytes, void* client_data);
static FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data);
static void metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data);
static void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data);
static decoder_context* decoder_context_create(flac_encoded_block* block);

flac_encoded_blocks* auxts_flac_encoded_blocks_create() {
    flac_encoded_blocks* blocks = malloc(sizeof(flac_encoded_blocks));
    if (!blocks) {
        perror("Failed to allocate flac_encoded_blocks");
        return NULL;
    }

    blocks->capacity = AUXTS_INITIAL_FLAC_STREAM_CAPACITY;
    blocks->num_blocks = 0;

    blocks->blocks = malloc(blocks->capacity * sizeof(flac_encoded_block));
    if (!blocks->blocks) {
        perror("Failed to allocate blocks to flac_encoded_blocks");
        auxts_flac_encoded_blocks_destroy(blocks);
        return NULL;
    }

    return blocks;
}

pcm_block* auxts_decode_flac_block(flac_encoded_block* block) {
    if (!block) {
        return NULL;
    }

    FLAC__StreamDecoder* decoder = FLAC__stream_decoder_new();
    if (!decoder) {
        perror("Failed to allocate FLAC__StreamDecoder");
        return NULL;
    }

    decoder_context* ctx = decoder_context_create(block);
    if (!ctx) {
        return NULL;
    }

    FLAC__stream_decoder_init_stream(decoder, read_callback, NULL, NULL, NULL, NULL, write_callback, metadata_callback, error_callback, ctx);
    FLAC__stream_decoder_process_until_end_of_stream(decoder);
    FLAC__stream_decoder_delete(decoder);

    pcm_block* pcm = ctx->pcm;
    free(ctx);

    return pcm;
}

void auxts_flac_encoded_blocks_append(flac_encoded_blocks* blocks, uint8_t* block_data, uint16_t size) {
    if (!blocks) {
        return;
    }

    if (blocks->num_blocks == blocks->capacity) {
        blocks->capacity = 1 << blocks->capacity;

        flac_encoded_block** new_blocks = realloc(blocks->blocks, blocks->capacity * sizeof(flac_encoded_block));
        if (!new_blocks) {
            perror("Error reallocating blocks to flac_encoded_blocks");
            return;
        }

        blocks->blocks = new_blocks;
    }

    flac_encoded_block* block = malloc(sizeof(flac_encoded_block));
    if (!block) {
        perror("Failed to allocate flac_encoded_block");
        return;
    }

    block->data = block_data;
    block->size = size;

    blocks->blocks[blocks->num_blocks] = block;
    ++blocks->num_blocks;
}

void auxts_flac_encoded_blocks_destroy(flac_encoded_blocks* blocks) {
    for (int i = 0; i < blocks->num_blocks; ++i) {
        flac_encoded_block* block = blocks->blocks[i];
        free(block->data);
        free(block);
    }

    free(blocks->blocks);
    free(blocks);
}

void metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data) {
    (void)decoder;

    if (!client_data) {
        perror("decoder_context cannot be null");
        return;
    }

    pcm_block *block = ((decoder_context*) client_data)->pcm;
    if (!block) {
        perror("pcm_block cannot be null");
        return;
    }

    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
        block->channels = metadata->data.stream_info.channels;
        block->bits_per_sample = metadata->data.stream_info.bits_per_sample;
        block->sample_rate = metadata->data.stream_info.sample_rate;
        block->total_samples = metadata->data.stream_info.total_samples;

        block->data = malloc(block->channels * sizeof(int32_t*));
        if (!block->data) {
            perror("Failed to allocate data to pcm_block");
            return;
        }

        for (int channel = 0; channel < block->channels; ++channel) {
            block->data[channel] = malloc(block->total_samples * sizeof(int32_t));
            if (!block->data[channel]) {
                perror("Failed to allocate samples to pcm_block");
                return;
            }
        }
    }
}

FLAC__StreamDecoderReadStatus read_callback(const FLAC__StreamDecoder* decoder, FLAC__byte buffer[], size_t* bytes, void* client_data) {
    (void)decoder;

    if (!client_data) {
        perror("decoder_context cannot be null");
        return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
    }

    flac_encoded_block* block = ((decoder_context*) client_data)->flac;
    if (!block) {
        perror("flac_encoded_block cannot be null");
        return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
    }

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

    if (!client_data) {
        perror("decoder_context cannot be null");
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    pcm_block* block = ((decoder_context*) client_data)->pcm;
    if (!block) {
        perror("pcm_block cannot be null");
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    uint32_t block_size = frame->header.blocksize;
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

decoder_context* decoder_context_create(flac_encoded_block* block) {
    decoder_context* ctx = malloc(sizeof(decoder_context));
    if (!ctx) {
        perror("Error allocating decoder_context");
        return NULL;
    }

    ctx->flac = block;
    ctx->pcm = malloc(sizeof(pcm_block));
    if (!ctx->pcm) {
        perror("Error allocating pcm_block");
        free(ctx);
        return NULL;
    }

    ctx->pcm->num_samples = 0;

    return ctx;
}
