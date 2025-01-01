#include "hashmap.h"

Hashmap* create_hashmap(size_t size) {
    Hashmap* map = malloc(sizeof(Hashmap));
    if (!map) {
        perror("Error allocating Hashmap");
        exit(-1);
    }

    map->size = size;
    map->buckets = malloc(size * sizeof(LinkedList*));

    for (int i = 0; i < map->size; ++i) {
        map->buckets[i] = create_list();
    }

    if (!map->buckets) {
        perror("Failed to allocate buckets");
        exit(-1);
    }

    return map;
}

HashmapEntry* create_hashmap_entry(const uint64_t* key, void* value) {
    HashmapEntry* entry = malloc(sizeof(HashmapEntry));
    if (!entry) {
        perror("Failed to allocate HashmapEntry");
        exit(-1);
    }

    entry->key[0] = key[0];
    entry->key[1] = key[1];
    entry->value = value;

    return entry;
}

void destroy_hashmap_entry(HashmapEntry* entry) {
    free(entry->value);
    free(entry);
}

void put_hashmap(Hashmap* map, uint64_t* key, void* value) {
    uint64_t _key = hash((uint8_t*) key, 2 * sizeof(uint64_t), map->size);

    LinkedList* list = map->buckets[_key];
    Node* curr = list->head;

    while (curr) {
        HashmapEntry* entry = (HashmapEntry*) curr->data;
        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            free(entry->value);
            entry->value = value;
            return;
        }

        curr = (Node*) curr->next;
    }

    HashmapEntry* entry = create_hashmap_entry(key, value);
    curr = create_node(entry);
    insert_at_tail(list, curr);
}

void* get_hashmap(Hashmap* map, uint64_t* key) {
    uint64_t _key = hash((uint8_t*) key, 2 * sizeof(uint64_t), map->size);

    LinkedList* list = map->buckets[_key];
    Node* curr = list->head;

    while (curr) {
        HashmapEntry* entry = (HashmapEntry*) curr->data;
        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            return entry->value;
        }

        curr = (Node*) curr->next;
    }

    return NULL;
}

void delete_hashmap(Hashmap* map, uint64_t* key) {
    uint64_t _key = hash((uint8_t*) key, 2 * sizeof(uint64_t), map->size);

    LinkedList* list = map->buckets[_key];
    Node* prev = NULL;
    Node* curr = list->head;

    while (curr) {
        HashmapEntry* entry = (HashmapEntry*) curr->data;
        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            if (prev) {
                prev->next = curr->next;
            } else {
                list->head = (Node*) curr->next;
            }

            if (curr == list->tail) {
                list->tail = prev;
            }

            destroy_hashmap_entry(entry);
            free(curr);
            return;
        }

        prev = curr;
        curr = (Node*) curr->next;
    }
}


void destroy_hashmap(Hashmap* map) {
    for (int i = 0; i < map->size; ++i) {
        LinkedList* list = map->buckets[i];
        Node* node = list->head;

        while (node) {
            Node* next = (Node*) node->next;

            HashmapEntry* entry = node->data;
            destroy_hashmap_entry(entry);
            free(node);

            node = next;
        }
    }

    free(map);
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

    printf("%s\n", (char*)get_hashmap(map, key1));
    printf("%s\n", (char*)get_hashmap(map, key2));

    uint8_t* data3 = (uint8_t*) strdup("data3");
    put_hashmap(map, key1, data3);

    printf("%s\n", (char*)get_hashmap(map, key1));

    destroy_hashmap(map);

    return 0;
}
