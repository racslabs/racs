#include "wal.h"

void racs_wal_append_(racs_wal *wal, racs_op_code op_code, size_t size, racs_uint8 *op) {
    racs_wal_entry entry;
    entry.op_code = op_code;
    entry.checksum = crc32c(0, op, size);
    entry.size = size;
    entry.op = op;
    entry.seq = wal->seq;

    racs_uint8 *buf = malloc(24 + entry.size);
    if (!buf) {
        racs_log_info("Failed to allocate buffer for racs_wal_entry");
        return;
    }

    off_t offset = 0;
    offset = racs_write_uint32(buf, entry.op_code, offset);
    offset = racs_write_uint32(buf, entry.checksum, offset);
    offset = racs_write_uint64(buf, entry.size, offset);

    memcpy(buf + offset, entry.op, entry.size);

    offset += (off_t) entry.size;
    offset = racs_write_uint64(buf, entry.seq, offset);

    pthread_mutex_lock(&wal->mutex);
    write(wal->fd, buf, offset);
    wal->seq++;
    pthread_mutex_unlock(&wal->mutex);
}

void racs_wal_open(racs_wal *wal) {
    char *path = NULL;
    char *dir  = NULL;

    asprintf(&dir, "%s/.racs", racs_wal_dir);
    asprintf(&path, "%s/wal", dir);

    mkdir(dir, 0777);

    wal->fd = open(path, O_RDONLY | O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (wal->fd == -1) {
        perror("Failed to open racs_wal");
        free(path);
        free(dir);
    }

    free(path);
    free(dir);
}

racs_wal *racs_wal_create() {
    racs_wal *wal = malloc(sizeof(racs_wal));
    if (!wal) {
        perror("Failed to create racs_wal");
        return NULL;
    }

    pthread_mutex_init(&wal->mutex, NULL);
    memset(wal, 0, sizeof(racs_wal));

    racs_wal_open(wal);
    return wal;
}

racs_wal *racs_wal_instance() {
    if (!_wal) {
        _wal = racs_wal_create();
    }
    return _wal;
}

void racs_wal_close(racs_wal *wal) {
    close(wal->fd);
}

void racs_wal_destroy(racs_wal *wal) {
    pthread_mutex_destroy(&wal->mutex);
    racs_wal_close(wal);
    free(wal);
}
