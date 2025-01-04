
#ifndef AUXTS_ENDIAN_H
#define AUXTS_ENDIAN_H

#include <stdint.h>
#include <stdio.h>
#include "macro.h"

#define AUXTS_IS_LITTLE_ENDIAN (*(unsigned char *)&(unsigned short){1} == 1)

AUXTS_FORCE_INLINE uint64_t AUXTS__swap64_if_big_endian(uint64_t d) {
    return AUXTS_IS_LITTLE_ENDIAN ? d : __builtin_bswap64(d);
}

AUXTS_FORCE_INLINE uint32_t AUXTS__swap32_if_big_endian(uint32_t d) {
    return AUXTS_IS_LITTLE_ENDIAN ? d : __builtin_bswap32(d);
}

AUXTS_FORCE_INLINE uint16_t AUXTS__swap16_if_big_endian(uint16_t d) {
    return AUXTS_IS_LITTLE_ENDIAN ? d : __builtin_bswap16(d);
}

#endif //AUXTS_ENDIAN_H
