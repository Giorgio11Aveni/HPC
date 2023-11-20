// functions.c

#include "header_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void* processRows(void* arg) {
   ThreadData* data = (ThreadData*)arg;

    printf("MPI Rank %d - Thread %d processing %d rows\n", data->mpiRank, data->threadID, data->rowCount);

    for (int i = 0; i < data->rowCount; ++i) {
        // Esegui l'elaborazione della riga
        char* row = data->rows[i];
        printf("MPI Rank %d - Thread %d - Row %d: %s\n", data->mpiRank, data->threadID, i + 1, row);

        // Libera la memoria allocata per la riga
        free(row);
    }

    // Libera la memoria allocata per i dati del thread
    free(data->rows);
    free(data);

    return NULL;
}

int group_calculations(int group_number, int rank,int media, int size) {
    if (rank > 3){
     FILE* file = fopen("C:/Users/tecnico/Desktop/UniME/Materie/HPC/MPI/Programs/Multithreading/X_train.csv", "r");
    if (!file) {
        perror("Errore nell'apertura del file");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int totalRowCount = 135;
    int rowsPerProcess = totalRowCount / size;
    int remainingRows = totalRowCount % size; // Righe extra da distribuire
    char line[MAX_LINE_LENGTH];  // Dichiarazione di line

    // Calcolo dell'offset del cursore del file per iniziare dal processo con rank 4
    int offset = (rank + size - 4) % size;

    // Avanza il cursore del file per il blocco di righe destinato a questo processo
    for (int i = 0; i < offset * rowsPerProcess + (offset < remainingRows ? offset : remainingRows); ++i) {
        if (fgets(line, sizeof(line), file) == NULL) {
            perror("Errore nella lettura del file");
            fclose(file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    pthread_t threads;

    ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData));
    data->mpiRank = rank;
    data->threadID = rank + 1;
    data->rowCount = rowsPerProcess + (rank < remainingRows ? 1 : 0);
    data->rows = (char**)malloc(data->rowCount * sizeof(char*));

    for (int j = 0; j < data->rowCount; ++j) {
        if (fgets(line, sizeof(line), file) == NULL) {
            perror("Errore nella lettura del file");
            fclose(file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Alloca memoria per la copia della riga
        data->rows[j] = strdup(line);
    }

    // Crea il thread
    if (pthread_create(&threads, NULL, processRows, (void*)data) != 0) {
        perror("Errore nella creazione del thread");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Attendi la terminazione del thread
    pthread_join(threads, NULL);

    fclose(file);
    }
}
