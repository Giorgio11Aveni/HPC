// header_thread.h

#include <pthread.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define NTHREADS 5
#define NTRAIN 135
#define NFEATURES 4
#define X_TRAIN_PATH "C:/Users/tecnico/Desktop/UniME/Materie/HPC/MPI/Programs/Federated_Parallel_KNN/X_train.csv"

// Struttura dati per passare argomenti ai thread
typedef struct {
    int thread_id;
    int start_row;
    int end_row;
    int num_columns;
    float *local_matrix;
} ThreadData;
// Funzione eseguita dai thread
void *processRows(void *arg);

int group_calculations(int rank, int size);

#endif // FUNCTIONS_H
