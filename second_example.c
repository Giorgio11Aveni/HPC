#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    float central_server_message[4];
    float intermediary_message[4];

    int color = world_rank / 4; // Determine color based on row

    // Split the communicator based on the color and use the
    // original rank for ordering
    MPI_Comm row_comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &row_comm);

    int row_rank, row_size;
    MPI_Comm_rank(row_comm, &row_rank);
    MPI_Comm_size(row_comm, &row_size);

    if (world_rank == 0)
    {
        central_server_message[0] = 1.0;
        central_server_message[1] = 2.0;
        central_server_message[2] = 3.0;
        central_server_message[3] = 4.0;
        MPI_Send(central_server_message, 4, MPI_FLOAT, 0, 0, row_comm);
    }
    
    if (row_rank == 0)
    {
        
         MPI_Recv(intermediary_message, 4, MPI_FLOAT, 0, 0, row_comm, MPI_STATUS_IGNORE);
         printf("Sono un server intermedio. Rank %d di %d \n Messaggio ricevuto dal server centrale: %f %f %f %f", row_rank, row_size, intermediary_message[0], intermediary_message[1], intermediary_message[2], intermediary_message[3]);
    }
    printf("WORLD RANK/SIZE: %d/%d \t ROW RANK/SIZE: %d/%d\n",
        world_rank, world_size, row_rank, row_size);

    MPI_Comm_free(&row_comm);

    MPI_Finalize();
    return 0;
}
