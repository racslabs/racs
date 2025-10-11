#include "conn.h"
#include "log.h"

void racs_init_socketopts(racs_conn *conn) {
    int on = 1, off = 0, rc;

    // Create an AF_INET6 stream socket to receive incoming
    // connections
    conn->listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
    if (conn->listen_sd < 0) {
        racs_log_fatal("socket() failed");
        exit(-1);
    }

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
