#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <pthread.h>
#include "header_thread.h"
#include "header_processes.h"
#include "processes_functions.c"
#include "knn.c"


int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int group_number;
    int intermediary_server_number_associated;

    float global_message[4];

    // Determine the group number for the current process
    group_number = determineGroupNumber(rank);

    MPI_Comm server_group;
    // Split the communicator into subgroups based on group_number
    MPI_Comm_split(MPI_COMM_WORLD, group_number, rank, &server_group);

    int group_server_rank;
    int group_server_size;

    // Get information about the subgroup
    getGroupInfo(server_group, &group_server_rank, &group_server_size);

    // Check if there are enough processes for the program to run
    if (size < 7) {
        if (rank == 0) {
            printf("This program requires at least 7 processes.\n");
        }
    } else {
        // Initialization of global message for rank 0
        if (rank == 0) {
            initializeGlobalMessage(global_message, rank, group_server_rank);
        }

        // Broadcast the global message to all processes in the subgroup
        if (rank >= 0 && rank <= 3) {
            MPI_Bcast(&global_message, 4, MPI_FLOAT, 0, server_group);
        }

        // Determine the intermediary server number associated with the current process
        intermediary_server_number_associated = getIntermediaryServerNumber(rank, group_number);

        MPI_Comm intermediary_server1;
        // Split the communicator into sub-communicators based on intermediary_server_number_associated
        MPI_Comm_split(MPI_COMM_WORLD, intermediary_server_number_associated, rank, &intermediary_server1);

        int group1_rank;
        int group1_size;

        // Get information about the sub-communicator
        getGroupInfo(intermediary_server1, &group1_rank, &group1_size);

        // Broadcast the global message within the sub-communicator
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

        // Print details about intermediary server processes
        printIntermediaryServerProcessDetails(rank, intermediary_server_number_associated, group_number, group1_rank, group2_rank, group3_rank, group_server_rank, global_message);

        // Print details about local device
        printLocalDeviceDetails(rank, group_number, intermediary_server_number_associated, group1_rank, group2_rank, group3_rank, global_message);

        int label = 0;

        // Perform group calculations and update average
        label = knn(rank, size, X_TRAIN_PATH, Y_TRAIN_PATH,global_message);
        
        int gathered_array0[group_server_size]; // The group 0 root process will collect all the arrays into this array of float array
        int gathered_array1[group1_size]; // The group 1 root process will collect all the arrays into this array of float arrays
        int gathered_array2[group2_size]; // The group 2 root process will collect all the arrays into this array of float arrays
        int gathered_array3[group3_size]; // The group 3 process will collect all the arrays into this array of float arrays

        // Gather values from each group and print
        if (rank == 1 || group_number == 1) {
            gatherAndPrintValues(rank, group_number, label, gathered_array1, group1_rank, group1_size, intermediary_server1, intermediary_server_number_associated);
        }
        if (rank == 2 || group_number == 2) {
            gatherAndPrintValues(rank, group_number, label, gathered_array2, group2_rank, group2_size, intermediary_server2, intermediary_server_number_associated);
        }
        if (rank == 3 || group_number == 3) {
            gatherAndPrintValues(rank, group_number, label, gathered_array3, group3_rank, group3_size, intermediary_server3, intermediary_server_number_associated);
        }

        int avg_intermediary_server1 = 0;
        int avg_intermediary_server2 = 0;
        int avg_intermediary_server3 = 0;

        // Calculate and print average values for each intermediary server
        if (intermediary_server_number_associated == 1) {
            avg_intermediary_server1 = calculateAndPrintAverage(rank, gathered_array1, avg_intermediary_server1, group1_size);
        }
        if (intermediary_server_number_associated == 2) {
            avg_intermediary_server2 = calculateAndPrintAverage(rank, gathered_array2, avg_intermediary_server2, group2_size);
        }
        if (intermediary_server_number_associated == 3) {
            avg_intermediary_server3 = calculateAndPrintAverage(rank, gathered_array3, avg_intermediary_server3, group3_size);
        }

        int final_gather[3];

        // Gather and print final labels for the point
        gatherAndPrintFinalLabel(rank, group_number, gathered_array0, final_gather, avg_intermediary_server1, avg_intermediary_server2, avg_intermediary_server3, group_server_rank, group_server_size, server_group);

        // Free intermediary communicators
        freeIntermediaryCommunicator(rank, group_number, &intermediary_server1, &intermediary_server2, &intermediary_server3);

        // Get group members
        group_members(rank, group_server_size, group_server_rank, server_group, group_number);
    }

    // Finalize MPI
    MPI_Finalize();
    return 0;
}
