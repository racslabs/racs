#include "create.h"


void racs_create(const char *path,
                 racs_uint32 sample_rate,
                 racs_uint8 channels,
                 racs_uint8 bit_depth) {
    racs_fs_mkdir(path);

    racs_info *info = racs_info_create(sample_rate, channels, bit_depth);
    racs_info_flush(info, path);
    racs_info_destroy(info);
}