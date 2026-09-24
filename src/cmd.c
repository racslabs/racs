
#include "cmd.h"


const racs_cmd cmds[3] = {
    {"ping", racs_cmd_ping, 1},
    {"create", racs_cmd_create, 0},
    {"stream", racs_cmd_stream, 0},
};


int racs_cmd_arg_str(msgpack_object *obj, char **arg, size_t *size);

int racs_cmd_arg_bin(msgpack_object *obj, racs_uint8 **arg, size_t *size);

int racs_cmd_arg_uint64(msgpack_object *obj, racs_uint64 *arg);

int racs_cmd_arg_uint32(msgpack_object *obj, racs_uint32 *arg);

int racs_cmd_arg_uint16(msgpack_object *obj, racs_uint16 * arg);

int racs_cmd_arg_uint8(msgpack_object *obj, racs_uint8 *arg);

void racs_cmd_call_ping(racs_ctx * ctx);

void racs_cmd_call_create(racs_ctx *ctx,
                          const char *stream_id,
                          racs_uint32 sample_rate,
                          racs_uint8 channels,
                          racs_uint8 bit_depth);

void racs_cmd_call_stream(racs_ctx *ctx,
                          const char *stream_id,
                          const char *codec,
                          const racs_uint8 *src,
                          size_t src_size);


racs_cmd_func racs_cmd_lookup(const char *name) {
    size_t num_cmds = sizeof(cmds) / sizeof(cmds[0]);

    for (size_t i = 0; i < num_cmds; i++) {
        racs_cmd cmd = cmds[i];

        if (strlen(name) == strlen(cmd.name) && strcasecmp(cmd.name, name) == 0) {
            return cmd.func;
        }
    }

    return NULL;
}

int racs_cmd_arg_str(msgpack_object *obj, char **arg, size_t *size) {
    if (obj->type != MSGPACK_OBJECT_STR) {
        return -1;
    }

    if (arg) {
        *arg = (char *) obj->via.str.ptr;
    }

    if (size) {
        *size = obj->via.str.size;
    }

    return 0;
}

int racs_cmd_arg_bin(msgpack_object *obj, racs_uint8 **arg, size_t *size) {
    if (obj->type != MSGPACK_OBJECT_BIN) {
        return -1;
    }

    if (arg) {
        *arg = (racs_uint8 *) obj->via.bin.ptr;
    }

    if (size) {
        *size = obj->via.bin.size;
    }

    return 0;
}

int racs_cmd_arg_uint64(msgpack_object *obj, racs_uint64 *arg) {
    if (obj->type != MSGPACK_OBJECT_POSITIVE_INTEGER) {
        return -1;
    }

    if (arg) {
        *arg = obj->via.u64;
    }

    return 0;
}

int racs_cmd_arg_uint32(msgpack_object *obj, racs_uint32 *arg) {
    if (obj->type != MSGPACK_OBJECT_POSITIVE_INTEGER || obj->via.u64 > 0xffffffff) {
        return -1;
    }

    if (arg) {
        *arg = (racs_uint32) obj->via.u64;
    }

    return 0;
}

int racs_cmd_arg_uint16(msgpack_object *obj, racs_uint16 *arg) {
    if (obj->type != MSGPACK_OBJECT_POSITIVE_INTEGER || obj->via.u64 > 0xffff) {
        return -1;
    }

    if (arg) {
        *arg = (racs_uint16) obj->via.u64;
    }

    return 0;
}

int racs_cmd_arg_uint8(msgpack_object *obj, racs_uint8 *arg) {
    if (obj->type != MSGPACK_OBJECT_POSITIVE_INTEGER || obj->via.u64 > 0xff) {
        return -1;
    }

    if (arg) {
        *arg = (racs_uint8) obj->via.u64;
    }

    return 0;
}

void racs_cmd_error(racs_ctx *ctx, const char *msg) {
    ctx->has_error = 1;
    racs_pack_err(&ctx->out_buf, msg);
}

void racs_cmd_call_ping(racs_ctx *ctx) {
    msgpack_sbuffer_clear(&ctx->out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &ctx->out_buf, msgpack_sbuffer_write);
    msgpack_pack_str_with_body(&pk, "pong", 4);
}

