#include "cache.h"

static auxts_cache_node* cache_node_create(auxts_cache_entry* entry);
static auxts_cache_entry* cache_entry_create(const uint64_t* key, uint8_t* value);
static void cache_node_destroy(auxts_cache_node* node);
static void cache_insert_at_head(auxts_cache* cache, auxts_cache_node* node);

auxts_cache* auxts_cache_create(size_t capacity) {
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
    cache->cache = auxts_hashtable_create(capacity);

    return cache;
}

void auxts_cache_put(auxts_cache* cache, const uint64_t* key, uint8_t* value) {
    if (!cache) {
        return;
    }

    pthread_rwlock_wrlock(&cache->rwlock);

    auxts_cache_entry* entry = auxts_hashtable_get(cache->cache, key);
    if (entry) {
        pthread_rwlock_unlock(&cache->rwlock);
        return;
    }

    if (cache->size >= cache->capacity) {
        auxts_cache_evict(cache);
    }

    entry = cache_entry_create(key, value);
    auxts_cache_node* node = cache_node_create(entry);

    cache_insert_at_head(cache, node);
    auxts_hashtable_put(cache->cache, key, entry);

    ++cache->size;

    pthread_rwlock_unlock(&cache->rwlock);
}

uint8_t* auxts_cache_get(auxts_cache* cache, const uint64_t* key) {
    if (!cache) {
        return NULL;
    }

    pthread_rwlock_rdlock(&cache->rwlock);

    auxts_cache_entry* entry = auxts_hashtable_get(cache->cache, key);
    pthread_rwlock_unlock(&cache->rwlock);

    if (entry) {
        return entry->value;
    }

    return NULL;
}

void auxts_cache_destroy(auxts_cache* cache) {
    if (!cache) {
        return;
    }

    pthread_rwlock_wrlock(&cache->rwlock);

    auxts_cache_node* node = cache->head;

    while (node) {
        auxts_cache_node* next = (auxts_cache_node*) node->next;
        cache_node_destroy(node);
        node = next;
    }

    auxts_hashtable_destroy(cache->cache);

    pthread_rwlock_unlock(&cache->rwlock);
    pthread_rwlock_destroy(&cache->rwlock);

    free(cache);
}

void auxts_cache_evict(auxts_cache* cache) {
    if (!cache) {
        return;
    }

    auxts_cache_node* tail = cache->tail;
    auxts_cache_entry* entry = tail->entry;

    free(entry->value);
    auxts_hashtable_delete(cache->cache, entry->key);

    cache->tail = (auxts_cache_node*) tail->prev;
    cache->tail->next = NULL;

    --cache->size;
}

auxts_cache_entry* cache_entry_create(const uint64_t* key, uint8_t* value) {
    auxts_cache_entry* entry = malloc(sizeof(auxts_cache_entry));
    if (!entry) {
        perror("Failed to allocate auxts_cache_entry");
        return NULL;
    }

    entry->key[0] = key[0];
    entry->key[1] = key[1];
    entry->value = value;

    return entry;
}

auxts_cache_node* cache_node_create(auxts_cache_entry* entry) {
    if (!entry) {
        return NULL;
    }

    auxts_cache_node* node = malloc(sizeof(auxts_cache_node));
    if (!node) {
        perror("Failed to allocate auxts_cache_node");
        return NULL;
    }

    node->entry = entry;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

void cache_insert_at_head(auxts_cache* cache, auxts_cache_node* node) {
    if (!cache || !node) {
        return;
    }

    node->prev = NULL;
    node->next = (struct auxts_cache_node*)cache->head;

    if (cache->head) {
        cache->head->prev = (struct auxts_cache_node*)node;
    } else {
        cache->tail = node;
    }

    cache->head = node;
}

void cache_node_destroy(auxts_cache_node* node) {
    if (!node) {
        return;
    }

    auxts_cache_entry* entry = node->entry;
    free(entry->value);
    free(node);
}
