// functions.c

#include "header_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* processRows(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    printf("Thread %d processing %d rows\n", data->threadID, data->rowCount);

    for (int i = 0; i < data->rowCount; ++i) {
        // Esegfaui l'elaborazione della riga
        char* row = data->rows[i];
        printf("Thread %d - Row %d: %s\n", data->threadID, i + 1, row);

        // Libera la memoria allocata per la riga
        free(row);
    }

    // Libera la memoria allocata per i dati del thread
    free(data->rows);
    free(data);

    pthread_exit(NULL);
}

int group_calculations(int group_number, int rank,int media) {
     FILE* file = fopen("C:/Users/tecnico/Desktop/UniME/Materie/HPC/MPI/Programs/Multithreading/X_train.csv", "r");
    if (!file) {
        perror("Errore nell'apertura del file");
        return 1;
    }

    int totalRowCount = 135;
    int threadCount = 5;
    int rowsPerThread = totalRowCount / threadCount;
    int remainingRows = totalRowCount % threadCount; // Righe extra da distribuire

    pthread_t threads[threadCount];

    for (int i = 0; i < threadCount; ++i) {
        ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData));
        data->threadID = i + 1;

        // Calcola il numero di righe che ogni thread deve elaborare
        data->rowCount = rowsPerThread + (i < remainingRows ? 1 : 0);

        // Alloca memoria per le righe del thread
        data->rows = (char**)malloc(data->rowCount * sizeof(char*));

        for (int j = 0; j < data->rowCount; ++j) {
            char line[MAX_LINE_LENGTH];
            if (fgets(line, sizeof(line), file) == NULL) {
                perror("Errore nella lettura del file");
                fclose(file);
                return 1;
            }

            // Alloca memoria per la copia della riga
            data->rows[j] = strdup(line);
        }

        // Crea il thread
        if (pthread_create(&threads[i], NULL, processRows, (void*)data) != 0) {
            perror("Errore nella creazione del thread");
            return 1;
        }
    }

    // Attendi la terminazione di tutti i thread
    for (int i = 0; i < threadCount; ++i) {
        pthread_join(threads[i], NULL);
    }

    fclose(file);
}
