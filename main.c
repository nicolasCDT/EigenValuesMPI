#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include <time.h>
#include "config.h"

#ifdef USE_MPI
    #include <mpi/mpi.h>
#endif

double** matrix = NULL;

/**
 * Entry point
 */
int main() {

    const char* filepath = "matrices/matrix4.txt";
    int m;

#ifdef USE_MPI
    // Work with MPI
    int rank, p;
    MPI_Init(&argc, & argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    printf ("Started process: Number: %d\n", rank);


    // Read file: get matrices
    if(rank == 0) { // Only master read the matrix
        if((matrix = readMatrixFromFile(filepath, &m)) == NULL) {
            printf("Cannot read %s. Check the path.\n", filepath);
            exit(EXIT_FAILURE);
        }

        // Cut matrix
        double*** matrices = cutMatrix((const double**) matrix, m);
        int size = m/2;
        // Send matrix size
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Send Each line
        for(int line = 0; line < size; line++) {
            // Send all lines
            MPI_Send(matrices[1][line], size, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
            MPI_Send(matrices[2][line], size, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD);
            MPI_Send(matrices[3][line], size, MPI_DOUBLE, 3, 0, MPI_COMM_WORLD);
        }

        clock_t t1 = clock(); // Save time before processing
        double value = masterSearchEigenValueFourComputers(matrices[0], size, ITERATION_COUNT);
        float processing_time = (float)(clock()-t1)/CLOCKS_PER_SEC; // Get diff

        printf("Valeur propre dominante: %lf\n", value);
        printf("Temps de calcul = %.8f secondes\n", processing_time);

        free3DTab(matrices, size);

    } else { // Waiting for matrix part
        int m = 0;
        MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
        double** matrix = calloc(m, sizeof(double*));

        for(int iLine = 0; iLine < m; iLine++) {
            // Receive all line
            double* line = calloc(m, sizeof(double));
            MPI_Recv(line, m, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            matrix[iLine] = line;
        }

        slaveSearchEigenValueFourComputers(matrix, m, ITERATION_COUNT);

        // Free memory
        free2DTab(matrix, m);

    }

    MPI_Finalize();
#else
    // Work without MPI

    // Read file: get matrices
    if((matrix = readMatrixFromFile(filepath, &m)) == NULL) {
        printf("Cannot read %s. Check the path.\n", filepath);
        exit(EXIT_FAILURE);
    }

    clock_t t1 = clock(); // Save time before processing
    double value = searchEigenValueOneThread(matrix, m, ITERATION_COUNT);
    float processing_time = (float)(clock()-t1)/CLOCKS_PER_SEC; // Get diff

    printf("Valeur propre dominante: %lf\n", value);
    printf("Temps de calcul = %.8f secondes\n", processing_time);
#endif

    return 0;
}
