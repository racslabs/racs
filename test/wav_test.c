#include "wav_test.h"
#include <math.h>

void test_write_wav() {
    auxts_int16 in[44100];

    double pi = 3.14159265359;
    double ts = 0.00002267573;

    for (int i = 0; i < 44100; ++i) {
        in[i] = (auxts_int16)(32767 * sin((double)i * 440 * 2 * pi * ts));
    }

    auxts_wav wav;
    auxts_wav_init(&wav, 1, 16, 44100);
    auxts_wav_write_s16(&wav, in, 44100);

    FILE* f = fopen("test1.wav", "wb");
    fwrite(wav.out_stream.data, 1, wav.out_stream.current_pos, f);

    fclose(f);
    auxts_wav_destroy(&wav);
}
