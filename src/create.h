
#ifndef RACS_CREATE_H
#define RACS_CREATE_H


#ifdef __cplusplus
extern "C" {
#endif


#include "fs.h"
#include "info.h"


void racs_create(const char *path,
                 racs_uint32 sample_rate,
                 racs_uint8 channels,
                 racs_uint8 bit_depth);


#ifdef __cplusplus
}
#endif

#endif // RACS_CREATE_H