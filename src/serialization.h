
#ifndef AUXTS_SERIALIZATION_H
#define AUXTS_SERIALIZATION_H

#include <msgpack.h>
#include "extract.h"
#include "executor.h"
#include "t_complex.h"

typedef enum {
    AUXTS_TYPE_STR,
    AUXTS_TYPE_BIN,
    AUXTS_TYPE_INT,
    AUXTS_TYPE_FLOAT,
    AUXTS_TYPE_MAP,
    AUXTS_TYPE_LIST,
    AUXTS_TYPE_NONE,
    AUXTS_TYPE_ERROR,
    AUXTS_TYPE_BOOL,
    AUXTS_TYPE_U16VEC,
    AUXTS_TYPE_I16VEC,
    AUXTS_TYPE_U32VEC,
    AUXTS_TYPE_I32VEC,
    AUXTS_TYPE_F32VEC,
    AUXTS_TYPE_C64VEC
} auxts_type;

#define auxts_parse_args(in_buf, pk) \
    if (msgpack_unpack_next(&msg, (in_buf)->data, (in_buf)->size, 0) == MSGPACK_UNPACK_PARSE_ERROR) { \
        return auxts_serialize_error((pk), "Error parsing args");\
    }

extern const char* const auxts_type_string[];

int auxts_pack_none_with_status_ok(msgpack_packer* pk);
int auxts_pack_none_with_status_not_found(msgpack_packer* pk);
int auxts_serialize_error(msgpack_packer* pk, const char* message);
int auxts_serialize_str(msgpack_packer* pk, const char* str);
int auxts_serialize_bin(msgpack_packer* pk, const uint8_t* data, size_t n);
int auxts_serialize_int64(msgpack_packer* pk, int64_t d);
int auxts_serialize_float64(msgpack_packer* pk, double d);
int auxts_serialize_bool(msgpack_packer* pk, bool d);
int auxts_serialize_i16v(msgpack_packer* pk, int16_t* data, size_t n);
int auxts_serialize_u16v(msgpack_packer* pk, uint16_t* data, size_t n);
int auxts_serialize_i32v(msgpack_packer* pk, int32_t* data, size_t n);
int auxts_serialize_u32v(msgpack_packer* pk, uint32_t* data, size_t n);
int auxts_serialize_f32v(msgpack_packer* pk, float* data, size_t n);
int auxts_serialize_c64v(msgpack_packer* pk, auxts_complex_t* data, size_t n);
int auxts_serialize_pcm32(msgpack_packer* pk, const auxts_pcm_buffer* pbuf);
void auxts_serialize_type(msgpack_packer* pk, int type);
int auxts_serialize_invalid_num_args(msgpack_packer* pk, int expected, int actual);
void auxts_deserialize_stream_id(uint64_t* stream_id, msgpack_object* obj, int arg_num);
void auxts_deserialize_from(int64_t* from, msgpack_object* obj);
void auxts_deserialize_to(int64_t* to, msgpack_object* obj);
int auxts_deserialize_range(int64_t* from, int64_t* to, msgpack_object* obj);
char* auxts_deserialize_str(msgpack_object* obj, int n);
int32_t auxts_deserialize_int32(msgpack_object* obj, int n);
uint32_t auxts_deserialize_uint32(msgpack_object* obj, int n);
uint64_t auxts_deserialize_uint64(msgpack_object* obj, int n);
int32_t* auxts_deserialize_i32v(msgpack_object* obj, int n);
size_t auxts_deserialize_i32v_size(msgpack_object* obj, int n);
int auxts_is_object_type(msgpack_object* obj, msgpack_object_type type, int arg_num);

#endif //AUXTS_SERIALIZATION_H
