#include "commands.h"

auxts_create_command(extract) {
    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    if (msgpack_unpack_next(&msg, in_buf->data, in_buf->size, 0) == MSGPACK_UNPACK_PARSE_ERROR) {
        auxts_serialize_status_not_ok(&pk, AUXTS_COMMAND_STATUS_ERROR, "Error deserializing args for EXTRACT command");
        return AUXTS_COMMAND_STATUS_ERROR;
    }

    msgpack_object* obj = &msg.data;

    uint64_t stream_id;
    auxts_deserialize_stream_id(&stream_id, obj);

    int64_t from, to;
    auxts_deserialize_range(&from, &to, obj);
    if (from == -1 || to == -1) {
        auxts_serialize_status_not_ok(&pk, AUXTS_COMMAND_STATUS_ERROR, "Invalid RFC-3339 timestamp. Expected format: yyyy-MM-ddTHH:mm:ss.SSSZ");
        return AUXTS_COMMAND_STATUS_ERROR;
    }

    auxts_pcm_buffer pbuf;
    auxts_extract_pcm_status status = auxts_extract_pcm_data(ctx->cache, &pbuf, stream_id, from, to);

    if (status == AUXTS_EXTRACT_PCM_STATUS_OK) {
        auxts_serialize_pcm_buffer(&pk, &pbuf);
        auxts_pcm_buffer_destroy(&pbuf);
        return AUXTS_COMMAND_STATUS_OK;
    }

    if (status == AUXTS_EXTRACT_PCM_STATUS_NO_DATA) {
        auxts_serialize_status_not_ok(&pk, AUXTS_COMMAND_STATUS_NO_DATA, "No data found");
        return AUXTS_COMMAND_STATUS_NO_DATA;
    }

    auxts_serialize_status_not_ok(&pk, AUXTS_COMMAND_STATUS_ERROR, "Error running EXTRACT command");
    return AUXTS_COMMAND_STATUS_ERROR;
}
