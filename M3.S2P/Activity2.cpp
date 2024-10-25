#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define VECTOR_SIZE 1000

void initialize_vectors(int* v1, int* v2) {
    for (int i = 0; i < VECTOR_SIZE; i++) {
        v1[i] = i;
        v2[i] = i;
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int *v1 = NULL, *v2 = NULL, *v3 = NULL;
    int local_size = VECTOR_SIZE / world_size;
    int *local_v1 = (int*)malloc(local_size * sizeof(int));
    int *local_v2 = (int*)malloc(local_size * sizeof(int));
    int *local_v3 = (int*)malloc(local_size * sizeof(int));

    if (world_rank == 0) {
        v1 = (int*)malloc(VECTOR_SIZE * sizeof(int));
        v2 = (int*)malloc(VECTOR_SIZE * sizeof(int));
        v3 = (int*)malloc(VECTOR_SIZE * sizeof(int));
        initialize_vectors(v1, v2);
    }

    MPI_Scatter(v1, local_size, MPI_INT, local_v1, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(v2, local_size, MPI_INT, local_v2, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < local_size; i++) {
        local_v3[i] = local_v1[i] + local_v2[i];
    }

    MPI_Gather(local_v3, local_size, MPI_INT, v3, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    int total_sum = 0;
    int local_sum = 0;
    for (int i = 0; i < local_size; i++) {
        local_sum += local_v3[i];
    }

    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        printf("Total sum of vector v3 elements: %d\n", total_sum);
    }

    MPI_Finalize();

    free(local_v1);
    free(local_v2);
    free(local_v3);
    if (world_rank == 0) {
        free(v1);
        free(v2);
        free(v3);
    }

    return 0;
}
