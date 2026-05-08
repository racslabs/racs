// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#include "sst.h"


// Table layout: [Audio blocks ...][Index entries ...][Entry count]
racs_sst *racs_sst_open(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return NULL;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        return NULL;
    }

    size_t size = st.st_size;
    racs_uint8 *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    madvise(data, size, MADV_WILLNEED | MADV_SEQUENTIAL);
    close(fd);

    if (data == MAP_FAILED) {
        return NULL;
    }

    racs_sst *sst = malloc(sizeof(racs_sst));
    if (!sst) {
        munmap(data, size);
        return NULL;
    }

    sst->data = data;
    sst->size = size;

    memcpy(&sst->num_entries,
           sst->data + (size - sizeof(racs_uint16)),
           sizeof(racs_uint16));

    if (sst->num_entries == 0) {
        sst->index_ptr = NULL;
    } else {
        size_t index_size = sst->num_entries * sizeof(racs_sst_index_entry);
        sst->index_ptr = sst->data + (size - sizeof(racs_uint16) - index_size);
    }

    return sst;
}

void racs_sst_destroy(racs_sst *sst) {
    if (!sst) {
        return;
    }

    if (sst->data) {
        munmap(sst->data, sst->size);
    }

    free(sst);
}

racs_sst_index_entry *racs_sst_get_index(racs_sst *sst) {
    return (racs_sst_index_entry *)sst->index_ptr;
}
