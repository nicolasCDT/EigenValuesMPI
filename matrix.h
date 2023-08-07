#ifndef EIGENVALUES_MATRIX_READER_C_H
#define EIGENVALUES_MATRIX_READER_C_H

// Util functions
void free2DTab(double** ma, int size);
void free3DTab(double*** tab, int size);

double*** cutMatrix(const double** matrix, int size);

double** readMatrixFromFile(const char* filename, int* m);


// Search functions
double searchEigenValueOneThread(double** matrix, int m, int iterations);

double masterSearchEigenValueFourComputers(double** matrix, int m, int iterations);
void slaveSearchEigenValueFourComputers(double** matrix, int m, int iterations);


// Math functions
double multiplication(const double* line, double* column, int size);
double norm(const double* vector, int size);
double squareSum(const double* vector, int size);


#endif //EIGENVALUES_MATRIX_READER_C_H
