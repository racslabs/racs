// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "server.h"

#define RACS_CHUNK_SIZE     (4096*16)

void racs_help() {
    printf("Usage: racs [options...] <file>\n");
    printf("-c, --config <file>     Run server with conf.yaml file\n");
    printf("-v, --version           Show version and quit\n");
    printf("-h, --help              Get help for commands\n");
}

void racs_args(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) goto try_help;

    if (argc == 2) {
        if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
            char ver[55];
            racs_version(ver);
            printf("%s\n", ver);
            exit(0);
        }

        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            racs_help();
            exit(0);
        }

        if (strcmp(argv[1], "--config") == 0 || strcmp(argv[1], "-c") == 0) {
            printf("racs: required arg <file> is missing\n");
        } else if (strncmp(argv[1], "--", 2) == 0 || strncmp(argv[1], "-", 1) == 0)
            printf("racs: option %s: is unknown\n", argv[1]);
    }

    if (argc == 3) {
        if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)
            goto try_help;
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
            goto try_help;

        if (strcmp(argv[1], "--config") == 0 || strcmp(argv[1], "-c") == 0)
            return;

        if (strncmp(argv[1], "--", 2) == 0 || strncmp(argv[1], "-", 1) == 0)
            printf("racs: option %s: is unknown\n", argv[1]);
    }

    try_help:
    printf("racs: try 'racs --help' for more information\n");
    exit(-1);
}

size_t racs_length_prefix(struct evbuffer *in) {
    size_t size = evbuffer_get_length(in);
    if (size < 8) return 0;

    racs_uint8 *buf = evbuffer_pullup(in, 8);

    racs_uint64 len;
    memcpy(&len, buf, 8);

    if (size < 8 + len) return 0;

    return len;
}

void racs_handle_replicas(struct evbuffer *in, racs_connection_context *ctx, size_t length) {
    size_t size = 8 + length;
    uint8_t *raw = malloc(size);

    evbuffer_copyout(in, raw, size);

    racs_broadcast_to_replicas(ctx, raw, size);

    free(raw);
}

void racs_read_callback(struct bufferevent *bev, void *data) {
    racs_connection_context *ctx = (racs_connection_context *) data;

    struct evbuffer *in  = bufferevent_get_input(bev);
    struct evbuffer *out = bufferevent_get_output(bev);

    size_t length = racs_length_prefix(in);
    if (length == 0) return;

    racs_handle_replicas(in, ctx, length);

    evbuffer_drain(in, 8);

    racs_uint8 *buf = malloc(length);
    evbuffer_remove(in, buf, length);

    racs_result res;
    if (racs_is_frame((const char *) buf))
        res = racs_db_stream(ctx->db, buf);
    else
        res = racs_db_exec(ctx->db, (const char *) buf);

    free(buf);

    racs_uint8 header_out[8];
    memcpy(header_out, &res.size, 8);

    evbuffer_add(out, header_out, 8);
    evbuffer_add(out, res.data, res.size);

    free(res.data);
}

void racs_event_callback(struct bufferevent *bev, short events, void *ctx) {
    if (events & BEV_EVENT_ERROR)
        racs_log_error("Error on bufferevent");

    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR))
        bufferevent_free(bev);
}

void racs_accept_callback(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *data) {
    racs_connection_context *ctx = data;

    struct event_base *base = ctx->base;
    evutil_make_socket_nonblocking(fd);

    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, racs_read_callback, NULL, racs_event_callback, ctx);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    racs_log_info("Client connected");
}

void racs_broadcast_to_replicas(racs_connection_context *ctx, racs_uint8 *buf, size_t len) {
    for (int i = 0; i < ctx->replica_count; i++) {
        if (!ctx->replicas[i].connected) continue;

        bufferevent_write(ctx->replicas[i].bev, buf, len);
    }
}

void racs_replicas_init(racs_connection_context *ctx) {
    for (int i = 0; i < ctx->replica_count; i++) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            racs_log_error("Failed to create socket for replica");
            ctx->replicas[i].connected = 0;
            continue;
        }

        evutil_make_socket_nonblocking(sock);

        struct bufferevent *bev = bufferevent_socket_new(ctx->base, sock, BEV_OPT_CLOSE_ON_FREE);
        bufferevent_setcb(bev, NULL, NULL, racs_event_callback, NULL);
        bufferevent_enable(bev, EV_WRITE);

        int rc = bufferevent_socket_connect(bev, (struct sockaddr*)&ctx->replicas[i].addr, sizeof(ctx->replicas[i].addr));
        if (rc != 0) {
            racs_log_error("Failed to initiate connection to replica");
            ctx->replicas[i].connected = 0;
        } else {
            ctx->replicas[i].bev = bev;
            ctx->replicas[i].connected = 1;
        }
    }
}

int main(int argc, char *argv[]) {
    racs_args(argc, argv);

    scm_init_guile();
    racs_scm_init_module();

    racs_db *db = racs_db_instance();
    racs_db_open(db, argv[2]);
    racs_log_instance();

    char ver[55];
    racs_version(ver);
    racs_log_info(ver);

    signal(SIGPIPE, SIG_IGN);

    racs_connection_context ctx;
    ctx.db = db;
    ctx.base = event_base_new();
    ctx.replica_count = 1;

    ctx.replicas[0].addr.sin_family = AF_INET;
    ctx.replicas[0].addr.sin_port = htons(6382);
    inet_pton(AF_INET, "locahost", &ctx.replicas[0].addr.sin_addr);

    racs_replicas_init(&ctx);

    if (!ctx.base) {
        racs_log_fatal("Could not initialize libevent.");
        return 1;
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(0);
    sin.sin_port = htons(db->ctx.config->port);

    struct evconnlistener *listener = evconnlistener_new_bind(
        ctx.base,
        racs_accept_callback,
        &ctx,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
        -1,
        (struct sockaddr*)&sin,
        sizeof(sin)
    );

    if (!listener) {
        racs_log_fatal("Could not create listener");
        return 1;
    }

    racs_log_info("Server listening on port %d...", db->ctx.config->port);

    /* Start event loop (runs forever) */
    event_base_dispatch(ctx.base);

    evconnlistener_free(listener);
    event_base_free(ctx.base);
    return 0;

}
