
#ifndef AUXTS_SERIALIZATION_H
#define AUXTS_SERIALIZATION_H

#include <msgpack.h>
#include "extract.h"
#include "command_exec.h"


extern const char* const auxts_status_code[];

void auxts_serialize_status_ok(msgpack_packer* pk);
void auxts_serialize_status_not_ok(msgpack_packer* pk, int status, const char* message);
void auxts_serialize_message(msgpack_packer* pk, const char* message);
void auxts_serialize_status(msgpack_packer* pk, int status);
void auxts_serialize_pcm_buffer(msgpack_packer* pk, const auxts_pcm_buffer* pbuf);
void auxts_deserialize_stream_id(uint64_t* stream_id, msgpack_object* obj);
void auxts_deserialize_from(int64_t* from, msgpack_object* obj);
void auxts_deserialize_to(int64_t* to, msgpack_object* obj);

#endif //AUXTS_SERIALIZATION_H
