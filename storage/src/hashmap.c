#include "hashmap.h"

Hashmap* create_hashmap(size_t size) {
    Hashmap* map = malloc(sizeof(Hashmap));
    if (!map) {
        perror("Error allocating Hashmap");
        exit(-1);
    }

    map->size = size;
    map->buckets = malloc(size * sizeof(LinkedList *));

    if (!map->buckets) {
        perror("Failed to allocate buckets");
        exit(-1);
    }

    return map;
}

HashmapEntry* create_hashmap_entry(uint64_t* key, uint8_t* value) {
    HashmapEntry* entry = malloc(sizeof(HashmapEntry));
    if (!entry) {
        perror("Failed to allocate HashmapEntry");
        exit(-1);
    }

    entry->key = key;
    entry->value = value;

    return entry;
}

void put_hashmap(Hashmap* map, uint64_t* key, uint8_t* value) {
    uint64_t _key = hash((uint8_t*) key, 2 * sizeof(uint64_t), map->size);

    if (!map->buckets[_key]) {
        map->buckets[_key] = create_list();
    }

    LinkedList* list = map->buckets[_key];
    Node* node = list->head;

    while (node) {
        HashmapEntry* entry = (HashmapEntry*) node->data;
        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            return;
        }

        node = (Node*) node->next;
    }

    HashmapEntry* entry = create_hashmap_entry(key, value);
    node = create_node(entry);
    insert_at_tail(list, node);
}

uint8_t* get_hashmap(Hashmap* map, uint64_t* key) {
    uint64_t _key = hash((uint8_t*) key, 2 * sizeof(uint64_t), map->size);

    LinkedList* list = map->buckets[_key];
    Node* node = list->head;

    while (node) {
        HashmapEntry* entry = (HashmapEntry*) node->data;
        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            return entry->value;
        }

        node = (Node*) node->next;
    }

    return NULL;
}

int test_hashmap() {
    uint64_t key1[2];
    key1[0] = 1;
    key1[1] = 2;

    uint64_t key2[2];
    key2[0] = 3;
    key2[1] = 4;

    Hashmap* map = create_hashmap(4);

    uint8_t* data1 = (uint8_t*)strdup("data1");
    put_hashmap(map, key1, data1);

    uint8_t* data2 = (uint8_t*)strdup("data2");
    put_hashmap(map, key2, data2);

    printf("%s\n", get_hashmap(map, key1));
    printf("%s\n", get_hashmap(map, key2));

    return 0;
}
