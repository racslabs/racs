
#ifndef RACS_PACK_H
#define RACS_PACK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <msgpack.h>


#define RACS_EXT_ERR 42


void racs_pack_obj(msgpack_sbuffer *buf, msgpack_object obj);

void racs_pack_err(msgpack_sbuffer *buf, const char *msg);

#ifdef __cplusplus
}
#endif

#endif //RACS_PACK_H
