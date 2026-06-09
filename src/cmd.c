
#include "cmd.h"


const racs_cmd cmds[4] = {
    { "ping"  , racs_cmd_ping, 1 },
    { "create", racs_cmd_create, 0 },
    { "open"  , racs_cmd_open, 0 },
    { "stream", racs_cmd_stream, 0 },
};


int racs_cmd_unpack_arg(racs_ctx *ctx, msgpack_unpacked *unpacked, msgpack_object_type type);


void racs_cmd_call_ping(racs_ctx *ctx);

void racs_cmd_call_create(racs_ctx *ctx, 
                          const char *name,
                          racs_uint32 sample_rate,
                          racs_uint8 channels,
                          racs_uint8 bit_depth);

void racs_cmd_call_open(racs_ctx *ctx, const char *name);

void racs_cmd_call_stream(racs_ctx *ctx, 
                          const char *name,
                          const char *mime_type,
                          const racs_uint8 *src,
                          racs_uint32 src_size);


racs_cmd_func racs_cmd_lookup(const char *name) {
    size_t num_cmds = sizeof(cmds) / sizeof(cmds[0]);

    for (size_t i = 0; i < num_cmds; i++) {
        racs_cmd cmd = cmds[i];

        if (strlen(name) == strlen(cmd.name) && strcmp(cmd.name, name) == 0) {
            return cmd.func;
        }
    }

    return NULL;
}

int racs_cmd_unpack_arg(racs_ctx *ctx, msgpack_unpacked *unpacked, msgpack_object_type type) {
     if (msgpack_unpack_next(unpacked, ctx->out_buf.data, ctx->out_buf.size, &ctx->offset) <= 0) { 
        return -1;
    } 

    if (unpacked->data.type != type) {
        return -1;
    }

    return 0;
}

int racs_cmd_arg_str(racs_ctx *ctx, char **arg, size_t *size) {
    msgpack_unpacked unpacked;
    msgpack_unpacked_init(&unpacked);

    if (racs_cmd_unpack_arg(ctx, &unpacked, MSGPACK_OBJECT_STR) == -1) {
        msgpack_unpacked_destroy(&unpacked);
        return -1;
    }

    *arg = (char *)unpacked.data.via.str.ptr;
    *size = unpacked.data.via.str.size;

    msgpack_unpacked_destroy(&unpacked);
    return 0;
}

int racs_cmd_arg_bin(racs_ctx *ctx, racs_uint8 **arg, size_t *size) {
    msgpack_unpacked unpacked;
    msgpack_unpacked_init(&unpacked);

    if (racs_cmd_unpack_arg(ctx, &unpacked, MSGPACK_OBJECT_BIN) == -1) {
        msgpack_unpacked_destroy(&unpacked);
        return -1;
    }

    *arg = (racs_uint8 *)unpacked.data.via.bin.ptr;
    *size = unpacked.data.via.bin.size;

    msgpack_unpacked_destroy(&unpacked);
    return 0;
}

int racs_cmd_arg_uint64(racs_ctx *ctx, racs_uint64 *arg) {
    msgpack_unpacked unpacked;
    msgpack_unpacked_init(&unpacked);

    if (racs_cmd_unpack_arg(ctx, &unpacked, MSGPACK_OBJECT_POSITIVE_INTEGER) == -1) {
        msgpack_unpacked_destroy(&unpacked);
        return -1;
    }

    *arg = unpacked.data.via.u64;

    msgpack_unpacked_destroy(&unpacked);
    return 0;
}

int racs_cmd_arg_uint32(racs_ctx *ctx, racs_uint32 *arg) {
    racs_uint64 arg64;

    if (racs_cmd_arg_uint64(ctx, &arg64) == -1) {
        return -1;
    }

    *arg = (racs_uint32) arg64;
    return 0;
}

int racs_cmd_arg_uint16(racs_ctx *ctx, racs_uint16 *arg) {
    racs_uint64 arg64;

    if (racs_cmd_arg_uint64(ctx, &arg64) == -1) {
        return -1;
    }

    *arg = (racs_uint16) arg64;
    return 0;
}

int racs_cmd_arg_uint8(racs_ctx *ctx, racs_uint8 *arg) {
    racs_uint64 arg64;

    if (racs_cmd_arg_uint64(ctx, &arg64) == -1) {
        return -1;
    }

    *arg = (racs_uint8) arg64;
    return 0;
}

void racs_cmd_err(racs_ctx *ctx, const char *msg) {
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
                          const char *name,
                          racs_uint32 sample_rate,
                          racs_uint8 channels,
                          racs_uint8 bit_depth) {
    int result = racs_streams_create(name, sample_rate, channels, bit_depth);

    if (result != RACS_STREAM_OK) {
        char msg[55];

        sprintf(msg, "CREATE %s", racs_stream_result_string[result]);
        racs_cmd_err(ctx, msg);
        return;
    }

    msgpack_sbuffer_clear(&ctx->out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &ctx->out_buf, msgpack_sbuffer_write);
    msgpack_pack_nil(&pk);
}

