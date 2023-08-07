# Eigenvalues with MPI

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Description
This project is a C program that utilizes the Message Passing Interface (MPI) to calculate the dominant eigenvalue of a matrix using the power iteration method.

Calculating the dominant eigenvalue of a matrix is a fundamental problem in linear algebra and has numerous applications in various fields such as physics, engineering, and computer science. The dominant eigenvalue provides essential information about the behavior and stability of systems represented by matrices.

## How to Use

To use this program, follow these steps:

To use this program, follow these steps:

1. Clone this repository to your local machine.
2. Navigate to the project directory.
3. Open the `Makefile` and note the available commands:
   - `make compile_sync`: Compile the program for synchronous usage.
   - `make run_sync`: Run the program synchronously.
   - `make make_mpi`: Compile the program for distributed usage with MPI.
   - `make run_mpi`: Run the program with MPI (distributed usage).
   - `make default`: Compile and run the program with MPI.

4. You can modify the number of iterations in the `config.h` file:
    ```cpp
    #define ITERATION_COUNT 100
    ```
5. And you can change the matrix used in the `main.c` file:
    ```c
    const char* filepath = "matrices/matrix5.txt";
    ```

## License
The project is distributed under the MIT License, which allows you to use, modify, and distribute the tool freely, including for commercial purposes. 
However, you must provide appropriate credit to the [original author](https://github.com/nicolasCDT/).

## Author
This project was created by Nicolas Coudert. 
* Email: [nicolas@coudert.pro](mailto:nicolas@coudert.pro)
* GitHub: [nicolasCDT](https://github.com/nicolasCDT)

Feel free to report any issues or sggest improvements via GitHub issues.
