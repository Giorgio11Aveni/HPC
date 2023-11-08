/*this code implement an hierarchical structure that is made up by a server and local devices. 
The server is the process with global rank 0 and it will create the message and it will send in
broadcast to all the processes.
All the processes are split into 3 different group. They will print the recived message form the server and they will print it.*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 4) {
        if (rank == 0) {
            printf("Questo programma richiede almeno 4 processi.\n");
        }
    } else {
        int group_number;

        if (rank == 0) {
            group_number = -1; // Processo con rank 0 non appartiene a nessun gruppo
        } else {
            group_number = (rank - 1) % 3; // Suddividi gli altri processi in 3 gruppi
        }

        MPI_Comm comm_group;
        MPI_Comm_split(MPI_COMM_WORLD, group_number, rank, &comm_group);

        int group_rank;
        int group_size;
        MPI_Comm_rank(comm_group, &group_rank);
        MPI_Comm_size(comm_group, &group_size);

        printf("Processo %d nel gruppo %d. Rank nel gruppo: %d\n", rank, group_number, group_rank);
        float global_message [4];
        global_message[0] = -1.0;
        global_message[1] = -1.0;
        global_message[2] = -1.0;
        global_message[3] = -1.0;
         // Inizializza il messaggio del processo con rank 0 globale

        if (group_rank == 0) {
            if (rank == 0) {
                // Processo con rank 0 globale inizia il messaggio
                global_message[0] = 10.0; // Esempio di messaggio
                global_message[1] = 11.0;
                global_message[2] = 12.0;
                global_message[3] = 13.0;
            }
        }

        // Invia il messaggio del processo con rank 0 globale a tutti i gruppi
        MPI_Bcast(&global_message, 4, MPI_FLOAT, 0, MPI_COMM_WORLD);

        // Stampa il messaggio ricevuto da ciascun processo nel gruppo
        printf("Ha ricevuto il messaggio: %f %f %f %f\n\n", global_message[0],global_message[1],global_message[2],global_message[3]);

        int group_members[group_size];
        MPI_Gather(&rank, 1, MPI_INT, group_members, 1, MPI_INT, 0, comm_group);

        if (group_rank == 0) {
            printf("Gruppo %d comprende i seguenti membri: ", group_number);
            for (int i = 0; i < group_size; i++) {
                printf("%d ", group_members[i]);
            }
            printf("\n\n");
        }

        if (group_number >= 0) {
            MPI_Comm_free(&comm_group);
        }
    }

    MPI_Finalize();
    return 0;
}
