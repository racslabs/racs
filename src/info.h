
#ifndef RACS_INFO_H
#define RACS_INFO_H


#ifdef __cplusplus
extern "C" {
#endif


#include "fs.h"
#include "types.h"
#include "racs_time.h"
#include <limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>


typedef struct __attribute__((packed)) {
    racs_time       ref;
    racs_time       ttl;
    racs_uint32     sample_rate;
    racs_uint8      channels;
    racs_uint8      bit_depth;
} racs_info;


racs_info *racs_info_create(racs_uint32 sample_rate,
                            racs_uint8  channels,
                            racs_uint8  bit_depth);

int racs_info_flush(racs_info *info, const char *path);

racs_info *racs_info_open(const char *path);

int racs_info_exist(const char *path);

racs_time racs_info_to_time(racs_info *info, racs_uint64 offset);

void racs_info_destroy(racs_info *info);


#ifdef __cplusplus
}
#endif

#endif //RACS_INFO_H
