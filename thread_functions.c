// functions.c

#include "header_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>



// Function performed by threads
void *threadFunction(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // Allocates a new matrix for the thread
    int thread_num_rows = data->end_row - data->start_row;
    float *thread_matrix = (float *)malloc(thread_num_rows * data->num_columns * sizeof(float));

    // Copy the matching rows from the local array
    for (int i = 0; i < thread_num_rows; i++) {
        for (int j = 0; j < data->num_columns; j++) {
            thread_matrix[i * data->num_columns + j] = data->local_matrix[(i + data->start_row) * data->num_columns + j];
        }
    }

    // Print the thread matrix
    printf("Thread %d prints rows %d to %d:\n", data->thread_id, data->start_row, data->end_row);

    for (int i = 0; i < thread_num_rows; i++) {
        for (int j = 0; j < data->num_columns; j++) {
            printf("%f ", thread_matrix[i * data->num_columns + j]);
        }
        printf("\n\n");
    }

    // Release the memory allocated for the thread matrix
    free(thread_matrix);

    pthread_exit(NULL);
}

int group_calculations(int rank, int size) {
    
    if (rank>3)
    {
      // Open the CSV file
    FILE *file = fopen(X_TRAIN_PATH, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening the CSV file.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Determine the total number of rows in the file
    int total_rows = 0;
    char line[MAX_ROW_LENGTH];
    while (fgets(line, MAX_ROW_LENGTH, file) != NULL) {
        total_rows++;
    }

    // Calculate the number of rows each process will read
    int rows_per_process = total_rows / (size-4);
    int remaining_rows = total_rows % (size-4);

    // Calculate the portion of the file to be read for each process
    int start_row = (rank - 4) * rows_per_process + ((rank - 4) < remaining_rows ? (rank - 4) : remaining_rows);
    int end_row = start_row + rows_per_process + ((rank - 4) < remaining_rows ? 1 : 0);

    // Allocate the matrix to store the rows read by each process
    int num_columns = NFEATURES;  // Number of columns in CSV file
    int num_rows = end_row - start_row;
    float *matrix = (float *)malloc(num_rows * num_columns * sizeof(float));

    // Read the rows assigned to the current process
    fseek(file, 0, SEEK_SET);  // Reset the file cursor to the beginning
    for (int i = 0; i < end_row; i++) {
        if (i >= start_row) {
            fgets(line, MAX_ROW_LENGTH, file);
            char *token = strtok(line, ",");
            for (int j = 0; j < num_columns; j++) {
                sscanf(token, "%f", &matrix[(i - start_row) * num_columns + j]);
                token = strtok(NULL, ",");
            }
        } else {
            fgets(line, MAX_ROW_LENGTH, file);  // Skip the rows not assigned to the current process
        }
    }

    // Print the process ID
    printf("Process %d:\n", rank);

    // Allocation and initialization of data for threads
    pthread_t threads[NTHREADS];
    ThreadData thread_data[NTHREADS];

    int thread_rows = num_rows / NTHREADS;
    int remaining_thread_rows = num_rows % NTHREADS;

    for (int i = 0; i < NTHREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].start_row = i * thread_rows + (i < remaining_thread_rows ? i : remaining_thread_rows);
        thread_data[i].end_row = thread_data[i].start_row + thread_rows + (i < remaining_thread_rows ? 1 : 0);
        thread_data[i].num_columns = num_columns;
        thread_data[i].local_matrix = matrix;

        pthread_create(&threads[i], NULL, threadFunction, (void *)&thread_data[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Release memory and close the file
    free(matrix);
    fclose(file);  
    }
    
    
}
