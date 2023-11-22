// functions.c

#include "header_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void checkFile(FILE *f) {
    if (f == NULL) {
        printf("Error while reading file\n");
        exit(1);
    }
}

float *getFloatMat(int m, int n) {
    float *mat = NULL;
    mat = (float*)calloc(m * n, sizeof(float));

    return mat;
}

float *initFeatures(char path[], int start_row, int end_row) {
    int index = 0;
    FILE *f = NULL;
    float *mat = NULL;

    mat = getFloatMat(end_row - start_row + 1, NFEATURES);

    f = fopen(path, "r");
    checkFile(f);

    // Leggi il file fino alla fine o fino a raggiungere la riga finale
    for (int i = 0; i < end_row && fscanf(f, "%f%*c", &mat[index]) == 1; i++) {
        // Ignora le righe fino a raggiungere la riga di inizio
        if (i < start_row) {
            continue;
        }

        index++;
    }

    fclose(f);
    return mat;
}

void* processRows(void* arg) {

     struct ThreadData *data = (struct ThreadData *)arg;

    // Inizializza la matrice per il thread corrente
    data->x_train = initFeatures(X_TRAIN_PATH, data->start_row, data->end_row);

    // Acquisisce il lock del mutex prima di stampare
    pthread_mutex_lock(&data->mutex);

    // Stampa la matrice per il thread corrente
    printf("Thread %d - Rows %d to %d:\n", data->thread_id, data->start_row, data->end_row);
    for (int i = 0; i <= data->end_row - data->start_row; i++) {
        for (int j = 0; j < NFEATURES; j++) {
            printf("%.2f ", data->x_train[i * NFEATURES + j]);
        }
        printf("\n");
    }

    // Rilascia il lock del mutex dopo la stampa
    pthread_mutex_unlock(&data->mutex);

    // Libera la memoria allocata per la matrice
    free(data->x_train);

    pthread_exit(NULL);
}

int group_calculations(int group_number, int rank, int media, int size) {
    
    if (group_number == 1) {

        pthread_t threads[NTHREADS];
        struct ThreadData thread_data[NTHREADS];
        pthread_mutex_t console_mutex = PTHREAD_MUTEX_INITIALIZER; // Inizializza il mutex

        // Calcola il numero di righe che ogni thread deve elaborare
        int rows_per_thread = NTRAIN / NTHREADS;

        // Inizializza la struttura dati dei thread con il mutex e gli intervalli di righe
        for (int i = 0; i < NTHREADS; i++) {
            thread_data[i].thread_id = i;
            thread_data[i].mutex = console_mutex;
            thread_data[i].start_row = i * rows_per_thread + 1;
            thread_data[i].end_row = (i + 1) * rows_per_thread;

            // L'ultimo thread può avere un intervallo di righe più piccolo se NTRAIN non è multiplo di NTHREADS
            if (i == NTHREADS - 1) {
                thread_data[i].end_row = NTRAIN;
            }
        }

        // Creazione e avvio dei thread
        for (int i = 0; i < NTHREADS; i++) {
            pthread_create(&threads[i], NULL, processRows, (void *)&thread_data[i]);
        }

        // Attesa della terminazione dei thread
        for (int i = 0; i < NTHREADS; i++) {
            pthread_join(threads[i], NULL);
        }
    }
}
