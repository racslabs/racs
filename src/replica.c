#include "replica.h"

void racs_replica_set_init(racs_replica_set *set, racs_config *cfg) {
    memset(set, 0, sizeof(racs_replica_set));

    if (cfg->replica_count > RACS_MAX_REPLICAS) {
        racs_log_fatal("replica: max number of replicas exceeded");
        exit(-1);
    }

    for (int i = 0; i < cfg->replica_count; i++) {
        racs_replica_open(&set->replicas[i], cfg->replicas[i].host, cfg->replicas[i].port);
        set->size++;
    }
}

void racs_replica_open(racs_replica *replica, const char *host, int port) {
    racs_replica_init_socket(replica);
    racs_replica_set_socketopts(replica);
    racs_replica_connect(replica, host, port);
    // racs_replica_set_nonblocking(replica);
}

void racs_replica_connect(racs_replica *replica, const char *host, int port) {
    struct addrinfo hints = {0}, *res;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%u", port);

    int rc = getaddrinfo(host, port_str, &hints, &res);
    if (rc != 0) {
        racs_log_fatal("replica: getaddrinfo() failed: %s", gai_strerror(rc));
        exit(-1);
    }

    rc = connect(replica->fd, res->ai_addr, res->ai_addrlen);
    if (rc < 0) {
        if (errno == EINPROGRESS) {
            racs_log_info("replica: connection in progress to addr=%s:%u", host, port);
        } else {
            racs_log_fatal("replica: connect() failed: %s", strerror(errno));
            close(replica->fd);
            exit(-1);
        }
    } else {
        racs_log_info("replica: connected to addr=%s:%u", host, port);
    }

    freeaddrinfo(res);
}

void racs_replica_set_socketopts(racs_replica *replica) {
    int on = 1;
    int bufsize = 1024 * 1024;

    int rc = setsockopt(replica->fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
    if (rc < 0) {
        racs_log_fatal("replica: setsockopt TCP_NODELAY failed");
        close(replica->fd);
        exit(-1);
    }

    rc = setsockopt(replica->fd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
    if (rc < 0) {
        racs_log_fatal("replica: setsockopt SO_SNDBUF failed");
        close(replica->fd);
        exit(-1);
    }

    rc = setsockopt(replica->fd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));
    if (rc < 0) {
        racs_log_fatal("replica: setsockopt SO_RCVBUF failed");
        close(replica->fd);
        exit(-1);
    }
}

void racs_replica_set_nonblocking(racs_replica *replica) {
    int rc, on = 1;

    // Set socket to be nonblocking. All the sockets for
    // the incoming connections will also be nonblocking since
    // they will inherit that state from the listening socket.
    rc = ioctl(replica->fd, FIONBIO, &on);
    if (rc < 0) {
        racs_log_fatal("replica: ioctl() failed");
        close(replica->fd);
        exit(-1);
    }
}

void racs_replica_init_socket(racs_replica *replica) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        racs_log_fatal("replica: socket() failed");
        exit(-1);
    }

    replica->fd = fd;
}
