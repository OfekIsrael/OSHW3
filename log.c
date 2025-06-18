#include "log.h"


// Opaque struct definition


server_log create_log() {
    server_log serLog = malloc(sizeof(struct Server_Log));
    serLog->log = create_list();
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
    free_list(log->log);
    pthread_mutex_destroy(&log->log_lock);
    pthread_cond_destroy(&log->read_allowed);
    pthread_cond_destroy(&log->write_allowed);
    free(log);
}


int get_log(server_log log, char** dst) {
    reader_lock(log);
    dst = malloc(log->log_size * sizeof(char*));
    char**temp = dst;
    int len = 0;
    Node *current = log->log->head;
    while (current) {
        const char* line = current->data;
        len += strlen(line);
        *temp = (char*)malloc(len + 1); // Allocate for caller
        if (*temp != NULL) {
            strcpy(*temp, line);
        }
        temp++;
        current = current->next;
    }
    reader_unlock(log);
    return len;
}

// Appends a new entry to the log (no-op stub)
void add_to_log(server_log log, const char* data, int data_len) {
    writer_lock(log);
    insert_back(log->log, data, data_len);
    log->log_size++;
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
    log->writers_waiting++;
    while (log->writers_inside + log->readers_inside > 0) {
        pthread_cond_wait(&log->write_allowed, &log->log_lock);
    }
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

