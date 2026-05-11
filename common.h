#ifndef COMMON_H
#define COMMON_H

#include <time.h>

typedef enum {
    PRIO_HIGH = 0,
    PRIO_MEDIUM = 1,
    PRIO_LOW = 2
} Priority;

typedef enum {
    TASK_COMPRESSION,
    TASK_MD5
} TaskType;

// Struktura pojedynczego zadania
typedef struct {
    int id;
    Priority priority;
    TaskType type;
    struct timespec time_added;    // Statystyki czasu oczekiwania
    struct timespec time_started;  // Statystyki czasu przetwarzania
} Task;

#endif
