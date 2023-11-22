#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 15
#define NTRAIN 135
#define NFEATURES 4
#define X_TRAIN_PATH "C:/Users/tecnico/Desktop/MPI/MS_MPI/Programs/Federated_Parallel_KNN/X_train.csv"

void checkFile(FILE *f) {
    if (f == NULL) {
        printf("Error while reading file\n");
        exit(1);
    }
}

float *getFloatMat(int m, int n) {
    float *mat = NULL;
    mat = (float *)calloc(m * n, sizeof(float));

    return mat;
}

float *initFeatures(char path[], int start_row, int end_row) {
    int index = 0;
    FILE *f = NULL;
    float *mat = NULL;

    mat = getFloatMat(end_row - start_row + 1, NFEATURES);

    f = fopen(path, "r");
    checkFile(f);

    // Skippa le righe prima di start_row
    for (int i = 0; i < start_row; i++)
        while (fgetc(f) != '\n')
            ;

    // Leggi i dati dalla riga start_row alla riga end_row
    for (int i = start_row; i <= end_row; i++) {
        for (int j = 0; j < NFEATURES; j++) {
            fscanf(f, "%f%*c", &mat[index]);
            index++;
        }
    }

    fclose(f);
    return mat;
}

struct ThreadData {
    int thread_id;
    int start_row;
    int end_row;
    float *x_train;
};

void *threadFunction(void *arg) {
    struct ThreadData *data = (struct ThreadData *)arg;

    // Inizializza la matrice per il thread corrente
    data->x_train = initFeatures(X_TRAIN_PATH, data->start_row, data->end_row);

    // Stampa la matrice per il thread corrente
    printf("Thread %d:\n", data->thread_id);
    for (int i = 0; i <= data->end_row - data->start_row; i++) {
        for (int j = 0; j < NFEATURES; j++) {
            printf("%.2f ", data->x_train[i * NFEATURES + j]);
        }
        printf("\n\n");
    }

    // Libera la memoria allocata per la matrice
    free(data->x_train);

    pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
    pthread_t threads[NTHREADS];
    struct ThreadData thread_data[NTHREADS];

    int rows_per_thread = NTRAIN / NTHREADS;
    
    // Creazione e avvio dei thread
    for (int i = 0; i < NTHREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].start_row = i * rows_per_thread + 1;
        thread_data[i].end_row = (i + 1) * rows_per_thread;
        pthread_create(&threads[i], NULL, threadFunction, (void *)&thread_data[i]);
    }

    // Attesa della terminazione dei thread
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
