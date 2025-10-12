#include "server.h"

#define TRUE             1
#define FALSE            0
#define CHUNK_SIZE       4096

void racs_help() {
    printf("Usage: racs [options...] <file>\n");
    printf("-c, --config <file>     Run server with config.yml file\n");
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

void racs_init_socket(racs_conn *conn) {
    // Create an AF_INET6 stream socket to receive incoming
    // connections
    conn->listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
    if (conn->listen_sd < 0) {
        racs_log_fatal("socket() failed");
        exit(-1);
    }
}

void racs_set_socketopts(racs_conn *conn) {
    int on = 1, off = 0, rc;

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

    rc = setsockopt(conn->listen_sd, IPPROTO_TCP, TCP_FASTOPEN, &off, sizeof(off));
    if (rc < 0) {
        racs_log_fatal("setsockopt(TCP_FASTOPEN) failed");
        exit(-1);
    }
}

void racs_set_nonblocking(racs_conn *conn) {
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

void racs_socket_bind(racs_conn *conn, int port) {
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

void racs_socket_listen(racs_conn *conn) {
    int rc = listen(conn->listen_sd, 32);
    if (rc < 0) {
        racs_log_fatal("listen() failed");
        close(conn->listen_sd);
        exit(-1);
    }
}


void racs_conn_stream_init(racs_conn_stream *stream) {
    racs_memstream_init(&stream->in_stream);
    racs_memstream_init(&stream->out_stream);
}

void racs_conn_stream_reset(racs_conn_stream *stream) {
    free(stream->in_stream.data);
    free(stream->out_stream.data);

    racs_conn_stream_init(stream);
}

int racs_recv_length_prefix(int fd, size_t *len) {
    int rc;

    rc = recv(fd, len, sizeof(size_t), 0);
    if (rc < 0) {
        if (errno != EWOULDBLOCK) {
            racs_log_fatal("  recv() failed");
            return -1;
        }
    }

    if (rc == 0) {
        racs_log_info("  Connection closed");
        return -1;
    }

    return 8;
}

int racs_recv(int fd, int len, racs_conn_stream *stream) {
    int rc;
    char buf[CHUNK_SIZE];

    while (stream->in_stream.current_pos < len) {
        rc = recv(fd, buf, sizeof(buf), 0);

        if (rc < 0) {
            if (errno != EWOULDBLOCK) {
                racs_log_fatal("  recv() failed");
                return -1;
            }
        }

        if (rc == 0) {
            racs_log_info("  Connection closed");
            return -1;
        }

        racs_memstream_write(&stream->in_stream, buf, rc);
    }

    return stream->in_stream.current_pos;
}

int racs_send(int fd, racs_conn_stream *stream) {
    size_t bytes = 0;
    size_t n = stream->out_stream.current_pos;
    ssize_t rc;

    signal(SIGPIPE, SIG_IGN);

    while (bytes < n) {
        size_t to_send = (n - bytes < CHUNK_SIZE) ? n - bytes : CHUNK_SIZE;
        rc = send(fd, stream->out_stream.data + bytes, to_send, MSG_NOSIGNAL);
        if (rc < 0) {
            if (errno == EPIPE) {
                racs_log_error("peer closed connection (EPIPE)");
            } else {
                racs_log_error("send() failed");
            }
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

int main(int argc, char *argv[]) {
    int rc;
    int new_sd = -1;
    int end = FALSE, compress = FALSE;
    int close_conn;
    int timeout;
    struct pollfd fds[200];
    int nfds = 1, current_size = 0, i, j;

    racs_args(argc, argv);

    racs_conn conn;
    racs_conn_stream streams[200];

    scm_init_guile();
    racs_scm_init_module();

    racs_db *db = racs_db_instance();
    racs_db_open(db, argv[2]);
    racs_log_instance();

    char ver[55];
    racs_version(ver);
    racs_log_info(ver);

    racs_init_socket(&conn);
    racs_set_socketopts(&conn);
    racs_set_nonblocking(&conn);

    racs_socket_bind(&conn, db->ctx.config->port);
    racs_socket_listen(&conn);

    // Initialize the pollfd structure
    memset(fds, 0, sizeof(fds));

    // Set up the initial listening socket
    fds[0].fd = conn.listen_sd;
    fds[0].events = POLLIN;

    timeout = -1;

    racs_log_info("Listening on port %d ...", db->ctx.config->port);
    racs_log_info("Log file: %s/racs.log", racs_log_dir);

    // Loop waiting for incoming connects or for incoming data
    // on any of the connected sockets.
    do {
        // Call poll() and wait for it to complete.
        rc = poll(fds, nfds, timeout);

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
            if (fds[i].revents == 0)
                continue;

            if (fds[i].revents & POLLHUP) {
                close(fds[i].fd);
                fds[i].fd = -1;
                continue;
            }

            if (fds[i].revents & POLLERR) {
                close(fds[i].fd);
                fds[i].fd = -1;
                continue;
            }

            // If revents is not POLLIN, it's an unexpected result,
            // log and end the server.
            if (fds[i].revents != POLLIN) {
                racs_log_fatal("  Error! revents = %d", fds[i].revents);
                end = TRUE;
                break;
            }

            if (fds[i].fd == conn.listen_sd) {

                // Accept all incoming connections that are
                // queued up on the listening socket before we
                // loop back and call poll again.
                do {
                    // Accept each incoming connection. If
                    // accept fails with EWOULDBLOCK, then we
                    // have accepted all of them. Any other
                    // failure on accept will cause us to end the
                    // server.
                    new_sd = accept(conn.listen_sd, NULL, NULL);
                    if (new_sd < 0) {
                        if (errno != EWOULDBLOCK) {
                            racs_log_fatal("  accept() failed");
                            end = TRUE;
                        }
                        break;
                    }

                    // Add the new incoming connection to the
                    // pollfd structure
                    fds[nfds].fd = new_sd;
                    fds[nfds].events = POLLIN;
                    racs_conn_stream_init(&streams[nfds]);
                    nfds++;
                } while (1);
            } else {
                // Receive all incoming data on this socket
                // before we loop back and call poll again.
                close_conn = FALSE;
                size_t length = 0;

                rc = racs_recv_length_prefix(fds[i].fd, &length);
                if (rc < 0) close_conn = TRUE;

                rc = racs_recv(fds[i].fd, (int) length, &streams[i]);
                if (rc < 0) close_conn = TRUE;

                // Echo the data back to the client
                if (streams[i].in_stream.current_pos > 0) {
                    racs_result res;

                    if (racs_is_frame((const char *) streams[i].in_stream.data)) {
                        res = racs_db_stream(db, streams[i].in_stream.data);
                    } else {
                        res = racs_db_exec(db, (const char *) streams[i].in_stream.data);
                    }

                    racs_memstream_write(&streams[i].out_stream, &res.size, sizeof(size_t));
                    racs_memstream_write(&streams[i].out_stream, res.data, res.size);
                    free(res.data);

                    rc = racs_send(fds[i].fd, &streams[i]);

                    if (rc < 0) {
                        racs_conn_stream_reset(&streams[i]);
                        close_conn = TRUE;
                        break;
                    }

                    racs_conn_stream_reset(&streams[i]);
                }

                // If the close_conn flag was turned on, we need
                // to clean up this active connection. This
                // clean up process includes removing the
                // descriptor.
                if (close_conn) {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    compress = TRUE;
                }
            }
        }

        // If the compress flag was turned on, we need
        // to squeeze together the array and decrement the number
        // of file descriptors. We do not need to move back the
        // events and revents fields because the events will always
        // be POLLIN in this case, and revents is output.
        if (compress) {
            compress = FALSE;
            for (i = 0; i < nfds; i++) {
                if (fds[i].fd == -1) {
                    for (j = i; j < nfds - 1; j++) {
                        fds[j].fd = fds[j + 1].fd;
                    }
                    i--;
                    nfds--;
                }
            }
        }

    } while (end == FALSE);

    // Clean up all the sockets that are open
    for (i = 0; i < nfds; i++) {
        if (fds[i].fd >= 0)
            close(fds[i].fd);
    }
}
