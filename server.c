#include "segel.h"
#include "request.h"
#include "log.h"
#include "Queue.h"
#include <stdlib.h>

Queue* requestQueue;
server_log log1;

// Parses command-line arguments
void getargs(int *port, int *numThreads, int *queueSize, int argc, char *argv[])
{
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <port> <threads> <queue_size>\n", argv[0]);
        exit(1);
    }
    *port = atoi(argv[1]);
    *numThreads = atoi(argv[2]);
    *queueSize = atoi(argv[3]);
}

void* workerThread(void* arg) {
    int id = *(int*)arg;

    threads_stats t = malloc(sizeof(struct Threads_stats));
    t->id = id;
    t->stat_req = 0;       // Static request count
    t->dynm_req = 0;       // Dynamic request count
    t->total_req = 0;      // Total request count
    t->post_req = 0;       // POST request count

    while(1) {

        Request* req = dequeueQueue(requestQueue);

        printf("Thread %d dequeued request fd %d\n", id, req->connfd);

        struct timeval dispatch, current;
        gettimeofday(&current, NULL);
        timersub(&current, &(req->arrival), &dispatch);

        t->total_req++;

        requestHandle(req->connfd, req->arrival, dispatch, t, log1);

        requestComplete(requestQueue);

        Close(req->connfd); // Close the connection
        free(req);
    }

    free(t); // Cleanup
}

int main(int argc, char *argv[])
{
    printf("OS-HW3 Web Server\n");
    // Create the global server log
    log1 = create_log();

    int listenfd, connfd, port, clientlen, numThreads, queueSize;
    struct sockaddr_in clientaddr;

    getargs(&port, &numThreads, &queueSize, argc, argv);

    requestQueue = createQueue(queueSize);
    pthread_t* threads = malloc(numThreads * sizeof(pthread_t));
    int* threadIds = malloc(numThreads * sizeof(int));

    if(!requestQueue || !threads || !threadIds) {
        return 0;
    }

    for(int i = 0; i < numThreads; i++) {
        threadIds[i] = i;
        if(pthread_create(&threads[i], NULL, workerThread, &threadIds[i]) != 0) {
            free(threads);
            free(threadIds);
            return 0;
        }
    }

    listenfd = Open_listenfd(port);
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

        struct timeval arrival;
        gettimeofday(&arrival, NULL);

        Request* req = malloc(sizeof(Request));
        if (!req) {
            return 0;
        }

        req->connfd = connfd;
        req->arrival = arrival;

        enqueueQueue(requestQueue, req);

    }

    // Clean up the server log before exiting
    destroy_log(log1);

    // TODO: HW3 — Add cleanup code for thread pool and queue
}
