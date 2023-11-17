#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <pthread.h>
#include "header_thread.h"
#include "thread_functions.c"
#include "header_processes.h"
#include "processes_functions.c"


int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int group_number;
    int intermediary_server_number_associated;

    float global_message[4];

    group_number = determineGroupNumber(rank);

    MPI_Comm server_group;
    MPI_Comm_split(MPI_COMM_WORLD, group_number, rank, &server_group);

    int group_server_rank;
    int group_server_size;

    getGroupInfo(server_group, &group_server_rank, &group_server_size);
 
    if (size < 7) {
        if (rank == 0) {
            printf("This program requires at least 7 processes.\n");
        }
    } else {

        if (rank == 0)
        {
           initializeGlobalMessage(global_message, rank, group_server_rank);

        }

        if(rank>=0 && rank <=3){

         MPI_Bcast(&global_message, 4, MPI_FLOAT, 0,server_group);
    }

    intermediary_server_number_associated = getIntermediaryServerNumber(rank, group_number);
    
    MPI_Comm intermediary_server1;
    MPI_Comm_split(MPI_COMM_WORLD, intermediary_server_number_associated, rank, &intermediary_server1);

    int group1_rank;
    int group1_size;

    getGroupInfo(intermediary_server1, &group1_rank, &group1_size);
  
    broadcastMessage(rank, group_number, global_message, intermediary_server1);
    
    MPI_Comm intermediary_server2;
    MPI_Comm_split(MPI_COMM_WORLD, intermediary_server_number_associated, rank, &intermediary_server2);

    int group2_rank;
    int group2_size;

    getGroupInfo(intermediary_server2, &group2_rank, &group2_size);

    broadcastMessage(rank, group_number, global_message, intermediary_server2);
    
    MPI_Comm intermediary_server3;
    MPI_Comm_split(MPI_COMM_WORLD, intermediary_server_number_associated, rank, &intermediary_server3);

    int group3_rank;
    int group3_size;

    getGroupInfo(intermediary_server3, &group3_rank, &group3_size); 

    broadcastMessage(rank, group_number, global_message, intermediary_server3);
    
    printIntermediaryServerProcessDetails(rank, intermediary_server_number_associated, group_number, group1_rank, group2_rank, group3_rank, group_server_rank, global_message);

    printLocalDeviceDetails(rank, group_number, intermediary_server_number_associated, group1_rank, group2_rank, group3_rank, global_message);

    int label = 0;
    int media = 0;

    media = group_calculations(group_number, rank, media);
    

    int gathered_array0[group_server_size]; // The grooup 0 root process will collect all the arrays into this array of float array
    int gathered_array1[group1_size]; // The group 1 root process will collect all the arrays into this array of float arrays
    int gathered_array2[group2_size]; // The group 2root process will collect all the arrays into this array of float arrays
    int gathered_array3[group3_size]; // The group 3 process will collect all the arrays into this array of float arrays
        

    // Chiamate alla funzione per ogni gruppo
    gather_and_print_values(rank, group_number, media, gathered_array1, group1_rank, group1_size, intermediary_server1);
    gather_and_print_values(rank, group_number, media, gathered_array2, group2_rank, group2_size, intermediary_server2);
    gather_and_print_values(rank, group_number, media, gathered_array3, group3_rank, group3_size, intermediary_server3);

    int avg_intermediary_server1 = 0;
    int avg_intermediary_server2 = 0;
    int avg_intermediary_server3 = 0;

        if (rank == 1)
        {
            int sum = 0;

            for (int i = 0; i < 4; i++) {
                sum += gathered_array1[i];
            }

             avg_intermediary_server1 = sum/3;
             printf("Average group 1: %d\n\n",avg_intermediary_server1);
        }

        if (rank == 2)
        {
            int sum = 0;

            for (int i = 0; i < 4; i++) {
                sum += gathered_array2[i];
            }

            avg_intermediary_server2 = sum/3;
            printf("Average group 2: %d\n\n",avg_intermediary_server2);
        }

         if (rank == 3)
        {
            int sum = 0;

            for (int i = 0; i < 4; i++) {
                sum += gathered_array3[i];
            }

            avg_intermediary_server3 = sum/3;
            printf("Average group 3: %d \n\n",avg_intermediary_server3);
        }
        
        int final_gather[3];

        if (rank == 0 || group_number == 0)
        {
            MPI_Gather(&avg_intermediary_server1, 1, MPI_INT, &gathered_array0, 1, MPI_INT, 0, server_group);
            final_gather[1] = gathered_array0[1];
            
            MPI_Gather(&avg_intermediary_server2, 1, MPI_INT, &gathered_array0, 1, MPI_INT, 0, server_group);
            final_gather[2] = gathered_array0[2];

            MPI_Gather(&avg_intermediary_server3, 1, MPI_INT, &gathered_array0, 1, MPI_INT, 0, server_group);
            final_gather[3] = gathered_array0[3];
            
           if (group_server_rank == 0) {
            printf("Values received from all group processes:\n");

            for (int i = 1; i < group_server_size; i++) {
                printf("Average group %d: %d\n", i, final_gather[i]);
            }
                    printf("\n");
                    
                    int sum = 0;

                    for (int i = 1; i < 4; i++) {
                        
                        sum += final_gather[i];
                     }
            int final_lable = 0;
             final_lable= sum/3;
            printf("Final lable for the point: %d \n\n",final_lable);

                }

                printf("\n");    
        }

         if (rank == 1 || group_number == 1)
        {
            MPI_Comm_free(&intermediary_server1);
        }
    
        if (rank == 2 || group_number == 2)
        {
            MPI_Comm_free(&intermediary_server2); 
        }
    
        if (rank == 3 || group_number == 3)
        {
             MPI_Comm_free(&intermediary_server3); 
        }

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

    MPI_Finalize();
    return 0;
}
