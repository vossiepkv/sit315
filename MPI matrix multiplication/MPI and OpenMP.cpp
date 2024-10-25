#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1024  // Matrix size

void matrixMultiply(int **A, int **B, int **C, int startRow, int endRow, int size) {
    #pragma omp parallel for
    for (int i = startRow; i < endRow; i++) {
        for (int j = 0; j < size; j++) {
            C[i][j] = 0;
            for (int k = 0; k < size; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int size = N; // assuming square matrix

    int **A, **B, **C;
    if (world_rank == 0) {
        // Master node initializes matrices
        A = (int**)malloc(size * sizeof(int*));
        B = (int**)malloc(size * sizeof(int*));
        C = (int**)malloc(size * sizeof(int*));
        for (int i = 0; i < size; i++) {
            A[i] = (int*)malloc(size * sizeof(int));
            B[i] = (int*)malloc(size * sizeof(int));
            C[i] = (int*)malloc(size * sizeof(int));
            for (int j = 0; j < size; j++) {
                A[i][j] = rand() % 100;
                B[i][j] = rand() % 100;
            }
        }
    }

    int rows_per_process = size / world_size;
    int **local_C = (int**)malloc(rows_per_process * sizeof(int*));
    for (int i = 0; i < rows_per_process; i++) {
        local_C[i] = (int*)malloc(size * sizeof(int));
    }

    // Scatter A and B across processes
    MPI_Bcast(&B[0][0], size*size, MPI_INT, 0, MPI_COMM_WORLD);  // Send matrix B to all nodes
    for (int i = 0; i < size; i++) {
        MPI_Scatter(&A[i][0], rows_per_process*size, MPI_INT, &local_C[i][0], rows_per_process*size, MPI_INT, 0, MPI_COMM_WORLD);
    }

    matrixMultiply(A, B, local_C, world_rank * rows_per_process, (world_rank + 1) * rows_per_process, size);

    // Gather result in matrix C
    for (int i = 0; i < size; i++) {
        MPI_Gather(&local_C[i][0], rows_per_process * size, MPI_INT, &C[i][0], rows_per_process * size, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    if (world_rank == 0) {
        // Master node prints result (or verifies correctness)
        printf("Multiplication complete using MPI and OpenMP.\n");
    }

    return 0;
}
