
#ifndef AUXTS_SERIALIZATION_H
#define AUXTS_SERIALIZATION_H

#include <msgpack.h>
#include "extract.h"
#include "command_exec.h"

#define AUXTS_PARSE_ARGS(in_buf, pk, error) \
    msgpack_unpacked msg;                             \
    msgpack_unpacked_init(&msg);                      \
                                                      \
    if (msgpack_unpack_next(&msg, (in_buf)->data, (in_buf)->size, 0) == \
        MSGPACK_UNPACK_PARSE_ERROR) {                 \
        auxts_serialize_status_not_ok((pk), AUXTS_COMMAND_STATUS_ERROR, \
            (error));                             \
        return AUXTS_COMMAND_STATUS_ERROR;            \
    }                                                 \
                                                      \
    msgpack_object obj = msg.data;

extern const char* const auxts_status_code[];

void auxts_serialize_invalid_num_args(msgpack_packer* pk, int expected, int actual);
void auxts_serialize_status_ok(msgpack_packer* pk);
int auxts_serialize_status_not_ok(msgpack_packer* pk, int status, const char* message);
void auxts_serialize_message(msgpack_packer* pk, const char* message);
void auxts_serialize_status(msgpack_packer* pk, int status);
void auxts_serialize_pcm_buffer(msgpack_packer* pk, const auxts_pcm_buffer* pbuf);
void auxts_deserialize_stream_id(uint64_t* stream_id, msgpack_object* obj);
void auxts_deserialize_from(int64_t* from, msgpack_object* obj);
void auxts_deserialize_to(int64_t* to, msgpack_object* obj);
int auxts_deserialize_range(int64_t* from, int64_t* to, msgpack_object* obj);

#endif //AUXTS_SERIALIZATION_H
