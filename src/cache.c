#include "cache.h"

static auxts_cache_node* cache_node_create(const uint64_t* key, uint8_t* value);
static void cache_node_destroy(auxts_cache_node* node);
static void cache_insert_at_head(auxts_cache* cache, auxts_cache_node* node);

auxts_cache* auxts_scache_create(size_t capacity) {
    auxts_cache* cache = malloc(sizeof(auxts_cache));
    if (!cache) {
        perror("Failed to allocate auxts_cache");
        return NULL;
    }

    pthread_rwlock_init(&cache->rwlock, NULL);

    cache->size = 0;
    cache->capacity = capacity;
    cache->head = NULL;
    cache->tail = NULL;
    cache->kv = auxts_kvstore_create(capacity, auxts_cache_hash, auxts_cache_cmp, auxts_scache_destroy);

    return cache;
}

void auxts_cache_put(auxts_cache* cache, const uint64_t* key, uint8_t* value) {
    if (!cache) return;

    pthread_rwlock_wrlock(&cache->rwlock);

    auxts_cache_entry* entry = auxts_kvstore_get(cache->kv, key);
    if (entry) {
        pthread_rwlock_unlock(&cache->rwlock);
        return;
    }

    if (cache->size >= cache->capacity)
        auxts_cache_evict(cache);

    auxts_cache_node* node = cache_node_create(key, value);

    cache_insert_at_head(cache, node);
    auxts_kvstore_put(cache->kv, key, &node->entry);

    ++cache->size;

    pthread_rwlock_unlock(&cache->rwlock);
}

uint8_t* auxts_cache_get(auxts_cache* cache, const uint64_t* key) {
    if (!cache) return NULL;

    pthread_rwlock_rdlock(&cache->rwlock);

    auxts_cache_entry* entry = auxts_kvstore_get(cache->kv, key);
    pthread_rwlock_unlock(&cache->rwlock);

    if (entry) return entry->value;

    return NULL;
}

void auxts_cache_destroy(auxts_cache* cache) {
    if (!cache) return;

    pthread_rwlock_wrlock(&cache->rwlock);

    auxts_cache_node* node = cache->head;

//    while (node) {
//        auxts_cache_node* next = (auxts_cache_node*) node->next;
//        cache_node_destroy(node);
//        node = next;
//    }

    auxts_kvstore_destroy(cache->kv);

    pthread_rwlock_unlock(&cache->rwlock);
    pthread_rwlock_destroy(&cache->rwlock);

    free(cache);
}

void auxts_cache_evict(auxts_cache* cache) {
    if (!cache) return;

    auxts_cache_node* tail = cache->tail;
    auxts_cache_entry* entry = &tail->entry;

    auxts_kvstore_delete(cache->kv, entry->key);

    cache->tail = (auxts_cache_node*) tail->prev;
    cache->tail->next = NULL;

    --cache->size;
}


auxts_cache_node* cache_node_create(const uint64_t* key, uint8_t* value) {
    auxts_cache_node* node = malloc(sizeof(auxts_cache_node));
    if (!node) {
        perror("Failed to allocate auxts_cache_node");
        return NULL;
    }

    node->entry.key[0] = key[0];
    node->entry.key[1] = key[1];
    node->entry.value = value;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

void cache_insert_at_head(auxts_cache* cache, auxts_cache_node* node) {
    if (!cache || !node) return;

    node->prev = NULL;
    node->next = (struct auxts_cache_node*)cache->head;

    if (cache->head) {
        cache->head->prev = (struct auxts_cache_node*)node;
    } else {
        cache->tail = node;
    }

    cache->head = node;
}

uint64_t auxts_cache_hash(void* key) {
    uint64_t hash[2];
    murmur3_x64_128(key, 2 * sizeof(uint64_t), 0, hash);
    return hash[0];
}

int auxts_cache_cmp(void* a, void* b) {
    uint64_t* x = (uint64_t*)a;
    uint64_t* y = (uint64_t*)b;
    return x[0] == y[0] && x[1] == y[1];
}

void auxts_scache_destroy(void* key, void* value) {
    auxts_cache_entry* entry = (auxts_cache_entry*)value;
    free(entry->value);
}

void cache_node_destroy(auxts_cache_node* node) {
    if (!node) return;

    auxts_cache_entry* entry = &node->entry;
    free(entry->value);
    free(node);
}
