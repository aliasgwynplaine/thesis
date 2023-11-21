#include "f4.h"

void pc_print(pc_t * pc) {
    u8 n = pc->f[0]->nvar;
    printf("( [ ");
    
    for (int i = 0; i < n; i++) printf("%ld ", pc->lcm[i]);

    printf("], ");
    printf("[ ");
    
    for (int i = 0; i < n; i++) printf("%ld ", pc->t[0][i]);

    printf("], ");
    llpol_print(pc->f[0]);
    printf(", ");
    printf("[ ");
    
    for (int i = 0; i < n; i++) printf("%ld ", pc->t[1][i]);

    printf("], ");
    llpol_print(pc->f[1]);
}

void pc_free(pc_t * pc) {
    free(pc->t[0]);
    free(pc->t[1]);
    free(pc->lcm);
    free(pc);
}

pc_t * llpol2pairecritique(llpol_t * f1, llpol_t * f2) {
    u8 n = f1->nvar;
    struct paire_critique_t * pc = malloc(sizeof(*pc));
    pc->lcm  = d_exp_lcm(f1->first->exp, f2->first->exp, n);
    pc->f[0] = f1;
    pc->f[1] = f2;

    for (int i = 0; i < 2; i++) {
        pc->t[i] = malloc(n * sizeof(*pc->t[i]));
        memcpy(pc->t[i], f1->first->exp, n * sizeof(*pc->t[i]));
    }

    return pc;
}

rbtree_t * f4(rbtree_t * F, enum MONOMIAL_ORDER mo) {
    if (F == NULL) return NULL;
    if (F->sz == 0) return NULL;
    
    rbtree_t * G = rbtree_cpy(F, NULL, NULL, NULL);
    
}
/*

void csr_rref(csr_t * m, dctx_t * ctx) {
    cuda_rref(m, ctx);
}

csr_t * aapol_list2csr(aapol_t ** loa, int sz) {
    return NULL;
}

aapol_t ** compute_groebner_f4(aapol_t ** loa, int sz) {

    csr_t * mat = aapol_list2csr(loa, sz);
    tmat_t * tmat = csr_decompose(mat);
    


   


    return NULL; // return groebner basis as a list of pols
}

*/