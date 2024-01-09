#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <string.h>
#include <unistd.h>
#include "helper.h"
#include "mergeSort.h"


#define NTRAIN 1000026
#define NFEATURES 4
#define NTEST 16
#define NCLASSES 3
#define X_TRAIN_PATH "C:/Users/tecnico/Desktop/MPI/MS_MPI/Programs/Federated_Parallel_KNN/Dataset/dataset1000000.csv"
#define Y_TRAIN_PATH "C:/Users/tecnico/Desktop/MPI/MS_MPI/Programs/Federated_Parallel_KNN/Dataset/label1000000.csv"
#define K 3
#define TOPN 3

char class[NCLASSES][25] = {"Iris-setosa", "Iris-versicolor", "Iris-virginica"};

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

float *initLabels(char path[])
{
	int index = 0;
	FILE *f  = NULL;
	float *mat = NULL;

	mat = getFloatMat(NTRAIN, 1);

	f = fopen(path, "r");
	checkFile(f);

	while (fscanf(f, "%f%*c", &mat[index]) == 1)
		index++;

	fclose(f);
	return mat;
}

void stampaMatriceFloat(int righe, int colonne, float *matrice)
{
    int i, j;
    for (i = 0; i < righe; i++)
    {
        for (j = 0; j < colonne; j++)
        {
            printf("%.2f ", matrice[i * colonne + j]);
        }
        printf("\n");
    }
}

void calcDistance(int ndata_per_process, float *pdistance, float *pdata, float *x)
{
	int index = 0, i, j;
	for(i=0; i<ndata_per_process; i=i+NFEATURES)
	{
		pdistance[index] = 0.0;

		for(j=0; j<NFEATURES; j++)
			pdistance[index] = pdistance[index] + (pdata[i+j]-x[j])*(pdata[i+j]-x[j]);

		index++;
	}
}

int predict(float *distance, float *labels) //topn < NCLASSES
{
	float* neighborCount = getFloatMat(NCLASSES, 1);
	float* probability = getFloatMat(NCLASSES, 1);

	int i;

	for(i=0; i<K; i++)
		neighborCount[(int)labels[i]]++;

	for(i=0; i<NCLASSES; i++)
		probability[i] = neighborCount[i]*1.0/(float)K*1.0;
	
	int predicted_class = (int)getMax(neighborCount, NCLASSES);

	printf("\nProbability:\n");
	for(i=0; i<TOPN; i++)
		printf("%s\t%f\n", class[i], probability[i]);

	free(neighborCount);
	free(probability);

	return predicted_class;
}


int main(int argc, char *argv[])
{
    int rank, size, i, j, ndata_per_process, nrows_per_process;
    float *X_train, *y_train, *pdata, *pdistance, *distance, *plabels, *labels;
    
    double t1, t2;
    float UserPoint[4];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Check if the correct number of command line arguments is provided
    if (argc != 5)
    {
        if (rank == 0)
            printf("Usage: %s <float1> <float2> <float3> <float4>\n", argv[0]);

        MPI_Finalize();
        exit(1);
    }

     if (NTRAIN % size != 0)
    {
        if (rank == 0)
            printf("Number of rows in the training dataset should be divisible by the number of processors\n");

        MPI_Finalize();
        exit(0);
    }

    // Parse command line arguments and initialize UserPoint array
    for (int i = 0; i < 4; i++)
    {
        UserPoint[i] = atof(argv[i + 1]);
    }

    if (rank == 0)
    {
        t1 = MPI_Wtime(); // Record start time

        X_train = initFeatures(X_TRAIN_PATH);
        y_train = initLabels(Y_TRAIN_PATH);

    }

    // initialise arrays
    nrows_per_process = NTRAIN / size;
    ndata_per_process = nrows_per_process * NFEATURES;

    pdata = getFloatMat(nrows_per_process, NFEATURES);
    plabels = getFloatMat(nrows_per_process, 1);
    pdistance = getFloatMat(nrows_per_process, 1);
    distance = getFloatMat(NTRAIN, 1);
    labels = getFloatMat(NTRAIN, 1);

    // Scatter the data to all processes
    MPI_Scatter(X_train, ndata_per_process, MPI_FLOAT, pdata, ndata_per_process, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Scatter(y_train, nrows_per_process, MPI_FLOAT, plabels, nrows_per_process, MPI_FLOAT, 0, MPI_COMM_WORLD);

    calcDistance(ndata_per_process, pdistance, pdata, UserPoint);

    //sort the distance array 
	mergeSort(pdistance, 0, nrows_per_process - 1, plabels);

    MPI_Gather(pdistance, nrows_per_process, MPI_FLOAT, distance, nrows_per_process, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Gather(plabels, nrows_per_process, MPI_FLOAT, labels, nrows_per_process, MPI_FLOAT, 0, MPI_COMM_WORLD);


    if (rank == 0)
    {
        mergeSort(distance, 0, NTRAIN -1, labels);
        int predicted_class = predict(distance, labels);
			printf("\nPredicted label: %d - %s   \n\n", predicted_class, class[predicted_class]);
    }

    if (rank == 0)
    {
        free(X_train);
        t2 = MPI_Wtime(); // Record end time
        printf("Time taken: %f seconds\n", t2 - t1);
    }

    // Finalize MPI
    MPI_Finalize();
    return 0;
}
