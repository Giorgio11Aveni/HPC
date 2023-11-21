// functions.c

#include "header_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <windows.h>

void* processRows(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    printf("Process %d - Thread %d processing %d rows\n", data->mpiRank, data->threadID, data->rowCount);

    for (int i = 0; i < data->rowCount; ++i) {
        pthread_mutex_lock(&data->mutex);  // Blocca l'accesso a stdout per gli altri thread
        printf("Process %d - Thread %d - Row %d: ", data->mpiRank, data->threadID, i + 1);

        float* row = data->rows[i][data->threadID - 1];
        for (int k = 0; k < MAX_ELEMENTS_IN_ROW; ++k) {
            printf("%.4f ", row[k]);
        }
        printf("\n");

        pthread_mutex_unlock(&data->mutex);  // Sblocca l'accesso a stdout per gli altri thread
        Sleep(10);  // Aggiunto ritardo per evidenziare la concorrenza (può essere rimosso)
    }

    for (int i = 0; i < data->rowCount; ++i) {
        free(data->rows[i][data->threadID - 1]);
    }
    free(data->rows);
    free(data);

    return NULL;
}

int group_calculations(int group_number, int rank, int media, int size) {
    if (rank > 3) {
        FILE* file = fopen("C:/Users/tecnico/Desktop/UniME/Materie/HPC/MPI/Programs/Multithreading/X_train.csv", "r");
        if (!file) {
            perror("Errore nell'apertura del file");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        int totalRowCount = 135;
        int rowsPerProcess = totalRowCount / (size - 4);
        int remainingRows = totalRowCount % (size - 4);
        char line[MAX_LINE_LENGTH];

        int offset = (rank + size - 4) % size;

        for (int i = 0; i < offset * rowsPerProcess + (offset < remainingRows ? offset : remainingRows); ++i) {
            if (fgets(line, sizeof(line), file) == NULL) {
                perror("Errore nella lettura del file");
                fclose(file);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }

        pthread_t threads[MAX_THREADS];
        ThreadData* data[MAX_THREADS];

        int rowsPerThread[MAX_THREADS];
        int remainingRowsToDistribute = rowsPerProcess;
        for (int t = 0; t < MAX_THREADS; ++t) {
            rowsPerThread[t] = remainingRowsToDistribute / (MAX_THREADS - t);
            remainingRowsToDistribute -= rowsPerThread[t];
        }

        for (int t = 0; t < MAX_THREADS; ++t) {
            data[t] = (ThreadData*)malloc(sizeof(ThreadData));
            data[t]->mpiRank = rank;
            data[t]->threadID = t + 1;
            data[t]->rowCount = rowsPerThread[t];
            data[t]->rows = (float***)malloc(data[t]->rowCount * sizeof(float**));
            pthread_mutex_init(&data[t]->mutex, NULL);  // Inizializza il mutex

            for (int i = 0; i < data[t]->rowCount; ++i) {
                data[t]->rows[i] = (float**)malloc(MAX_ELEMENTS_IN_ROW * sizeof(float*));

                if (fgets(line, sizeof(line), file) == NULL) {
                    perror("Errore nella lettura del file");
                    fclose(file);
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
                
                char* token = strtok(line, ",");
                int k = 0;
                while (token != NULL && k < MAX_ELEMENTS_IN_ROW) {
                     if (strcmp(token, "Errore") == 0) {
    printf("Errore nella conversione della stringa a float: Token non valido\n");
    fclose(file);
    MPI_Abort(MPI_COMM_WORLD, 1);
                     }
                    float value;
                    if (sscanf(token, "%f", &value) != 1) {
                        perror("Errore nella conversione della stringa a float");
                        fclose(file);
                        MPI_Abort(MPI_COMM_WORLD, 1);
                    }
                    data[t]->rows[i][k] = (float*)malloc(sizeof(float));
                    *(data[t]->rows[i][k]) = value;
                    token = strtok(NULL, ",");
                    k++;
                }
            }
        }

        for (int t = 0; t < MAX_THREADS; ++t) {
            if (pthread_create(&threads[t], NULL, processRows, (void*)data[t]) != 0) {
                perror("Errore nella creazione del thread");
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }

        for (int t = 0; t < MAX_THREADS; ++t) {
            pthread_join(threads[t], NULL);
            pthread_mutex_destroy(&data[t]->mutex);  // Distruggi il mutex dopo che il thread ha terminato
        }

        fclose(file);
    }
}
