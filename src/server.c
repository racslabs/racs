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

void racs_read_callback(struct bufferevent *bev, void *ctx) {
    racs_db *db = ctx;

    struct evbuffer *in  = bufferevent_get_input(bev);
    struct evbuffer *out = bufferevent_get_output(bev);

    while (1) {
        size_t size = evbuffer_get_length(in);
        if (size < 8) return;

        racs_uint8 *header = evbuffer_pullup(in, 8);

        racs_uint64 payload_len;
        memcpy(&payload_len, header, 8);

        if (size < 8 + payload_len) return;

        evbuffer_drain(in, 8);

        racs_uint8 *buf = malloc(payload_len);
        evbuffer_remove(in, buf, payload_len);

        racs_result res;
        if (racs_is_frame((const char *) buf))
            res = racs_db_stream(db, buf);
        else
            res = racs_db_exec(db, (const char *) buf);

        free(buf);

        racs_uint8 header_out[8];
        memcpy(header_out, &res.size, 8);

        evbuffer_add(out, header_out, 8);
        evbuffer_add(out, res.data, res.size);

        free(res.data);
    }
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
    bufferevent_setcb(bev, racs_read_callback, NULL, racs_event_callback, ctx->db);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    racs_log_info("Client connected");
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

    if (!ctx.base) {
        racs_log_fatal("Could not initialize libevent.");
        return 1;
    }

    /* Listening address */
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(0);   /* 0.0.0.0 */
    sin.sin_port = htons(db->ctx.config->port);

    /* Create listener */
    struct evconnlistener *listener = evconnlistener_new_bind(
        ctx.base,
        racs_accept_callback,               /* on new connection */
        &ctx,                    /* context for callback */
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
        -1,                      /* backlog */
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
