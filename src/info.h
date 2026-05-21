
#ifndef RACS_INFO_H
#define RACS_INFO_H


#ifdef __cplusplus
extern "C" {
#endif


#include "types.h"
#include "racs_time.h"
#include <limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>


typedef struct __attribute__((packed)) {
    racs_time       ref;
    racs_time       ttl;
    racs_uint32     sample_rate;
    racs_uint8      channels;
    racs_uint8      bit_depth;
    char            name[PATH_MAX];
} racs_info;


void racs_info_init(racs_info  *info,
                    const char *name,
                    racs_uint32 sample_rate,
                    racs_uint8  channels,
                    racs_uint8  bit_depth);

int racs_info_flush(racs_info *info, const char *path);


#ifdef __cplusplus
}
#endif

#endif //RACS_INFO_H
