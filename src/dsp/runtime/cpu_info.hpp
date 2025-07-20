/*
 * Copyright (c) 2024 Computer Music Research Lab (https://cmrlab.ai)
 *
 * This file is part of ZForm.
 *
 * ZForm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ZForm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ZForm. If not, see <http://www.gnu.org/licenses/>.
 *
 * If you are using this library in a commercial, closed-source, or
 * proprietary project, you must purchase a ZForm license.
 */

#ifndef RACS_CPU_INFO_HPP
#define RACS_CPU_INFO_HPP

#include <string>
#include <cstring>
#include <sstream>
#include "../simd/constants.hpp"

namespace racs::runtime {

#if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
    #define RACS_SUPPORTS_INLINE_ASM 1
#else
    #define RACS_SUPPORTS_INLINE_ASM 0
#endif

typedef struct {
    uint8_t avx;
    uint8_t avx2;
    uint8_t neon;
} _simd;

RACS_INLINE std::string cpuid() {

    #if defined(RACS_SUPPORTS_INLINE_ASM) && !defined(__aarch64__)
        uint32_t eax, ebx, ecx, edx;

        eax = 0;

        __asm__ __volatile__("cpuid"
                : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                : "a"(eax));

        char cpuid[13];

        memcpy(cpuid, &ebx, 4);
        memcpy(cpuid + 4, &edx, 4);
        memcpy(cpuid + 8, &ecx, 4);

        cpuid[12] = '\0';

        if (strcmp(cpuid, "GenuineIntel") == 0) {
            return "x86_64 (Intel)";
        }

        if (strcmp(cpuid, "AuthenticAMD") == 0) {
            return "x86_64 (AMD)";
        }
    #endif

    return "x86_64";
}

RACS_INLINE std::string arch() {
    #ifdef __arm__
        return "ARM";
    #elif defined(__aarch64__)
        return "ARM64";
    #elif defined(__x86_64__) || defined(_M_X64)
        return cpuid();
    #elif defined(__i386__) || defined(_M_IX86)
        return "x86 (32-bit)";
    #else
        return "Unknown";
    #endif
}

RACS_INLINE std::string simd() {
    _simd s = {};

    #ifdef __AVX__
        s.avx = 1;
    #endif

    #ifdef __AVX2__
        s.avx2 = 1;
    #endif

    #ifdef __ARM_NEON__
        s.neon = 1;
    #endif

    std::ostringstream oss;

    oss << "simd support: ";
    oss << "avx=" << (s.avx ? "yes" : "no") << "|";
    oss << "avx2=" << (s.avx2 ? "yes" : "no") << "|";
    oss << "neon=" << (s.neon ? "yes" : "no");

    return oss.str();
}

}

#endif //RACS_CPU_INFO_HPP
