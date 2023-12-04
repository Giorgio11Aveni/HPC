// functions.c

#include "header_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>


float *getFloatMat(int m, int n)
{
	float *mat = NULL;
	mat = (float*)calloc(m*n, sizeof(float));

	return mat;
}

float getMax(float *x, int n)
{
	int i;
	float max = x[0];
	int maxIndex = 0;

	for(i=0; i<n; i++)
	{
		if (x[i] >= max)
		{
			max = x[i];
			maxIndex = i;
		}
	}

	return (float)maxIndex;
}

// Definisci una funzione di confronto per qsort
int compareDistances(const void *a, const void *b) {
    // Confronta i valori di distanza
    float distanceA = *((float *)a);
    float distanceB = *((float *)b);

    if (distanceA < distanceB) return -1;
    else if (distanceA > distanceB) return 1;
    else return 0;
}

void openAndCheckCSVFiles(const char *data_filename, const char *label_filename, FILE **data_file, FILE **label_file){

    // Open the CSV file
    *data_file = fopen(data_filename, "r");
    *label_file = fopen(label_filename, "r");

    // Check for errors in file opening
    if (*data_file == NULL || *label_file == NULL) {
        fprintf(stderr, "Errore nell'apertura dei file CSV.\n");

        // Clean up and abort MPI
        if (*data_file != NULL) fclose(*data_file);
        if (*label_file != NULL) fclose(*label_file);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
}

int countTotalRows(FILE *file, char *line) {
    int total_rows = 0;

    // Legge ciascuna riga dal file e incrementa il contatore
    while (fgets(line, MAX_ROW_LENGTH, file) != NULL) {
        total_rows++;
    }

    // Riporta il numero totale di righe nel file
    return total_rows;
}

void checkEqualNumberRows(int total_rows_data, int total_rows_label){

    if (total_rows_data != total_rows_label) {
        fprintf(stderr, "Il numero di righe nei file di dati ed etichette non corrisponde.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
}

void calculateRowRange(int rank, int size, int total_rows_data, int *start_row, int *end_row) {
    
    // Calcola il numero di righe per processo e le righe rimanenti
    int rows_per_process = total_rows_data / (size - 4);
    int remaining_rows = total_rows_data % (size - 4);

    // Calcola l'intervallo di righe da leggere per ogni processo
    *start_row = (rank - 4) * rows_per_process + ((rank - 4) < remaining_rows ? (rank - 4) : remaining_rows);
    *end_row = *start_row + rows_per_process + ((rank - 4) < remaining_rows ? 1 : 0);
}

void allocateMatrix(int start_row, int end_row, int *num_columns, float **data_matrix, int **label_matrix, float **all_distances, int *num_rows) {
    *num_rows = end_row - start_row;
    *num_columns = NFEATURES;

    // Alloca la matrice per memorizzare i dati letti da ciascun processo
    *data_matrix = (float *)malloc(*num_rows * *num_columns * sizeof(float));

    // Alloca la matrice per memorizzare le etichette lette da ciascun processo
    *label_matrix = (int *)malloc(*num_rows * sizeof(int));

    // Alloca l'array per memorizzare le somme delle distanze per ogni dato
    *all_distances = (float *)malloc(*num_rows * sizeof(float));
}

void readAssignedRows(FILE *data_file, FILE *label_file, int start_row, int end_row, int num_columns, float *data_matrix, int *label_matrix, char *line_data, char *line_label) {
    

    // Riporta il cursore al principio del file
    fseek(data_file, 0, SEEK_SET);
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
}

// Funzione per gestire l'al\-locazione e l'inizializzazione dei dati per i thread
void initializeThreads(pthread_t *threads, ThreadData *thread_data, int num_rows, int num_columns, float *data_matrix, int *label_matrix, float *test_point, float *all_distances) {
    int thread_rows = num_rows / 3;
    int remaining_thread_rows = num_rows % 3;

    for (int i = 0; i < NTHREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].start_row = i * thread_rows + (i < remaining_thread_rows ? i : remaining_thread_rows);
        thread_data[i].end_row = thread_data[i].start_row + thread_rows + (i < remaining_thread_rows ? 1 : 0);
        thread_data[i].num_columns = num_columns;
        thread_data[i].data_matrix = data_matrix;
        thread_data[i].label_matrix = label_matrix;
        thread_data[i].test_point = test_point;
        thread_data[i].local_distances = all_distances;

        pthread_create(&threads[i], NULL, threadFunction, (void *)&thread_data[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
}

int predict(float *distance, int *labels) //topn < NCLASSES
{
	float* neighborCount = getFloatMat(NCLASSES, 1);
	float* probability = getFloatMat(NCLASSES, 1);

	int i;

	for(i=0; i<K; i++)
		neighborCount[(int)labels[i]]++;

	for(i=0; i<NCLASSES; i++)
		probability[i] = neighborCount[i]*1.0/(float)K*1.0;
	
	int predicted_class = (int)getMax(neighborCount, NCLASSES);

	printf("Probability:\n");
	for(i=0; i<TOPN; i++)
		printf("%s\t%f\n", class[i], probability[i]);

	free(neighborCount);
	free(probability);
    
	return predicted_class;
}

void cleanupAndClose(FILE *data_file, FILE *label_file, float *data_matrix, int *label_matrix) {
    free(data_matrix);
    free(label_matrix);

    fclose(data_file);
    fclose(label_file);
}
