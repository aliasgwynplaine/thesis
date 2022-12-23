#ifndef _F4_H
#define _F4_H

#include "pol.h"
#include "matrix.h"
#include "cuda_sparse_gauss_elim.h"

void       csr_rref(csr_t * , dctx_t *);
csr_t *    aapol_list2csr(aapol_t **, int sz);
aapol_t ** compute_groebner_f4(aapol_t **, int sz);


#endif