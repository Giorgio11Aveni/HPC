//knn.c

#include "header_thread.h"
#include "thread_functions.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include "mergeSort.h"

void *threadFunction(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    printf("Thread %d del processo:\n\n", data->thread_id);

    for (int i = data->start_row; i < data->end_row; i++) {
        // Inizializza la somma delle distanze per ogni dato
        float sum_distance = 0.0;

        for (int j = 0; j < data->num_columns; j++) {
            // Calcola la distanza euclidea tra il punto della matrice e il punto inserito manualmente
            float test_point_value = data->test_point[j];
            float point_distance = sqrt(pow(data->data_matrix[i * data->num_columns + j] - data->test_point[j], 2));


            // Aggiungi la distanza alla somma delle distanze per il dato corrente
            sum_distance += point_distance;
        }

        // Memorizza la somma delle distanze nell'array locale del thread
        data->local_distances[i - data->start_row] = sum_distance;

    
    }

    int num_rows = data->end_row - data->start_row;
     // Ordina le distanze locali utilizzando mergeSort
    mergeSort(data->local_distances, 0, data->end_row - data->start_row - 1, data->label_matrix);

    int predicted_class = predict(data->local_distances, data->label_matrix);
    printf("Predicted Class: %d\n\n", predicted_class);

    pthread_exit(NULL);
}

int group_calculations(int rank, int size, char *data_filename, char *label_filename, float *test_point) {
    
    if (rank>3){

        char line_data[MAX_ROW_LENGTH];
        char line_label[MAX_ROW_LENGTH];

        // Open the CSV file
        FILE *data_file = fopen(data_filename, "r");
        FILE *label_file = fopen(label_filename, "r");

        openAndCheckCSVFiles(data_filename, label_filename, &data_file, &label_file);

        // Determina il numero totale di righe nei file
        int total_rows_data = countTotalRows(data_file, line_data);
        int total_rows_label = countTotalRows(label_file, line_label);

        // Verifica che il numero di righe nei file sia lo stesso
        checkEqualNumberRows(total_rows_data,total_rows_label);
        
        int start_row, end_row;
        calculateRowRange(rank, size, total_rows_data, &start_row, &end_row);

        int num_rows, num_columns;

        // Alloca la matrice per memorizzare i dati letti da ciascun processo
        float *data_matrix, *all_distances ;

        // Alloca la matrice per memorizzare le etichette lette da ciascun processo
        int *label_matrix ;
        
        allocateMatrix(start_row, end_row, &num_columns, &data_matrix, &label_matrix, &all_distances, &num_rows);

        readAssignedRows(data_file, label_file, start_row, end_row, num_columns, data_matrix, label_matrix, line_data, line_label);

    // Stampa l'ID del processo
    printf("Processo %d:\n", rank);

    // Allocazione e inizializzazione dati per i thread
    pthread_t threads[3];
    ThreadData thread_data[3];

    initializeThreads(threads, thread_data, num_rows, num_columns, data_matrix, label_matrix, test_point, all_distances);

    // Chiama la funzione di pulizia
    cleanupAndClose(data_file, label_file, data_matrix, label_matrix);
    
    }
    
}
