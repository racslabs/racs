
#include "list.h"

Node* create_node(void* data) {
    Node* node = malloc(sizeof(Node));
    if (!node) {
        perror("Failed to allocate Node");
        exit(-1);
    }

    node->data = data;
    node->next = NULL;
    node->prev = NULL;

    return node;
}

LinkedList* create_list() {
    LinkedList* list = malloc(sizeof(LinkedList));
    if (!list) {
        perror("Failed to allocate LinkedList");
        exit(-1);
    }

    list->head = NULL;
    list->tail = NULL;

    return list;
}

void insert_at_tail(LinkedList* list, Node* node) {
    if (!list || !node) return;

    node->prev = NULL;
    node->next = (struct Node*) list->head;

    if (list->head) {
        list->head->prev = (struct Node*) node;
    } else {
        list->tail = node;
    }

    list->head = node;
}

void insert_at_head(LinkedList* list, Node* node) {
    if (!list || !node) return;

    node->prev = NULL;
    node->next = (struct Node*) list->head;

    if (list->head) {
        list->head->prev = (struct Node*) node;
    } else {
        list->tail = node;
    }

    list->head = node;
}

void delete_at_tail(LinkedList* list) {
    if (!list || !list->tail) return;

    Node* tail = list->tail;
    list->tail = (Node*) tail->prev;
    list->tail->next = NULL;

    destroy_node(tail);
}

void destroy_node(Node* node) {
    if (node->data) {
        free(node->data);
    }

    free(node);
}

void destroy_list(LinkedList* list) {
    Node* node = list->head;

    while (node) {
        Node* next = (Node*) node->next;
        destroy_node(node);
        node = next;
    }
}

int test_list() {
    LinkedList* list = create_list();

    uint8_t* data1 = (uint8_t*)strdup("abc");
    Node* node1 = create_node(data1);

    uint8_t* data2 = (uint8_t*)strdup("123");
    Node* node2 = create_node(data2);

    insert_at_tail(list, node1);
    insert_at_tail(list, node2);

    Node* node = list->head;
    while (node) {
        printf("%s\n", (char*)node->data);
        node = (Node*) node->next;
    }

    destroy_list(list);

    return 0;
}

