#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000 // Adjust the size of the matrix as needed

void initialize_matrix(int matrix[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
}

void multiply_matrices(int A[N][N], int B[N][N], int C[N][N]) {
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
    int A[N][N], B[N][N], C[N][N];
    
    srand(time(0));

    initialize_matrix(A);
    initialize_matrix(B);

    clock_t start = clock();
    multiply_matrices(A, B, C);
    clock_t end = clock();

    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Execution Time: %f seconds\n", time_taken);

    write_matrix_to_file(C, "output_sequential.txt");

    return 0;
}
