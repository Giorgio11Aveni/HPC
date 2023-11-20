// header_thread.h

#include <pthread.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define MAX_LINE_LENGTH 1024


// Struttura dati per passare l'ID del thread
typedef struct {
    int threadID;
    char** rows;
    int rowCount;
} ThreadData;

// Funzione eseguita dai thread
void *calcolaNumero(void *arg);

int group_calculations(int group_number, int rank, int media);

#endif // FUNCTIONS_H
