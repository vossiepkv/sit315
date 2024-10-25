#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); // Initialize MPI environment

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Get the number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Get the rank of the process

    const int MASTER = 0;
    char message[20] = "Hello World!";
    
    // Using MPI_Send and MPI_Recv
    if (world_rank == MASTER) {
        for (int i = 1; i < world_size; i++) {
            MPI_Send(message, sizeof(message), MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(message, sizeof(message), MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received message: %s\n", world_rank, message);
    }

    // Using MPI_Bcast
    if (world_rank == MASTER) {
        printf("Broadcasting message: %s\n", message);
    }
    MPI_Bcast(message, sizeof(message), MPI_CHAR, MASTER, MPI_COMM_WORLD);
    printf("Process %d received broadcasted message: %s\n", world_rank, message);

    MPI_Finalize(); // Finalize the MPI environment
    return 0;
}
