#include "commands.h"

AUXTS_CREATE_COMMAND(extract) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    AUXTS_PARSE_ARGS(in_buf, &pk)
    AUXTS_CHECK_NUM_ARGS(&pk, msg, 3)

    msgpack_object obj = msg.data;

    uint64_t stream_id;
    auxts_deserialize_stream_id(&stream_id, &obj);

    int64_t from, to;
    if (auxts_deserialize_range(&from, &to, &obj)) {
        return auxts_serialize_status_error(&pk,"Invalid RFC-3339 timestamp. Expected format: yyyy-MM-ddTHH:mm:ss.SSSZ");
    }

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
