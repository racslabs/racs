#include "slave.h"

#include "bytes.h"


void racs_slave_open(racs_slave *slave, const char *host, int port) {
    racs_slave_init_socket(slave);
    racs_slave_set_socketopts(slave);
    racs_slave_set_nonblocking(slave);
    racs_slave_connect(slave, host, port);
}

void racs_slave_connect(racs_slave *slave, const char *host, int port) {
    struct addrinfo hints = {0}, *res;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%u", port);

    int rc = getaddrinfo(host, port_str, &hints, &res);
    if (rc != 0) {
        racs_log_fatal("slave: getaddrinfo() failed: %s", gai_strerror(rc));
        exit(-1);
    }

    rc = connect(slave->fd, res->ai_addr, res->ai_addrlen);
    if (rc < 0) {
        if (errno == EINPROGRESS) {
            racs_log_info("slave: connection in progress to addr=%s:%u", host, port);
        } else {
            racs_log_fatal("slave: connect() failed: %s", strerror(errno));
            close(slave->fd);
            exit(-1);
        }
    } else {
        racs_log_info("slave: connected to addr=%s:%u", host, port);
    }

    freeaddrinfo(res);
}

void racs_slave_set_socketopts(racs_slave *slave) {
    int on = 1;
    int bufsize = 1024 * 1024;

    int rc = setsockopt(slave->fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt TCP_NODELAY failed");
        close(slave->fd);
        exit(-1);
    }

    rc = setsockopt(slave->fd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt SO_SNDBUF failed");
        close(slave->fd);
        exit(-1);
    }

    rc = setsockopt(slave->fd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt SO_RCVBUF failed");
        close(slave->fd);
        exit(-1);
    }
}

void racs_slave_set_nonblocking(racs_slave *slave) {
    int rc, on = 1;

    // Set socket to be nonblocking. All the sockets for
    // the incoming connections will also be nonblocking since
    // they will inherit that state from the listening socket.
    rc = ioctl(slave->fd, FIONBIO, &on);
    if (rc < 0) {
        racs_log_fatal("ioctl() failed");
        close(slave->fd);
        exit(-1);
    }
}

void racs_slave_init_socket(racs_slave *slave) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        racs_log_fatal("slave: socket() failed");
        exit(-1);
    }

    slave->fd = fd;
}
