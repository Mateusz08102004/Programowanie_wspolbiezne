#include "queue.h"
#include <stdlib.h>

void queue_init(JobQueue* q) {
    for(int i=0; i<3; i++) {
        q->counts[i] = 0; q->head[i] = 0; q->tail[i] = 0;
    }
    q->stop_flag = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

void queue_push(JobQueue* q, Task* t) {
    pthread_mutex_lock(&q->mutex);
    
    int p = t->priority;
    if (q->counts[p] < MAX_QUEUE_SIZE) {
        q->tasks[p][q->tail[p]] = t;
        q->tail[p] = (q->tail[p] + 1) % MAX_QUEUE_SIZE;
        q->counts[p]++;
        
        // Powiadomienie konsumenta o pojawieniu się zadania
        pthread_cond_signal(&q->cond);
    }
    
    pthread_mutex_unlock(&q->mutex);
}

Task* queue_pop(JobQueue* q) {
    pthread_mutex_lock(&q->mutex);
    
    // Oczekiwanie przy pustych kolejkach i działającym systemie
    while (q->counts[0] == 0 && q->counts[1] == 0 && q->counts[2] == 0 && !q->stop_flag) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    
    if (q->stop_flag && q->counts[0] == 0 && q->counts[1] == 0 && q->counts[2] == 0) {
        pthread_mutex_unlock(&q->mutex);
        return NULL;
    }

    // Wybór zadania od najwyższego priorytetu [cite: 32]
    Task* t = NULL;
    for (int i = 0; i < 3; i++) {
        if (q->counts[i] > 0) {
            t = q->tasks[i][q->head[i]];
            q->head[i] = (q->head[i] + 1) % MAX_QUEUE_SIZE;
            q->counts[i]--;
            break;
        }
    }
    
    pthread_mutex_unlock(&q->mutex);
    return t;
}