void racs_cmd_call_open(racs_ctx *ctx, const char *name) {
    racs_streams *streams = racs_streams_get();

    int result = racs_streams_open(streams, name);

    if (result != RACS_STREAM_OK) {
        char msg[55];

        sprintf(msg, "OPEN %s", racs_stream_result_string[result]);
        racs_cmd_err(ctx, msg);
        return;
    }

    msgpack_sbuffer_clear(&ctx->out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &ctx->out_buf, msgpack_sbuffer_write);
    msgpack_pack_nil(&pk);
}

void racs_cmd_call_stream(racs_ctx *ctx, 
                          const char *name,
                          const char *mime_type,
                          const racs_uint8 *src,
                          racs_uint32 src_size) {
    racs_streams *streams = racs_streams_get();

    int result = racs_streams_append(streams, name, mime_type, src, src_size);
    if (result != RACS_STREAM_OK) {
        char msg[55];

        sprintf(msg, "STREAM %s", racs_stream_result_string[result]);
        racs_cmd_err(ctx, msg);
        return;
    }

    msgpack_sbuffer_clear(&ctx->out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &ctx->out_buf, msgpack_sbuffer_write);
    msgpack_pack_nil(&pk);
}

void racs_cmd_ping(racs_ctx *ctx, size_t num_args) {
    ctx->offset = 0;
    if (num_args != 0) {
        racs_cmd_err(ctx, "PING requires 0 args");
        return;
    }

    racs_cmd_call_ping(ctx);
}

void racs_cmd_create(racs_ctx *ctx, size_t num_args) {
    ctx->offset = 0;
    if (num_args != 4) {
        racs_cmd_err(ctx, "CREATE requires 4 args");
        return;
    }

    size_t size;
    char *name = NULL;
    if (racs_cmd_arg_str(ctx, &name, &size) == -1) {
        racs_cmd_err(ctx, "CREATE error at arg1. expected string");
        return;
    }

    racs_uint32 sample_rate;
    if (racs_cmd_arg_uint32(ctx, &sample_rate) == -1) {
        racs_cmd_err(ctx, "CREATE error at arg2. expected int");
        return;
    }

    racs_uint8 channels;
    if (racs_cmd_arg_uint8(ctx, &channels) == -1) {
        racs_cmd_err(ctx, "CREATE error at arg3. expected int");
        return;
    }

    racs_uint8 bit_depth;
     if (racs_cmd_arg_uint8(ctx, &bit_depth) == -1) {
        racs_cmd_err(ctx, "CREATE error at arg4. expected int");
        return;
    }

    char *s_name = strndup(name, size);

    racs_cmd_call_create(ctx, s_name, sample_rate, channels, bit_depth);
    free(s_name);
}

void racs_cmd_open(racs_ctx *ctx, size_t num_args) {
    ctx->offset = 0;
    if (num_args != 1) {
        racs_cmd_err(ctx, "OPEN requires 1 arg");
        return;
    }

    size_t size;
    char *name = NULL;
    if (racs_cmd_arg_str(ctx, &name, &size) == -1) {
        racs_cmd_err(ctx, "OPEN error at arg1. expected string");
        return;
    }

    char *s_name = strndup(name, size);

    racs_cmd_call_open(ctx, s_name);
    free(s_name);
}

void racs_cmd_stream(racs_ctx *ctx, size_t num_args) {
    ctx->offset = 0;
    if (num_args != 3) {
        racs_cmd_err(ctx, "STREAM requires 3 args");
        return;
    }
    
    size_t size;
    char *name = NULL;
    if (racs_cmd_arg_str(ctx, &name, &size) == -1) {
        racs_cmd_err(ctx, "STREAM error at arg1. expected string");
        return;
    }

    size_t mime_size;
    char *mime_type = NULL;
    if (racs_cmd_arg_str(ctx, &mime_type, &mime_size) == -1) {
        racs_cmd_err(ctx, "STREAM error at arg2. expected string");
        return;
    }

    size_t src_size;
    racs_uint8 *src = NULL;
    if (racs_cmd_arg_bin(ctx, &src, &src_size) == -1) {
        racs_cmd_err(ctx, "STREAM error at arg3. expected binary");
        return;
    }

    char *s_name = strndup(name, size);
    char *s_mime_type = strndup(mime_type, mime_size);

    racs_cmd_call_stream(ctx, s_name, s_mime_type, src, src_size);

    free(s_name);
    free(s_mime_type);
}
