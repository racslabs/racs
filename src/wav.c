#include "wav.h"

void auxts_wav_init(auxts_wav* wav, auxts_uint16 channels, auxts_uint16 bit_depth, auxts_uint32 sample_rate) {
    memset(wav, 0, sizeof(auxts_wav));

    wav->format.channels = channels;
    wav->format.sample_rate = sample_rate;
    wav->format.bit_depth = bit_depth;

    size_t size = 1024 * channels * wav->format.bit_depth/8;

    void* data = malloc(size);
    if (!data) {
        perror("Failed to allocate data to auxts_wav");
        return;
    }

    auxts_memory_stream_init(&wav->data.memory_stream, data, size);
}

size_t auxts_wav_write_s24(auxts_wav* wav, auxts_int24* in, size_t samples) {
    return auxts_wav_write(wav, in, samples);
}

size_t auxts_wav_write_s16(auxts_wav* wav, auxts_int16* in, size_t samples) {
    return auxts_wav_write(wav, in, samples);
}

size_t auxts_wav_write(auxts_wav* wav, void* in, size_t samples) {
    auxts_wav_encode_header(wav, samples);
    auxts_wav_encode_format(wav);
    auxts_wav_encode_data(wav, in, samples);

    return wav->data.memory_stream.current_pos;
}

void auxts_wav_encode_header(auxts_wav* wav, auxts_uint32 samples) {
    memcpy(wav->header.chunk_id, "RIFF", 4);
    wav->header.chunk_size = samples * wav->format.channels * wav->format.bit_depth/8;
    memcpy(wav->header.format, "WAVE", 4);

    auxts_memory_stream_write(&wav->data.memory_stream, wav->header.chunk_id, 4);
    auxts_memory_stream_write(&wav->data.memory_stream, &wav->header.chunk_size, 4);
    auxts_memory_stream_write(&wav->data.memory_stream, wav->header.format, 4);
}

void auxts_wav_encode_format(auxts_wav* wav) {
    wav->format.block_align = wav->format.channels * wav->format.bit_depth/8;
    wav->format.byte_rate = wav->format.sample_rate * wav->format.block_align;
    wav->format.audio_format = 1;

    memcpy(wav->format.sub_chunk1_id, "fmt ", 4);

    auxts_memory_stream_write(&wav->data.memory_stream, &wav->format.sub_chunk1_id, 4);
    auxts_memory_stream_write(&wav->data.memory_stream, &wav->format.sub_chunk1_size, 4);
    auxts_memory_stream_write(&wav->data.memory_stream, &wav->format.audio_format, 2);
    auxts_memory_stream_write(&wav->data.memory_stream, &wav->format.channels, 2);
    auxts_memory_stream_write(&wav->data.memory_stream, &wav->format.sample_rate, 4);
    auxts_memory_stream_write(&wav->data.memory_stream, &wav->format.byte_rate, 4);
    auxts_memory_stream_write(&wav->data.memory_stream, &wav->format.block_align, 2);
    auxts_memory_stream_write(&wav->data.memory_stream, &wav->format.bit_depth, 2);
}

void auxts_wav_encode_data(auxts_wav* wav, void* data, auxts_uint32 samples) {
    memcpy(wav->data.sub_chunk2_id, "data", 4);
    wav->data.sub_chunk2_size  = samples * wav->format.channels * wav->format.bit_depth/8;

    auxts_memory_stream_write(&wav->data.memory_stream, wav->data.sub_chunk2_id, 4);
    auxts_memory_stream_write(&wav->data.memory_stream, &wav->data.sub_chunk2_size, 4);
    auxts_memory_stream_write(&wav->data.memory_stream, data, wav->data.sub_chunk2_size);
}
