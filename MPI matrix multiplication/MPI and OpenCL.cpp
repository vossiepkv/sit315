#include <mpi.h>
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1024 // Matrix size

// Error handling function for OpenCL
void checkError(cl_int error, const char* operation) {
    if (error != CL_SUCCESS) {
        fprintf(stderr, "Error during operation '%s': %d\n", operation, error);
        exit(1);
    }
}

// Function to initialize OpenCL context and command queue
cl_context createContext() {
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    
    clGetPlatformIDs(1, &platform, NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    
    context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    return context;
}

cl_command_queue createCommandQueue(cl_context context) {
    cl_device_id device;
    clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(device), &device, NULL);
    return clCreateCommandQueue(context, device, 0, NULL);
}

cl_program createProgram(cl_context context, const char* fileName) {
    FILE* file = fopen(fileName, "r");
    if (!file) {
        perror("Failed to load kernel.");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    size_t source_size = ftell(file);
    rewind(file);

    char* source_str = (char*)malloc(source_size + 1);
    source_str[source_size] = '\0';
    fread(source_str, sizeof(char), source_size, file);
    fclose(file);

    cl_int error;
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source_str, &source_size, &error);
    checkError(error, "clCreateProgramWithSource");

    error = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (error != CL_SUCCESS) {
        char build_log[16384];
        clGetProgramBuildInfo(program, NULL, CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, NULL);
        fprintf(stderr, "Error in kernel: \n%s\n", build_log);
        checkError(error, "clBuildProgram");
    }

    free(source_str);
    return program;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int size = N;  // Assuming square matrix
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
    int *local_A = (int*)malloc(rows_per_process * size * sizeof(int));
    int *local_C = (int*)malloc(rows_per_process * size * sizeof(int));

    // Scatter A and broadcast B
    MPI_Scatter(A[0], rows_per_process * size, MPI_INT, local_A, rows_per_process * size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B[0], size * size, MPI_INT, 0, MPI_COMM_WORLD);

    // Initialize OpenCL context, queue, and program
    cl_context context = createContext();
    cl_command_queue queue = createCommandQueue(context);
    cl_program program = createProgram(context, "kernel.cl");
    cl_kernel kernel = clCreateKernel(program, "matrixMultiply", NULL);

    // Buffers for OpenCL
    cl_int error;
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, rows_per_process * size * sizeof(int), local_A, &error);
    checkError(error, "clCreateBuffer A");
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size * size * sizeof(int), B[0], &error);
    checkError(error, "clCreateBuffer B");
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, rows_per_process * size * sizeof(int), NULL, &error);
    checkError(error, "clCreateBuffer C");

    // Set kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);
    clSetKernelArg(kernel, 3, sizeof(int), &size);

    // Execute the kernel
    size_t global[2] = { rows_per_process, size };
    error = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, NULL, 0, NULL, NULL);
    checkError(error, "clEnqueueNDRangeKernel");

    // Read the result back to host
    clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, rows_per_process * size * sizeof(int), local_C, 0, NULL, NULL);

    // Gather results
    MPI_Gather(local_C, rows_per_process * size, MPI_INT, C[0], rows_per_process * size, MPI_INT, 0, MPI_COMM_WORLD);

    // Cleanup
    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    MPI_Finalize();

    if (world_rank == 0) {
        printf("Multiplication complete using MPI and OpenCL.\n");
    }

    return 0;
}
