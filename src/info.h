
#ifndef RACS_INFO_H
#define RACS_INFO_H


#ifdef __cplusplus
extern "C" {
#endif


#include "fs.h"
#include "mmh3.h"
#include "types.h"
#include "config.h"
#include "racs_time.h"
#include <limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>


typedef struct __attribute__ ((packed)) {
    racs_time ref;
    racs_time ttl;
    racs_uint32 sample_rate;
    racs_uint8 channels;
    racs_uint8 bit_depth;
    racs_uint32 stream_id_size;
    char stream_id[];
}

racs_info;


racs_info *racs_info_create(const char *stream_id,
                            racs_uint32 sample_rate,
                            racs_uint8 channels,
                            racs_uint8 bit_depth);

racs_uint64 racs_info_hash(const char *stream_id);

void racs_info_path(char *path, const char *stream_id);

int racs_info_flush(racs_info *info, const char *path);

racs_info *racs_info_open(const char *path);

int racs_info_exist(const char *path);

racs_time racs_info_to_time(racs_info *info, racs_uint64 offset);

void racs_info_destroy(racs_info * info);

void racs_info_munmap(racs_info * info);


#ifdef __cplusplus
}
#endif

#endif //RACS_INFO_H
