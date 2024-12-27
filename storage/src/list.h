
#ifndef AUXTS_LIST_H
#define AUXTS_LIST_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    void* data;
    struct Node* prev;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
} LinkedList;

Node* create_node(void* data);

void destroy_node(Node* node);

LinkedList* create_list();

void destroy_list(LinkedList* list);

void insert_at_head(LinkedList* list, Node* node);

void insert_at_tail(LinkedList* list, Node* node);

void delete_at_tail(LinkedList* list);

int test_list();

#endif //AUXTS_LIST_H
