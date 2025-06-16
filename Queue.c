#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Queue.h"
#include <pthread.h>

Queue* createQueue(int capacity) {
    if (capacity <= 0) {
        return NULL;
    }

    Queue* q = (Queue*)malloc(sizeof(Queue) + capacity * sizeof(Request));
    if (!q) {
        return NULL;
    }

    q->front = 0;
    q->rear = 0;
    q->capacity = capacity;
    q->requestsProcessing = 0;
    q->size = 0;

    if (pthread_mutex_init(&(q->lock), NULL) != 0) {
        free(q);
        return NULL;
    }

    pthread_cond_init(&(q->emptyCond), NULL);
    pthread_cond_init(&(q->fullCond), NULL);

    return q;
}

void destroyQueue(Queue* q) {
    if(q) {
        pthread_mutex_destroy(&(q->lock));
        pthread_cond_destroy(&(q->emptyCond));
        pthread_cond_destroy(&(q->fullCond));
        free(q);
    }
}

void enqueueQueue(Queue* q, Request* data) {

    if(!q) return;

    pthread_mutex_lock(&(q->lock));

    while(q->size + q->requestsProcessing >= q->capacity) {
        pthread_cond_wait(&(q->fullCond), &(q->lock));
    }

    q->arr[q->rear] = data;
    q->rear = (q->rear + 1) % q->capacity;
    q->size++;

    pthread_cond_signal(&(q->emptyCond));
    pthread_mutex_unlock(&(q->lock));
}

Request* dequeueQueue(Queue* q) {

    if(!q) return NULL;

    pthread_mutex_lock(&(q->lock));

    while(q->size == 0) {
        pthread_cond_wait(&(q->emptyCond), &(q->lock));
    }

    Request* data = q->arr[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    q->requestsProcessing++;

    pthread_mutex_unlock(&(q->lock));

    return data;
}

void requestComplete(Queue* q) {
    if(!q) return;

    pthread_mutex_lock(&(q->lock));

    if(q->requestsProcessing > 0) {
        q->requestsProcessing--;
    }

    pthread_cond_signal(&(q->fullCond));
    pthread_mutex_unlock(&(q->lock));
}
