#include "f4.h"

void csr_rref(csr_t * m, dctx_t * ctx) {
    cuda_rref(m, ctx);
}

csr_t * aapol_list2csr(aapol_t ** loa, int sz) {
    return NULL;
}

aapol_t ** compute_groebner_f4(aapol_t ** loa, int sz) {
    /* some stuff here */

    csr_t * mat = aapol_list2csr(loa, sz);
    dctx_t * ctx = csr_analyse(mat);
    csr_rref(mat, ctx);

    /* process matrix to extract gb information */


    return NULL; // return groebner basis as a list of pols
}