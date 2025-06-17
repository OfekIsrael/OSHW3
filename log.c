#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "log.h"


// Opaque struct definition
struct Server_Log {
    const char** data;
    int log_size;

    int readers_inside, writers_inside, writers_waiting;
    pthread_cond_t read_allowed;
    pthread_cond_t write_allowed;
    pthread_mutex_t log_lock;
};


server_log create_log() {
    server_log serLog = malloc(sizeof(struct Server_Log));
    serLog->data = malloc(MAXBUF * sizeof(char*));

    for (int i = 0; i < MAXBUF; i++) {
        serLog->data[i] = NULL;
    }

    serLog->log_size = 0;
    serLog->readers_inside = 0;
    serLog->writers_inside = 0;
    serLog->writers_waiting = 0;

    pthread_mutex_init(&serLog->log_lock, NULL);
    pthread_cond_init(&serLog->read_allowed, NULL);
    pthread_cond_init(&serLog->write_allowed, NULL);

    return serLog;
}

// Destroys and frees the log (stub)
void destroy_log(server_log log) {
    for (int i = 0; i < log->log_size; i++) {
        free((char*)log->data[i]);
    }
    free(log->data);
    free(log);
}

// Returns dummy log content as string (stub)
int get_log(server_log log, char** dst) {
    reader_lock(log);
    int len = 0;
    for (int i = 0; i < log->log_size; i++) {
        const char* line = log->data[i];
        len += strlen(line);
        dst[i] = (char*)malloc(len + 1); // Allocate for caller
        if (dst[i] != NULL) {
            strcpy(dst[i], line);
        }
    }
    reader_unlock(log);
    return len;
}

// Appends a new entry to the log (no-op stub)
void add_to_log(server_log log, const char* data, int data_len) {
    writer_lock(log);
    char* line = (char*)malloc(strlen(data) + 1);
    strcpy(line, data);
    log->data[log->log_size++] = line;
    writer_unlock(log);
}


void reader_lock(server_log log) {
    pthread_mutex_lock(&log->log_lock);
    while (log->writers_inside > 0 || log->writers_waiting > 0)
        pthread_cond_wait(&log->read_allowed, &log->log_lock);
    log->readers_inside++;
    pthread_mutex_unlock(&log->log_lock); 
}

void reader_unlock(server_log log) {
    pthread_mutex_lock(&log->log_lock);
    log->readers_inside--;
    if (log->readers_inside == 0)
        pthread_cond_signal(&log->write_allowed);
    pthread_mutex_unlock(&log->log_lock);
}


void writer_lock(server_log log) {
    pthread_mutex_lock(&log->log_lock);
    while (log->writers_inside > 0 || log->writers_waiting > 0)
        pthread_cond_wait(&log->read_allowed, &log->log_lock);
        pthread_cond_wait(&log->write_allowed, &log->log_lock);
    log->writers_waiting--;
    log->writers_inside++;
    pthread_mutex_unlock(&log->log_lock);
}

void writer_unlock(server_log log) {
    pthread_mutex_lock(&log->log_lock);
    log->writers_inside--;
    if (log->writers_inside == 0) {
        pthread_cond_broadcast(&log->read_allowed);
        pthread_cond_signal(&log->write_allowed);
    }
    pthread_mutex_unlock(&log->log_lock);
}

