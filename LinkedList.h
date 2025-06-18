#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "segel.h"

typedef struct Node {
    char *data;
    struct Node *next;
} Node;

typedef struct LinkedList {
    Node *head;
    Node *back;
    int size;
} LinkedList;

LinkedList* create_list(void);

int insert_back(LinkedList *list, const char *str, int strLen);

void free_list(LinkedList *list);







#endif //LINKEDLIST_H
