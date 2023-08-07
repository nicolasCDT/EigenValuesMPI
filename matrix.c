#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include "config.h"

#ifdef USE_MPI
    #include <mpi/mpi.h>
#endif

/**
 * Frees the memory of a 2D array
 * @param tab Pointer to the array
 * @param size Size of the array
 */
void free2DTab(double** tab, int size) {
    for(int i = 0; i < size; i++)
        free(tab[i]); // Free each line
    free(tab); // Free column
}

/**
 * Frees the memory of a 3D array
 * @param tab Pointer to the array
 * @param size Size of the container array
 */
void free3DTab(double*** tab, int size) {
    for(int i = 0; i < 4; i++) // For each 2DTab
        free2DTab(tab[i], size); // Free it
    free(tab); // Free global list
}

/**
 * Function that reads a matrix from a file and returns a pointer to it
 * @param filename Path to the file containing the matrix
 * @param m Number of rows in the matrix
 * @return Address of the matrix (double**)
 */
double** readMatrixFromFile(const char* filename, int* m) {

    double** matrix;

    // Open file
    FILE* file = fopen(filename, "r");

    // If file pointer is NULL (cannot open file)
    if(file == NULL)
        return NULL;

    char* current_line = NULL;
    int is_sized = 0;
    size_t current_line_size = 0;
    int current_index = 0;
    int resize = 0;

    while (getline(&current_line, &current_line_size, file) != -1) {
        if(current_line_size > 0 && current_line[0] == '#')
            continue;

        // Get matrices size
        if(is_sized == 0) {
            is_sized = 1;
            long size = strtol(current_line, NULL, 10);
            assert(size < INT32_MAX);

            if(size%2) {
                size++;
                resize++;
            }

            *m = (int) size;
            matrix = (double**) calloc(*m, sizeof(double*));

            if(matrix == NULL) {
                printf("Cannot init matrices\n");
                return NULL; // Return with error
            }
            continue;
        }

        // Read current line and add it into matrices[current_index]
        char* sep = " ";
        char* token = strtok(current_line, sep);
        int i = 0;

        matrix[current_index] = (double*) calloc(*m, sizeof(double));
        if(matrix[current_index] == NULL) {
            printf("Cannot init matrices\n");
            return NULL; // Return with error
        }

        while(token != NULL) {
            double item = strtod(token, NULL);
            matrix[current_index][i] = item;
            token = strtok(NULL, " ");
            i++;
        }
        if(resize)
            matrix[current_index][i] = 0;
        current_index++;
    }

    if(resize) {
        matrix[*m-1] = calloc(*m, sizeof(double));
        for(int i = 0; i < *m; i++)
            matrix[*m-1][i] = 0;
    }

    fclose(file); // Close the file

    return matrix; // Return 0 -> No error
}

/**
 * Splits a matrix into 4 equal blocks
 * @param matrix 2n*2n matrix (n in N*)
 * @param size Size of the matrix
 * @return List with the 4 matrices (pointers to the matrices)
 */
double*** cutMatrix(const double** matrix, int size) {
    // Allocate global tab
    double*** matrices = calloc(4, sizeof(double**));
    // Get size of a block
    int newMatricesSize = size/2;

    // For each matrix blocks
    matrices[0] = calloc(newMatricesSize, sizeof(double*));
    matrices[1] = calloc(newMatricesSize, sizeof(double*));
    matrices[2] = calloc(newMatricesSize, sizeof(double*));
    matrices[3] = calloc(newMatricesSize, sizeof(double*));

    // For each line in matrix
    for(int line = 0; line < newMatricesSize; line++) {
        matrices[0][line] = calloc(newMatricesSize, sizeof(double));
        matrices[1][line] = calloc(newMatricesSize, sizeof(double));
        matrices[2][line] = calloc(newMatricesSize, sizeof(double));
        matrices[3][line] = calloc(newMatricesSize, sizeof(double));

        for(int column = 0; column < newMatricesSize; column++) {
            matrices[0][line][column] = matrix[line][column];
            matrices[1][line][column] = matrix[line][newMatricesSize + column];
            matrices[2][line][column] = matrix[newMatricesSize + line][column];
            matrices[3][line][column] = matrix[newMatricesSize + line][newMatricesSize + column];
        }
    }

    return matrices;
}

/**
 * Multiplication of two vectors (dot product)
 * @param vector1 First vector to be multiplied
 * @param vector2 Second vector to be multiplied
 * @param size Length of the vectors
 * @return Result of the multiplication
 */
