#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void initializeGlobalMessage(float global_message[4], int rank, int group_server_rank) {
    if (rank == 0) {
        // Process with global rank 0 initializes the message
        global_message[0] = 10.0;
        global_message[1] = 11.0;
        global_message[2] = 12.0;
        global_message[3] = 13.0;

        printf("Process rank %d.\nCentral server.\nRank in the group: %d.\nMessage to send: %f %f %f %f\n\n",
               rank, group_server_rank, global_message[0], global_message[1], global_message[2], global_message[3]);
    }
}

int determineGroupNumber(int rank) {
    int group_number;

    if (rank >= 0 && rank <= 3) {
        group_number = 0;
    } else {
        group_number = (rank - 4) % 3 + 1;
    }

    return group_number;
}

void getGroupInfo(MPI_Comm group, int *group_rank, int *group_size) {
    MPI_Comm_rank(group, group_rank);
    MPI_Comm_size(group, group_size);
}

