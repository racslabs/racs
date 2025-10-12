// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RSAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_SERVER_H
#define RACS_SERVER_H


#include <sys/types.h>      // for socket types
#include <sys/socket.h>     // for socket, connect, send, recv
#include <sys/ioctl.h>      // for ioctl, FIONBIO
#include <netinet/in.h>     // for sockaddr_in, htons, etc.
#include <netinet/tcp.h>    // for TCP_FASTOPEN
#include <arpa/inet.h>      // for inet_pton, inet_ntop
#include <poll.h>

#include "db.h"
#include "scm_bindings.h"
#include "log.h"
#include "version.h"

typedef struct {
    int fd;
    racs_memstream in_stream;
    racs_memstream out_stream;
} racs_conn_stream;

typedef struct {
    int listen_sd, new_sd;
    struct sockaddr_in6 addr;
} racs_conn;

void racs_help();

void racs_args(int argc, char *argv[]);

void racs_init_socket(racs_conn *conn);

void racs_set_socketopts(racs_conn *conn);

void racs_set_nonblocking(racs_conn *conn);

void racs_socket_bind(racs_conn *conn, int port);

void racs_socket_listen(racs_conn *conn);

void racs_conn_stream_init(racs_conn_stream *stream);

void racs_conn_stream_reset(racs_conn_stream *stream);

int racs_recv_length_prefix(int fd, size_t *len);

int racs_recv(int fd, int len, racs_conn_stream *stream);

int racs_send(int fd, racs_conn_stream *stream);

#endif //RACS_SERVER_H
