#include "flac_test.h"
#include <math.h>

void test_flac() {

    auxts_int16 data[44100*2*5];

    for (int i = 0; i < 44100; ++i) {
        data[i * 2] = (auxts_int16)(32767 * sin((float)i * 440 * 2 * 3.14159265359 * (1.0f/44100)));
        data[i * 2 + 1] = (auxts_int16)(32767 * sin((float)i * 440 * 2 * 3.14159265359 * (1.0f/44100)));
    }

    auxts_flac flac;
    flac.channels = 2;
    flac.samples_rate = 44100;
    flac.bit_depth = 16;
    flac.block_size = 4096;
    size_t n = auxts_flac_write_pcm_s16(&flac, data, 44100*5);

    FILE* f = fopen("test111.flac", "wb");
    fwrite(flac.out_stream.data, 1, n, f);
    fclose(f);





}