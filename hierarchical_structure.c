#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int group_number;
    int intermediary_server_number_associated;

    float global_message[4];

    if(rank>=0 && rank <=3){

        group_number = 0;

    }else{

        group_number = (rank - 4) % 3 + 1; // Subdivision of the other processes into 3 groups with numbers 1, 2, and 3
    }

    MPI_Comm comm_group;
    MPI_Comm_split(MPI_COMM_WORLD, group_number, rank, &comm_group);

    int group_server_rank;
    int group_server_size;

    MPI_Comm_rank(comm_group, &group_server_rank);
    MPI_Comm_size(comm_group, &group_server_size);
 
    if (size < 7) {
        if (rank == 0) {
            printf("This program requires at least 7 processes.\n");
        }
    } else {

        if (rank == 0)
        {
            // Process with global rank 0 initializes the message
            global_message[0] = 10.0;
            global_message[1] = 11.0;
            global_message[2] = 12.0;
            global_message[3] = 13.0;

            printf("Process rank %d. Central server. Rank in the group: %d. Message to send: %f %f %f %f\n\n",rank, group_server_rank, global_message[0], global_message[1], global_message[2], global_message[3]);
 
        }

        if(rank>=0 && rank <=3){

         MPI_Bcast(&global_message, 4, MPI_FLOAT, 0,comm_group);
    }

    if (rank == 1 || group_number == 1)
    {
        intermediary_server_number_associated = 1;
    }else if (rank == 2 || group_number == 2)
    {
        intermediary_server_number_associated = 2; 
    }else if (rank == 3 || group_number == 3)
    {
        intermediary_server_number_associated = 3; 
    }
    
    MPI_Comm intermediary_server1;
    MPI_Comm_split(MPI_COMM_WORLD, intermediary_server_number_associated, rank, &intermediary_server1);

    int group1_rank;
    int group1_size;

    MPI_Comm_rank(comm_group, &group1_rank);
    MPI_Comm_size(comm_group, &group1_size); 

    if (rank == 1 || group_number == 1)
    {
        MPI_Bcast(&global_message, 4, MPI_FLOAT, 0, intermediary_server1);
    }
    
    MPI_Comm intermediary_server2;
    MPI_Comm_split(MPI_COMM_WORLD, intermediary_server_number_associated, rank, &intermediary_server2);

    int group2_rank;
    int group2_size;

    MPI_Comm_rank(comm_group, &group2_rank);
    MPI_Comm_size(comm_group, &group2_size);

        if (rank == 2 || group_number == 2)
        {
            MPI_Bcast(&global_message, 4, MPI_FLOAT, 0, intermediary_server2);
        }
    
    MPI_Comm intermediary_server3;
    MPI_Comm_split(MPI_COMM_WORLD, intermediary_server_number_associated, rank, &intermediary_server3);

    int group3_rank;
    int group3_size;

    MPI_Comm_rank(comm_group, &group3_rank);
    MPI_Comm_size(comm_group, &group3_size);

        if (rank == 3 || group_number == 3)
        {
            MPI_Bcast(&global_message, 4, MPI_FLOAT, 0, intermediary_server3);
        }
        if(rank>=1 && rank <=3)
        {
            if(intermediary_server_number_associated == 1)
            {
            printf("Process rank %d. Intermediary Server %d. Group number: %d. Rank in the group: %d.\n Message received: %f %f %f %f\n\n",rank, intermediary_server_number_associated, group_number, group1_rank, global_message[0], global_message[1], global_message[2], global_message[3]);
            }else if (intermediary_server_number_associated ==2)
            {
                printf("Process rank %d. Intermediary Server %d. Group number: %d. Rank in the group: %d.\n Message received: %f %f %f %f\n\n",rank, intermediary_server_number_associated, group_number, group2_rank, global_message[0], global_message[1], global_message[2], global_message[3]);
            }else if (intermediary_server_number_associated ==3)
            {
                printf("Process rank %d. Intermediary Server %d. Group number: %d. Rank in the group: %d.\n Message received: %f %f %f %f\n\n",rank, intermediary_server_number_associated, group_number, group3_rank, global_message[0], global_message[1], global_message[2], global_message[3]);
            }
            
        }else if (group_number == 1)
        {
            printf("Process rank %d. Local Device. Group number: %d. Rank in the group: %d. Intermediary Server associated: %d.\n Message received: %f %f %f %f\n",rank, group_number, group1_rank, intermediary_server_number_associated, global_message[0], global_message[1], global_message[2], global_message[3]);
        }else if (group_number == 2)
        {
            printf("Process rank %d. Local Device. Group number: %d. Rank in the group: %d. Intermediary Server associated: %d.\n Message received: %f %f %f %f\n\n",rank, group_number, group2_rank, intermediary_server_number_associated, global_message[0], global_message[1], global_message[2], global_message[3]);
        }else if (group_number == 3)
        {
            printf("Process rank %d. Local Device. Group number: %d. Rank in the group: %d. Intermediary Server associated: %d.\n Message received: %f %f %f %f\n\n",rank, group_number, group3_rank, intermediary_server_number_associated, global_message[0], global_message[1], global_message[2], global_message[3]);
        }

        if (group_number == 1)
        {
            for (int i = 0; i < 4; i++) {
                global_message[i] += 10.0;
            }
            
            printf("Message to send: %f %f %f %f\n\n", global_message[0], global_message[1], global_message[2], global_message[3]);
            //MPI_Send(global_message, 4, MPI_FLOAT, 1, 0, intermediary_server1);
        }

        if (rank == 1)
        {
           // MPI_Recv(global_message, 4, MPI_FLOAT, group1_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
           // printf("Process rank %d. Intermediary Server %d. Group number: %d. Rank in the group: %d.\n Message received from %d: %f %f %f %f\n\n",rank, intermediary_server_number_associated, group_number, group1_rank, global_message[0], global_message[1], global_message[2], global_message[3]);
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

    }

    MPI_Finalize();
    return 0;
}