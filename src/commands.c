#include "commands.h"

auxts_create_command(extract) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    auxts_parse_args(in_buf, &pk)
    auxts_validate_num_args(&pk, msg, 3)

    uint64_t stream_id;
    int64_t from, to;

    auxts_validate_arg_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected: string")
    auxts_validate(&pk, auxts_deserialize_range(&from, &to, &msg.data), "Invalid RFC-3339 timestamp. Expected format: yyyy-MM-ddTHH:mm:ss.SSSZ")
    auxts_deserialize_stream_id(&stream_id, &msg.data, 0);

    auxts_pcm_buffer pbuf;
    auxts_extract_pcm_status status = auxts_extract_pcm_data(ctx->cache, &pbuf, stream_id, from, to);

    if (status == AUXTS_EXTRACT_PCM_STATUS_OK) {
        return auxts_serialize_pcm_buffer(&pk, &pbuf);
    }

    if (status == AUXTS_EXTRACT_PCM_STATUS_NO_DATA) {
        return auxts_serialize_status_not_found(&pk);
    }

    return auxts_serialize_status_error(&pk, "Cause unknown");
}
