#ifndef _CUDA_SPARSE_GAUSS_ELIM_CUH
#define _CUDA_SPARSE_GAUSS_ELIM_CUH

#include "matrix.h"

void cuda_sparse_trsm(stbm_t * a, srbm_t * b);
void cuda_sparse_gauss_elim(hrbm_t * d);


#endif