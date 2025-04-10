#include "wav.h"

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
