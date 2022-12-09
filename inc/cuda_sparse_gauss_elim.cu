#include <stdio.h>
#include <cuda_runtime.h>
#include "matrix.h"
#include "pol.h"

__global__ void sparse_trsm(stbm_t * a, srbm_t * b) {

}


__global__ void sparse_gauss_elim(hrbm_t * d) {

}


void cuda_sparse_trsm(stbm_t * a, srbm_t * b) {
    /*
        prepare blocks and threads
        call the fux...
    */

}

void cuda_sparse_gauss_elim(hrbm_t * d) {
    /*
        prepare blocks and threads
        call the fux...
    */

}
