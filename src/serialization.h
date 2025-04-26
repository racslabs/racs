
#ifndef RATS_SERIALIZATION_H
#define RATS_SERIALIZATION_H

#include <msgpack.h>
#include "extract.h"
#include "exec.h"
#include "types.h"

typedef enum {
    RATS_TYPE_STR,
    RATS_TYPE_INT,
    RATS_TYPE_FLOAT,
    RATS_TYPE_LIST,
    RATS_TYPE_NULL,
    RATS_TYPE_ERROR,
    RATS_TYPE_BOOL,
    RATS_TYPE_U8VEC,
    RATS_TYPE_I8VEC,
    RATS_TYPE_U16VEC,
    RATS_TYPE_I16VEC,
    RATS_TYPE_U32VEC,
    RATS_TYPE_I32VEC,
    RATS_TYPE_F32VEC,
    RATS_TYPE_C64VEC
} rats_type;

#define rats_parse_buf(buf, pk, msg, message) \
    if (msgpack_unpack_next(msg, (buf)->data, (buf)->size, 0) == MSGPACK_UNPACK_PARSE_ERROR) { \
        return rats_serialize_error((pk), message);\
    }

extern const char *const rats_type_string[];

int rats_serialize_null_with_status_ok(msgpack_packer *pk);

int rats_serialize_null_with_status_not_found(msgpack_packer *pk);

int rats_serialize_error(msgpack_packer *pk, const char *message);

int rats_serialize_str(msgpack_packer *pk, const char *str);

int rats_serialize_int64(msgpack_packer *pk, rats_int64 d);

int rats_serialize_uint64(msgpack_packer *pk, rats_uint64 d);

int rats_serialize_float64(msgpack_packer *pk, double d);

int rats_serialize_bool(msgpack_packer *pk, bool d);

int rats_serialize_s8v(msgpack_packer *pk, rats_int8 *data, size_t n);

int rats_serialize_u8v(msgpack_packer *pk, rats_uint8 *data, size_t n);

int rats_serialize_s16v(msgpack_packer *pk, rats_int16 *data, size_t n);

int rats_serialize_u16v(msgpack_packer *pk, rats_uint16 *data, size_t n);

int rats_serialize_s32v(msgpack_packer *pk, rats_int32 *data, size_t n);

int rats_serialize_u32v(msgpack_packer *pk, rats_uint32 *data, size_t n);

int rats_serialize_f32v(msgpack_packer *pk, float *data, size_t n);

int rats_serialize_c64v(msgpack_packer *pk, rats_complex *data, size_t n);

void rats_serialize_type(msgpack_packer *pk, int type);

int rats_serialize_invalid_num_args(msgpack_packer *pk, int expected, int actual);

char *rats_deserialize_str(msgpack_object *obj, int n);

rats_uint16 rats_deserialize_uint16(msgpack_object *obj, int n);

rats_int32 rats_deserialize_int32(msgpack_object *obj, int n);

rats_uint32 rats_deserialize_uint32(msgpack_object *obj, int n);

rats_int64 rats_deserialize_int64(msgpack_object *obj, int n);

rats_uint64 rats_deserialize_uint64(msgpack_object *obj, int n);

rats_uint8 *rats_deserialize_u8v(msgpack_object *obj, int n);

rats_int16 *rats_deserialize_s16v(msgpack_object *obj, int n);

rats_int32 *rats_deserialize_s32v(msgpack_object *obj, int n);

size_t rats_deserialize_u8v_size(msgpack_object *obj, int n);

size_t rats_deserialize_s16v_size(msgpack_object *obj, int n);

size_t rats_deserialize_s32v_size(msgpack_object *obj, int n);

int rats_is_object_type(msgpack_object *obj, msgpack_object_type type, int arg_num);

#endif //RATS_SERIALIZATION_H
