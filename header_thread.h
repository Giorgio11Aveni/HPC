// header_thread.h

#include <pthread.h>

#ifndef HEADER_THREAD_H
#define HEADER_THREAD_H

#define NTHREADS 5
#define NTRAIN 135
#define NFEATURES 4
#define MAX_ROW_LENGTH 1024
#define X_TRAIN_PATH "C:/Users/tecnico/Desktop/MPI/MS_MPI/Programs/Federated_Parallel_KNN/X_train.csv"

// Struttura dati per passare argomenti ai thread
typedef struct {
    int thread_id;
    int start_row;
    int end_row;
    int num_columns;
    float *local_matrix;
} ThreadData;
// Funzione eseguita dai thread
void *threadFunction(void *arg);

int group_calculations(int rank, int size);

#endif // HEADER_THREAD_H