double multiplication(const double* vector1, const double* vector2, int size) {
    double result = 0;

    for(int i = 0; i < size; i++)
        result += vector1[i]*vector2[i];

    return result;
}

/**
 * Returns the norm of a vector
 * @param vector Vector
 * @param size Number of components
 * @return Norm of the vector
 */
double norm(const double* vector, int size) {
    return sqrt(multiplication(vector, vector, size));
}

/**
 * Returns the sum of squared components of a vector
 * @param vector Vector
 * @param size Size of the vector
 * @return Sum of squared components
 */
double squareSum(const double* vector, int size) {
    double sum = 0;

    for(int i = 0; i < size; i++) {
        sum += vector[i]*vector[i];
    }

    return sum;
}

#ifdef USE_MPI
/**
 * Searches for the dominant eigenvalue of a square matrix using 4 computers
 * Function for computers with a rank different from 0
 * @param matrix Square matrix
 * @param m Size of the matrix
 * @param iterations Number of iterations to perform
 */
void slaveSearchEigenValueFourComputers(double** matrix, int m, int iterations) {
    double* vector = calloc(m, sizeof(double));
    double* temp = calloc(m, sizeof(double));
    double sumOfSquares = 0.;
    double norm_value = 0.;

    for(int i = 0; i < m; i++)
        vector[i] = 1;

    // For each iteration
    for(int i = 0; i < iterations; i++) {

        // For line in matrices
        for(int l_index = 0; l_index < m; l_index++)
            temp[l_index] = multiplication(matrix[l_index], vector, m);

        memcpy(vector, temp, m*sizeof(double)); // write V_{i}

        MPI_Send(vector, m, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD); // Send to master

        // Receive new vector
        MPI_Recv(vector, m, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    free(vector);
    free(temp);
}

/**
 * Searches for the dominant eigenvalue of a square matrix using 4 computers
 * Function for the computer with rank 0
 * @param matrix Square matrix
 * @param m Size of the matrix
 * @param iterations Number of iterations to perform
 * @return Dominant eigenvalue
 */
double masterSearchEigenValueFourComputers(double** matrix, int m, int iterations) {
    double* vector = calloc(m, sizeof(double));
    double* temp = calloc(m, sizeof(double));
    double* vBuffer = calloc(m, sizeof(double));
    double sumOfSquares = 0.;
    double tempSum = 0;

    for(int i = 0; i < m; i++)
        vector[i] = 1;

    // For each iteration
    for(int i = 0; i < iterations; i++) {

        // For line in matrices
        for(int l_index = 0; l_index < m; l_index++)
            temp[l_index] = multiplication(matrix[l_index], vector, m);

        memcpy(vector, temp, m*sizeof(double)); // write V_{i}

        MPI_Recv(vBuffer, m, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int c = 0; c < m; c++)
            vector[c] += vBuffer[c];

        MPI_Recv(temp, m, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Recv(vBuffer, m, MPI_DOUBLE, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int c = 0; c < m; c++)
            vBuffer[c] += temp[c];

        sumOfSquares = sqrt(squareSum(vBuffer, m) + squareSum(vector, m));
        
        for(int j = 0; j < m; j++) {
            vector[j] /= sumOfSquares;
            vBuffer[j] /= sumOfSquares;
        }

        // Send new Vector
        MPI_Send(vector, m, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
        MPI_Send(vBuffer, m, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD);
        MPI_Send(vBuffer, m, MPI_DOUBLE, 3, 0, MPI_COMM_WORLD);

    }

    free(vector);
    free(temp);
    free(vBuffer);

    return sumOfSquares;
}
#endif

/**
 * Searches for the dominant eigenvalue of a square matrix
 * @param matrix Square matrix
 * @param size Size of the matrix
 * @param iterations Number of iterations to perform
 * @return Dominant eigenvalue of the square matrix
 */
double searchEigenValueOneThread(const double** matrix, int size, int iterations) {
    double* vector = calloc(size, sizeof(double)); // 1
    double* temp = calloc(size, sizeof(double)); // 2
    double norm_value;

    for(int i = 0; i < size; i++) {
        vector[i] = 1; // Fill v0 with 1
    }

    // For each iteration
    for(int i = 0; i < iterations; i++) {

        // For line in matrices
        for(int l_index = 0; l_index < size; l_index++)
            temp[l_index] = multiplication(matrix[l_index], vector, size);

        memcpy(vector, temp, size*sizeof(double)); // write V_{i}

        norm_value = norm(vector, size);

        for(int j = 0; j < size; j++)
            vector[j] /= norm_value;

    }

    free(temp); // 1
    free(vector); // 2

    return norm_value;
}
