#include "wav_test.h"
#include <math.h>

void test_write_wav() {
    auxts_int16 in[44100];

    double pi = 3.14159265359;
    double ts = 0.00002267573;

    for (int i = 0; i < 44100; ++i) {
        in[i] = (auxts_int16)(32767 * sin((double)i * 440 * 2 * pi * ts));
    }

    void* out = malloc(1024);

    auxts_wav wav;

    auxts_wav_set_channels(&wav, 1);
    auxts_wav_set_sample_rate(&wav, 44100);

    size_t size = auxts_wav_write(&wav, in, out, 44100, 1024);

    FILE* f = fopen("test1.wav", "wb");
    fwrite(wav.out_stream.data, 1, size, f);

    fclose(f);
    auxts_wav_destroy(&wav);
}
