// functions.c

#include "header_thread.h"
#include <stdio.h>
#include <stdlib.h>

void *calcolaNumero(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // Calcola il numero
    int numero_calcolato = data->thread_id * 2;

    // Ritorna il risultato come puntatore
    int *result = malloc(sizeof(int));
    *result = numero_calcolato;

    pthread_exit(result);
}
