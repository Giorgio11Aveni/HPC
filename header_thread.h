// header_thread.h

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <pthread.h>

// Struttura dati per passare l'ID del thread
typedef struct {
    int thread_id;
} ThreadData;

// Funzione eseguita dai thread
void *calcolaNumero(void *arg, int group_number);

int group_calculations(int group_number, int rank, int media);

#endif // FUNCTIONS_H
