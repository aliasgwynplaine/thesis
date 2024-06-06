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
    u64       deg;
    llpol_t * f[2]; // f_0 y f_1
    u64     * t[2]; // t_0 y t_1
};

struct n_mo_t {
    u8                   n;
    enum MONOMIAL_ORDER mo;
};

int paire_critique_cmp(pc_t * a, pc_t * b, enum MONOMIAL_ORDER mo);
int paire_critique_cmp_wrap(const void * a, const void * b, void * param);
int d_exp_cmp_wrap(const void * a, const void *b, void * param);

void pc_print(pc_t *);
void pc_free(pc_t *);


pc_t * llpol2pairecritique(llpol_t * f1, llpol_t * f2, enum MONOMIAL_ORDER);


void f4(rbtree_t * F, rbtree_t * out, enum MONOMIAL_ORDER mo);
/*
void       csr_rref(csr_t * , dctx_t *);
csr_t   *  aapol_list2csr(aapol_t **, int sz);
aapol_t ** compute_groebner_f4(aapol_t **, int sz);
*/

#endif