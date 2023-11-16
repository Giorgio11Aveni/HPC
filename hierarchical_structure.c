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

    getGroupInfo(intermediary_server1, &group1_rank, &group1_size);
  

    if (rank == 1 || group_number == 1)
    {
        MPI_Bcast(&global_message, 4, MPI_FLOAT, 0, intermediary_server1);
    }
    
    MPI_Comm intermediary_server2;
    MPI_Comm_split(MPI_COMM_WORLD, intermediary_server_number_associated, rank, &intermediary_server2);

    int group2_rank;
    int group2_size;

    getGroupInfo(intermediary_server2, &group2_rank, &group2_size);

        if (rank == 2 || group_number == 2)
        {
            MPI_Bcast(&global_message, 4, MPI_FLOAT, 0, intermediary_server2);
        }
    
    MPI_Comm intermediary_server3;
    MPI_Comm_split(MPI_COMM_WORLD, intermediary_server_number_associated, rank, &intermediary_server3);

    int group3_rank;
    int group3_size;

    getGroupInfo(intermediary_server3, &group3_rank, &group3_size); 

        if (rank == 3 || group_number == 3)
        {
            MPI_Bcast(&global_message, 4, MPI_FLOAT, 0, intermediary_server3);
        }
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
            
        }else if (group_number == 1)
        {
            printf("Process rank %d.\nLocal Device.\nGroup number: %d.\nRank in the group: %d.\nIntermediary Server associated: %d.\nMessage received form the intermediary server %d: %f %f %f %f\n\n",rank, group_number, group1_rank, intermediary_server_number_associated, intermediary_server_number_associated, global_message[0], global_message[1], global_message[2], global_message[3]);
        }else if (group_number == 2)
        {
            printf("Process rank %d.\nLocal Device.\nGroup number: %d.\nRank in the group: %d.\nIntermediary Server associated: %d.\nMessage received form the intermediary server %d: %f %f %f %f\n\n",rank, group_number, group2_rank, intermediary_server_number_associated, intermediary_server_number_associated, global_message[0], global_message[1], global_message[2], global_message[3]);
        }else if (group_number == 3)
        {
            printf("Process rank %d.\nLocal Device.\nGroup number: %d.\nRank in the group: %d.\nIntermediary Server associated: %d.\nMessage received form the intermediary server %d: %f %f %f %f\n\n",rank, group_number, group3_rank, intermediary_server_number_associated, intermediary_server_number_associated, global_message[0], global_message[1], global_message[2], global_message[3]);
        }

        int label = 0;
        int media = 0;


        if (group_number == 1 && rank != 1)
        {
            // Numero di thread
    const int num_thread = 5;

    // Array di dati per i thread
    ThreadData thread_data[num_thread];

    // Array per memorizzare i risultati
    int *risultati[num_thread];

    // Inizializza i dati e avvia i thread
    pthread_t threads[num_thread];
    for (int i = 0; i < num_thread; ++i) {
        thread_data[i].thread_id = i;

        pthread_create(&threads[i], NULL, calcolaNumero, (void *)&thread_data[i]);
    }

    // Attendi la fine di tutti i thread e calcola la media dei risultati
    int somma = 0;
    for (int i = 0; i < num_thread; ++i) {
        pthread_join(threads[i], (void **)&risultati[i]);
        somma += *risultati[i];
    }

    for (int i = 0; i < num_thread; ++i) {

        printf("Thread %d: %d\n", i, *risultati[i]); 
        

        // Libera la memoria allocata per il risultato
        free(risultati[i]);
    }


    // Calcola la media
     media = somma / num_thread;

    printf("Average process %d: %d\n\n", rank, media);

        }else if (group_number == 2 && rank != 2)
        {
            label = 19;
             printf("Message to send: %d\n\n", label);
        }else if (group_number == 3)
        {
            label = 30;
             printf("Message to send: %d\n\n", label);
        }

    int gathered_array0[group_server_size]; // The root process will collect all the arrays into this array of float array
    int gathered_array1[group1_size]; // The root process will collect all the arrays into this array of float arrays
    int gathered_array2[group2_size]; // The root process will collect all the arrays into this array of float arrays
    int gathered_array3[group3_size]; // The root process will collect all the arrays into this array of float arrays
        

        if (rank == 1 || group_number == 1)
        {
            MPI_Gather(&media, 1, MPI_INT, &gathered_array1, 1, MPI_INT, 0, intermediary_server1);

           if (group1_rank == 0) {
            printf("Values received from group 1 processes:\n");

            for (int i = 1; i < group1_size; i++) {
                printf("Process %d: %d\n", i, gathered_array1[i]);
            }
                    printf("\n");
                }

            }


        if (rank == 2 || group_number == 2)
        {
            MPI_Gather(&label, 1, MPI_INT, &gathered_array2, 1, MPI_INT, 0, intermediary_server2);

           if (group2_rank == 0) {
            printf("Values received from group 2 processes:\n");

            for (int i = 1; i < group2_size; i++) {
                printf("Process %d: %d\n", i, gathered_array2[i]);
            }
                    printf("\n");
                }

            }
        
        if (rank == 3 || group_number == 3)
        {
            MPI_Gather(&label, 1, MPI_INT, &gathered_array3, 1, MPI_INT, 0, intermediary_server3);

           if (group3_rank == 0) {
            printf("Values received from group 3 processes:\n");

            for (int i = 1; i < group3_size; i++) {
                printf("Process %d: %d\n", i, gathered_array3[i]);
            }
                    printf("\n");
                }

                
            }

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
