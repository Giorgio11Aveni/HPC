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

int getIntermediaryServerNumber(int rank, int group_number) {
    int intermediary_server_number_associated;

    if (rank == 1 || group_number == 1) {
        intermediary_server_number_associated = 1;
    } else if (rank == 2 || group_number == 2) {
        intermediary_server_number_associated = 2;
    } else if (rank == 3 || group_number == 3) {
        intermediary_server_number_associated = 3;
    } else {
        //A default value if none of the conditions are met
        intermediary_server_number_associated = -1;
    }

    return intermediary_server_number_associated;
}

void broadcastMessage(int rank, int group_number, float *global_message, MPI_Comm intermediary_server) {
    if (rank == 1 || group_number == 1) {
        MPI_Bcast(global_message, 4, MPI_FLOAT, 0, intermediary_server);
    }
    if (rank == 2 || group_number == 2) {
        MPI_Bcast(global_message, 4, MPI_FLOAT, 0, intermediary_server);
    }
    if (rank == 3 || group_number == 3) {
        MPI_Bcast(global_message, 4, MPI_FLOAT, 0, intermediary_server);
    }
}

void printIntermediaryServerProcessDetails(int rank, int intermediary_server_number_associated, int group_number, int group1_rank, int group2_rank, int group3_rank, int group_server_rank, float *global_message) {
    if(rank>=1 && rank <=3)
        {
            if(intermediary_server_number_associated == 1)
            {
                printf("Process rank %d.\nIntermediary Server %d.\nGroup number: %d.\nRank in the group 1: %d.\nRank in the server group: %d.\nMessage received from the central server: %f %f %f %f\n\n",rank, intermediary_server_number_associated, group_number, group1_rank, group_server_rank, global_message[0], global_message[1], global_message[2], global_message[3]);
            }else if (intermediary_server_number_associated ==2)
            {
                printf("Process rank %d.\nIntermediary Server %d.\nGroup number: %d.\nRank in the group 2: %d.\nRank in the server group: %d.\nMessage received from the central server: %f %f %f %f\n\n",rank, intermediary_server_number_associated, group_number, group1_rank, group_server_rank, global_message[0], global_message[1], global_message[2], global_message[3]);
            }else if (intermediary_server_number_associated ==3)
            {
                printf("Process rank %d.\nIntermediary Server %d.\nGroup number: %d.\nRank in the group 3: %d.\nRank in the server group: %d.\nMessage received from the central server: %f %f %f %f\n\n",rank, intermediary_server_number_associated, group_number, group1_rank, group_server_rank, global_message[0], global_message[1], global_message[2], global_message[3]);
            }
            
        }
}

void printLocalDeviceDetails(int rank, int group_number, int intermediary_server_number_associated, int group1_rank, int group2_rank, int group3_rank, float *global_message) {
    if (group_number == 1 || group_number == 2 || group_number == 3) {
        printf("Process rank %d.\nLocal Device.\nGroup number: %d.\nRank in the group %d: %d.\nIntermediary Server associated: %d.\nMessage received from the intermediary server %d: %f %f %f %f\n\n",
               rank, group_number, group_number, (group_number == 1) ? group1_rank : ((group_number == 2) ? group2_rank : group3_rank),
               intermediary_server_number_associated, intermediary_server_number_associated, global_message[0], global_message[1], global_message[2], global_message[3]);
    }
}
