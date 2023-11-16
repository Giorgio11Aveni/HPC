// header_processes.h
#include <mpi.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void initializeGlobalMessage(float global_message[4], int rank, int group_server_rank);

int determineGroupNumber(int rank);

void getGroupInfo(MPI_Comm group, int *group_rank, int *group_size);


#endif // FUNCTIONS_H
