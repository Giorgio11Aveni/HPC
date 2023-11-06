#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Comm server_centrale_comm, server_intermedio_comm, dispositivi_locali_comm;
    int new_color;

    if (rank == 0) {
        // Il processo con rango 0 è il server centrale
        new_color = 0;
    } else if (rank == 1) {
        // Il processo con rango 1 è il server intermedio
        new_color = 1;
    } else {
        // Gli altri processi sono dispositivi locali
        new_color = 2;
    }

    MPI_Comm_split(MPI_COMM_WORLD, new_color, rank, &server_centrale_comm);

   float messaggio[4]; // Inizializza un array di numeri in virgola mobile

    if (new_color == 0) {
        // Questo è il server centrale
        printf("Sono il server centrale. Rango %d di %d.\n", rank, size);
        // Inizializza l'array con i numeri desiderati
        messaggio[0] = 1.0;
        messaggio[1] = 2.0;
        messaggio[2] = 3.0;
        messaggio[3] = 4.0;
        MPI_Bcast(messaggio, 4, MPI_FLOAT, 0, server_centrale_comm); // Broadcast a tutti
    } else if (new_color == 1) {
        // Questo è un server intermedio
        printf("Sono il server intermedio. Rango %d di %d.\n", rank, size);
        MPI_Bcast(messaggio, 4, MPI_FLOAT, 0, server_centrale_comm); // Ricevi il messaggio da server centrale
        printf("Messaggio ricevuto da server centrale: %f %f %f %f\n", messaggio[0], messaggio[1], messaggio[2], messaggio[3]);
    } else {
        // Questo è un dispositivo locale
        printf("Sono un dispositivo locale. Rango %d di %d.\n", rank, size);
        MPI_Bcast(messaggio, 4, MPI_FLOAT, 0, server_centrale_comm); // Ricevi il messaggio da server intermedio
        printf("Messaggio ricevuto da server intermedio: %f %f %f %f\n", messaggio[0], messaggio[1], messaggio[2], messaggio[3]);
    }

    MPI_Comm_free(&server_centrale_comm);

    MPI_Finalize();
    return 0;
}
