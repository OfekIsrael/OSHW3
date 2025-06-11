#ifndef OSHW3_QUEUE_H
#define OSHW3_QUEUE_H

#include <pthread.h>

typedef struct {
    int connfd;
    struct timeval arrival;
} Request;

typedef struct {
    int front;
    int rear;
    int capacity;
    pthread_mutex_t lock;
    pthread_cond_t emptyCond, fullCond;
    Request* arr[];
} Queue;

Queue* createQueue(int capacity);

void destroyQueue(Queue* q);

void enqueueQueue(Queue* q, Request* data);

Request* dequeueQueue(Queue* q);

#endif