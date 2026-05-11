#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include "common.h"

#define MAX_QUEUE_SIZE 1000

typedef struct {
    // Trzy oddzielne kolejki do obsługi priorytetów
    Task* tasks[3][MAX_QUEUE_SIZE];
    int counts[3];          // Liczba zadań w każdej kolejce
    int head[3];            // Wskaźnik na początek (FIFO)
    int tail[3];            // Wskaźnik na koniec
    
    pthread_mutex_t mutex;  // Ochrona sekcji krytycznej
    pthread_cond_t cond;    // Zmienna warunkowa do oczekiwania konsumentów
    int stop_flag;          // Mechanizm zatrzymania systemu
} JobQueue;

void queue_init(JobQueue* q);
void queue_push(JobQueue* q, Task* t);
Task* queue_pop(JobQueue* q);
void queue_shutdown(JobQueue* q);

#endif
