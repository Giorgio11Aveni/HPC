#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int central_server_rank = 0;
    int intermediary_server_rank = 1;

    float message[4]; // Inizializza un array di numeri in virgola mobile

    if (rank == central_server_rank) {
        // Questo è il central server
        printf("Sono il central server. Rank %d di %d.\n", rank, size);
        // Inizializza l'array con i numeri desiderati
        message[0] = 1.0;
        message[1] = 2.0;
        message[2] = 3.0;
        message[3] = 4.0;
        // Invia il messaggio all'intermediary server
        MPI_Send(message, 4, MPI_FLOAT, intermediary_server_rank, 0, MPI_COMM_WORLD);
    } else if (rank == intermediary_server_rank) {
        // Questo è l'intermediary server
        printf("Sono l'intermediary server. Rank %d di %d.\n", rank, size);
        // Ricevi il messaggio dal central server
        MPI_Recv(message, 4, MPI_FLOAT, central_server_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Messaggio ricevuto dal server centrale: %f %f %f %f\n", message[0], message[1], message[2], message[3]);
        // Invia il messaggio ai dispositivi locali
        for (int i = 2; i < size; i++) {
            if (i != central_server_rank && i != intermediary_server_rank) {
                MPI_Send(message, 4, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
            }
        }
    } else {
        // Questo è un dispositivo locale
        printf("Sono un dispositivo locale. Rank %d di %d.\n", rank, size);
        // Ricevi il messaggio dall'intermediary server
        MPI_Recv(message, 4, MPI_FLOAT, intermediary_server_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Messaggio ricevuto dall'intermediary server: %f %f %f %f\n", message[0], message[1], message[2], message[3]);
    }

    MPI_Finalize();
    return 0;
}
