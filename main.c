#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_LINE_LENGTH 1024
#define MAX_FIELDS 4

typedef struct {
    int threadID;
    char** rows;
    int rowCount;
} ThreadData;

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

int main() {
    FILE* file = fopen("C:/Users/tecnico/Desktop/MPI/MS_MPI/Programs/Parallel-KNN_example/Knn-MPI/X_train2.csv", "r");
    if (!file) {
        perror("Errore nell'apertura del file");
        return 1;
    }

    int threadCount = 6;  // Specifica il numero di thread desiderato
    int rowCount = 30;   // Specifica il numero totale di righe nel file
    int rowsPerThread = rowCount / threadCount;

    pthread_t threads[threadCount];

    for (int i = 0; i < threadCount; ++i) {
        ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData));
        data->threadID = i + 1;
        data->rowCount = rowsPerThread;

        // Alloca memoria per le righe del thread
        data->rows = (char**)malloc(rowsPerThread * sizeof(char*));

        for (int j = 0; j < rowsPerThread; ++j) {
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

    return 0;
}
