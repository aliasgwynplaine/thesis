#include "f4.h"

int paire_critique_cmp(pc_t * a, pc_t * b, enum MONOMIAL_ORDER mo) {
    u8 n = a->f[0]->n;
    int cmp = d_exp_cmp(a->lcm, b->lcm, n, mo);

    if (cmp != 0) return cmp;

    cmp = llpol_hard_cmp(a->f[0], b->f[0]);

    if (cmp != 0) return cmp;

    cmp = llpol_hard_cmp(a->f[1], b->f[1]);

    if (cmp != 0) return cmp;

    return cmp;
}


int paire_critique_cmp_wrap(const void * a, const void * b, void * param) {
    int cmp = paire_critique_cmp(
        (pc_t *)a, (pc_t *)b, 
        *((enum MONOMIAL_ORDER*)param)
    );

    return cmp;
}


int d_exp_cmp_wrap(const void * a, const void *b, void * param) {
    struct n_mo_t * nmo = param;
    int cmp = d_exp_cmp((u64*)a, (u64*) b, nmo->n, nmo->mo);

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
    pc->deg  = 0;
    for (int i = 0; i < pc->f[0]->n; i++ ) pc->deg += pc->lcm[i];

    for (int i = 0; i < 2; i++) {
        pc->t[i] = malloc(n * sizeof(*pc->t[i]));
        
        for (int j = 0; j < n; j++) 
            pc->t[i][j] = pc->lcm[j] - pc->f[i]->first->exp[j];
    }

    return pc;
}

rbtree_t * f4(rbtree_t * F, enum MONOMIAL_ORDER mo) {
    if (F == NULL) return NULL;
    if (F->sz == 0) return NULL;
    
    struct n_mo_t param = {((llpol_t *)(F->root->d))->n, mo};
    rbtree_t * G = rbtree_cpy(F, NULL, NULL, NULL);
    rbtree_t * P = rbtree_create(paire_critique_cmp_wrap, &mo, NULL);
    rbt_trav_t t, u;
    llpol_t * f1, * f2;
    u64 min_d = 0;

    for (f1 = rbtree_trav_first(&t, G); f1 != NULL; f1 = rbtree_trav_next(&t)) {
        rbtree_trav_cpy(&u, &t);
        
        for (f2 = rbtree_trav_next(&u); f2 != NULL; f2 = rbtree_trav_next(&u)) {
            pc_t * pc = llpol2pairecritique(f1, f2);
            if (min_d == 0) min_d = pc->deg;
            min_d = __min(min_d, pc->deg);
            rbtree_probe(P, pc);
        }
    }
    // while (P->sz != 0) {
    /* Selection */
    pc_t * p;
    int sz = P->sz >> 2;
    pc_t ** Pd = malloc((sz) * sizeof(*Pd));
    int h = 0;

    for (p = rbtree_trav_first(&t, P); p != NULL; p = rbtree_trav_next(&t)) {
        pc_print(p);
        printf("\n");
        if (min_d == p->deg) {
            sz = sz << 1;
            if (sz == h) Pd = realloc(Pd, sz * sizeof(*Pd));
            Pd[h++] = p; 
        }
    }

    rbtree_t * M = rbtree_create(pol_monomial_cmp_wrap, F->param, NULL);
    rbtree_t * T = rbtree_create(d_exp_cmp_wrap, &param, NULL);
    rbtree_t * D = rbtree_create(d_exp_cmp_wrap, &param, NULL);
    rbtree_t * diff = rbtree_create(d_exp_cmp_wrap, &param, NULL);
    printf("SElectec: \n");
    
    for (int i = 0; i < h; i++) {
        pc_print(Pd[i]);
        printf("\n");

        for (int j = 0; j < 2; j++) {
            llpol_t * pol    = Pd[i]->f[j];
            llpol_t * newpol = llpol_create(pol->n);
            lpol_t  * it;
            
            for (it = pol->first; it != NULL; it = it->nxt){
                u64 * exp = d_exp_add(it->exp, Pd[i]->t[j], pol->n);
                llpol_addterm(newpol, it->coef, exp, *(enum MONOMIAL_ORDER *)M->param);
                if (exp != * rbtree_probe(T, exp)) free(exp);
                //free(exp);
            }
            
            rbtree_probe(M, newpol);
            u64 * ht = malloc(newpol->n * sizeof(*ht));
            memcpy(ht, newpol->first->exp, newpol->n * sizeof(*ht));
            if(ht != * rbtree_probe(D, ht)) free(ht);
            
        }
    }


    /* here! */

    llpol_t * i;

    printf("Pols for matrix:\n");
    for (i = rbtree_trav_first(&t, M); i != NULL; i = rbtree_trav_next(&t)) {
        llpol_print(i);
        printf("\n");
    }

    u64 * e;

    printf("T: ");

    for (e = rbtree_trav_first(&t, T); e != NULL; e = rbtree_trav_next(&t)) {
        printf("[ ");
    
        for (int k = 0; k < Pd[0]->f[0]->n; k++) printf("%ld ", e[k]);

        printf("], ");
    }
    printf("\n");

    printf("HT: ");
    for (e = rbtree_trav_first(&t, D); e != NULL; e = rbtree_trav_next(&t)) {
        printf("[ ");
    
        for (int k = 0; k < Pd[0]->f[0]->n; k++) printf("%ld ", e[k]);

        printf("], ");
    }
    printf("\n");


    // }

    rbtree_destroy(G, NULL);
    rbtree_destroy(P, pc_free);
    rbtree_destroy(M, llpol_free);
    rbtree_destroy(T, free);
    rbtree_destroy(D, free);
    rbtree_destroy(diff, free);
    free(Pd);

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