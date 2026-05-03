// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#include "sstable.h"

// Table layout: [Audio blocks ...][Index entries ...][Entry count]
racs_sstable *racs_sstable_open(const char *path) {
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
    close(fd);

    if (data == MAP_FAILED) {
        return NULL;
    }

    racs_sstable *sst = malloc(sizeof(racs_sstable));
    if (!sst) {
        munmap(data, size);
        return NULL;
    }

    sst->data = data;
    sst->size = size;

    memcpy(&sst->num_entries,
           sst->data + (size - sizeof(racs_uint16)),
           sizeof(racs_uint16));

    size_t index_size = sst->num_entries * sizeof(racs_sstable_index_entry);
    sst->index_ptr = sst->data + (size - sizeof(racs_uint16) - index_size);

    return sst;
}

void racs_sstable_destroy(racs_sstable *sst) {
    if (!sst) return;

    if (sst->data) {
        munmap(sst->data, sst->size);
    }

    free(sst);
}

racs_sstable_index_entry *racs_sstable_get_index(racs_sstable *sst) {
    return (racs_sstable_index_entry *)sst->index_ptr;
}
