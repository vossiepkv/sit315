#include <iostream>
#include <vector>
#include <thread>
#include <chrono>  // For timing

// Function to add elements of two vectors in a given range
void vectorAddition(const std::vector<int>& A, const std::vector<int>& B, std::vector<int>& C, int start, int end) {
    for (int i = start; i < end; ++i) {
        C[i] = A[i] + B[i];
    }
}

int main() {
    const int size = 1000000;  // Size of the vectors

    // Initialize vectors A and B
    std::vector<int> A(size, 1);  // Vector A filled with 1s
    std::vector<int> B(size, 2);  // Vector B filled with 2s
    std::vector<int> C(size, 0);  // Resultant vector C initialized to 0

    int num_threads = 4;  // Number of threads
    std::vector<std::thread> threads;  // Vector to hold thread objects

    // Calculate the partition size for each thread
    int partition_size = size / num_threads;

    auto start_time = std::chrono::high_resolution_clock::now();  // Start timing

    // Create threads to perform vector addition in parallel
    for (int i = 0; i < num_threads; ++i) {
        int start = i * partition_size;
        int end = (i == num_threads - 1) ? size : (i + 1) * partition_size;
        threads.emplace_back(vectorAddition, std::cref(A), std::cref(B), std::ref(C), start, end);
    }

    // Join all threads to ensure they complete execution
    for (auto& th : threads) {
        th.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();  // End timing
    std::chrono::duration<double> duration = end_time - start_time;

    std::cout << "Time taken for parallel vector addition: " << duration.count() << " seconds" << std::endl;

    return 0;
}
