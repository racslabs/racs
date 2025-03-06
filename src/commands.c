#include "commands.h"

auxts_create_command(extract) {
    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    if (msgpack_unpack_next(&msg, in_buf->data, in_buf->size, 0) != MSGPACK_UNPACK_PARSE_ERROR) {
    }

    msgpack_object* obj = &msg.data;

    uint64_t stream_id;
    auxts_deserialize_stream_id(&stream_id, obj);

    int64_t from;
    auxts_deserialize_from(&from, obj);
    if (from == -1) {
    }

    int64_t to;
    auxts_deserialize_to(&to, obj);
    if (to == -1) {
    }

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    auxts_pcm_buffer pbuf;
    auxts_extract_pcm_status status = auxts_extract_pcm_data(ctx->cache, &pbuf, stream_id, from, to);

    //TODO: abstract into another method
    if (status == AUXTS_EXTRACT_PCM_STATUS_OK) {
        auxts_serialize_status_ok(&pk, &pbuf);
        auxts_pcm_buffer_destroy(&pbuf);
    } else {
        auxts_serialize_status_not_ok(&pk, status);
    }
}
