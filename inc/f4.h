#ifndef _F4_H
#define _F4_H

#include "pol.h"
#include "matrix.h"
#include "tree.h"
//#include "cuda_sparse_gauss_elim.h"

typedef struct paire_critique_t pc_t;

/* paire critique */
struct paire_critique_t {
    u64     * lcm;
    llpol_t * f[2]; // f_0 y f_1
    u64     * t[2]; // t_0 y t_1
};

void pc_print(pc_t *);
void pc_free(pc_t *);


pc_t * llpol2pairecritique(llpol_t * f1, llpol_t * f2);


rbtree_t * f4(rbtree_t * F, enum MONOMIAL_ORDER mo);
/*
void       csr_rref(csr_t * , dctx_t *);
csr_t   *  aapol_list2csr(aapol_t **, int sz);
aapol_t ** compute_groebner_f4(aapol_t **, int sz);
*/

#endif