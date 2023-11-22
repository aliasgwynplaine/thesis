#include "f4.h"

int paire_critique_cmp(pc_t * a, pc_t * b, void * param) { // todo: improve
    u8 n = a->f[0]->n;
    int cmp = d_exp_cmp(a->lcm, b->lcm, n, grevlex);

    if (cmp != 0) return cmp;

    cmp = llpol_hard_cmp(a->f[0], b->f[0]);

    if (cmp != 0) return cmp;

    cmp = llpol_hard_cmp(a->f[1], b->f[1]);

    if (cmp != 0) return cmp;

    return cmp;
}

void pc_print(pc_t * pc) {
    u8 n = pc->f[0]->n;
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
    printf(" )");
}

void pc_free(pc_t * pc) {
    free(pc->t[0]);
    free(pc->t[1]);
    free(pc->lcm);
    free(pc);
}

pc_t * llpol2pairecritique(llpol_t * f1, llpol_t * f2) {
    u8 n = f1->n;
    struct paire_critique_t * pc = malloc(sizeof(*pc));
    pc->lcm  = d_exp_lcm(f1->first->exp, f2->first->exp, n);
    pc->f[0] = f1;
    pc->f[1] = f2;

    for (int i = 0; i < 2; i++) {
        pc->t[i] = malloc(n * sizeof(*pc->t[i]));
        memcpy(pc->t[i], pc->f[i]->first->exp, n * sizeof(*pc->t[i]));
    }

    return pc;
}

rbtree_t * f4(rbtree_t * F, enum MONOMIAL_ORDER mo) {
    if (F == NULL) return NULL;
    if (F->sz == 0) return NULL;
    
    rbtree_t * G = rbtree_cpy(F, NULL, NULL, NULL);
    rbtree_t * P = rbtree_create(paire_critique_cmp, NULL, NULL);
    rbt_trav_t t1, t2;
    llpol_t * f1, * f2;

    for (f1 = rbtree_trav_first(&t1, G); f1 != NULL; f1 = rbtree_trav_next(&t1)) {
        rbtree_trav_cpy(&t2, &t1);
        
        for (f2 = rbtree_trav_next(&t2); f2 != NULL; f2 = rbtree_trav_next(&t2)) {
            pc_t * pc = llpol2pairecritique(f1, f2);
            rbtree_probe(P, pc);
        }
    }





    return NULL;
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