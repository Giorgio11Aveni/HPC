// functions.c

#include "header_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_ROW_LENGTH 1024

// Funzione eseguita dai thread
void *threadFunction(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // Alloca una nuova matrice per il thread
    int thread_num_rows = data->end_row - data->start_row;
    float *thread_matrix = (float *)malloc(thread_num_rows * data->num_columns * sizeof(float));

    // Copia le righe corrispondenti dalla matrice locale
    for (int i = 0; i < thread_num_rows; i++) {
        for (int j = 0; j < data->num_columns; j++) {
            thread_matrix[i * data->num_columns + j] = data->local_matrix[(i + data->start_row) * data->num_columns + j];
        }
    }

    // Stampa la matrice del thread
    printf("Thread %d stampa righe da %d a %d:\n", data->thread_id, data->start_row, data->end_row);

    for (int i = 0; i < thread_num_rows; i++) {
        for (int j = 0; j < data->num_columns; j++) {
            printf("%f ", thread_matrix[i * data->num_columns + j]);
        }
        printf("\n");
    }

    // Rilascia la memoria allocata per la matrice del thread
    free(thread_matrix);

    pthread_exit(NULL);
}

int group_calculations(int rank, int size) {
    
    if (rank>3)
    {
      // Apertura del file CSV
    FILE *file = fopen("C:/Users/tecnico/Desktop/UniME/Materie/HPC/MPI/Programs/Federated_Parallel_KNN/X_train.csv", "r");
    if (file == NULL) {
        fprintf(stderr, "Errore nell'apertura del file CSV.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Determina il numero totale di righe nel file
    int total_rows = 0;
    char line[MAX_ROW_LENGTH];
    while (fgets(line, MAX_ROW_LENGTH, file) != NULL) {
        total_rows++;
    }

    // Calcola il numero di righe che ogni processo leggerà
    int rows_per_process = total_rows / (size-4);
    int remaining_rows = total_rows % (size-4);

    // Calcola la porzione del file da leggere per ogni processo
    int start_row = (rank - 4) * rows_per_process + ((rank - 4) < remaining_rows ? (rank - 4) : remaining_rows);
    int end_row = start_row + rows_per_process + ((rank - 4) < remaining_rows ? 1 : 0);

    // Alloca la matrice per memorizzare le righe lette da ciascun processo
    int num_columns = 4;  // Numero di colonne nel tuo file CSV
    int num_rows = end_row - start_row;
    float *matrix = (float *)malloc(num_rows * num_columns * sizeof(float));

    // Leggi le righe assegnate al processo corrente
    fseek(file, 0, SEEK_SET);  // Riporta il cursore al principio del file
    for (int i = 0; i < end_row; i++) {
        if (i >= start_row) {
            fgets(line, MAX_ROW_LENGTH, file);
            char *token = strtok(line, ",");
            for (int j = 0; j < num_columns; j++) {
                sscanf(token, "%f", &matrix[(i - start_row) * num_columns + j]);
                token = strtok(NULL, ",");
            }
        } else {
            fgets(line, MAX_ROW_LENGTH, file);  // Salta le righe non assegnate al processo corrente
        }
    }

    // Stampa l'ID del processo
    printf("Processo %d:\n", rank);

    // Allocazione e inizializzazione dati per i thread
    pthread_t threads[3];
    ThreadData thread_data[3];

    int thread_rows = num_rows / 3;
    int remaining_thread_rows = num_rows % 3;

    for (int i = 0; i < 3; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].start_row = i * thread_rows + (i < remaining_thread_rows ? i : remaining_thread_rows);
        thread_data[i].end_row = thread_data[i].start_row + thread_rows + (i < remaining_thread_rows ? 1 : 0);
        thread_data[i].num_columns = num_columns;
        thread_data[i].local_matrix = matrix;

        pthread_create(&threads[i], NULL, threadFunction, (void *)&thread_data[i]);
    }

    // Attendi la terminazione di tutti i thread
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    // Rilascia la memoria e chiudi il file
    free(matrix);
    fclose(file);  
    }
    
    
}
