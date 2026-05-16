#include "pack.h"


void racs_pack_obj(msgpack_sbuffer *buf, msgpack_object obj) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, buf, msgpack_sbuffer_write);
    msgpack_pack_object(&pk, obj);
}

void racs_pack_err(msgpack_sbuffer *buf, const char *msg) {
    msgpack_sbuffer_clear(buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, buf, msgpack_sbuffer_write);
    msgpack_pack_ext_with_body(&pk, msg, strlen(msg), RACS_EXT_ERR);
}
