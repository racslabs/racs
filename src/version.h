
#ifndef RACS_VERSION_H
#define RACS_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define RACS_VERSION_MAJOR  0
#define RACS_VERSION_MINOR  1
#define RACS_VERSION_PATCH  0

#define RACS_VERSION ((RACS_VERSION_MAJOR * 10000) + \
                      (RACS_VERSION_MINOR * 100) +  \
                       RACS_VERSION_PATCH)

static __attribute__((always_inline)) void racs_version(char* buf) {
    sprintf(buf, "racs %d.%d.%d  (%s)", RACS_VERSION_MAJOR, RACS_VERSION_MINOR, RACS_VERSION_PATCH, __DATE__);
}

#ifdef __cplusplus
}
#endif

#endif //RACS_VERSION_H
