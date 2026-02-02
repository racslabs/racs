#include "ttl.h"

void racs_ttl(racs_offsets *offsets) {
    char *path = NULL;
    asprintf(&path, "%s/.racs/md", racs_metadata_dir);

    racs_filelist *list = racs_sorted_filelist(path);

    for (int i = 0; i < list->num_files; ++i) {
        racs_uint64 stream_id = racs_path_to_stream_id(list->files[i]);

        racs_metadata metadata;
        int rc = racs_metadata_get(&metadata, stream_id);

        if (rc == 1) {
            racs_time ttl = metadata.ttl;
            if (racs_ttl_is_expired(ttl))
                racs_ttl_delete_stream(offsets, stream_id);

            racs_metadata_destroy(&metadata);
        }
    }

    racs_filelist_destroy(list);
    free(path);
}

void racs_ttl_async(racs_offsets *offsets) {
    pthread_t thread;
    pthread_create(&thread, NULL, racs_ttl_worker, offsets);
    pthread_detach(thread);
}

void *racs_ttl_worker(void *arg) {
    while (1) {
        racs_offsets *offsets = (racs_offsets *)arg;
        racs_ttl(offsets);
        usleep(1000);
    }
}

int racs_ttl_expire(racs_uint64 stream_id, racs_time ttl) {
    racs_metadata metadata;
    int rc = racs_metadata_get(&metadata, stream_id);
    if (rc == 1) {
        metadata.ttl = racs_time_now() + (ttl * 1000);
        racs_metadata_put(&metadata, stream_id);
        racs_metadata_destroy(&metadata);

        return 1;
    }

    return 0;
}

int racs_ttl_is_expired(racs_time ttl) {
    if (ttl == -1) return 0;
    return ttl < racs_time_now();
}

void racs_ttl_delete_stream(racs_offsets *offsets, racs_uint64 stream_id) {
    char *path = NULL;

    asprintf(&path, "%s/.racs/md/%llu", racs_metadata_dir, stream_id);
    racs_remove(path);
    free(path);

    asprintf(&path, "%s/.racs/seg/%llu", racs_metadata_dir, stream_id);
    racs_log_info("deleting files in %s", path);
    racs_remove(path);

    racs_offsets_put(offsets, stream_id, 0);

    free(path);
}

static int unlink_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    return remove(fpath);
}

int racs_remove(const char *path) {
    return nftw(path, unlink_callback, 64, FTW_DEPTH | FTW_PHYS);
}
