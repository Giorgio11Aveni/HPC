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

void gather_and_print_values(int rank, int group_number, int media, int* gathered_array, int group_rank, int group_size, MPI_Comm intermediary_server);

#endif // FUNCTIONS_H
