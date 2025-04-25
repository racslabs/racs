#include "cache.h"

static rats_cache_node* cache_node_create(const uint64_t* key, uint8_t* value);
static void cache_insert_at_head(rats_cache* cache, rats_cache_node* node);

rats_cache* rats_scache_create(size_t capacity) {
    rats_cache* cache = malloc(sizeof(rats_cache));
    if (!cache) {
        perror("Failed to allocate rats_cache");
        return NULL;
    }

    pthread_rwlock_init(&cache->rwlock, NULL);

    cache->size = 0;
    cache->capacity = capacity;
    cache->head = NULL;
    cache->tail = NULL;
    cache->kv = rats_kvstore_create(capacity, rats_cache_hash, rats_cache_cmp, rats_scache_destroy);

    return cache;
}

void rats_cache_put(rats_cache* cache, const uint64_t* key, uint8_t* value) {
    if (!cache) return;

    rats_cache_entry* entry = rats_kvstore_get(cache->kv, key);
    if (entry) return;

    pthread_rwlock_wrlock(&cache->rwlock);

    if (cache->size >= cache->capacity)
        rats_cache_evict(cache);

    rats_cache_node* node = cache_node_create(key, value);

    cache_insert_at_head(cache, node);
    rats_kvstore_put(cache->kv, key, &node->entry);

    ++cache->size;

    pthread_rwlock_unlock(&cache->rwlock);
}

uint8_t* rats_cache_get(rats_cache* cache, const uint64_t* key) {
    if (!cache) return NULL;

    pthread_rwlock_rdlock(&cache->rwlock);

    rats_cache_entry* entry = rats_kvstore_get(cache->kv, key);
    pthread_rwlock_unlock(&cache->rwlock);

    if (entry) return entry->value;

    return NULL;
}

void rats_cache_destroy(rats_cache* cache) {
    if (!cache) return;

    pthread_rwlock_wrlock(&cache->rwlock);
    rats_kvstore_destroy(cache->kv);

    rats_cache_node* node = cache->head;
    while (node) {
        rats_cache_node* next = (rats_cache_node*) node->next;
        free(node);
        node = next;
    }

    pthread_rwlock_unlock(&cache->rwlock);
    pthread_rwlock_destroy(&cache->rwlock);

    free(cache);
}

void rats_cache_evict(rats_cache* cache) {
    if (!cache) return;

    rats_cache_node* tail = cache->tail;
    rats_cache_entry* entry = &tail->entry;

    rats_kvstore_delete(cache->kv, entry->key);

    cache->tail = (rats_cache_node*) tail->prev;
    cache->tail->next = NULL;

    --cache->size;
}

rats_cache_node* cache_node_create(const uint64_t* key, uint8_t* value) {
    rats_cache_node* node = malloc(sizeof(rats_cache_node));
    if (!node) {
        perror("Failed to allocate rats_cache_node");
        return NULL;
    }

    node->entry.key[0] = key[0];
    node->entry.key[1] = key[1];
    node->entry.value = value;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

void cache_insert_at_head(rats_cache* cache, rats_cache_node* node) {
    if (!cache || !node) return;

    node->prev = NULL;
    node->next = (struct rats_cache_node*)cache->head;

    if (cache->head) {
        cache->head->prev = (struct rats_cache_node*)node;
    } else {
        cache->tail = node;
    }

    cache->head = node;
}

uint64_t rats_cache_hash(void* key) {
    uint64_t hash[2];
    murmur3_x64_128(key, 2 * sizeof(uint64_t), 0, hash);
    return hash[0];
}

int rats_cache_cmp(void* a, void* b) {
    uint64_t* x = (uint64_t*)a;
    uint64_t* y = (uint64_t*)b;
    return x[0] == y[0] && x[1] == y[1];
}

void rats_scache_destroy(void* key, void* value) {
    rats_cache_entry* entry = (rats_cache_entry*)value;
    free(entry->value);
}
