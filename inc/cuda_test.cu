#include "cuda_test.cuh"


__global__ void hello_test() {
    int bid = blockIdx.x;
    int tid = threadIdx.x;

    printf("[[%d], %d]: Hello, friend, from GPU!\n", bid, tid);
}

extern "C" {
    int hello_test_wrapper() {
        debug("Calling kernel hello_test from the hello_test_wrapper fux...");
        hello_test<<<2,1>>>();
        cudaDeviceSynchronize();
        return 0;
    }
}
