#ifndef _F4_H
#define _F4_H

#include "pol.h"
#include "matrix.h"
//#include "cuda_sparse_gauss_elim.h"

typedef struct paire_critique_t pair_critique_t;

/* paire critique */
struct paire_critique_t {
    term_t * lcm;
    term_t * f[2]; // f_0 y f_1
    term_t * t[2]; // t_0 y t_1
};


pair_critique_t * aapol2paircritique(aapol_t * f1, aapol_t * f2);
pair_critique_t * llpol2paircritique(llpol_t * f1, llpol_t * f2);



void       csr_rref(csr_t * , dctx_t *);
csr_t   *  aapol_list2csr(aapol_t **, int sz);
aapol_t ** compute_groebner_f4(aapol_t **, int sz);


#endif