#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#define PRINT 1

int SZ = 8;
int *v;

// ToDo: Add comment (what is the purpose of this variable)
// bufV is a memory buffer object that will be used to store the vector (v) in the device's memory (GPU/CPU).
cl_mem bufV;

// ToDo: Add comment (what is the purpose of this variable)
// device_id represents the device (GPU or CPU) that will be used to execute the OpenCL kernel.
cl_device_id device_id;

// ToDo: Add comment (what is the purpose of this variable)
// context is the environment where the OpenCL objects (kernels, programs, memory objects) are created and managed.
cl_context context;

// ToDo: Add comment (what is the purpose of this variable)
// program holds the OpenCL program (compiled code) that contains the kernel(s) to be executed on the device.
cl_program program;

// ToDo: Add comment (what is the purpose of this variable)
// kernel represents the specific OpenCL kernel (function) that will run on the device.
cl_kernel kernel;

// ToDo: Add comment (what is the purpose of this variable)
// queue is the command queue where commands (like executing kernels, reading memory) are placed for the device to process.
cl_command_queue queue;
cl_event event = NULL;

int err;

// ToDo: Add comment (what is the purpose of this function)
// create_device finds and returns an available OpenCL device, such as a GPU or CPU.
cl_device_id create_device();

// ToDo: Add comment (what is the purpose of this function)
// setup_openCL_device_context_queue_kernel initializes the OpenCL environment, including device, context, command queue, and kernel setup.
void setup_openCL_device_context_queue_kernel(char *filename, char *kernelname);

// ToDo: Add comment (what is the purpose of this function)
// build_program reads, compiles, and builds an OpenCL program from the specified source file for the given context and device.
cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename);

// ToDo: Add comment (what is the purpose of this function)
// setup_kernel_memory allocates memory on the device for the required data (like vectors) that will be used by the kernel.
void setup_kernel_memory();

// ToDo: Add comment (what is the purpose of this function)
// copy_kernel_args sets the kernel's arguments (e.g., data size and memory buffers) that the kernel will use during execution.
void copy_kernel_args();

// ToDo: Add comment (what is the purpose of this function)
// free_memory releases the allocated OpenCL resources (buffers, kernel, queue, etc.) and cleans up memory.
void free_memory();

void init(int *&A, int size);
void print(int *A, int size);

int main(int argc, char **argv)
{
    if (argc > 1)
        SZ = atoi(argv[1]);

    init(v, SZ);

    // ToDo: Add comment (what is the purpose of this variable)
    // global represents the total number of work-items (threads) that will execute the kernel in parallel.
    size_t global[1] = {(size_t)SZ};

    // Initial vector
    print(v, SZ);

    setup_openCL_device_context_queue_kernel((char *)"./vector_ops.cl", (char *)"square_magnitude");

    setup_kernel_memory();
    copy_kernel_args();

    // ToDo: Add comment (what is the purpose of this function? What are its arguments?)
    // clEnqueueNDRangeKernel schedules the kernel for execution on the device. 
    // Arguments: queue (command queue), kernel, 1 (work dimensions), NULL (global work offset), global (number of work-items), NULL (local work size), etc.
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global, NULL, 0, NULL, &event);
    clWaitForEvents(1, &event);

    // ToDo: Add comment (what is the purpose of this function? What are its arguments?)
    // clEnqueueReadBuffer reads data from the device's buffer (bufV) back into the host memory (v).
    // Arguments: queue (command queue), bufV (device buffer), CL_TRUE (blocking read), offset (0), size, destination (v), etc.
    clEnqueueReadBuffer(queue, bufV, CL_TRUE, 0, SZ * sizeof(int), &v[0], 0, NULL, NULL);

    // Result vector
    print(v, SZ);

    // Free memory for device, kernel, queue, etc.
    free_memory();
}

void init(int *&A, int size)
{
    A = (int *)malloc(sizeof(int) * size);

    for (long i = 0; i < size; i++)
    {
        A[i] = rand() % 100; // Generate random numbers less than 100
    }
}

void print(int *A, int size)
{
    if (PRINT == 0)
    {
        return;
    }

    if (PRINT == 1 && size > 15)
    {
        for (long i = 0; i < 5; i++)
        {                        
            printf("%d ", A[i]); // Print first 5 elements
        }
        printf(" ..... ");
        for (long i = size - 5; i < size; i++)
        {                        
            printf("%d ", A[i]); // Print last 5 elements
        }
    }
    else
    {
        for (long i = 0; i < size; i++)
        {                        
            printf("%d ", A[i]); // Print all elements if small size
        }
    }
    printf("\n----------------------------\n");
}

void free_memory()
{
    // Free the buffers
    clReleaseMemObject(bufV);

    // Free OpenCL objects
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    free(v);
}

void copy_kernel_args()
{
    // ToDo: Add comment (what is the purpose of clSetKernelArg function? What are its arguments?)
    // clSetKernelArg sets the kernel's arguments. Here we set argument 0 to SZ (size of the vector) and argument 1 to bufV (the vector buffer).
    clSetKernelArg(kernel, 0, sizeof(int), (void *)&SZ);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bufV);

    if (err < 0)
    {
        perror("Couldn't create a kernel argument");
        printf("error = %d", err);
        exit(1);
    }
}

void setup_kernel_memory()
{
    // ToDo: Add comment (what is the purpose of clCreateBuffer function? What are its arguments?)
    // clCreateBuffer allocates memory on the device. CL_MEM_READ_WRITE allows reading and writing to this memory by both the device and host.
    bufV = clCreateBuffer(context, CL_MEM_READ_WRITE, SZ * sizeof(int), NULL, NULL);

    // Copy vector to the GPU
    clEnqueueWriteBuffer(queue, bufV, CL_TRUE, 0, SZ * sizeof(int), &v[0], 0, NULL, NULL);
}

void setup_openCL_device_context_queue_kernel(char *filename, char *kernelname)
{
    device_id = create_device();
    cl_int err;

    // ToDo: Add comment (what is the purpose of clCreateContext function?)
    // clCreateContext creates a context for the device, which is used to manage resources like kernels, memory buffers, etc.
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    if (err < 0)
    {
        perror("Couldn't create a context");
        exit(1);
    }

    program = build_program(context, device_id, filename);

    // ToDo: Add comment (what is the purpose of clCreateCommandQueueWithProperties function?)
    // clCreateCommandQueueWithProperties creates a command queue for the device. The queue is used to submit commands (like kernel executions and memory operations) for execution.
    queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    if (err < 0)
    {
        perror("Couldn't create a command queue");
        exit(1);
    };

    kernel = clCreateKernel(program, kernelname, &err);
    if (err < 0)
    {
        perror("Couldn't create a kernel");
        printf("error = %d", err);
        exit(1);
    };
}

cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename)
{

    cl_program program;
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;

    /* Read program file and place content into buffer */
    program_handle = fopen(filename, "r");
    if (program_handle == NULL)
    {
        perror("Couldn't find the program file");
        exit(1);
    }
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char *)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle;

    // ToDo: Add comment (what is the purpose of clCreateProgramWithSource function? What are its arguments?)
    // clCreateProgramWithSource creates a program from the source code (in program_buffer). ctx is the context and dev is
