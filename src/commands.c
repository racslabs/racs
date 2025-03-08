#include "commands.h"

AUXTS_CREATE_COMMAND(extract) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    AUXTS_PARSE_ARGS(in_buf, &pk, "Error parsing args for EXTRACT command")
    AUXTS_CHECK_NUM_ARGS(&pk, obj, 3)

    uint64_t stream_id;
    auxts_deserialize_stream_id(&stream_id, &obj);

    int64_t from, to;
    if (auxts_deserialize_range(&from, &to, &obj)) {
        return auxts_serialize_status_not_ok(&pk, AUXTS_COMMAND_STATUS_ERROR, "Invalid RFC-3339 timestamp. Expected format: yyyy-MM-ddTHH:mm:ss.SSSZ");
    }

    auxts_pcm_buffer pbuf;
    auxts_extract_pcm_status status = auxts_extract_pcm_data(ctx->cache, &pbuf, stream_id, from, to);

    if (status == AUXTS_EXTRACT_PCM_STATUS_OK) {
        auxts_serialize_pcm_buffer(&pk, &pbuf);
        auxts_pcm_buffer_destroy(&pbuf);
        return AUXTS_COMMAND_STATUS_OK;
    }

    if (status == AUXTS_EXTRACT_PCM_STATUS_NO_DATA) {
        return auxts_serialize_status_not_ok(&pk, AUXTS_COMMAND_STATUS_NO_DATA, "No data found");
    }

    return auxts_serialize_status_not_ok(&pk, AUXTS_COMMAND_STATUS_ERROR, "Error running EXTRACT command");
}
