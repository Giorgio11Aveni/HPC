#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 7) {
        if (rank == 0) {
            printf("Questo programma richiede almeno 7 processi.\n");
        }
    } else {
        int group_number;

        if (rank == 0) {
            group_number = -1; // Processo con rank 0 non appartiene a nessun gruppo
        } else if (rank >= 1 && rank <= 3) {
            group_number = 0; // Nuovo gruppo formato dai processi con rank 1, 2, e 3
        } else {
            group_number = (rank - 4) % 3 + 1; // Suddividi gli altri processi in 3 gruppi con numeri 1, 2, e 3
        }

        MPI_Comm comm_group;
        MPI_Comm_split(MPI_COMM_WORLD, group_number, rank, &comm_group);

        int group_rank;
        int group_size;
        MPI_Comm_rank(comm_group, &group_rank);
        MPI_Comm_size(comm_group, &group_size);

        if (rank == 0) {
            printf("Processo %d non appartiene a nessun gruppo.\n\n", rank);
        } else if(group_number != 0) {
            printf("Processo %d nel gruppo %d. Rank nel gruppo: %d\n\n", rank, group_number, group_rank);
        }else{
            printf("Processo %d nel gruppo %d. Rank nel gruppo: %d\n", rank, group_number, group_rank);
        }

        float global_message[4];
       // Processo con rank 0 globale inizia il messaggio
                global_message[0] = 10.0; // Esempio di messaggio
                global_message[1] = 11.0;
                global_message[2] = 12.0;
                global_message[3] = 13.0;

       

        // Invia il messaggio solo ai processi nel nuovo gruppo (gruppo 0)
        if (group_number == 0) {
            MPI_Bcast(&global_message, 4, MPI_FLOAT, 0, comm_group);
        }

        if (rank != 0 ) {
            // Stampa il messaggio ricevuto da ciascun processo nel gruppo 0
            printf("Ha ricevuto il messaggio: %f %f %f %f\n\n", global_message[0], global_message[1], global_message[2], global_message[3]);
        }

        int group_members[group_size];
        MPI_Gather(&rank, 1, MPI_INT, group_members, 1, MPI_INT, 0, comm_group);

        if (group_number != -1) {
            if (group_rank == 0) {
                printf("Gruppo %d comprende i seguenti membri: ", group_number);
                for (int i = 0; i < group_size; i++) {
                    printf("%d ", group_members[i]);
                }
                printf("\n\n");
            }
        }

        if (group_number >= 0 && group_number != 0) {
            MPI_Comm_free(&comm_group);
        }
    }

    MPI_Finalize();
    return 0;
}
