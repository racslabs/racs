#include "wav_test.h"

void test_write_wav() {
    rats_int16 in[44100];

    double pi = 3.14159265359;
    double ts = 0.00002267573;

    for (int i = 0; i < 44100; ++i) {
        in[i] = (rats_int16)(32767 * sin((double)i * 440 * 2 * pi * ts));
    }

    void* out = malloc(44100 + 44);

    rats_wav wav;

    rats_wav_set_channels(&wav, 1);
    rats_wav_set_sample_rate(&wav, 44100);

    size_t size = rats_wav_write(&wav, in, out, 44100, 1024);

    FILE* f = fopen("test1.wav", "wb");
    fwrite(out, 1, size, f);
    free(out);

    fclose(f);
}
