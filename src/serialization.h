
#ifndef AUXTS_SERIALIZATION_H
#define AUXTS_SERIALIZATION_H

#include <msgpack.h>
#include "extract.h"
#include "executor.h"

#define auxts_parse_args(in_buf, pk) \
    if (msgpack_unpack_next(&msg, (in_buf)->data, (in_buf)->size, 0) == MSGPACK_UNPACK_PARSE_ERROR) { \
        return auxts_serialize_status_error((pk), "Error parsing args");\
    }

extern const char* const auxts_status_code[];

int auxts_serialize_invalid_num_args(msgpack_packer* pk, int expected, int actual);
int auxts_serialize_status_ok(msgpack_packer* pk);
int auxts_serialize_status_not_found(msgpack_packer* pk);
int auxts_serialize_status_error(msgpack_packer* pk, const char* message);
void auxts_serialize_message(msgpack_packer* pk, const char* message);
int auxts_serialize_status(msgpack_packer* pk, int status);
int auxts_serialize_pcm_buffer(msgpack_packer* pk, auxts_pcm_buffer* pbuf);
void auxts_deserialize_stream_id(uint64_t* stream_id, msgpack_object* obj, int arg_num);
void auxts_deserialize_from(int64_t* from, msgpack_object* obj);
void auxts_deserialize_to(int64_t* to, msgpack_object* obj);
int auxts_deserialize_range(int64_t* from, int64_t* to, msgpack_object* obj);
char* auxts_deserialize_str(msgpack_object* obj, int arg_num);
int32_t auxts_deserialize_int32(msgpack_object* obj, int arg_num);
uint32_t auxts_deserialize_uint32(msgpack_object* obj, int arg_num);
uint64_t auxts_deserialize_uint64(msgpack_object* obj, int arg_num);
int auxts_is_object_type(msgpack_object* obj, msgpack_object_type type, int arg_num);

#endif //AUXTS_SERIALIZATION_H
