#include "f4.h"

int paire_critique_cmp(pc_t * a, pc_t * b, enum MONOMIAL_ORDER mo) {
    u8 n = a->f[0]->n;
    int cmp = d_exp_cmp(a->lcm, b->lcm, n, mo);

    if (cmp != 0) return cmp;

    cmp = llpol_hard_cmp(a->f[0], b->f[0]);

    if (cmp != 0) return -1;

    cmp = llpol_hard_cmp(a->f[1], b->f[1]);

    if (cmp != 0) return 1;

    return cmp; // same paire
}


int paire_critique_cmp_wrap(const void * a, const void * b, void * param) {
    int cmp = paire_critique_cmp(
        (pc_t *)a, (pc_t *)b, 
        *((enum MONOMIAL_ORDER*)param)
    );

    return cmp;
}

void * llpol_cpy_wrapp(void * llpol, void * param) {
    enum MONOMIAL_ORDER mo = *((enum MONOMIAL_ORDER*)param);
    llpol_t * cpy = llpol_create(0);
    llpol_cpy(cpy, llpol, mo);

    return cpy;
}

void llpol_free_wrap(void * pol, void * param) {
    llpol_free(pol);
}

void pc_free_wrap(void * pc, void * param) {
    pc_free(pc);
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

void free_wrap(void * ptr, void * param) {
    free(ptr);
}

pc_t * llpol2pairecritique(llpol_t * f1, llpol_t * f2, enum MONOMIAL_ORDER mo) {
    u8 n = f1->n;
    struct paire_critique_t * pc = malloc(sizeof(*pc));
    pc->lcm  = d_exp_lcm(f1->first->exp, f2->first->exp, n);

    if (llpol_monomial_cmp(f1, f2, mo) < 0) {
        pc->f[0] = f2; // only a reference
        pc->f[1] = f1; // only a reference
    } else {
        pc->f[0] = f1; // only a reference
        pc->f[1] = f2; // only a reference
    }

    pc->deg  = 0;

    for (int i = 0; i < pc->f[0]->n; i++ ) pc->deg += pc->lcm[i];

    for (int i = 0; i < 2; i++) {
        pc->t[i] = malloc(n * sizeof(*pc->t[i]));
        
        for (int j = 0; j < n; j++) 
            pc->t[i][j] = pc->lcm[j] - pc->f[i]->first->exp[j];
    }

    return pc;
}


rbtree_t * compute_paires_critiques(rbtree_t * G, enum MONOMIAL_ORDER * mo, u64 * min_d) {
    rbtree_t * P = rbtree_create(paire_critique_cmp_wrap, mo, NULL);
    rbt_trav_t t, u;
    llpol_t * f1, * f2;
    *min_d = 0;

    for (f1 = rbtree_trav_first(&t, G); f1 != NULL; f1 = rbtree_trav_next(&t)) {
        rbtree_trav_cpy(&u, &t);
        
        for (f2 = rbtree_trav_next(&u); f2 != NULL; f2 = rbtree_trav_next(&u)) {
            pc_t * pc = llpol2pairecritique(f1, f2, *mo);
            if (*min_d == 0) *min_d = pc->deg;
            *min_d = __min(*min_d, pc->deg);
            rbtree_probe(P, pc);
        }
    }

    return P;
}


void f4(rbtree_t * F, rbtree_t * out, enum MONOMIAL_ORDER mo) {
    if (F == NULL) return;
    if (F->sz == 0) return;
    
    struct n_mo_t param = {((llpol_t *)(F->root->d))->n, mo};
    rbtree_t * G = rbtree_cpy(F, llpol_cpy_wrapp, NULL, NULL); // hard copy
    //rbtree_t * P = rbtree_create(paire_critique_cmp_wrap, &mo, NULL);
    rbt_trav_t t, u;
    llpol_t * f1, * f2;
    u64 min_d = 0;

    /* compute paires critiques (1.2) */
    rbtree_t * P = compute_paires_critiques(G, &param.mo, &min_d);
    
    while (P->sz != 0) {
        /* Selection (1.5) */
        pc_t * p;
        int sz = P->sz;
        pc_t ** Pd = malloc((sz) * sizeof(*Pd));
        int h = 0;

        for (p = rbtree_trav_first(&t, P); p != NULL; p = rbtree_trav_next(&t)) {
            pc_print(p);    
            printf("\n");
            if (min_d == p->deg) { // criterion
                if (sz == h) {
                    sz = sz << 1;
                    printf("sz: %d\n", sz);
                    Pd = realloc(Pd, sz * sizeof(*Pd));
                    CHECKPTR(Pd);
                }

                Pd[h++] = p;
            }
        }

        // symbolic preprocessing
        rbtree_t * M = rbtree_create(pol_monomial_cmp_wrap, F->param, NULL); // F
        rbtree_t * T = rbtree_create(d_exp_cmp_wrap, &param, NULL);  // T(M)
        rbtree_t * D = rbtree_create(d_exp_cmp_wrap, &param, NULL); // Terms done

        printf("SElectec: \n");

        for (int i = 0; i < h; i++) { // (3.1)
            pc_print(Pd[i]);
            printf("\n");

            for (int j = 0; j < 2; j++) {
                llpol_t * pol    = Pd[i]->f[j];
                llpol_t * newpol = llpol_create(pol->n);
                lpol_t  * it;
                
                for (it = pol->first; it != NULL; it = it->nxt) {
                    u64 * exp = d_exp_add(it->exp, Pd[i]->t[j], pol->n);
                    llpol_addterm(newpol, it->coef, exp, *(enum MONOMIAL_ORDER *)M->param);
                    if (exp != * rbtree_probe(T, exp)) free(exp);
                }
                
                llpol_print(newpol);printf("!!!!\n");
                u64 * ht = malloc(newpol->n * sizeof(*ht));
                memcpy(ht, newpol->first->exp, newpol->n * sizeof(*ht));
                if(ht != * rbtree_probe(D, ht)) free(ht);
                if (newpol != *rbtree_probe(M, newpol)) llpol_free(newpol);
            }

            p = rbtree_delete(P, Pd[i]);
            
            if (p != NULL) pc_free(p);
        }

        llpol_t * i;
        u64 * e;

        //rbtree_t * diff; // T - D
        //diff = rbtree_cpy(T, NULL, NULL, NULL); // soft copy .... better use a 
        u64 ** diff = malloc(T->sz * sizeof(*diff)); // T - D
        int hh = 0;
        int ssz = T->sz;
        
        for (e = rbtree_trav_first(&t, T); e != NULL; e = rbtree_trav_next(&t)) {
            if (rbtree_find(D, e) == NULL) diff[hh++] = e; 
        }

        while (hh > 0) { // (3.3)
            llpol_t * mf;
            u64 * m = diff[--hh];
            u64 * mt = malloc(param.n * sizeof(*mt));
            memcpy(mt, m, param.n * sizeof(*mt));
            
            if (mt != * rbtree_probe(D, mt)) free(mt); // (3.5)

            for (i = rbtree_trav_first(&u, G); i != NULL; i = rbtree_trav_next(&u)) {
                u64 * c = malloc(param.n * sizeof(*c));

                if (d_exp_div(m, i->first->exp, c, i->n)) {
                    mf = llpol_create(i->n);

                    for (lpol_t * it = i->first; it != NULL; it = it->nxt) {
                        u64 * exp = d_exp_add(it->exp, c, i->n);
                        llpol_addterm(mf, it->coef, exp, param.mo);

                        if (exp != * rbtree_probe(T, exp)) free(exp);
                        else {
                            if (hh == ssz) {
                                ssz = ssz << 1;
                                diff = realloc(diff, ssz * sizeof(*diff));
                                CHECKPTR(diff);
                            }
                            diff[hh++] = exp;
                        }
                    }

                    rbtree_probe(M, mf); // (3.8)
                }
                
                free(c);
            }

            // debug
            printf("diff: ");
            for (int k = 0; k < hh; k++) {
                printf("[ ");
                for (int z = 0; z < param.n; z++) 
                    printf("%ld ", diff[k][z]);
                printf("], ");
            }
            printf("\n");
        }

        rbtree_t * HT = rbtree_create(d_exp_cmp_wrap, &param, NULL);

        for (i = rbtree_trav_first(&t, M); i != NULL; i = rbtree_trav_next(&t)) {
            u64 * hte = malloc(param.n * sizeof(*hte));
            memcpy(hte, i->first->exp, param.n * sizeof(*hte));
            
            if (hte != * rbtree_probe(HT, hte)) free(hte); // hard
        }

        /* debug */


        printf("Pols for matrix:\n");
        for (i = rbtree_trav_first(&t, M); i != NULL; i = rbtree_trav_next(&t)) {
            llpol_print(i);
            printf("\n");
        }


        u64 ** loT = malloc(T->sz * sizeof(*loT)); // list of T
        idx_t h_loT = T->sz - 1;
        printf("T: ");

        for (e = rbtree_trav_first(&t, T); e != NULL; e = rbtree_trav_next(&t)) {
            loT[h_loT--] = e;
            printf("[ ");
        
            for (int k = 0; k < param.n; k++) printf("%ld ", e[k]);

            printf("], ");
        }
        printf("\n");

        printf("D: ");
        for (e = rbtree_trav_first(&t, D); e != NULL; e = rbtree_trav_next(&t)) {
            printf("[ ");
        
            for (int k = 0; k < param.n; k++) printf("%ld ", e[k]);

            printf("], ");
        }
        printf("\n");

        printf("HT: ");
        for (e = rbtree_trav_first(&t, HT); e != NULL; e = rbtree_trav_next(&t)) {
            printf("[ ");
        
            for (int k = 0; k < param.n; k++) printf("%ld ", e[k]);

            printf("], ");
        }
        printf("\n");

        printf("T->sz: %ld\n", T->sz);
        
        // (2.2) information in M
        nsm_t * nsm = malloc(sizeof(*nsm)); // don't forget to free

        CHECKPTR(nsm);
        nsm->nnz = 0;
        nsm->d   = .0;
        nsm->x = malloc(sizeof(*nsm->x) * M->sz);
        CHECKPTR(nsm->x);
        nsm->c = malloc(sizeof(*nsm->c) * M->sz);
        CHECKPTR(nsm->c);
        nsm->w = malloc(sizeof(*nsm->w) * M->sz);
        CHECKPTR(nsm->w);

        COEFTYPE * buffx = malloc(sizeof(*buffx) * T->sz);
        CHECKPTR(buffx);
        u64 * buffc = malloc(sizeof(*buffc) * T->sz);
        CHECKPTR(buffc);

        int j = 0;

        // Set of polynomials to matrix transformation
        for (i = rbtree_trav_first(&t, M); i != NULL; i = rbtree_trav_next(&t)) {
            //llpol_print(i);printf("\n");
            //nsm->x[j] = malloc(sizeof(*nsm->x[j]) * . )
            nsm->w[j] = 0;
            u64 c = 0;

            for (e = rbtree_trav_last(&u, T); e != NULL; e = rbtree_trav_prev(&u)) {
                for (lpol_t * p = i->first; p != NULL; p = p->nxt) {
                    if (d_exp_cmp(e, p->exp, param.n, param.mo) == 0) {
                        printf("%0.0f ", p->coef);
                        buffc[nsm->w[j]]   = c;
                        buffx[nsm->w[j]++] = p->coef;
                        nsm->nnz++;

                        goto label;
                    }
                }
                printf("0 ");
                label:
                c++;
            }
            printf("\n");

            nsm->x[j] = malloc(sizeof(*nsm->x[j]) * nsm->w[j]);
            CHECKPTR(nsm->x[j]);
            nsm->c[j] = malloc(sizeof(*nsm->c[j]) * nsm->w[j]);
            CHECKPTR(nsm->c[j]);
            memcpy(nsm->x[j], buffx, sizeof(*buffx)*nsm->w[j]);
            memcpy(nsm->c[j], buffc, sizeof(*buffc)*nsm->w[j]);
            nsm->d = (float) nsm->nnz / (M->sz * T->sz);
            j++;
        }

        nsm->m = M->sz;
        nsm->n = T->sz;
        nsm_print(nsm);
        nsm_rref(nsm);
        printf("REDUCED MATRIX: \n");
        nsm_print(nsm);
        // extract the polynomials
        for (j = 0; j < nsm->m; j++) {
            for (e = rbtree_trav_first(&t, HT); e != NULL; e = rbtree_trav_next(&t)) {
                printf("[ ");
        
                for (int k = 0; k < param.n; k++) printf("%ld ", loT[nsm->c[j][0]][k]);

                printf("] ? [ ");

                for (int k = 0; k < param.n; k++) printf("%ld ", e[k]);
                
                printf("]\n");
                
                if (d_exp_cmp(e, loT[nsm->c[j][0]], param.n, param.mo) == 0) {
                    goto lbl_break;
                }
            }

            llpol_t * llpol = llpol_create(param.n);

            for (idx_t k = 0; k < nsm->w[j]; k++) {
                llpol_addterm(llpol, nsm->x[j][k], loT[nsm->c[j][k]], param.mo);
            }

            printf("new pol found! ");
            llpol_print(llpol); printf("\n");

            if (rbtree_find(G, llpol) != NULL) {
                printf("pol found in G!\n");
                free(llpol);
                goto lbl_break;
            }

            for (i = rbtree_trav_first(&t, G); i != NULL; i = rbtree_trav_next(&t)) {
                pc_t * pc = llpol2pairecritique(llpol, i, param.mo);
                if (pc != *rbtree_probe(P, pc)) pc_free(pc);
            }

            rbtree_probe(G, llpol);
            
            lbl_break:
        }

        p = rbtree_trav_first(&t, P);
        if (p != NULL){
            min_d = p->deg;
            pc_print(p);printf("\n");
            p = rbtree_trav_next(&t);

            for (/* empty */; p!= NULL; p = rbtree_trav_next(&t)) {
                min_d = __min(min_d, p->deg);
                pc_print(p);printf("\n");
            }
        }

        printf("Pols in G: \n");

        for (i = rbtree_trav_first(&t, G); i != NULL; i = rbtree_trav_next(&t)) {
            llpol_print(i);
            printf("\n");
        }

        rbtree_destroy(M, llpol_free_wrap);
        rbtree_destroy(T, free_wrap);
        rbtree_destroy(D, free_wrap);
        rbtree_destroy(HT, free_wrap);
        free(diff);
        free(Pd);
        nsm_free(nsm);
        free(buffx);
        free(buffc);
        free(loT);
    }

    llpol_t * i;

    for (i = rbtree_trav_first(&t, G); i != NULL; i = rbtree_trav_next(&t)) {
        llpol_t * newpol = llpol_create(i->n);
        llpol_cpy(newpol, i, mo);
        rbtree_probe(out, newpol);
    }

    rbtree_destroy(P, pc_free_wrap);
    rbtree_destroy(G, llpol_free_wrap);
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