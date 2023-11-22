// header_thread.h

#include <pthread.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define NTHREADS 3
#define NTRAIN 10
#define NFEATURES 4
#define X_TRAIN_PATH "C:/Users/tecnico/Desktop/UniME/Materie/HPC/MPI/Programs/Federated_Parallel_KNN/X_train.csv"


// Struttura dati per passare l'ID del thread

struct ThreadData {
    int thread_id;
    float *x_train;
    int start_row;
    int end_row;
    pthread_mutex_t mutex; // Mutex per la sincronizzazione della console
};

// Funzione eseguita dai thread
void *processRows(void *arg);

int group_calculations(int group_number, int rank, int media, int size);

#endif // FUNCTIONS_H
