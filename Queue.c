#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Queue.h"

Queue* createQueue(int capacity) {

    if (capacity <= 0) {
        return NULL;
    }

    Queue* q = (Queue*)malloc(sizeof (Queue) + (capacity + 1) * sizeof (int));

    if(!q) {
        return NULL;
    }

    q->front = 0;
    q->rear = 0;
    q->capacity = capacity + 1;

    return q;
}

void destroyQueue(Queue* q) {
    if(q) {
        free(q);
    }
}

int isEmptyQueue(Queue *q) {
    return q && (q->front == q->rear);
}

void enqueueQueue(Queue* q, int data) {
    if(!q) return;
    if((q->rear + 1) % q->capacity != q->front) {
        q->arr[q->rear] = data;
        q->rear = (q->rear + 1) % q->capacity;
    }
}

int dequeueQueue(Queue* q) {
    if(!q) return -1;
    if(q->front != q->rear) {
        int data = q->arr[q->front];
        q->front = (q->front + 1) % q->capacity;
        return data;
    }
    return -1;
}

int queueHead(Queue* q) {
    if(!q) return -1;
    if(q->front != q->rear) {
        return q->arr[q->front];
    }
    return -1;
}
