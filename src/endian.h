
#ifndef AUXTS_ENDIAN_H
#define AUXTS_ENDIAN_H

#include <stdint.h>
#include <stdio.h>
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUXTS_IS_LITTLE_ENDIAN (*(unsigned char *)&(unsigned short){1} == 1)

AUXTS_FORCE_INLINE uint64_t auxts_swap64_if_big_endian(uint64_t d) {
    return AUXTS_IS_LITTLE_ENDIAN ? d : __builtin_bswap64(d);
}

AUXTS_FORCE_INLINE uint32_t auxts_swap32_if_big_endian(uint32_t d) {
    return AUXTS_IS_LITTLE_ENDIAN ? d : __builtin_bswap32(d);
}

AUXTS_FORCE_INLINE uint16_t auxts_swap16_if_big_endian(uint16_t d) {
    return AUXTS_IS_LITTLE_ENDIAN ? d : __builtin_bswap16(d);
}

#ifdef __cplusplus
}
#endif

#endif //AUXTS_ENDIAN_H
