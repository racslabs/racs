// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "wal.h"

void racs_wal_append_(racs_wal *wal, racs_op_code op_code, size_t size, racs_uint8 *op) {
    racs_uint8 *buf = malloc(24 + size);
    if (!buf) {
        racs_log_info("Failed to allocate buffer for racs_wal_entry");
        return;
    }

    off_t offset = 0;
    offset = racs_write_uint32(buf, op_code, offset);
    offset = racs_write_uint32(buf, crc32c(0, op, size), offset);
    offset = racs_write_uint64(buf, size, offset);

    memcpy(buf + offset, op, size);

    offset += (off_t) size;
    offset = racs_write_uint64(buf, wal->lsn, offset);

    pthread_mutex_lock(&wal->mutex);
    if (wal->size + offset >= RACS_WAL_SIZE_64MB)
        racs_wal_rotate(wal);

    write(wal->fd, buf, offset);
    wal->size += offset;
    free(buf);

    if (wal->lsn % RACS_WAL_FSYNC == 0) {
        if (fsync(wal->fd) < 0)
            racs_log_error("fsync failed on racs_wal");
    }

    wal->lsn++;
    pthread_mutex_unlock(&wal->mutex);
}

void racs_wal_open(racs_wal *wal) {
    char *dir1 = NULL;
    char *dir2 = NULL;
    char *path = NULL;

    char filename[25];
    struct stat st;

    asprintf(&dir1, "%s/.racs", racs_wal_dir);
    asprintf(&dir2, "%s/wal", dir1);

    mkdir(dir1, 0777);
    mkdir(dir2, 0777);

    racs_uint64 segno = racs_wal_segno(dir2);

    racs_wal_filename(filename, sizeof(filename), segno);
    asprintf(&path, "%s/%s", dir2, filename);

    if (stat(path, &st) == 0 && st.st_size >= RACS_WAL_SIZE_64MB) {
        free(path);

        racs_wal_filename(filename, sizeof(filename), ++segno);
        asprintf(&path, "%s/%s", dir2, filename);
    }

    wal->fd = open(path, O_RDWR | O_CREAT | O_APPEND, 0644);
    if (wal->fd == -1)
        racs_log_error("Failed to open racs_wal");

    free(path);
    free(dir1);
    free(dir2);
}

void racs_wal_rotate(racs_wal *wal) {
    char *dir = NULL;
    char *path = NULL;

    char filename[25];

    asprintf(&dir, "%s/.racs/wal", racs_wal_dir);

    racs_uint64 segno = racs_wal_segno(dir);
    racs_wal_filename(filename, sizeof(filename), ++segno);

    asprintf(&path, "%s/%s", dir, filename);

    if (fsync(wal->fd) < 0)
        racs_log_error("fsync failed on racs_wal");

    close(wal->fd);
    wal->size = 0;

    wal->fd = open(path, O_RDWR | O_CREAT | O_APPEND, 0644);
    if (wal->fd == -1)
        racs_log_error("Failed to open racs_wal");

    free(dir);
    free(path);
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
    racs_wal_init_lsn(wal);

    return wal;
}

void racs_wal_truncate() {
    char *dir = NULL;
    char *active_wal = NULL;

    char filename[25];
    asprintf(&dir, "%s/.racs/wal", racs_wal_dir);

    racs_uint64 segno = racs_wal_segno(dir);
    racs_uint64 checkpoint_lsn = racs_wal_checkpoint_lsn();

    racs_wal_filename(filename, sizeof(filename), segno);
    asprintf(&active_wal, "%s/%s", dir, filename);

    racs_filelist *list = get_sorted_filelist(dir);

    for (int i = 0; i < list->num_files; ++i) {
        if (strcmp(list->files[i], active_wal) == 0) continue;
        if (strcmp(basename(list->files[i]), "manifest") == 0) continue;

        int fd = open(list->files[i], O_RDONLY, 0644);
        if (fd == -1) {
            racs_log_error("Failed to open racs_wal_segment");
            continue;
        }

        racs_uint64 max_lsn = racs_wal_segment_max_lsn(fd);
        close(fd);

        if (max_lsn < checkpoint_lsn) {
            if (remove(list->files[i]) < 0)
                racs_log_error("Failed to delete racs_wal_segment");
        }
    }

    free(dir);
    free(active_wal);
    racs_filelist_destroy(list);
}

racs_uint64 racs_wal_checkpoint_lsn() {
    char *path = NULL;
    if (asprintf(&path, "%s/.racs/wal/manifest", racs_wal_dir) == -1)
        return 0;

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        free(path);
        return 0;
    }

    racs_uint64 lsn = 0;
    ssize_t rc = read(fd, &lsn, sizeof(lsn));
    if (rc != sizeof(lsn)) lsn = 0;

    close(fd);
    free(path);
    return lsn;
}

racs_wal *racs_wal_instance() {
    if (!_wal) _wal = racs_wal_create();
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

void racs_wal_filename(char *buf, size_t buflen, uint64_t segno) {
    snprintf(buf, buflen, "%020" PRIu64 ".log", segno);
}

racs_uint64 racs_wal_segno(const char *wal_dir) {
    DIR *dir = opendir(wal_dir);
    if (!dir) return 0;

    racs_uint64 max_seg = UINT64_MAX;
    int found = false;

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strlen(ent->d_name) != 24) // "00000000000000000000.log"
            continue;

        if (strcmp(ent->d_name + 20, ".log") != 0)
            continue;

        char numbuf[21];
        memcpy(numbuf, ent->d_name, 20);
        numbuf[20] = '\0';

        char *end;
        uint64_t seg = strtoull(numbuf, &end, 10);
        if (*end != '\0')
            continue;

        if (!found || seg > max_seg) {
            max_seg = seg;
            found = true;
        }
    }

    closedir(dir);

    return found ? max_seg: 0;
}

void racs_wal_init_lsn(racs_wal *wal) {
    wal->lsn = racs_wal_segment_max_lsn(wal->fd);
}

racs_uint64 racs_wal_segment_max_lsn(int fd) {
    struct stat st;
    uint64_t lsn = 0;

    if (fstat(fd, &st) == -1)
        return 0;

    if (st.st_size < sizeof(uint64_t))
        return 0;

    if (lseek(fd, -sizeof(uint64_t), SEEK_END) == -1)
        return 0;

    if (read(fd, &lsn, sizeof(uint64_t)) != sizeof(uint64_t))
        return 0;

    return lsn;
}
