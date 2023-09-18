#ifndef _F4_H
#define _F4_H

#include "pol.h"
#include "matrix.h"
//#include "cuda_sparse_gauss_elim.h"

typedef struct paire_critique_t pair_critique_t;

/* paire critique */
struct paire_critique_t {
    u64     * lcm;
    aapol_t * f[2]; // f_0 y f_1
    u64     * t[2]; // t_0 y t_1
};

pair_critique_t * aapol2paircritique(aapol_t * f1, aapol_t * f2);
pair_critique_t * btpol2paircritique(btpol_t * f1, btpol_t * f2);


rbtree_t * f4(rbtree_t * F, enum MONOMIAL_ORDER mo);

void       csr_rref(csr_t * , dctx_t *);
csr_t   *  aapol_list2csr(aapol_t **, int sz);
aapol_t ** compute_groebner_f4(aapol_t **, int sz);


#endif