// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "wal.h"

void racs_wal_mf0_(racs_wal *wal, racs_uint64 seq) {
    pthread_mutex_lock(&wal->mutex);
    write(wal->mf0_fd, &seq, 8);
    pthread_mutex_unlock(&wal->mutex);
}

void racs_wal_mf1_(racs_wal *wal, racs_uint64 seq) {
    pthread_mutex_lock(&wal->mutex);
    write(wal->mf1_fd, &seq, 8);
    pthread_mutex_unlock(&wal->mutex);
}

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

    if (wal->seq % RACS_WAL_FSYNC) {
        if (fsync(wal->fd) < 0)
            racs_log_error("fsync failed on racs_wal");
    }

    wal->seq++;
    pthread_mutex_unlock(&wal->mutex);
}

void racs_wal_open(racs_wal *wal) {
    char *path1 = NULL;
    char *path2 = NULL;
    char *path3 = NULL;
    char *dir  = NULL;

    asprintf(&dir, "%s/.racs", racs_wal_dir);
    asprintf(&path1, "%s/wal", dir);
    asprintf(&path2, "%s/mf0", dir);
    asprintf(&path3, "%s/mf1", dir);

    mkdir(dir, 0777);

    wal->fd = open(path1, O_RDONLY | O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (wal->fd == -1)
        perror("Failed to open racs_wal");

    wal->mf0_fd = open(path2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (wal->mf0_fd == -1)
        perror("Failed to open racs_wal manifest");

    wal->mf1_fd = open(path3, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (wal->mf0_fd == -1)
        perror("Failed to open racs_wal manifest");

    free(path1);
    free(path2);
    free(path3);
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
    close(wal->mf0_fd);
}

void racs_wal_destroy(racs_wal *wal) {
    pthread_mutex_destroy(&wal->mutex);
    racs_wal_close(wal);
    free(wal);
}
