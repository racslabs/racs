
#ifndef RACS_EXPORT_H
#define RACS_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#define RACS_FORCE_INLINE inline __attribute__((always_inline))

#define RACS_PACKED __attribute__((packed))

#define RACS_ALIGN 32

#ifdef __cplusplus
}
#endif

#endif //RACS_EXPORT_H
