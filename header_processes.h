// header_processes.h
#include <mpi.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void initializeGlobalMessage(float global_message[4], int rank, int group_server_rank);

int determineGroupNumber(int rank);

void getGroupInfo(MPI_Comm group, int *group_rank, int *group_size);

int getIntermediaryServerNumber(int rank, int group_number);

void broadcastMessage(int rank, int group_number, float *global_message, MPI_Comm intermediary_server);

void printIntermediaryServerProcessDetails(int rank, int intermediary_server_number_associated, int group_number, int group1_rank, int group2_rank, int group3_rank, int group_server_rank, float *global_message);

void printLocalDeviceDetails(int rank, int group_number, int intermediary_server_number_associated, int group1_rank, int group2_rank, int group3_rank, float *global_message);

void gatherAndPrintValues(int rank, int group_number, int average, int* gathered_array, int group_rank, int group_size, MPI_Comm intermediary_server, int intermediary_server_associated);

int calculateAndPrintAverage(int rank, int gathered_array[], int avg_intermediary_server, int group_size);

void gatherAndPrintFinalLabel(int rank, int group_number, int* gathered_array0, int* final_gather, int avg_intermediary_server1, int avg_intermediary_server2, int avg_intermediary_server3, int group_server_rank, int group_server_size, MPI_Comm server_group);

void freeIntermediaryCommunicator(int rank, int group_number, MPI_Comm* intermediary_server1, MPI_Comm* intermediary_server2, MPI_Comm* intermediary_server3);

void group_members(int rank, int group_server_size, int group_server_rank, MPI_Comm server_group, int group_number);

#endif // FUNCTIONS_H
