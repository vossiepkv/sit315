#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define N 1000

int A[N][N], B[N][N], C[N][N];

void initialize_matrix(int matrix[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
}

void multiply_matrices() {
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void write_matrix_to_file(int matrix[N][N], const char* filename) {
    FILE *f = fopen(filename, "w");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(f, "%d ", matrix[i][j]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

int main() {
    srand(time(0));

    initialize_matrix(A);
    initialize_matrix(B);

    double start = omp_get_wtime();
    multiply_matrices();
    double end = omp_get_wtime();

    printf("Execution Time: %f seconds\n", end - start);

    write_matrix_to_file(C, "output_openmp.txt");

    return 0;
}
