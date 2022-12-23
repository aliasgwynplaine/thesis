#include <stdio.h>
#include <cuda_runtime.h>
#include "matrix.h"
#include "pol.h"
#include "cuda_outils.h"

__global__ void sparse_trsm(stbm_t * a, srbm_t * b) {

}


__global__ void sparse_gauss_elim(hrbm_t * d) {

}


void cuda_csr_rref(csr_t * m, dctx_t * ctx) {
    dctx_t * ctx = csr_analyse(m);
    idx_t cc = 0;

    for (idx_t k = 0; cc < ctx->npiv; k++) {
        if (ctx->rpc[k] != -1 && ctx->rpc[k] != k) {
            printf("swaping %d <> %d -> ", k, ctx->rpc[k]);
            csr_swap_col(m, ctx->rpc[k], k);
        }
        cc++;
    }

    /* copy mem from host to device */
    cudaError_t err = cudaSuccess;

    stm_t * b = NULL;
    CUDAMALLOC(b, sizeof(stm_t), err);

    


    /* call fux to operate mat on device */

    /* copy mem from dev to host */


    /* free mem */

    dctx_free(ctx);
    CUDAFREE(b, err);
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
