// functions.c

#include "header_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include <math.h>

// ...

void *threadFunction(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    printf("Thread %d del processo stampa righe da %d a %d:\n", data->thread_id, data->start_row, data->end_row);

    for (int i = data->start_row; i < data->end_row; i++) {
        printf("Etichetta: %d\n, Dati: ", data->label_matrix[i]);
        for (int j = 0; j < data->num_columns; j++) {
            printf("%f \n", data->data_matrix[i * data->num_columns + j]);

            // Calcola la distanza euclidea tra il punto della matrice e il punto inserito manualmente
            float point_distance = sqrt(pow(data->data_matrix[i * data->num_columns + j] - data->test_point[i+j], 2));
            printf("Distanza dal punto utente: %f \n", point_distance);
        }
        printf("\n");
    }

    pthread_exit(NULL);
}


int group_calculations(int rank, int size, char *data_filename, char *label_filename, float *test_point) {
    
    if (rank>3)
    {
      // Open the CSV file
    FILE *data_file = fopen(data_filename, "r");
    FILE *label_file = fopen(label_filename, "r");

    if (data_file == NULL || label_file == NULL) {
        fprintf(stderr, "Errore nell'apertura dei file CSV.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Determina il numero totale di righe nei file
    int total_rows_data = 0;
    char line_data[MAX_ROW_LENGTH];
    while (fgets(line_data, MAX_ROW_LENGTH, data_file) != NULL) {
        total_rows_data++;
    }

    int total_rows_label = 0;
    char line_label[MAX_ROW_LENGTH];
    while (fgets(line_label, MAX_ROW_LENGTH, label_file) != NULL) {
        total_rows_label++;
    }

    // Verifica che il numero di righe nei file sia lo stesso
    if (total_rows_data != total_rows_label) {
        fprintf(stderr, "Il numero di righe nei file di dati ed etichette non corrisponde.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

        // Calculate the number of rows each process will read
    int rows_per_process = total_rows_data / (size-4);
    int remaining_rows = total_rows_data % (size-4);

    // Calculate the portion of the file to be read for each process
    int start_row = (rank - 4) * rows_per_process + ((rank - 4) < remaining_rows ? (rank - 4) : remaining_rows);
    int end_row = start_row + rows_per_process + ((rank - 4) < remaining_rows ? 1 : 0);

    // Alloca la matrice per memorizzare i dati letti da ciascun processo
    int num_columns = 4;  // Numero di colonne nel tuo file CSV dei dati
    int num_rows = end_row - start_row;
    float *data_matrix = (float *)malloc(num_rows * num_columns * sizeof(float));

    // Alloca la matrice per memorizzare le etichette lette da ciascun processo
    int *label_matrix = (int *)malloc(num_rows * sizeof(int));
    
    // Leggi le righe assegnate al processo corrente
    fseek(data_file, 0, SEEK_SET);  // Riporta il cursore al principio del file
    fseek(label_file, 0, SEEK_SET);
    
    for (int i = 0; i < end_row; i++) {
        if (i >= start_row) {
            // Leggi i dati
            fgets(line_data, MAX_ROW_LENGTH, data_file);
            char *token_data = strtok(line_data, ",");
            for (int j = 0; j < num_columns; j++) {
                sscanf(token_data, "%f", &data_matrix[(i - start_row) * num_columns + j]);
                token_data = strtok(NULL, ",");
            }

            // Leggi le etichette
            fgets(line_label, MAX_ROW_LENGTH, label_file);
            sscanf(line_label, "%d", &label_matrix[i - start_row]);
        } else {
            fgets(line_data, MAX_ROW_LENGTH, data_file);  // Salta le righe non assegnate al processo corrente
            fgets(line_label, MAX_ROW_LENGTH, label_file);
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
        thread_data[i].data_matrix = data_matrix;
        thread_data[i].label_matrix = label_matrix;
        thread_data[i].test_point = test_point;

        pthread_create(&threads[i], NULL, threadFunction, (void *)&thread_data[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    free(data_matrix);
    free(label_matrix);
    fclose(data_file);
    fclose(label_file);
}
    
}
