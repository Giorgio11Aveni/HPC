#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void initializeGlobalMessage(float global_message[4], int rank, int group_server_rank) {
    // Initialization of global message for rank 0
        if (rank == 0) {
        // Input 4 float numbers from the user
        for (int i = 0; i < 4; ++i) {
            scanf("%f", &global_message[i]);
        }
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

void gatherAndPrintValues(int rank, int group_number, int average, int* gathered_array, int group_rank, int group_size, MPI_Comm intermediary_server, int intermediary_server_associated) {
        
    MPI_Gather(&average, 1, MPI_INT, gathered_array, 1, MPI_INT, 0, intermediary_server);

    if ((group_rank == 0 && intermediary_server_associated == 1) || (group_rank == 0 && intermediary_server_associated == 2) || (group_rank == 0 && intermediary_server_associated == 3)) {
        printf("Values received from group %d processes:\n", intermediary_server_associated);

        for (int i = 1; i < group_size; i++) {
            printf("Process %d: %d\n", i, gathered_array[i]);
        }
    }
}

int calculateAndPrintAverage(int rank, int gathered_array[], int avg_intermediary_server) {
    int sum = 0;

    for (int i = 0; i < 4; i++) {
        sum += gathered_array[i];
    }

    switch(rank) {
        case 1:
            avg_intermediary_server = sum / 3;
            printf("Average group 1: %d\n\n", avg_intermediary_server);
            return avg_intermediary_server;
        case 2:
            avg_intermediary_server = sum / 3;
            printf("Average group 2: %d\n\n", avg_intermediary_server);
            return avg_intermediary_server;
        case 3:
            avg_intermediary_server = sum / 3;
            printf("Average group 3: %d\n\n", avg_intermediary_server);
            return avg_intermediary_server;
        default:
            
    }
}

void gatherAndPrintFinalLabel(int rank, int group_number, int* gathered_array0, int* final_gather, int avg_intermediary_server1, int avg_intermediary_server2, int avg_intermediary_server3, int group_server_rank, int group_server_size, MPI_Comm server_group) {
    
    if (rank == 0 || group_number == 0){
    MPI_Gather(&avg_intermediary_server1, 1, MPI_INT, gathered_array0, 1, MPI_INT, 0, server_group);
    final_gather[1] = gathered_array0[1];

    MPI_Gather(&avg_intermediary_server2, 1, MPI_INT, gathered_array0, 1, MPI_INT, 0, server_group);
    final_gather[2] = gathered_array0[2];

    MPI_Gather(&avg_intermediary_server3, 1, MPI_INT, gathered_array0, 1, MPI_INT, 0, server_group);
    final_gather[3] = gathered_array0[3];

        if (group_server_rank == 0) {
            printf("Values received from all group processes:\n");

            for (int i = 1; i < group_server_size; i++) {
                printf("Average group %d: %d\n", i, final_gather[i]);
            }

            printf("\n");

            int sum = 0;

            for (int i = 1; i <= 3; i++) {
                sum += final_gather[i];
            }

            int final_label = sum / 3;
            printf("Final label for the point: %d \n\n", final_label);
        }

        printf("\n");
    }
}

void freeIntermediaryCommunicator(int rank, int group_number, MPI_Comm* intermediary_server1, MPI_Comm* intermediary_server2, MPI_Comm* intermediary_server3) {
    if (rank == 1 || group_number == 1) {
        MPI_Comm_free(intermediary_server1);
    }

    if (rank == 2 || group_number == 2) {
        MPI_Comm_free(intermediary_server2);
    }

    if (rank == 3 || group_number == 3) {
        MPI_Comm_free(intermediary_server3);
    }
}

void group_members(int rank, int group_server_size, int group_server_rank, MPI_Comm server_group, int group_number){

    int group_members[group_server_size];
        MPI_Gather(&rank, 1, MPI_INT, group_members, 1, MPI_INT, 0, server_group);

        if (group_number != -1) {
            if (group_server_rank == 0) {
                printf("Group %d includes the following processes: ", group_number);
                for (int i = 0; i < group_server_size; i++) {
                    printf("%d ", group_members[i]);
                }
                printf("\n\n");
            }
        }

}
