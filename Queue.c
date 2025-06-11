#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Queue.h"
#include <pthread.h>

Queue* createQueue(int capacity) {
    if (capacity <= 0) {
        return NULL;
    }

    Queue* q = (Queue*)malloc(sizeof(Queue) + (capacity + 1) * sizeof(int));
    if (!q) {
        return NULL;
    }

    q->front = 0;
    q->rear = 0;
    q->capacity = capacity + 1;

    if (pthread_mutex_init(&(q->lock), NULL) != 0) {
        free(q);
        return NULL;
    }
    if (pthread_cond_init(&(q->emptyCond), NULL) != 0) {
        pthread_mutex_destroy(&(q->lock));
        free(q);
        return NULL;
    }
    if (pthread_cond_init(&(q->fullCond), NULL) != 0) {
        pthread_mutex_destroy(&(q->lock));
        pthread_cond_destroy(&(q->emptyCond));
        free(q);
        return NULL;
    }

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

int isEmptyQueue(Queue *q) {

    if (!q) return 1;

    pthread_mutex_lock(&(q->lock));
    int empty = (q->front == q->rear);
    pthread_mutex_unlock(&(q->lock));

    return empty;
}

void enqueueQueue(Queue* q, int data) {

    if(!q) return;

    pthread_mutex_lock(&(q->lock));

    while((q->rear + 1) % q->capacity == q->front) {
        pthread_cond_wait(&(q->fullCond), &(q->lock));
    }

    q->arr[q->rear] = data;
    q->rear = (q->rear + 1) % q->capacity;

    pthread_cond_signal(&(q->emptyCond));
    pthread_mutex_unlock(&(q->lock));
}

int dequeueQueue(Queue* q) {

    if(!q) return -1;

    pthread_mutex_lock(&(q->lock));

    while(q->front == q->rear) {
        pthread_cond_wait(&(q->emptyCond), &(q->lock));
    }

    int data = q->arr[q->front];
    q->front = (q->front + 1) % q->capacity;

    pthread_cond_signal(&(q->fullCond));
    pthread_mutex_unlock(&(q->lock));

    return data;
}

int queueHead(Queue* q) {

    if(!q) return -1;

    pthread_mutex_lock(&(q->lock));

    int data = (q->front != q->rear) ? q->arr[q->front] : -1;

    pthread_mutex_unlock(&(q->lock));

    return data;
}
