// header_thread.h

#include <pthread.h>

#ifndef HEADER_THREAD_H
#define HEADER_THREAD_H

#define NTHREADS 5
#define NTRAIN 135
#define NFEATURES 4
#define MAX_ROW_LENGTH 1024
#define X_TRAIN_PATH "C:/Users/tecnico/Desktop/MPI/MS_MPI/Programs/Federated_Parallel_KNN/X_train.csv"
#define Y_TRAIN_PATH "C:/Users/tecnico/Desktop/MPI/MS_MPI/Programs/Federated_Parallel_KNN/y_train.csv"

// Struttura dati per passare argomenti ai thread
typedef struct {
    int thread_id;
    int start_row;
    int end_row;
    int num_columns;
    float *data_matrix;
    int *label_matrix;
    float *test_point;
} ThreadData;
// Funzione eseguita dai thread
void *threadFunction(void *arg);

int group_calculations(int rank, int size, char *data_filename, char *, float *test_point);

#endif // HEADER_THREAD_H
