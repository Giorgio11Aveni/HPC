#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
   int central_server_message[4];

    int color = world_rank / 4; // Determine color based on row

    // Split the communicator based on the color and use the
    // original rank for ordering
    MPI_Comm row_comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &row_comm);

    int row_rank, row_size;
    MPI_Comm_rank(row_comm, &row_rank);
    MPI_Comm_size(row_comm, &row_size);

    if (world_rank == 0) {
        central_server_message[0] = 1;
        central_server_message[1] = 2;
        central_server_message[2] = 3;
        central_server_message[3] = 4;

        printf("Sono il server centrale. Rank globale %d di %d\n Messaggio che voglio inoltrare: %d %d %d %d\n\n", world_rank, world_size, central_server_message[0], central_server_message[1], central_server_message[2], central_server_message[3]);
        
        MPI_Bcast(central_server_message, 4, MPI_INT, 0, row_comm); // Broadcast il messaggio nel nuovo comunicatore       
    }

    

    if (row_rank == 0) {
        printf("Sono un server intermedio. Rank %d di %d\n Messaggio ricevuto dal server centrale: %d %d %d %d\n\n", row_rank, row_size, central_server_message[0], central_server_message[1], central_server_message[2], central_server_message[3]);
    } else {
        printf("Sono un dispositivo locale. Rank %d di %d\n Messaggio ricevuto dal server centrale: %d %d %d %d\n\n", row_rank, row_size, central_server_message[0], central_server_message[1], central_server_message[2], central_server_message[3]);
    }

    MPI_Comm_free(&row_comm);

    MPI_Finalize();
    return 0;
}
