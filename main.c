#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "queue.h"

// Funkcja pomocnicza do tworzenia zadań
Task* create_task(int id, Priority prio, TaskType type) {
    Task* t = (Task*)malloc(sizeof(Task));
    t->id = id;
    t->priority = prio;
    t->type = type;
    clock_gettime(CLOCK_MONOTONIC, &t->time_added); 
    return t;
}

// Funkcja wątku konsumenta (Worker)
// Nowa, ulepszona funkcja wątku konsumenta do celów testowych
void* worker_thread(void* arg) {
    JobQueue* q = (JobQueue*)arg;
    printf("[Worker] Watek roboczy uruchomiony.\n");

    // Jedna sekunda dla wątku Main na dodanie pierwszych zadań z pierwszej fazy
    // zapobiegająca pobraniu ich w locie
    sleep(1); 

    while (1) {
        Task* t = queue_pop(q);
        
        if (t == NULL) {
            printf("[Worker] Odebrano sygnal stopu. Koncze prace.\n");
            break;
        }

        printf("[Worker] Pobrano zadanie ID: %d (Priorytet: %d)\n", t->id, t->priority);
        
        // Symulacja czasu przetwarzania zadania (np. kompresji / MD5)
        usleep(100000); // 100 milisekund
        
        free(t);
    }
    return NULL;
}

int main() {
    JobQueue q;
    queue_init(&q);

    // 1. Utworzenie jednego wątku konsumenta
    pthread_t worker;
    pthread_create(&worker, NULL, worker_thread, &q);

    // Chwila na uruchomienie i oczekiwanie
    usleep(50000);

    // 2. Pierwsza faza: dodawanie pojedynczych zadań
    printf("[Main] Dodaje zadanie LOW (ID: 1)\n");
    queue_push(&q, create_task(1, PRIO_LOW, TASK_MD5));
    usleep(150000); // Czas dla konsumenta na pobranie i przetworzenie

    printf("[Main] Dodaje zadanie MEDIUM (ID: 2)\n");
    queue_push(&q, create_task(2, PRIO_MEDIUM, TASK_COMPRESSION));
    usleep(150000);

    printf("[Main] Dodaje zadanie HIGH (ID: 3)\n");
    queue_push(&q, create_task(3, PRIO_HIGH, TASK_MD5));
    
    // Oczekiwanie na przetworzenie wszystkich zadań z pierwszej fazy
    sleep(1);

    // 3. FAZA ZATORU (Poprawiona)
    // Symulacja zatoru bez blokowania mutexu w Main:
    // Ekstremalnie szybkie wrzucenie zadań jedno po drugim.
    // Czas przetwarzania każdego zadania wynosi 100 ms (usleep w Workerze),
    // więc natychmiastowe wrzucenie trzech zadań powoduje ich ułożenie w kolejce
    // i wybór drugiego oraz trzeciego zadania przez konsumenta na podstawie priorytetu.
    
    printf("\n[Main] Symulacja zatoru: dodaje 3 zadania bardzo szybko...\n");
    queue_push(&q, create_task(10, PRIO_LOW, TASK_MD5));
    queue_push(&q, create_task(20, PRIO_HIGH, TASK_MD5));
    queue_push(&q, create_task(30, PRIO_MEDIUM, TASK_COMPRESSION));

    // Oczekiwanie 1,5 sekundy na zakończenie przetwarzania wszystkich zadań z zatoru
    sleep(2);

    // 4. Bezpieczne zamknięcie systemu (shutdown)
    printf("\n[Main] Zamykam kolejke...\n");
    pthread_mutex_lock(&q.mutex);
    q.stop_flag = 1;
    pthread_cond_broadcast(&q.cond); 
    pthread_mutex_unlock(&q.mutex);

    // Oczekiwanie na zakończenie wątku konsumenta
    pthread_join(worker, NULL);
    printf("[Main] Program zakonczyl sie poprawnie.\n");

    return 0;
}
