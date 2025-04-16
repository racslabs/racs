#include "wav.h"

void auxts_wav_set_channels(auxts_wav* wav, auxts_uint16 channels) {
    wav->format.channels = channels;
}

void auxts_wav_set_bit_depth(auxts_wav* wav, auxts_uint16 bit_depth) {
    wav->format.bit_depth = bit_depth;
}

void auxts_wav_set_sample_rate(auxts_wav* wav, auxts_uint32 sample_rate) {
    wav->format.sample_rate = sample_rate;
}

size_t auxts_wav_write_s16(auxts_wav* wav, const auxts_int16* in, void* out, size_t samples, size_t size) {
    if (wav->format.channels == 2) {
        auxts_int16* _in = malloc(wav->format.channels * samples * sizeof(auxts_int16));
        auxts_simd_planar_s16(in, _in, wav->format.channels * samples);

        size_t _size = auxts_wav_write(wav, _in, out, samples, size);

        free(_in);
        return _size;
    }

    return auxts_wav_write(wav, in, out, samples, size);
}

/* For internal use ONLY */
size_t auxts_wav_write(auxts_wav* wav, const void* in, void* out, size_t samples, size_t size) {
    auxts_memory_stream_init(&wav->out_stream, out, size);

    auxts_wav_encode_header(wav, samples);
    auxts_wav_encode_format(wav);
    auxts_wav_encode_data(wav, in, samples);

    return wav->out_stream.current_pos;
}

void auxts_wav_encode_header(auxts_wav* wav, auxts_uint32 samples) {
    memcpy(wav->header.chunk_id, "RIFF", 4);
    wav->header.chunk_size = (samples * wav->format.channels * wav->format.bit_depth/8) + 36;
    memcpy(wav->header.format, "WAVE", 4);

    auxts_memory_stream_write(&wav->out_stream, wav->header.chunk_id, 4);
    auxts_memory_stream_write(&wav->out_stream, &wav->header.chunk_size, 4);
    auxts_memory_stream_write(&wav->out_stream, wav->header.format, 4);
}

void auxts_wav_encode_format(auxts_wav* wav) {
    wav->format.block_align = wav->format.channels * wav->format.bit_depth/8;
    wav->format.byte_rate = wav->format.sample_rate * wav->format.block_align;
    wav->format.sub_chunk1_size = 16;
    wav->format.audio_format = 1;

    memcpy(wav->format.sub_chunk1_id, "fmt ", 4);

    auxts_memory_stream_write(&wav->out_stream, &wav->format.sub_chunk1_id, 4);
    auxts_memory_stream_write(&wav->out_stream, &wav->format.sub_chunk1_size, 4);
    auxts_memory_stream_write(&wav->out_stream, &wav->format.audio_format, 2);
    auxts_memory_stream_write(&wav->out_stream, &wav->format.channels, 2);
    auxts_memory_stream_write(&wav->out_stream, &wav->format.sample_rate, 4);
    auxts_memory_stream_write(&wav->out_stream, &wav->format.byte_rate, 4);
    auxts_memory_stream_write(&wav->out_stream, &wav->format.block_align, 2);
    auxts_memory_stream_write(&wav->out_stream, &wav->format.bit_depth, 2);
}

void auxts_wav_encode_data(auxts_wav* wav, const void* data, auxts_uint32 samples) {
    memcpy(wav->data.sub_chunk2_id, "data", 4);
    wav->data.sub_chunk2_size = samples * wav->format.channels * wav->format.bit_depth/8;

    auxts_memory_stream_write(&wav->out_stream, wav->data.sub_chunk2_id, 4);
    auxts_memory_stream_write(&wav->out_stream, &wav->data.sub_chunk2_size, 4);
    auxts_memory_stream_write(&wav->out_stream, data, wav->data.sub_chunk2_size);
}

void auxts_wav_destroy(auxts_wav* wav) {
    free(wav->out_stream.data);
}
