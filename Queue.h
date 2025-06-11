#ifndef OSHW3_QUEUE_H
#define OSHW3_QUEUE_H

#include <pthread.h>

typedef struct {
    int front;
    int rear;
    int capacity;
    pthread_mutex_t lock;
    pthread_cond_t emptyCond, fullCond;
    int arr[];
} Queue;

Queue* createQueue(int capacity);

void destroyQueue(Queue* q);

int isEmptyQueue(Queue *q);

void enqueueQueue(Queue* q, int data);

int dequeueQueue(Queue* q);

int queueHead(Queue* q);

#endif