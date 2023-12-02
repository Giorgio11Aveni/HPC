#include <stdio.h>
#include <stdlib.h>

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
	mat = (float*)calloc(m*n, sizeof(float));

	return mat;
}

// Definizione di una funzione di stampa della matrice (da sostituire con la tua implementazione)
void stampaMatrice(float *mat, int numRows) {
    for (int i = 0; i < numRows; i++) {
        printf("%.2f ", mat[i]);
    }
    printf("\n");
}

// Dichiarazione di una variabile globale
int variabileGlobale = 0;
int rows_per_process = 15;
// Funzione che modifica il valore di una variabile globale
void initMatrix(int *p, char path[]) {
    FILE *f = NULL;
    float *mat = NULL;
    mat = getFloatMat(rows_per_process, 1);

    f = fopen(path, "r");
    checkFile(f);

    int index = 0;
    int lastReadRow = 0; // Variabile per tenere traccia dell'ultima riga letta

    // Iterazione per leggere e stampare le righe
    while (fscanf(f, "%f%*c", &mat[index]) == 1) {
        index++;

        // Se abbiamo letto rows_per_process valori, stampa la matrice e resetta l'indice
        if (index == rows_per_process) {
            stampaMatrice(mat, rows_per_process); // Sostituisci con la tua funzione di stampa
            index = 0;
            lastReadRow += rows_per_process;

            // Esci se abbiamo raggiunto la fine del file
            if (feof(f)) {
                break;
            }

            // Imposta la posizione del cursore nel file all'ultima riga letta
            fseek(f, lastReadRow, SEEK_SET);
        }
    }

    fclose(f);
}



int main() {

    int startRow = 0; // Puoi impostare questa variabile se vuoi iniziare da una specifica riga

    // Sostituisci "tuofile.csv" con il percorso effettivo del tuo file CSV
    char filePath[] = "C:/Users/tecnico/Desktop/UniME/Materie/HPC/MPI/Programs/Federated_Parallel_KNN/X_train.csv";
    
initMatrix(&startRow, filePath);

    return 0;
}
