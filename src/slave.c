#include "slave.h"

int racs_slave_open(const char *host, int port) {
    int fd = racs_slave_init_socket();

    racs_slave_set_socketopts(fd);
    racs_slave_set_nonblocking(fd);
    racs_slave_connect(fd, host, port);

    return fd;
}

void racs_slave_connect(int fd, const char *host, int port) {
    struct addrinfo hints = {0}, *res;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%u", port);

    int rc = getaddrinfo(host, port_str, &hints, &res);
    if (rc != 0) {
        racs_log_fatal("slave: getaddrinfo() failed: %s", gai_strerror(rc));
        close(fd);
        exit(-1);
    }

    rc = connect(fd, res->ai_addr, res->ai_addrlen);
    if (rc == 0) {
        racs_log_info("slave: connected immediately");
    } else if (rc < 0) {
        if (errno == EINPROGRESS) {
            racs_log_info("slave: connect in progress (EINPROGRESS)");
        } else {
            racs_log_fatal("slave: connect() failed: %s", strerror(errno));
            close(fd);
            freeaddrinfo(res);
            exit(-1);
        }
    }

    freeaddrinfo(res);
}

void racs_slave_set_nonblocking(int fd) {
    int on = 1;

    int rc = ioctl(fd, FIONBIO, &on);
    if (rc < 0) {
        racs_log_fatal("slave: ioctl(FIONBIO) failed: %s", strerror(errno));
        close(fd);
        exit(-1);
    }
}

void racs_slave_set_socketopts(int fd) {
    int on = 1, off = 0;
    int bufsize = 1024 * 1024; // 1 MB

    int rc = setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(off));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt(IPV6_V6ONLY) failed");
        close(fd);
        exit(-1);
    }

    rc = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt TCP_NODELAY failed");
        close(fd);
        exit(-1);
    }

    rc = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt SO_SNDBUF failed");
        close(fd);
        exit(-1);
    }

    rc = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt SO_RCVBUF failed");
        close(fd);
        exit(-1);
    }

    struct linger ling = {0};
    rc = setsockopt(fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt SO_LINGER failed");
        close(fd);
        exit(-1);
    }
}

int racs_slave_init_socket() {
    int fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (fd < 0) {
        racs_log_fatal("slave: socket() failed");
        exit(-1);
    }

    return fd;
}