#ifndef RACS_ATOMIC_H
#define RACS_ATOMIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include <stdatomic.h>
#include <stdbool.h>

typedef _Atomic(racs_int8)   racs_atomic_int8;
typedef _Atomic(racs_uint8)  racs_atomic_uint8;

typedef _Atomic(racs_int16)  racs_atomic_int16;
typedef _Atomic(racs_uint16) racs_atomic_uint16;

typedef _Atomic(racs_int32)  racs_atomic_int32;
typedef _Atomic(racs_uint32) racs_atomic_uint32;

typedef _Atomic(racs_int64)  racs_atomic_int64;
typedef _Atomic(racs_uint64) racs_atomic_uint64;

typedef _Atomic(size_t)      racs_atomic_size;
typedef _Atomic(bool)        racs_atomic_bool;


#define racs_atomic_load(ptr) \
    atomic_load_explicit((ptr), memory_order_seq_cst)

#define racs_atomic_store(ptr, val) \
    atomic_store_explicit((ptr), (val), memory_order_seq_cst)

#define racs_atomic_add(ptr, val) \
    atomic_fetch_add_explicit((ptr), (val), memory_order_seq_cst)

#define racs_atomic_sub(ptr, val) \
    atomic_fetch_sub_explicit((ptr), (val), memory_order_seq_cst)

#define racs_atomic_swap(ptr, val) \
    atomic_exchange_explicit((ptr), (val), memory_order_seq_cst)

#define racs_atomic_cas(ptr, exp, des) \
    atomic_compare_exchange_strong_explicit( \
        (ptr), (exp), (des), \
        memory_order_seq_cst, memory_order_seq_cst)

static inline void racs_cpu_pause(void) {
    #if defined(__x86_64__) || defined(_M_X64)
        __builtin_ia32_pause();
    #elif defined(__aarch64__)
        __asm__ volatile("yield" ::: "memory");
    #endif
}

#ifdef __cplusplus
}
#endif

#endif // RACS_ATOMIC_H