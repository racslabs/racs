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

void racs_conn_init(racs_conn *conn, int port) {
    conn->stop = false;
    conn->compress = false;
    conn->timeout = -1;

    racs_conn_init_socket(conn);
    racs_conn_set_socketopts(conn);
    racs_conn_set_nonblocking(conn);

    racs_conn_socket_bind(conn, port);
    racs_conn_socket_listen(conn);
    racs_conn_init_fds(conn);
}

void racs_conn_init_socket(racs_conn *conn) {
    // Create an AF_INET6 stream socket to receive incoming
    // connections
    conn->listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
    if (conn->listen_sd < 0) {
        racs_log_fatal("socket() failed");
        exit(-1);
    }
}

void racs_conn_set_socketopts(racs_conn *conn) {
    int on = 1, off = 0, rc;
    int bufsize = 1024*1024; // 1 MB

    // Allow socket descriptor to be reusable
    rc = setsockopt(conn->listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (rc < 0) {
        racs_log_fatal("setsockopt() failed");
        close(conn->listen_sd);
        exit(-1);
    }

    // Disable IPV6 only.
    rc = setsockopt(conn->listen_sd, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(off));
    if (rc < 0) {
        racs_log_fatal("setsockopt(IPV6_V6ONLY) failed");
        close(conn->listen_sd);
        exit(-1);
    }

    rc = setsockopt(conn->listen_sd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
    if (rc < 0) {
        perror("setsockopt TCP_NODELAY failed");
        close(conn->listen_sd);
        exit(-1);
    }

    rc = setsockopt(conn->listen_sd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
    if (rc < 0) {
        perror("setsockopt SO_SNDBUF failed");
        close(conn->listen_sd);
        exit(-1);
    }

    rc = setsockopt(conn->listen_sd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));
    if (rc < 0) {
        perror("setsockopt SO_RCVBUF failed");
        close(conn->listen_sd);
        exit(-1);
    }
}

void racs_conn_set_nonblocking(racs_conn *conn) {
    int rc, on = 1;

    // Set socket to be nonblocking. All the sockets for
    // the incoming connections will also be nonblocking since
    // they will inherit that state from the listening socket.
    rc = ioctl(conn->listen_sd, FIONBIO, &on);
    if (rc < 0) {
        racs_log_fatal("ioctl() failed");
        close(conn->listen_sd);
        exit(-1);
    }
}

void racs_conn_socket_bind(racs_conn *conn, int port) {
    memset(&conn->addr, 0, sizeof(conn->addr));
    conn->addr.sin6_family = AF_INET6;
    memcpy(&conn->addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
    conn->addr.sin6_port = htons(port);

    int rc = bind(conn->listen_sd, (struct sockaddr *) &conn->addr, sizeof(conn->addr));
    if (rc < 0) {
        racs_log_fatal("bind() failed");
        close(conn->listen_sd);
        exit(-1);
    }
}

void racs_conn_socket_listen(racs_conn *conn) {
    int rc = listen(conn->listen_sd, 32);
    if (rc < 0) {
        racs_log_fatal("listen() failed");
        close(conn->listen_sd);
        exit(-1);
    }
}

void racs_conn_stream_init(racs_conn_stream *stream) {
    memset(stream, 0, sizeof(racs_conn_stream));
    racs_memstream_init(&stream->in_stream);
    racs_memstream_init(&stream->out_stream);
}

void racs_conn_stream_reset(racs_conn_stream *stream) {
    free(stream->in_stream.data);
    free(stream->out_stream.data);

    stream->prefix_pos = 0;
    racs_conn_stream_init(stream);
}

int racs_recv_length_prefix(int fd, size_t *len, racs_conn_stream *stream) {
    ssize_t rc;

    while (stream->prefix_pos < 8) {

        rc = recv(fd, stream->prefix_buf + stream->prefix_pos, 8 - stream->prefix_pos, 0);
        if (rc < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;   // need more data
            return -1;       // real error
        }

        if (rc == 0) {
            return -1;       // connection closed
        }

        stream->prefix_pos += rc;
        if (stream->prefix_pos < 8)
            return 0;       // still not enough bytes yet
    }

    // full prefix received
    memcpy(len, stream->prefix_buf, 8);
    stream->prefix_pos = 0; // reset for next message
    return 8;
}

int racs_recv(int fd, int len, racs_conn_stream *stream) {
    int rc;
    char buf[RACS_CHUNK_SIZE];

    while (stream->in_stream.pos < len) {
        rc = recv(fd, buf, sizeof(buf), 0);

        if (rc < 0) {
            if (errno != EWOULDBLOCK) {
                racs_log_fatal("recv() failed: %s", strerror(errno));
                return -1;
            }
            continue;
        }

        if (rc == 0) {
            racs_log_info("  Connection closed");
            return -1;
        }

        racs_memstream_write(&stream->in_stream, buf, rc);
    }

    return stream->in_stream.pos;
}

int racs_send(int fd, racs_conn_stream *stream) {
    size_t bytes = 0;
    size_t n = stream->out_stream.pos;
    ssize_t rc;

    signal(SIGPIPE, SIG_IGN);

    while (bytes < n) {
        size_t rem = n - bytes;
        size_t to_send = rem < RACS_CHUNK_SIZE ? rem : RACS_CHUNK_SIZE;

        racs_log_info("size=%zu", to_send);

        rc = send(fd, stream->out_stream.data + bytes, to_send, MSG_NOSIGNAL);
        if (rc < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;

            if (errno == EPIPE) {
                racs_log_error("peer closed connection: %s", strerror(errno));
                return -1;
            }

            racs_log_error("send() failed: %s", strerror(errno));
            return -1;
        }

        if (rc == 0) {
            racs_log_error("send() returned 0 (peer closed?)");
            return -1;
        }

        bytes += (size_t)rc;
    }

    return 0;
}

void racs_conn_init_fds(racs_conn *conn) {
    // Initialize the pollfd structure
    memset(conn->fds, 0, sizeof(racs_fds));

    // Set up the initial listening socket
    conn->fds[0].fd = conn->listen_sd;
    conn->fds[0].events = POLLIN;
    conn->fd_types[0] = RACS_FD_LISTEN;
}

int main(int argc, char *argv[]) {
    int rc;

    int nfds = 1, current_size = 0, i, j;

    racs_args(argc, argv);

    scm_init_guile();
    racs_scm_init_module();

    racs_db *db = racs_db_instance();
    racs_db_open(db, argv[2]);
    racs_log_instance();

    char ver[55];
    racs_version(ver);
    racs_log_info(ver);

    racs_conn conn;
    racs_conn_init(&conn, db->ctx.config->port);

    racs_conn_stream streams[200];

    racs_conn_stream slave_stream;
    racs_conn_stream_init(&slave_stream);

    racs_log_info("Listening on port %d ...", db->ctx.config->port);
    racs_log_info("Log file: %s/racs.log", racs_log_dir);

    racs_replica_set replicas;
    racs_replica_set_init(&replicas, db->ctx.config);

    for (i = 0; i < replicas.size; i++) {
        conn.fds[nfds].fd = replicas.replicas[i].fd;
        conn.fds[nfds].events = 0;
        conn.fd_types[nfds] = RACS_FD_REPLICA;

        nfds++;
    }

    // Loop waiting for incoming connects or for incoming data
    // on any of the connected sockets.
    do {
        // Call poll() and wait for it to complete.
        rc = poll(conn.fds, nfds, conn.timeout);

        // Check to see if the poll call failed.
        if (rc < 0) {
            racs_log_error("  poll() failed");
            break;
        }

        // Check to see if time out expired.
        if (rc == 0) {
            racs_log_error("  poll() timed out.  End program.");
            break;
        }

        // One or more descriptors are readable.  Need to
        // determine which ones they are.
        current_size = nfds;
        for (i = 0; i < current_size; i++) {

            // Loop through to find the descriptors that returned
            // POLLIN and determine whether it's the listening
            // or the active connection.
            if (conn.fds[i].revents == 0)
                continue;

            if (conn.fds[i].revents & POLLHUP) {
                close(conn.fds[i].fd);
                conn.fds[i].fd = -1;
                continue;
            }

            if (conn.fds[i].revents & POLLERR) {
                close(conn.fds[i].fd);
                conn.fds[i].fd = -1;
                continue;
            }

            // If revents is not POLLIN, it's an unexpected result,
            // log and end the server.
            if (conn.fds[i].revents != POLLIN) {
                racs_log_fatal("  Error! revents = %d", conn.fds[i].revents);
                conn.stop = true;
                break;
            }

            if (conn.fds[i].fd == conn.listen_sd) {

                // Accept all incoming connections that are
                // queued up on the listening socket before we
                // loop back and call poll again.
                do {
                    // Accept each incoming connection. If
                    // accept fails with EWOULDBLOCK, then we
                    // have accepted all of them. Any other
                    // failure on accept will cause us to end the
                    // server.
                    int new_sd = accept(conn.listen_sd, NULL, NULL);
                    if (new_sd < 0) {
                        if (errno != EWOULDBLOCK) {
                            racs_log_fatal("  accept() failed");
                            conn.stop = true;
                        }
                        break;
                    }

                    // Add the new incoming connection to the
                    // pollfd structure
                    conn.fds[nfds].fd = new_sd;
                    conn.fds[nfds].events = POLLIN;
                    conn.fd_types[nfds] = RACS_FD_PRIMARY;

                    racs_conn_stream_init(&streams[nfds]);
                    nfds++;
                } while (1);
            } else {
                // Receive all incoming data on this socket
                // before we loop back and call poll again.
                conn.closed = false;
                size_t length = 0;

                rc = racs_recv_length_prefix(conn.fds[i].fd, &length, &streams[i]);
                if (rc < 0) conn.closed = true;

                rc = racs_recv(conn.fds[i].fd, (int) length, &streams[i]);
                if (rc < 0) conn.closed = true;

                // Echo the data back to the client
                if (streams[i].in_stream.pos > 0) {
                    if (conn.fd_types[i] != RACS_FD_REPLICA) {
                        for (int k = 0; k < replicas.size; ++k) {
                            racs_conn_stream_reset(&slave_stream);

                            racs_memstream_write(&slave_stream.out_stream, &streams[i].in_stream.pos, sizeof(size_t));
                            racs_memstream_write(&slave_stream.out_stream, streams[i].in_stream.data, streams[i].in_stream.pos);

                            rc = racs_send(replicas.replicas[k].fd, &slave_stream);
                            racs_log_info("send for fd=%u size=%zu rc=%u", replicas.replicas[k].fd, streams[i].in_stream.pos, rc);
                            racs_conn_stream_reset(&slave_stream);
                        }
                    }

                    racs_result res;

                    if (racs_is_frame((const char *) streams[i].in_stream.data)) {
                        res = racs_db_stream(db, streams[i].in_stream.data);
                    } else {
                        res = racs_db_exec(db, (const char *) streams[i].in_stream.data);
                    }

                    racs_memstream_write(&streams[i].out_stream, &res.size, sizeof(size_t));
                    racs_memstream_write(&streams[i].out_stream, res.data, res.size);
                    free(res.data);

                    rc = racs_send(conn.fds[i].fd, &streams[i]);

                    if (rc < 0) {
                        racs_conn_stream_reset(&streams[i]);
                        conn.closed = true;
                        break;
                    }

                    racs_conn_stream_reset(&streams[i]);
                }

                // If the close_conn flag was turned on, we need
                // to clean up this active connection. This
                // clean up process includes removing the
                // descriptor.
                if (conn.closed) {
                    close(conn.fds[i].fd);
                    conn.fds[i].fd = -1;
                    conn.compress = true;
                }
            }
        }

        // If the compress flag was turned on, we need
        // to squeeze together the array and decrement the number
        // of file descriptors. We do not need to move back the
        // events and revents fields because the events will always
        // be POLLIN in this case, and revents is output.
        if (conn.compress) {
            conn.compress = false;
            for (i = 0; i < nfds; i++) {
                if (conn.fds[i].fd == -1) {
                    for (j = i; j < nfds - 1; j++) {
                        conn.fds[j].fd = conn.fds[j + 1].fd;
                    }
                    i--;
                    nfds--;
                }
            }
        }

    } while (conn.stop == false);

    // Clean up all the sockets that are open
    for (i = 0; i < nfds; i++) {
        if (conn.fds[i].fd >= 0)
            close(conn.fds[i].fd);
    }
}
