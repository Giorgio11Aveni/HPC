#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mergeSort.h"

#define NFEATURES 4
#define MAX_LINE_LENGTH 1024

#define X_TRAIN_PATH "C:/Users/tecnico/Desktop/UniME/Materie/HPC/MPI/Programs/Parallel-KNN_example/Knn-MPI/X_train.csv"
#define Y_TRAIN_PATH "C:/Users/tecnico/Desktop/UniME/Materie/HPC/MPI/Programs/Parallel-KNN_example/Knn-MPI/y_train.csv"

#define NCLASSES 3 	
char class[NCLASSES][25] = {"Iris-setosa", "Iris-versicolor", "Iris-virginica"};
#define K 3 

// Function for initializing matrices from CSV files
void initializeMatricesFromFile(char *data_filename, const char *label_filename, float **data_matrix, int **label_matrix, int *num_rows, int num_columns) {
    // Open the data file
    FILE *data_file = fopen(data_filename, "r");
    if (!data_file) {
        perror("Error opening data file");
        exit(EXIT_FAILURE);
    }

    // Open the labels file
    FILE *label_file = fopen(label_filename, "r");
    if (!label_file) {
        perror("Error opening labels file");
        fclose(data_file);
        exit(EXIT_FAILURE);
    }

    // Count the number of lines in files
    *num_rows = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), data_file) != NULL) {
        (*num_rows)++;
    }

    // Reset cursor position in files
    fseek(data_file, 0, SEEK_SET);
    fseek(label_file, 0, SEEK_SET);

    // Allocate matrices
    *data_matrix = (float *)calloc(*num_rows * num_columns, sizeof(float));
    *label_matrix = (int *)calloc(*num_rows, sizeof(int));

    // Read data from files and initialize arrays
    for (int i = 0; i < *num_rows; i++) {
        // Read the row corresponding to the data
        fgets(line, sizeof(line), data_file);
        char *token = strtok(line, ",");
        for (int j = 0; j < num_columns; j++) {
            (*data_matrix)[i * num_columns + j] = atof(token);
            token = strtok(NULL, ",");
        }

        // Read the label from the labels file
        fgets(line, sizeof(line), label_file);
        (*label_matrix)[i] = atoi(line);
    }

    // Close files
    fclose(data_file);
    fclose(label_file);
}

// Function for printing matrices
void printMatrices(float *data_matrix, int *label_matrix, int num_rows, int num_columns) {
    // Print the data matrix
    printf("Data Matrix:\n");
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_columns; j++) {
            printf("%.2f\t", data_matrix[i * num_columns + j]);
        }
        printf("\n");
    }

    // Print the labels matrix
    printf("\nLabel Matrix:\n");
    for (int i = 0; i < num_rows; i++) {
        printf("%d\n", label_matrix[i]);
    }
}

// Function to insert the point to be classified
float* inputCoordinates() {
    float* array = (float*)malloc(4 * sizeof(float));

    if (array == NULL) {
        perror("Error allocating memory for the array");
        exit(EXIT_FAILURE);
    }

    printf("\nEnter the coordinates of the point to classify:\n\n");

    printf("Coordinate x: ");
    scanf("%f", &array[0]);

    printf("Coordinate y: ");
    scanf("%f", &array[1]);

    printf("Coordinate z: ");
    scanf("%f", &array[2]);

    printf("Coordinate w: ");
    scanf("%f", &array[3]);

    printf("\n");
    return array;
}

// Function to print the point to be classified
void printArray(float *userPoint){

    printf("Point to classify: ");
    for (int i = 0; i < 4; i++)
    {
      printf("%f ", userPoint[i]);
    }
    printf("\n\n");
}

// Function to calculate the Euclidean distance between two points in space
float calculateEuclideanDistance(float point1, float point2) {
    return sqrt(pow(point1 - point2, 2));
}

// Function to add the distances of each coordinate of the point to be classified
float *calculateRowSums(float *data_matrix, int num_rows, int num_columns, float *userPoint) {
    float *rowSums = (float *)malloc(num_rows * sizeof(float));

    for (int i = 0; i < num_rows; i++) {
        float rowSum = 0.0;  // Initialize the sum for each point (matrix row)

        for (int j = 0; j < num_columns; j++) {
            float matrixElement = data_matrix[i * num_columns + j];
            float userElement = userPoint[j];
            float distance = calculateEuclideanDistance(matrixElement, userElement);
            rowSum += distance;  // Add the distance to the sum for the current row
        }

        rowSums[i] = rowSum;
    }

    return rowSums;
}

// Function to predict and print the class for the point
void predict(int *labels) {
    int neighborCount[NCLASSES] = {0};

    for (int i = 0; i < K; i++) {
        neighborCount[labels[i]]++;
    }

    float probability[NCLASSES];
    for (int i = 0; i < NCLASSES; i++) {
        probability[i] = neighborCount[i] * 1.0 / K * 1.0;
    }

    printf("Label      Class      Probability\n");
    for (int i = 0; i < NCLASSES; i++) {
        printf("%d\t%s\t%f\n", i , class[i], probability[i]);
    }

    // Trova la classe più frequente
    int maxClass = 0;
    int maxCount = neighborCount[0];

    for (int i = 0; i < NCLASSES; i++) {
        if (neighborCount[i] > maxCount) {
            maxClass = i;
            maxCount = neighborCount[i];
        }
    }

    // Print the most frequent class
    printf("\nClass for the point: %d - %s\n\n", maxClass, class[maxClass]);
}

int main() {
    char *data_filename = X_TRAIN_PATH;  
    char *label_filename = Y_TRAIN_PATH;  

    int num_rows;
    int num_columns = NFEATURES;
    float *data_matrix;
    int *label_matrix;
    int total_rows = 135;
    FILE *data_file;
    FILE *label_file;
    
    // Call to the function for inserting the coordinates of the point to be classified
    float* userPoint = inputCoordinates();
    float* distances;

    // Call to the initialization function from CSV file
    initializeMatricesFromFile(data_filename, label_filename, &data_matrix, &label_matrix, &num_rows, num_columns);
    printArray(userPoint);

    // Call to the print function
    //printMatrices(data_matrix, label_matrix, num_rows, num_columns);

    // Call to the function for calculating Euclidean distances
    float *rowSums = calculateRowSums(data_matrix, num_rows, num_columns, userPoint);

    // Call the function to sort in the distances in ascending order and synchronize the labels
    mergeSort(rowSums, 0, total_rows-1, label_matrix);

    // Call to the Print sums of ordered distances function
    /*printf("Sums of distances sorted for each row:\n");
    for (int i = 0; i < num_rows; i++) {
        printf("Row %d: %.4f - Label %d \n", i + 1, rowSums[i], label_matrix[i]);
    }*/

    //Call to the class prediction function
    predict(label_matrix);
   
    // Free the allocated memory
    free(userPoint);
    free(data_matrix);
    free(label_matrix);

    return 0;
}
