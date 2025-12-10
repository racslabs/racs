// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_SERVER_H
#define RACS_SERVER_H

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "db.h"
#include "scm_bindings.h"
#include "log.h"
#include "version.h"

#define RACS_MAX_SLAVES 5

typedef struct {
    struct bufferevent *bev;
    struct sockaddr_in addr;
    int connected;
} racs_slave_connection;

typedef struct {
    racs_db *db;
    struct event_base *base;
    racs_slave_connection slaves[RACS_MAX_SLAVES];
    int replica_count;
} racs_connection_context;

void racs_help();

void racs_args(int argc, char *argv[]);

void racs_read_callback(struct bufferevent *bev, void *data);

void racs_event_callback(struct bufferevent *bev, short events, void *ctx);

void racs_accept_callback(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *data);

void racs_broadcast_to_slaves(racs_connection_context *ctx, racs_uint8 *buf, size_t len);

void racs_slaves_init_sockets(racs_connection_context *ctx);

void racs_slaves_init_addrs(racs_connection_context *ctx, racs_config *config);

size_t racs_length_prefix(struct evbuffer *in) ;

void racs_handle_slaves(struct evbuffer *in, racs_connection_context *ctx, size_t length);

#endif //RACS_SERVER_H
