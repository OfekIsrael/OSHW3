//
// Created by student on 6/18/25.
//
#include "LinkedList.h"

LinkedList* create_list(void) {
    LinkedList *list = malloc(sizeof(LinkedList));
    if (!list) return NULL;
    list->head = NULL;
    list->back = NULL;
    list->size = 0;
    return list;
}

int insert_back(LinkedList *list, const char *str, int strLen) {
    Node *node = malloc(sizeof(Node));
    if (!node) return 0;
    node->data = malloc(strLen +1);
    if (!node->data) {
        free(node);
        return 0;
    }
    strcpy(node->data, str);
    node->next = NULL;
    if (!list->head && !list->back) {
        list->head = node;
        list->back = node;
    }else {
        list->back->next = node;
        list->back = list->back->next;
    }
    list->size++;
    return 1;
}

void free_list(LinkedList *list) {
    if (!list) return;

    Node *current = list->head;
    while (current) {
        Node *temp = current;
        current = current->next;
        free(temp->data);
        free(temp);
    }
    free(list);
}