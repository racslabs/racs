
#ifndef AUXTS_EXPORT_H
#define AUXTS_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#define AUXTS__FORCE_INLINE inline __attribute__((always_inline))

#define AUXTS_API

extern AUXTS_API const int AUXTS__ALIGN;

#ifdef __cplusplus
}
#endif

#endif //AUXTS_EXPORT_H
