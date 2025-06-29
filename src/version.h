
#ifndef RACS_VERSION_H
#define RACS_VERSION_H

#include "export.h"
#include <stdio.h>

#define RACS_VERSION_MAJOR  0
#define RACS_VERSION_MINOR  0
#define RACS_VERSION_PATCH  1

#define RACS_VERSION ((RACS_VERSION_MAJOR * 10000) + \
                      (RACS_VERSION_MINOR * 100) +  \
                       RACS_VERSION_PATCH)

RACS_FORCE_INLINE void racs_version(char* buf) {
    sprintf(buf, "RACS %d.%d.%d  (%s)", RACS_VERSION_MAJOR, RACS_VERSION_MINOR, RACS_VERSION_PATCH, __DATE__);
}

#endif //RACS_VERSION_H
