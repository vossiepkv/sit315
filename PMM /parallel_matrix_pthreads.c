#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N 1000
#define NUM_THREADS 4

int A[N][N], B[N][N], C[N][N];

void initialize_matrix(int matrix[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
}

void* multiply(void* arg) {
    int thread_id = *(int*)arg;
    int chunk_size = N / NUM_THREADS;

    int start = thread_id * chunk_size;
    int end = (thread_id == NUM_THREADS - 1) ? N : start + chunk_size;

    for (int i = start; i < end; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    pthread_exit(0);
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
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    initialize_matrix(A);
    initialize_matrix(B);

    clock_t start = clock();
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, multiply, &thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_t end = clock();

    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Execution Time: %f seconds\n", time_taken);

    write_matrix_to_file(C, "output_pthreads.txt");

    return 0;
}
