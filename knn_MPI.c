#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <string.h>
#include <unistd.h>

#define NTRAIN 135
#define NFEATURES 4
#define NTEST 16
#define NCLASSES 3
#define X_TRAIN_PATH "C:/Users/tecnico/Desktop/UniME/Materie/HPC/MPI/Programs/Parallel-KNN_example/Knn-MPI/X_train.csv"
#define Y_TRAIN_PATH "C:/Users/tecnico/Desktop/UniME/Materie/HPC/MPI/Programs/Parallel-KNN_example/Knn-MPI/y_train.csv"
#define X_TEST_PATH "C:/Users/tecnico/Desktop/UniME/Materie/HPC/MPI/Programs/Parallel-KNN_example/Knn-MPI/X_test.csv"
#define Y_TEST_PATH "C:/Users/tecnico/Desktop/UniME/Materie/HPC/MPI/Programs/Parallel-KNN_example/Knn-MPI/y_test.csv"
#define K 3
#define TOPN 3

char class[NCLASSES][25] = {"Iris-setosa", "Iris-versicolor", "Iris-virginica"};

void checkFile(FILE *f)
{
    if (f == NULL)
    {
        printf("Error while reading file\n");
        exit(1);
    }
}

float *getFloatMat(int m, int n)
{
    float *mat = NULL;
    mat = (float *)calloc(m * n, sizeof(float));
    return mat;
}

float *initFeatures(char path[])
{
    int index = 0;
    FILE *f = NULL;
    float *mat = NULL;

    mat = getFloatMat(NTRAIN, NFEATURES);

    f = fopen(path, "r");
    checkFile(f);

    while (fscanf(f, "%f%*c", &mat[index]) == 1)
        index++;

    fclose(f);
    return mat;
}

void stampaMatriceFloat(int righe, int colonne, float matrice[righe][colonne])
{
    int i, j;

    for (i = 0; i < righe; i++)
    {
        for (j = 0; j < colonne; j++)
        {
            printf("%.2f ", matrice[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    int rank;
    int size;
    float *X_train;
    int i, j;
    int ndata_per_process, nrows_per_process;
    float *pdata;
    int t1,t2;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        t1 = MPI_Wtime();
    }
    

    if (NTRAIN % size != 0)
    {
        if (rank == 0)
            printf("Number of rows in the training dataset should be divisible by the number of processors\n");

        MPI_Finalize();
        exit(0);
    }

    // Process 0 reads the training dataset (X_train) from a CSV file and prints it.
    if (rank == 0)
    {
		
        X_train = initFeatures(X_TRAIN_PATH);
        
    }

    // Wait for process 0 to finish reading and printing before proceeding
    MPI_Barrier(MPI_COMM_WORLD);

    // initialise arrays
    nrows_per_process = NTRAIN / size;
    ndata_per_process = nrows_per_process * NFEATURES;

    pdata = getFloatMat(nrows_per_process, NFEATURES);

    // Scatter the data to all processes
    MPI_Scatter(X_train, ndata_per_process, MPI_FLOAT, pdata, ndata_per_process, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Every process prints its own portion of the data
    /*printf("Process %d printing its portion of data:\n", rank);
    stampaMatriceFloat(nrows_per_process, NFEATURES, (float(*)[NFEATURES])pdata);*/
    sleep(2);
    if (rank == 0)
    {
        t2 = MPI_Wtime();
    }

	if (rank == 0)
	{
		printf("Time for execution (%d Processors): %f\n", size, t2 - t1);
		free(X_train);

	}

    // Finalize MPI
    MPI_Finalize();
    return 0;
}