void racs_cmd_call_create(racs_ctx *ctx,
                          const char *stream_id,
                          racs_uint32 sample_rate,
                          racs_uint8 channels,
                          racs_uint8 bit_depth) {
    int result = racs_streams_create(stream_id, sample_rate, channels, bit_depth);

    if (result != RACS_STREAM_OK) {
        char msg[55];

        snprintf(msg, sizeof(msg), "create %s", racs_stream_result_string[result]);
        racs_cmd_error(ctx, msg);
        return;
    }

    msgpack_sbuffer_clear(&ctx->out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &ctx->out_buf, msgpack_sbuffer_write);
    msgpack_pack_nil(&pk);
}

void racs_cmd_call_stream(racs_ctx *ctx,
                          const char *stream_id,
                          const char *codec,
                          const racs_uint8 *src,
                          size_t src_size) {

    int status = racs_stream(stream_id, codec, src, src_size);                       
    if (status != RACS_STREAM_OK) {
        char msg[55];

        snprintf(msg, sizeof(msg), "stream %s", racs_stream_result_string[status]);
        racs_cmd_error(ctx, msg);
        return;
    }

    msgpack_sbuffer_clear(&ctx->out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &ctx->out_buf, msgpack_sbuffer_write);
    msgpack_pack_nil(&pk);
}

void racs_cmd_ping(racs_ctx *ctx, msgpack_object *args, size_t num_args) {
    if (num_args != 0) {
        racs_cmd_error(ctx, "ping requires 0 args");
        return;
    }

    racs_cmd_call_ping(ctx);
}

void racs_cmd_create(racs_ctx *ctx, msgpack_object *args, size_t num_args) {
    if (num_args != 4) {
        racs_cmd_error(ctx, "create requires 4 args");
        return;
    }

    size_t stream_id_size;
    char *stream_id = NULL;
    if (racs_cmd_arg_str(&args[0], &stream_id, &stream_id_size) == -1) {
        racs_cmd_error(ctx, "create error at arg1. expected string");
        return;
    }

    racs_uint32 sample_rate;
    if (racs_cmd_arg_uint32(&args[1], &sample_rate) == -1) {
        racs_cmd_error(ctx, "create error at arg2. expected int");
        return;
    }

    racs_uint8 channels;
    if (racs_cmd_arg_uint8(&args[2], &channels) == -1) {
        racs_cmd_error(ctx, "create error at arg3. expected int");
        return;
    }

    racs_uint8 bit_depth;
    if (racs_cmd_arg_uint8(&args[3], &bit_depth) == -1) {
        racs_cmd_error(ctx, "create error at arg4. expected int");
        return;
    }

    char *s_stream_id = strndup(stream_id, stream_id_size);

    racs_cmd_call_create(ctx, s_stream_id, sample_rate, channels, bit_depth);
    free(s_stream_id);
}

void racs_cmd_stream(racs_ctx *ctx, msgpack_object *args, size_t num_args) {
    if (num_args != 3) {
        racs_cmd_error(ctx, "stream requires 3 args");
        return;
    }

    size_t stream_id_size;
    char *stream_id = NULL;
    if (racs_cmd_arg_str(&args[0], &stream_id, &stream_id_size) == -1) {
        racs_cmd_error(ctx, "stream error at arg1. expected string");
        return;
    }

    size_t codec_size;
    char *codec = NULL;
    if (racs_cmd_arg_str(&args[1], &codec, &codec_size) == -1) {
        racs_cmd_error(ctx, "stream error at arg2. expected string");
        return;
    }

    size_t src_size;
    racs_uint8 *src = NULL;
    if (racs_cmd_arg_bin(&args[2], &src, &src_size) == -1) {
        racs_cmd_error(ctx, "stream error at arg3. expected binary");
        return;
    }

    char *s_stream_id = strndup(stream_id, stream_id_size);
    char *s_codec = strndup(codec, codec_size);

    racs_cmd_call_stream(ctx, s_stream_id, s_codec, src, src_size);

    free(s_stream_id);
    free(s_codec);
}
