// functions.c

#include "header_thread.h"
#include <stdio.h>
#include <stdlib.h>

void *calcolaNumero(void *arg, int group_number) {
    ThreadData *data = (ThreadData *)arg;

    srand(time(NULL));

    // Calcola il numero
    int numero_calcolato = (rand() % 15) + group_number;

    // Ritorna il risultato come puntatore
    int *result = malloc(sizeof(int));
    *result = numero_calcolato;

    pthread_exit(result);
}

int group_calculations(int group_number, int rank,int media) {
     if ((group_number == 1 && rank != 1) || (group_number == 2 && rank != 2) || (group_number == 3 && rank != 3))
    {
        // Numero di thread
        const int num_thread = 5;

        // Array di dati per i thread
        ThreadData thread_data[num_thread];

        // Array per memorizzare i risultati
        int *risultati[num_thread];

        // Inizializza i dati e avvia i thread
        pthread_t threads[num_thread];
        for (int i = 0; i < num_thread; ++i) {
            thread_data[i].thread_id = i;

            pthread_create(&threads[i], NULL, (void *(*)(void *))calcolaNumero, (void *)&thread_data[i]);

        }

    // Attendi la fine di tutti i thread e calcola la media dei risultati
    int somma = 0;
    for (int i = 0; i < num_thread; ++i) {
        pthread_join(threads[i], (void **)&risultati[i]);
        somma += *risultati[i];
    }

    for (int i = 0; i < num_thread; ++i) {

        printf("Thread %d: %d\n", i, *risultati[i]); 
        

        // Libera la memoria allocata per il risultato
        free(risultati[i]);
    }


    // Calcola la media
     media = somma / num_thread;

    printf("Average process %d: %d\n\n", rank, media);
    return media;

        }
}
