#include "version.h"


racs_uint64 racs_versions_hash(void *key) {
    racs_uint64 hash[2];
    murmur3_x64_128(key, 2 * sizeof(racs_uint64), 0, hash);
    return hash[0];
}

int racs_versions_cmp(void *a, void *b) {
    racs_uint64 *x = a;
    racs_uint64 *y = b;
    return x[0] == y[0];
}

void racs_versions_destroy_entry(void *key, void *value) {
    free(key);
    free(value);
}

racs_versions *racs_versions_create() {
    racs_versions *versions = malloc(sizeof(racs_versions));
    if (!versions) {
        racs_log_error("Failed to allocate racs_versions");
        return NULL;
    }

    versions->kv = racs_kvstore_create(RACS_MAX_VERSIONS, racs_versions_hash, racs_versions_cmp, racs_versions_destroy_entry);
    pthread_rwlock_init(&versions->rwlock, NULL);

    return versions;
}

racs_uint64 racs_versions_get(racs_versions *versions, racs_uint64 stream_id) {
    pthread_rwlock_rdlock(&versions->rwlock);

    racs_uint64 key[2] = { stream_id, 0 };
    racs_uint64 *version = racs_kvstore_get(versions->kv, key);

    pthread_rwlock_unlock(&versions->rwlock);

    if (!version) return 0;
    return *version;
}

void racs_versions_put(racs_versions *versions, racs_uint64 stream_id, racs_uint64 version) {
    pthread_rwlock_wrlock(&versions->rwlock);

    racs_uint64 *key = malloc(2 * sizeof(racs_uint64));
    if (!key) {
        racs_log_error("Failed to allocate key.");
        return;
    }

    key[0] = stream_id;
    key[1] = 0;

    racs_uint64 *_version = malloc(sizeof(racs_uint64));
    if (!_version) {
        racs_log_error("Failed to allocate version");
        return;
    }

    memcpy(_version, &version, sizeof(racs_uint64));

    racs_kvstore_put(versions->kv, key, _version);
    pthread_rwlock_unlock(&versions->rwlock);
}

void racs_versions_init(racs_versions *versions) {
    char *path = NULL;
    asprintf(&path, "%s/.racs/md", racs_metadata_dir);

    racs_filelist *list = racs_sorted_filelist(path);

    for (int i = 0; i < list->num_files; ++i) {
        racs_uint64 stream_id = racs_path_to_stream_id(list->files[i]);

        racs_metadata metadata;
        int rc = racs_metadata_get(&metadata, stream_id);

        if (rc == 1)
            racs_versions_put(versions, stream_id, metadata.version);
    }

    racs_filelist_destroy(list);
    free(path);
}

void racs_versions_destroy(racs_versions *versions) {
    pthread_rwlock_wrlock(&versions->rwlock);
    racs_kvstore_destroy(versions->kv);
    pthread_rwlock_unlock(&versions->rwlock);

    pthread_rwlock_destroy(&versions->rwlock);
}
