// ToDo: Add Comment (what is the purpose of this function? Where is it going to get executed?)
// This kernel function 'square_magnitude' computes the square of each element in the vector 'v'. 
// It will be executed in parallel on the device (GPU or CPU), with each processing element (PE) handling a different part of the vector.

__kernel void square_magnitude(const int size, __global int* v) {

    // Thread identifiers
    // get_global_id(0) returns the unique ID of the work-item (thread) in the 1st dimension.
    // This ID will be used to access different elements of the vector 'v' in parallel.
    const int globalIndex = get_global_id(0);   

    // Uncomment to see the index each PE works on
    // Each processing element will print the index it is working on.
    //printf("Kernel process index :(%d)\n ", globalIndex);

    // Each processing element (PE) squares the element of the vector at its corresponding global index.
    v[globalIndex] = v[globalIndex] * v[globalIndex];
}
