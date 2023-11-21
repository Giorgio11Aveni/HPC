// header_thread.h

#include <pthread.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define MAX_LINE_LENGTH 1024
#define MAX_THREADS 3
#define MAX_ELEMENTS_IN_ROW 4


// Struttura dati per passare l'ID del thread

typedef struct {
    int mpiRank;
    int threadID;
    int rowCount;
    float*** rows;
    pthread_mutex_t mutex;  // Aggiunto mutex per sincronizzazione della stampa
} ThreadData;

// Funzione eseguita dai thread
void *processRows(void *arg);

int group_calculations(int group_number, int rank, int media, int size);

#endif // FUNCTIONS_H
