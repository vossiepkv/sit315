#include <iostream>
#include <vector>
#include <omp.h>

int main() {
    const int size = 1000000;  // Size of the vectors
    std::vector<int> A(size, 1);  // Vector A filled with 1s
    std::vector<int> B(size, 2);  // Vector B filled with 2s
    std::vector<int> C(size, 0);  // Resultant vector C initialized to 0
    int total = 0;  // Variable to store the total sum

    // Activity 1: Parallel Vector Addition using OpenMP
    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 0; i < size; ++i) {
            C[i] = A[i] + B[i];
        }
    }
    std::cout << "Activity 1: Vector addition completed using OpenMP." << std::endl;

    // Activity 2.1: Parallel Vector Addition with default(none)
    #pragma omp parallel default(none) shared(A, B, C, size)
    {
        #pragma omp for
        for (int i = 0; i < size; ++i) {
            C[i] = A[i] + B[i];
        }
    }
    std::cout << "Activity 2.1: Vector addition with default(none) in OpenMP completed." << std::endl;

    // Activity 2.2: Compute total sum using atomic directive
    #pragma omp parallel shared(A, B, C, size, total)
    {
        #pragma omp for
        for (int i = 0; i < size; ++i) {
            #pragma omp atomic
            total += C[i];
        }
    }
    std::cout << "Activity 2.2: Total sum using atomic directive: " << total << std::endl;

    // Reset total for next activity
    total = 0;

    // Activity 2.3: Compute total sum using reduction clause
    #pragma omp parallel shared(A, B, C, size) reduction(+:total)
    {
        #pragma omp for
        for (int i = 0; i < size; ++i) {
            C[i] = A[i] + B[i];
            total += C[i];
        }
    }
    std::cout << "Activity 2.3: Total sum using reduction clause: " << total << std::endl;

    // Reset total for next activity
    total = 0;

    // Activity 2.4: Compute total sum using private variable and critical section
    #pragma omp parallel shared(A, B, C, size, total)
    {
        int partial_sum = 0;  // Private variable for each thread

        #pragma omp for
        for (int i = 0; i < size; ++i) {
            C[i] = A[i] + B[i];
            partial_sum += C[i];
        }

        #pragma omp critical
        {
            total += partial_sum;  // Update total sum in a critical section
        }
    }
    std::cout << "Activity 2.4: Total sum using critical section: " << total << std::endl;

    // Activity 2.5: Example with different OpenMP scheduling techniques
    #pragma omp parallel shared(A, B, C, size, total)
    {
        #pragma omp for schedule(dynamic, 1000)
        for (int i = 0; i < size; ++i) {
            C[i] = A[i] + B[i];
        }
    }
    std::cout << "Activity 2.5: Vector addition completed with dynamic scheduling." << std::endl;

    return 0;
}
