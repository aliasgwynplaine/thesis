#include "pol.h"
#include <math.h>

typedef struct parser_ctx_t parser_ctx_t;

struct parser_ctx_t {
    u8 nvar;
    const char * pol_str_head;
    const char * pol_str;
    char ** var_lst; // ordered list of vars
    int status;
};

enum parser_ctx_status {
    Ok,
    NameError,
    SyntaxError
};


/**
 * @brief creates an alternate array like pol in n
 * vars. It's necesary to call freeaapol_t at
 * the end. 
 * 
 * @param n number of variables
 * @return aapol_t* pointer with head pointing
 * to NULL, nvars equals n and sz equals to 0
 */
aapol_t * aapol_create(u8 n) {
    if (n > MAX_NUM_O_VARS) SAYNEXITWERROR("Not implemented!");
    aapol_t * aapol = malloc(sizeof(aapol_t));
    CHECKPTR(aapol);
    aapol->nvar  = n;
    aapol->sz    = 0;
    aapol->cap   = 0;
    aapol->terms = NULL;

    return aapol;
}


void minheapify(term_t * terms, int i, int hsz) {
    int l = LEFT(i);
    int r = RIGHT(i);
    int smallest = i;

    if (l <= hsz) { 
        if(terms[l].exp < terms[i].exp)
            smallest = l;
        else {
            if (terms[l].exp == terms[i].exp) {
                terms[l].coef += terms[i].coef;
                terms[i].coef = 0;
                terms[i].exp  = 0;
            }
        }
    }

    if (r <= hsz) {
        if (terms[r].exp < terms[smallest].exp)
            smallest = r;
        else if (terms[r].exp == terms[smallest].exp) {
            terms[r].coef += terms[smallest].coef;
            terms[smallest].coef = 0;
            terms[smallest].exp  = 0;
        }
    }
    if (smallest != i) {
        term_t aux       = terms[i];
        terms[i]        = terms[smallest];
        terms[smallest] = aux;
        minheapify(terms, smallest, hsz);
    }
}


void buildminheap(term_t * terms, int hsz) {
    debug("building heap... %d", hsz);
    for (int i = hsz / 2; i >= 0; i--)
        minheapify(terms, i, hsz);

}


void mergeaapol(aapol_t * a, int p, int q, int r) {
    int n_1, n_2, i, j, k, c;
    term_t * l_;
    term_t * r_;
    n_1 = q - p + 1;
    n_2 = r - q;
    l_ = malloc((n_1 + 1) * sizeof(term_t));
    CHECKPTR(l_);
    r_ = malloc((n_2 + 1) * sizeof(term_t));
    CHECKPTR(r_);

    for (i = 0; i < n_1; i++) l_[i] = a->terms[i + p];

    for (i = 0; i < n_2; i++) r_[i] = a->terms[i + q + 1];

    l_[n_1].exp = -1; r_[n_2].exp = -1;
    i = 0; j = 0; k = p;

    while (k <= r) {
        if (i != n_1 || j != n_2) {
            c = l_[i].exp - r_[j].exp;

            if (c > 0) a->terms[k] = l_[i++];
            else if (c < 0) a->terms[k] = r_[j++];
            else if (c == 0) {
                a->terms[k].coef = l_[i++].coef + r_[j].coef;
                a->terms[k].exp  = r_[j++].exp;
                if (a->terms[k].coef == 0) {continue;}
            }
        } else { 
            a->terms[k].coef = 0;
            a->terms[k].exp = 0;
        }

        k++;
    }

    FREE(l_);
    FREE(r_);
}

void mergesortaapol(aapol_t * aapol, int p, int r) {
    if (p < r) {
        int q = (p + r) / 2;
        mergesortaapol(aapol, p, q);
        mergesortaapol(aapol, q + 1, r);
        mergeaapol(aapol, p, q, r);
    }

}

void aapol_sort(aapol_t * aapol) {
    if (aapol->sz == 0) return;
    debug("preparing pol...");
    int i;
    mergesortaapol(aapol, 0, aapol->sz - 1);
    /* todo: a smarter way to get rid off the remaining 0's */
    i = aapol->sz - 1;
    while (aapol->terms[i].coef == 0 && i >= 0) i--;
    aapol->sz = i + 1;
}


int btpol_quicksort_partition(btpol_t ** lobtpol, int p, int r) {
    int i, cmp;
    btpol_t * x = lobtpol[r];
    btpol_t * aux;
    i = p - 1;

    for (int j = p; j < r; j++) {
        //cmp = exp_cmp(lobtpol[j]->root->exp, x->root->exp, x->nvar);
        cmp = btpol_monomial_cmp(lobtpol[j], x);
        if (cmp <= 0) { // todo: implement other comparition methods
            i++;
            aux = lobtpol[i];
            lobtpol[i] = lobtpol[j];
            lobtpol[j] = aux;
        }
    }

    aux = lobtpol[i + 1];
    lobtpol[i + 1] = lobtpol[r];
    lobtpol[r] = aux;

    return i + 1;
}

void btpol_list_quicksort(btpol_t ** lobtpol, int p, int r) {
    if (p < r) {
        int q = btpol_quicksort_partition(lobtpol, p, r);
        btpol_list_quicksort(lobtpol, p, q - 1);
        btpol_list_quicksort(lobtpol, q + 1, r);
    }
}



void btpol_list_sort(btpol_t ** lobtpol, int sz) {
    btpol_list_quicksort(lobtpol, 0, sz - 1);
}


int aapol_list_quicksort_partition(aapol_t ** loaapol, int p, int r) {
    int i, cmp;
    aapol_t * x = loaapol[r];
    aapol_t * aux;
    i = p - 1;

    for (int j = 0; j < r; j++) {
        cmp = aapol_monomial_cmp(loaapol[j], x);

        if (cmp <= 0) {
            i++;
            aux = loaapol[i];
            loaapol[i] = loaapol[j];
            loaapol[j] = aux;
        }
    }

    SWAP(loaapol[i + 1], loaapol[r], aux);

    return i + 1;
}


void aapol_list_quicksort(aapol_t ** loaapol, int p, int r) {
    if (p > r) {
        int q = aapol_list_quicksort_partition(loaapol, p, r);
        aapol_list_quicksort(loaapol, p, q - 1);
        aapol_list_quicksort(loaapol, q + 1, r);
    }
}


void aapol_list_sort(aapol_t ** loaapol, int sz) {
    aapol_list_quicksort(loaapol, 0, sz - 1);
}


term_t * term_add(term_t * p, COEFTYPE a, term_t * q, COEFTYPE b) {
    term_t * r = term_malloc(sizeof(term_t));
    r->coef = a * p->coef + b * q->coef;
    exp_add(&p->exp, &q->exp, &r->exp);
    
    return r;
}


term_t * aapol_head(aapol_t * aapol) {
    term_t * term = term_malloc(sizeof(term_t));
    term->coef = aapol->terms[0].coef;
    term->exp  = aapol->terms[0].exp;

    return term;
}

/**
 * @brief adds a term to the aapol.
 * Note that is not necesary to store the 
 * returning value because it is, in fact, the
 * same as aapol
 * @param aapol pointer to aapol
 * @param coef  coefficient of the new term
 * @param exp   exponent of the new term
 * @return aapol_t* pointer to the aapol with
 * the new term
 */
aapol_t * aapol_addterm(aapol_t * aapol, COEFTYPE coef, u64 exp) {
    if (aapol == NULL) SAYNEXITWERROR("aapol is null");

    if (coef == 0) {
        return aapol;
    }

    if (!aapol->terms) {
        aapol->terms = malloc(2*sizeof(term_t));
        CHECKPTR(aapol);
        aapol->cap = 2;
    }


    if (aapol->sz + 1 >= aapol->cap) {
        // need to realloc
        // todo: make a smarter use of space.
        debug("trying to alloc %d chunks", aapol->cap<<1);
        aapol->terms = realloc(aapol->terms, (aapol->cap<<1) * sizeof(term_t));
        CHECKPTR(aapol->terms);
        aapol->cap = aapol->cap<<1;
    }

    debug("creating a new term...");
    (aapol->terms + aapol->sz)->coef = coef;
    (aapol->terms + aapol->sz)->exp  = exp;
    int i  = aapol->sz++;
    int pi = PARENT(i);

    for (;;) {
        if (pi >= 0) {
            if (aapol->terms[i].exp == aapol->terms[pi].exp) {
                aapol->terms[pi].coef += aapol->terms[i].coef;
                aapol->terms[i].coef  = 0;
                aapol->terms[i].exp   = 0;
                i  = pi;
                pi = PARENT(i);
            } else if (aapol->terms[pi].exp < aapol->terms[i].exp) {
                term_t aux        = aapol->terms[i];
                aapol->terms[i]  = aapol->terms[pi];
                aapol->terms[pi] = aux;
                i  = pi;
                pi = PARENT(i);
            } else break;
        } else break;
    }

    return aapol;
}


aapol_t * aapol_add(aapol_t * a, COEFTYPE alpha, aapol_t * b, COEFTYPE betha) {
    if (a->nvar != b->nvar) SAYNEXITWERROR("Cannot sum aapols of different vars.");
    aapol_t * r = NULL;

    if (alpha == 0 && betha != 0) {
        aapol_cpy(r, b);
        return r;
    }

    if (betha == 0 && alpha != 0) {
        aapol_cpy(r, a);
        return r;
    }

    r = aapol_create(a->nvar);

    if (alpha == 0 && betha == 0) {
        return r;
    }

    r->cap = a->sz + b->sz;
    r->terms = term_malloc(sizeof(term_t) * r->cap);

    term_t * aterms = a->terms;
    term_t * bterms = b->terms;
    term_t * rterms = r->terms;

    int i = 0; int j = 0; int k = 0;
    int cmp;


    while (i < a->sz && j < b->sz) {
        cmp = exp_cmp(aterms[i].exp, bterms[j].exp, a->nvar, lex);
        if (cmp < 0) {
            rterms[k].coef = betha * bterms[j].coef;
            rterms[k].exp  = bterms[j++].exp;
        } else if (cmp > 0) {
            rterms[k].coef = alpha * aterms[i].coef;
            rterms[k].exp  = aterms[i++].exp;
        } else {
            rterms[k].coef = alpha * aterms[i++].coef + betha * bterms[j++].coef;
            if (rterms[k].coef == 0) continue;
            rterms[k].exp = aterms[i-1].exp;
        }

        k++;
    }

    while (i < a->sz) {
        rterms[k].coef = alpha * aterms[i].coef;
        rterms[k++].exp  = aterms[i++].exp;
    }

    while (j < b->sz) {
        rterms[k].coef = betha * bterms[j].coef;
        rterms[k++].exp  = bterms[j++].exp;
    }

    r->sz = k;

    return r;
}


aapol_t * aapol_coef_multiply(aapol_t * a, COEFTYPE alpha) {
    if (a == NULL) SAYNEXITWERROR("aapol is null");
    
    aapol_t * res = aapol_create(a->nvar);
    res->sz = a->sz;
    res->cap = res->sz;
    res->terms = malloc(sizeof(term_t) * res->sz);

    for (int i = 0; i < a->sz; i++) {
        res->terms[i].coef = alpha * a->terms[i].coef;
        res->terms[i].exp  = a->terms[i].exp;
    }

    return res;
}


aapol_t * aapol_inplace_coef_multiply(aapol_t * a, COEFTYPE alpha) {
    for (int i = 0; i < a->sz; i++) {
        a->terms[i].coef *= alpha;
    }

    return a;
}

#define _DEBUG
aapol_t * aapol_multiply(aapol_t * a, aapol_t * b) {
    if (a->nvar != b->nvar) SAYNEXITWERROR("Cannot multiply pols with different num o vars");

    if (a->sz == 0 || b->sz == 0) return NULL;

    //aapol_sort(a);
    //aapol_sort(b);

    aapol_t * r = aapol_create(a->nvar);
    r->cap = a->sz * b->sz;
    r->terms = malloc(sizeof(term_t) * r->cap);
    term_t * rterms = r->terms;
    term_t * aterms = a->terms;
    term_t * bterms = b->terms;
    int i = 0, k = 0, cmp;
    int s;
    int * f = malloc(sizeof(int) * a->sz);
    CHECKPTR(f);
    u64 * dp = malloc(sizeof(u64) * a->sz);
    CHECKPTR(dp);
    exp_add(&aterms[0].exp, &bterms[0].exp, &rterms[0].exp);
    rterms[0].coef = 0;
    for (int j = 0; j < a->sz; j++) {
        f[j]  = 0;
        // dp[i] = aterms[i].exp + bterms[f[i]].exp;
        exp_add(&aterms[j].exp, &bterms[f[j]].exp, &dp[j]);
    }

    
    while (i < a->sz) {
        s   = u64_max_idx(dp, i, a->sz - 1);
        cmp = exp_cmp(rterms[k].exp, dp[s], a->nvar, lex);

        if (cmp != 0) {
            if (rterms[k].coef != 0) rterms[++k].coef = 0;

            rterms[k].exp = dp[s];
        }

        rterms[k].coef += aterms[s].coef * bterms[f[s]].coef;
        f[s]++;
        exp_add(&aterms[s].exp, &bterms[f[s]].exp, &dp[s]);

        if (f[s] > b->sz - 1) i = s + 1;    
    }

    r->sz = k + 1;
    FREE(f);
    FREE(dp);

    return r;
}
#undef _DEBUG

int aapol_monomial_cmp(aapol_t * a, aapol_t * b) {
    if (a->nvar != b->nvar) SAYNEXITWERROR("Cannot compare polynomials of different number of variables.");

    aapol_sort(a);
    aapol_sort(b);

    term_t * a_term_p = a->terms;
    term_t * b_term_p = b->terms;

    int cmp;

    for (int i = 0; i < a->sz; i++) {
        cmp = exp_cmp(a_term_p[i].exp, b_term_p[i].exp, a->nvar, lex);

        if (cmp != 0) return cmp;
        
    }

    return 0;
}

/**
 * @brief tests if polinomials are strictly equals.
 * Mostly used to test other functions
*/
int aapol_hard_cmp(aapol_t * a, aapol_t * b) {
    if (a == NULL || b == NULL) return 1;
    if (a->nvar != b->nvar) return 1;

    aapol_sort(a);
    aapol_sort(b);

    if (a->sz != b->sz) return 1;

    if (a->sz == 0) return 0;

    term_t * a_term_p = a->terms;
    term_t * b_term_p = b->terms;

    int cmp;

    for (int i = 0; i < a->sz; i++) {
        cmp = exp_cmp(a_term_p[i].exp, b_term_p[i].exp, a->nvar, lex);

        if (cmp != 0 || (a_term_p[i].coef != b_term_p[i].coef)) return 1;
        
    }

    return 0;
}

aapol_t * aapol_cpy(aapol_t * dst, aapol_t * src) {
    if (src == NULL) return NULL;

    dst->sz = src->sz;
    dst->cap = dst->sz;
    dst->terms = malloc(sizeof(term_t) * dst->cap);
    memcpy(dst->terms, src->terms, sizeof(term_t) * src->sz);
    
    return dst;
}

term_t   * aapol_head_lcm(aapol_t * a1, aapol_t * a2, enum MONOMIAL_ORDER mo) {
    if (a1->nvar != a2->nvar) {
        dbgerr("Cannot get lcm from pols of diverent numovars.");
        return NULL;
    }
    term_t * head_lcm = term_malloc(sizeof(*head_lcm));
    term_t * h1 = aapol_head(a1);
    term_t * h2 = aapol_head(a2);
    
    for (int i = 0; i < a1->nvar; i++) {
        /* choose the min */
        /* consider handle exponents in different way...*/
    }

    FREE(h1);
    FREE(h2);

    return head_lcm;
}


int btpol_monomial_cmp(btpol_t * a, btpol_t * b) {
    if (a->nvar != b->nvar) SAYNEXITWERROR("Cannot cmp polynomials of different nvar");

    bpol_t ** stacka = malloc(sizeof(bpol_t *) * a->sz);
    CHECKPTR(stacka);
    bpol_t ** stackb = malloc(sizeof(bpol_t *) * b->sz);
    CHECKPTR(stackb);
    int ha = 0;
    int hb = 0;
    bpol_t * nodea = a->root;
    bpol_t * nodeb = b->root;
    int cmp;

    while(1) {
        while (nodea != NULL || nodeb != NULL) {
            if (nodea != NULL) {
                stacka[ha++] = nodea;
                nodea = nodea->r;
            }

            if (nodeb != NULL) {
                stackb[hb++] = nodeb;
                nodeb = nodeb->r;
            }
        }

        if (ha == 0 || hb == 0) break;

        nodea = stacka[--ha];
        nodeb = stackb[--hb];
        cmp = exp_cmp(nodea->exp, nodeb->exp, a->nvar, lex);

        if (cmp != 0) {
            FREE(stackb);
            FREE(stacka);
            return cmp;
        }

        nodea = nodea->l;
        nodeb = nodeb->l;
    }
    FREE(stackb);
    FREE(stacka);
    return ha - hb;
}

int btpol_hard_cmp(btpol_t * a, btpol_t * b) {
    if (a == NULL || b == NULL) return 1;
    if (a->nvar != b->nvar) return 1;
    if (a->sz != b->sz) return 1;

    bpol_t ** stacka = malloc(sizeof(bpol_t *) * a->sz);
    CHECKPTR(stacka);
    bpol_t ** stackb = malloc(sizeof(bpol_t *) * b->sz);
    CHECKPTR(stackb);
    int ha = 0;
    int hb = 0;
    bpol_t * nodea = a->root;
    bpol_t * nodeb = b->root;
    int cmp;

    while(1) {
        while (nodea != NULL || nodeb != NULL) {
            if (nodea != NULL) {
                stacka[ha++] = nodea;
                nodea = nodea->r;
            }

            if (nodeb != NULL) {
                stackb[hb++] = nodeb;
                nodeb = nodeb->r;
            }
        }

        if (ha == 0 || hb == 0) break;

        nodea = stacka[--ha];
        nodeb = stackb[--hb];
        cmp = exp_cmp(nodea->exp, nodeb->exp, a->nvar, lex);

        if (cmp != 0 || (nodea->coef != nodeb->coef)) {
            FREE(stackb);
            FREE(stacka);
            return 1;
        }

        nodea = nodea->l;
        nodeb = nodeb->l;
    }

    FREE(stackb);
    FREE(stacka);

    return ha - hb;
}


COEFTYPE extractcoef(aapol_t * aapol, u64 exp) {
    return 0;
}

/**
 * @brief works as malloc but sets 
 * first coef and exp to 0
 * 
 * @param sz size of block
 * @return pointer to the beginning
 * of the block
 * 
 * @note you need to call free. In 
 * the case you have decided to alloc
 * more than one term, only the first
 * pointed coef and exp will set to 0
*/
term_t * term_malloc(size_t sz) {
    term_t * term = malloc(sz);
    term->coef = 0;
    term->exp  = 0;

    return term;
}

lpol_t * lpol_malloc(size_t) {
    lpol_t * lpol;
    lpol = malloc(sizeof(*lpol));
    CHECKPTR(lpol);
    lpol->coef = 0;
    lpol->exp  = 0;
    lpol->nxt  = NULL;

    return lpol;
}


void lpol_free(lpol_t * lpol) {
    if (lpol->nxt != NULL) lpol_free(lpol->nxt);
    FREE(lpol);
}

/**
 * @brief works as malloc but sets 
 * first coef and exp to 0 and, l and
 * r to NULL
 * 
 * @param sz size of block
 * @return pointer to the beginning
 * of the block
 * 
 * @note you need to call free. In 
 * the case you have decided to alloc
 * more than one term, only the first
 * pointed coef and exp will set to 0
 * and the first pointed l and r will 
 * be set to NULL
*/
bpol_t * bpol_malloc(size_t sz) {
    bpol_t * lpol = malloc(sz);
    lpol->coef = 0;
    lpol->exp  = 0;
    lpol->l    = NULL;
    lpol->r    = NULL;

    return lpol;
}


llpol_t * llpol_create(u8 n) {
    llpol_t * llpol = malloc(sizeof(*llpol));
    CHECKPTR(llpol);
    llpol->first = NULL;
    llpol->nvar  = n;
    llpol->sz    = 0;

    return llpol;
}

void llpol_free(llpol_t * llpol) {
    if (llpol->first != NULL) lpol_free(llpol->first);
    FREE(llpol);
}


/**
 * @brief creates a tree like pol struct in n
 * vars. It's necesary to call freebtpol_t at
 * the end. 
 * 
 * @param n number of variables
 * @return btpol_t* pointer with head pointing
 * to NULL, nvars equals n and sz equals to 0
 */
btpol_t * btpol_create(u8 n) {
    if (n > MAX_NUM_O_VARS) SAYNEXITWERROR("Not implemented!");
    btpol_t * btpol = malloc(sizeof(btpol_t));
    CHECKPTR(btpol);
    btpol->root = NULL;
    btpol->nvar = n;
    btpol->sz   = 0;

    return btpol;
}


lpol_t  * llpol_head(llpol_t * llpol) {
    return llpol->first;
}


llpol_t * llpol_addterm(llpol_t * llpol, COEFTYPE coef, u64 exp) {
    if (llpol == NULL) SAYNEXITWERROR("llpol is null");

    if (coef == 0) return llpol;
    
    lpol_t ** indirect = &(llpol->first);
    lpol_t *  newterm;

    while ((*indirect) != NULL) {
        int cmp = exp_lex_cmp((*indirect)->exp, exp, llpol->nvar);

        if (cmp < 0) {
            newterm = lpol_malloc(sizeof(*newterm));
            newterm->coef = coef;
            newterm->exp  = exp;
            newterm->nxt  = (*indirect);
            (*indirect)   = newterm;
            llpol->sz++;

            return llpol;
        } else if (cmp > 0) {
            indirect = &(*indirect)->nxt;
        } else {
            (*indirect)->coef += coef;

            return llpol;
        }
    }
    
    *indirect = lpol_malloc(sizeof(*newterm));
    (*indirect)->coef = coef;
    (*indirect)->exp  = exp;
    llpol->sz++;

    return llpol;
}



llpol_t * llpol_add(llpol_t * a, COEFTYPE alpha, llpol_t * b, COEFTYPE betha) {
    if (a->nvar != b->nvar) SAYNEXITWERROR("pols with diferent nvars");

    if (a == NULL || b == NULL) SAYNEXITWERROR("NULL pointer received!");

    if (a->sz == 0 || alpha == 0) 
        return llpol_coef_multiply(b, betha);

    if (b->sz == 0 || betha == 0) 
        return llpol_coef_multiply(a, alpha);

    llpol_t * c = llpol_create(a->nvar);
    lpol_t * pa = a->first;
    lpol_t * pb = b->first;
    lpol_t ** indirect = &c->first;

    while (pa != NULL && pb != NULL) {
        int cmp = exp_lex_cmp(pa->exp, pb->exp, a->nvar);
        *indirect = lpol_malloc(sizeof(**indirect));

        if (cmp < 0) {
            (*indirect)->coef = betha * pb->coef;
            (*indirect)->exp  = pb->exp;
            pb = pb->nxt;
        } else if (cmp > 0) {
            (*indirect)->coef = alpha * pa->coef;
            (*indirect)->exp  = pa->exp;
            pa = pa->nxt;
        } else {
            (*indirect)->coef = alpha * pa->coef + betha * pb->coef;
            (*indirect)->exp  = pa->exp;
            pa = pa->nxt;
            pb = pb->nxt;
        }

        indirect = &(*indirect)->nxt;
        c->sz++;
    }

    while (pa != NULL) {
        *indirect = lpol_malloc(sizeof(**indirect));
        (*indirect)->coef = pa->coef;
        (*indirect)->exp  = pa->exp;
        indirect = &(*indirect)->nxt;
        pa = pa->nxt;
        c->sz++;
    }

    while (pb != NULL) {
        *indirect = lpol_malloc(sizeof(**indirect));
        (*indirect)->coef = pb->coef;
        (*indirect)->exp  = pb->exp;
        indirect = &(*indirect)->nxt;
        pb = pb->nxt;
        c->sz++;
    }

    return c;
}


void llpol_inplace_add(llpol_t * a, COEFTYPE alpha, llpol_t * b, COEFTYPE betha) {
    if (a->nvar != b->nvar) SAYNEXITWERROR("pols with diferent nvars");

    if (a == NULL || b == NULL) SAYNEXITWERROR("NULL pointer received!");

    if (a->sz == 0 || alpha == 0) 
        llpol_cpy(a, llpol_coef_multiply(b, betha));

    if (b->sz == 0 || betha == 0) 
        llpol_cpy(b, llpol_coef_multiply(a, alpha));

    lpol_t * newterm;
    lpol_t * pb = b->first;
    lpol_t ** indirect = &a->first;

    while ((*indirect) != NULL && pb != NULL) {
        int cmp = exp_lex_cmp((*indirect)->exp, pb->exp, a->nvar);

        if (cmp < 0) {
            newterm = lpol_malloc(sizeof(*newterm));
            newterm->coef = pb->coef;
            newterm->exp  = pb->exp;
            newterm->nxt  = *indirect;
            *indirect     = newterm;
            a->sz++;
        } else if (cmp > 0) {

        } else {
            (*indirect)->coef += pb->coef;
        }

        indirect = &(*indirect)->nxt;
    }

    while ((*indirect) != NULL) {
        indirect = &(*indirect)->nxt;
    }

    while (pb != NULL) {
        (*indirect) = lpol_malloc(sizeof(**indirect));
        (*indirect)->coef = pb->coef;
        (*indirect)->exp  = pb->exp;
        indirect = &(*indirect)->nxt;
        pb = pb->nxt;
    }
}

llpol_t * llpol_coef_multiply(llpol_t * a, COEFTYPE alpha) {
    if (a == NULL) SAYNEXITWERROR("null pol received");

    llpol_t * llpol = llpol_create(a->nvar);

    if (alpha != 0) {
        lpol_t *  ps = a->first;
        lpol_t ** pd = &llpol->first;
        
        while (ps != NULL) {
            *pd = lpol_malloc(sizeof(**pd));
            (*pd)->coef = alpha * ps->coef;
            (*pd)->exp  = ps->exp;
            ps = ps->nxt;
            pd = &(*pd)->nxt;
        }
    } else llpol->first = lpol_malloc(sizeof(*llpol->first));

    return llpol;
}


int llpol_hard_cmp(llpol_t * a, llpol_t * b) {
    if (a == NULL || b == NULL) SAYNEXITWERROR("cannot compare null llpol");

    if (a->nvar != b->nvar) SAYNEXITWERROR("cannot compare. nvar diff");

    if (a->sz != b->sz) return 1;

    lpol_t * pa = a->first;
    lpol_t * pb = b->first;

    while (pa && pb) {
        if (pa->exp != pb->exp) return 1;
        if (pa->coef != pb->coef) return 1;
        pa = pa->nxt;
        pb = pb->nxt;
    }

    return 0;
}


llpol_t * llpol_cpy(llpol_t * dst, llpol_t * src) {
    if (src == NULL || dst == NULL) SAYNEXITWERROR("src or dst is null");
    if (src->first != NULL) lpol_free(src->first);

    dst = llpol_coef_multiply(src, 1);

    return dst;
}


term_t * btpol_head(btpol_t * btpol) {
    term_t * term = term_malloc(sizeof(term_t));
    bpol_t * x = btpol->root;

    while (x->r != NULL) x = x->r;

    term->coef = x->coef;
    term->exp  = x->exp;

    return term;
}


/**
 * @brief add term to polinomial at the begining
 * of the linked list.
 * free_pol_t must be called.
 * 
 * @param pol pointer to term_t that is a ll
 * @param coef coefficient of the new term
 * @param exp exponent of the new term
 * @return pointer to the new
 * term of polynomial
 */

btpol_t * btpol_addterm(btpol_t * btpol, COEFTYPE coef, u64 exp) {
    if (!btpol) {
        dbgerr("pol is null");
        exit(EXIT_FAILURE);
    }
    
    if (coef == 0) return btpol;

    bpol_t * curr = btpol->root;
    bpol_t * aux  = NULL;
    bpol_t * newterm;

    while (curr != NULL) {
        aux = curr;

        if (exp < curr->exp) curr = curr->l;
        else curr = curr->r;
    }

    newterm = bpol_malloc(sizeof(bpol_t));
    CHECKPTR(newterm);
    newterm->coef = coef;
    newterm->exp  = exp;
    btpol->sz++;

    if (aux == NULL) btpol->root = newterm;
    else if (exp < aux->exp) aux->l = newterm;
    else if (exp > aux->exp) aux->r = newterm;
    else {
        aux->coef += coef;
        FREE(newterm);
        btpol->sz--;
    }

    return btpol;
}


btpol_t  * btpol_add(btpol_t * a, COEFTYPE alpha, btpol_t * b, COEFTYPE betha) {
    if (a->nvar != b->nvar) SAYNEXITWERROR("Cannot add polynomials of different nvar");
    
    
    if (alpha == 0 && betha != 0) {
        return btpol_coef_multiply(b, betha);
    }

    if (betha == 0 && alpha != 0) {
        return btpol_coef_multiply(a, alpha);
    }

    btpol_t * res = btpol_create(a->nvar);
    int i, j, k; i = j = k = 0;

    bpol_t * pa = a->root;
    bpol_t * pb = b->root;

    while (pa != NULL && pb != NULL) {
        if (exp_cmp(pa->exp, pb->exp, a->nvar, lex) < 0) {
            btpol_addterm(res, pa->coef, pa->exp);
        }
    }

    return res;
}


btpol_t * btpol_coef_multiply(btpol_t *a, COEFTYPE alpha) {
    btpol_t * res = btpol_create(a->nvar);
    bpol_t ** stack = malloc(sizeof(bpol_t) * a->sz);
    CHECKPTR(stack);
    bpol_t * node = a->root;
    int h = 0;

    while (1) {
        while (node != NULL) {
            stack[h++] = node;
            node = node->r;
        }

        if (h == 0) break;

        node = stack[--h];
        btpol_addterm(res, alpha * node->coef, node->exp);
        node = node->l;
    }

    FREE(stack);

    return res;
}


btpol_t * btpol_inplace_coef_multiply(btpol_t * a, COEFTYPE alpha) {
    bpol_t ** stack = malloc(sizeof(bpol_t) * a->sz);
    CHECKPTR(stack);
    bpol_t * node = a->root;
    int h = 0;

    while (1) {
        while (node != NULL) {
            stack[h++] = node;
            node = node->r;
        }

        if (h == 0) break;

        node = stack[--h];
        node->coef *= alpha;
        node = node->l;
    }

    FREE(stack);

    return a;
}

btpol_t * btpol_multiply(btpol_t *a, btpol_t *b) {
    return NULL;
}

/// @brief make a copy of an btpol
/// @param dst pointer to the dest btpol.
/// @param src pointer to the src btpol.
/// @return pointer to the dest btpol
btpol_t * btpol_cpy(btpol_t * dst, btpol_t * src) {
    bpol_t ** stack = malloc(sizeof(bpol_t *) * src->sz);
    CHECKPTR(stack);
    bpol_t * node = src->root;
    int h = 0;
    
    while (1) {
        while (node != NULL) {
            stack[h++] = node;
            node = node->r;
        }

        if (h == 0) break;

        node = stack[--h];
        btpol_addterm(dst, node->coef, node->exp);
        node = node->l;
    }
    
    FREE(stack);
    return dst;
}

void printlpol(bpol_t * lpol, u8 nvar) {
    u64 * e = exp_unpack(lpol->exp, nvar);
    if (lpol->coef >= 0) printf("+ ");
    if (lpol->exp == 0) {
        printf("%f", lpol->coef);
    } else {
        printf("%f*x^(", lpol->coef);

        for (int i = 0; i < nvar - 1; i++) {
            printf("%ld, ", *(e + i));
        }

        printf("%ld)", *(e + nvar - 1));

    }
    FREE(e);
}

void inorderprintbtpol(bpol_t * root, u8 nvar) {
    if (root == NULL) return;
    
    inorderprintbtpol(root->r, nvar);
    printlpol(root, nvar);
    inorderprintbtpol(root->l, nvar);
    
}


static void accept_space(parser_ctx_t * ctx) {
    while (isspace(*ctx->pol_str)) ctx->pol_str++;
}


static int accept_sign(parser_ctx_t * ctx) {
    int sign = 1;

    accept_space(ctx);

    if (*ctx->pol_str == '-') {
        sign = -1;
        ctx->pol_str++;
    } else if (*ctx->pol_str == '+') {
        ctx->pol_str++;
    }

    return sign;
}


static inline int char_varlist_lookup(char * c, char **var_lst, int n) {
    for (int i = 0; i < n; i++) {
        //printf("cmprng: %s with %s\n", c, var_lst[i]);
        if (strcmp(c, var_lst[i]) == 0) return i;
    }

    return -1;
}


static COEFTYPE accept_number(parser_ctx_t * ctx) {
    COEFTYPE num;
    char * end;

    if (*ctx->pol_str == '-' || *ctx->pol_str == '+' ) {
        ctx->status = SyntaxError;
        fprintf(stderr, "Syntax error: 1'%s' at ", ctx->pol_str_head);
        fprintf(stderr, "'%c' <-\n", *ctx->pol_str);
        return -1;
    }

    num = strtof(ctx->pol_str, &end);

    if (ctx->pol_str == end) num = 1.0;

    //printf("num: %f | ", num);
    //printf("end: '%c'\n", *end);

    ctx->pol_str = end;
    
    return num;
}


static COEFTYPE accept_coef(parser_ctx_t * ctx) {
    int sign      = accept_sign(ctx);
    COEFTYPE coef = accept_number(ctx);

    return sign * coef;
}


static u64 accept_exp(parser_ctx_t * ctx) {
    u64 e = 0;
    int idx;
    char var[2];
    u64 * exp = calloc(ctx->nvar, sizeof(u64));
    CHECKPTR(exp);

    if (ctx->pol_str && ctx->pol_str[0] == 0) {FREE(exp); return e;}

    accept_space(ctx);

    do {
        //printf("nxt: %c\n", *ctx->pol_str);
        if (*ctx->pol_str == '*') {
            //printf("skipping * ... ");
            ctx->pol_str++;
            accept_space(ctx);
        }
        //printf("nxt: %c\n", *ctx->pol_str);

        if (isalpha(*ctx->pol_str)) {
            var[0] = *ctx->pol_str++;
            var[1] = 0;
            idx = char_varlist_lookup(var, ctx->var_lst, ctx->nvar);
            //printf("idx: %d\n", idx);

            if (idx < 0) {
                ctx->status = NameError;
                fprintf(stderr, "Name '%s' is not in variable set\n", var);
                FREE(exp);
                return -1;
            }

            if (*ctx->pol_str == '^') {
                *ctx->pol_str++;
                exp[idx] = (u64)accept_number(ctx);
            } else {
                exp[idx] = 1;
                //printf("coef to 1 ... nxt: %c\n", *ctx->pol_str);
            }
            accept_space(ctx);
        } else if (*ctx->pol_str == '+' || *ctx->pol_str == '-') {
            continue;
        } else if (*ctx->pol_str == 0) {
            return e;
        } else {
            //printf("curr: %c\n", *ctx->pol_str);
            // this part will change in the future
            fprintf(stderr, "Syntax error: 2'%s' at ", ctx->pol_str_head);
            fprintf(stderr, "'%c' <-\n", *ctx->pol_str);
            ctx->status = SyntaxError;
            FREE(exp);
            return -1;
        }
    } while (*ctx->pol_str == '*');

    accept_space(ctx);
    if (*ctx->pol_str != '+' && *ctx->pol_str != '-' && *ctx->pol_str != 0) {
        fprintf(stderr, "Syntax error: 3'%s' at ", ctx->pol_str_head);
        fprintf(stderr, "'%c' <-\n", *ctx->pol_str);
        ctx->status = SyntaxError;
        FREE(exp);
        return -1;
    }

    e = exp_pack(exp, ctx->nvar);
    //printf("exp: %ld\n", e);
    FREE(exp);

    return e;
}

static term_t * accept_term(parser_ctx_t * ctx) {
    term_t * term = term_malloc(sizeof(term_t));
    CHECKPTR(term);

    term->coef = accept_coef(ctx);
    //printf("readed: %f | nxt: %c\n", term->coef, *ctx->pol_str);
    term->exp  = accept_exp(ctx);
    
    return term;
}

btpol_t * str2btpol(const char * btpol_str, char ** var_lst, u8 nvar) {
    btpol_t * btpol;
    term_t  * term;
    parser_ctx_t * ctx = malloc(sizeof(parser_ctx_t));
    btpol        = btpol_create(nvar);
    ctx->nvar    = nvar;
    ctx->pol_str_head = btpol_str;
    ctx->pol_str = btpol_str;
    ctx->var_lst = var_lst;
    ctx->status  = 0;

    while (*ctx->pol_str) {
        term = accept_term(ctx);

        if (ctx->status == NameError) {
            FREE(ctx);
            FREE(term);
            btpol_free(btpol);
            return NULL;
        }

        if (ctx->status == SyntaxError) {
            FREE(ctx);
            FREE(term);
            btpol_free(btpol);
            return NULL;
        }

        btpol_addterm(btpol, term->coef, term->exp);
        FREE(term);
    }

    FREE(ctx);

    return btpol;
}


aapol_t  * str2aapol(const char * aapol_str, char ** var_lst, u8 nvar) {
    if (aapol_str && aapol_str[0] == 0) {printf("aapol_str is empty"); return NULL;}
    
    aapol_t * aapol;
    term_t  * term;
    parser_ctx_t * ctx = malloc(sizeof(parser_ctx_t));

    aapol    = aapol_create(nvar);
    ctx->nvar    = nvar;
    ctx->pol_str_head = aapol_str;
    ctx->pol_str = aapol_str;
    ctx->var_lst = var_lst;
    ctx->status  = 0;

    while (*ctx->pol_str) {
        term = accept_term(ctx);

        if (ctx->status == NameError) {
            FREE(ctx);
            FREE(term);
            aapol_free(aapol);
            return NULL;
        }

        if (ctx->status == SyntaxError) {
            FREE(ctx);
            FREE(term);
            aapol_free(aapol);
            return NULL;
        }

        aapol_addterm(aapol, term->coef, term->exp);
        FREE(term);
    }

    FREE(ctx);

    return aapol;
}




/**
 * @brief prints a term_t *
 * 
 * @param pol term of pol
 */
void term_print(term_t * pol) {
    printf("coef: %f exp: %ld\n", pol->coef, pol->exp);
}


void llpol_print(llpol_t * llpol) {
    lpol_t * p = llpol->first;
    u64 * e;

    while (p) {
        if (p->coef >= 0) printf("+ ");
        if (p->exp == 0) {
            printf("%f", p->coef);
            p = p->nxt;
            continue;
        }

        e = exp_unpack(p->exp, llpol->nvar);
        printf("%0.1f*x^(", p->coef);

        for (int i = 0; i < llpol->nvar - 1; i++) {
            printf("%ld, ", *(e + i));
        }

        printf("%ld) ", *(e + llpol->nvar - 1));
        FREE(e);

        p = p->nxt;
    }

    printf("\n");
}


void btpol_print(btpol_t * btpol) {
    //debug("checking if pol is null");
    if (btpol == NULL) {
        printf("pol is  empty!\n");
        return;
    }

    inorderprintbtpol(btpol->root, btpol->nvar);
    printf("\n");
}

void aapol_print(aapol_t * aapol) {
    //debug("checking if pol is null");
    if (aapol == NULL) {
        printf("pol is  empty!\n");
        return;
    }
    term_t * terms;
    u64 * e;
    terms = aapol->terms;

    for (int i = 0; i < aapol->sz; i++) {
        if (terms[i].coef >= 0) printf("+ ");
        
        if (terms[i].exp == 0) {
            printf("%f", terms[i].coef);
            continue;
        }
        
        e = exp_unpack(terms[i].exp, aapol->nvar);
        printf("%0.1f*x^(", terms[i].coef);

        for (int i = 0; i < aapol->nvar - 1; i++) {
            printf("%ld, ", *(e + i));
        }
        printf("%ld) ", *(e + aapol->nvar - 1));
        FREE(e);
    }
    printf("\n");
}

char * btpol_repr(btpol_t * btpol) {
    char * repr = malloc(REPR_MAX_SZ * sizeof(*repr));

}

/**
 * @brief return string representation of aapol
 * as a char *
 * @param aapol alternet array polynomial
 * @return pointer to char that needs to be freed
*/
char * aapol_repr(aapol_t * aapol) {
    int c;
    u64 * e;
    int p = REPR_MAX_SZ; // space left in repr
    char buff[32];
    char * repr = calloc(REPR_MAX_SZ, sizeof(*repr));
    for (int i = 0; i < aapol->sz; i++) {
        if (aapol->terms[i].coef >= 0) {
            p = p - 3;
            if (p >= 3) strncat(repr, "+ ", p);
            else {
                strncat(repr, "...", p);
                return repr;
            }
        }

        c = snprintf(buff, 32, "%.2f", aapol->terms[i].coef);
        p = p - c;
        if (p >= 3) strcat(repr, buff);
        else {
            strncat(repr, "...", p);
            return repr;
        }
        if (aapol->terms[i].exp == 0) {
            continue;
        }
        p = p - 4;
        if (p >= 3) strncat(repr, "x^(", p);
        else {
            strncat(repr, "...", p);
            return repr;
        }

        e = exp_unpack(aapol->terms[i].exp, aapol->nvar);

        for (int j = 0; j < aapol->nvar - 1; j++) {
            c = snprintf(buff, 32, "%ld,", *(e+j));
            p = p - c;
            if (p >= 3) strcat(repr, buff);
            else {
                strncat(repr, "...", p);
                return repr;
            }
        }

        c = snprintf(buff, 32, "%ld)", *(e+aapol->nvar-1));
        p = p - c;
        if (p >= 3) strcat(repr, buff);
        else {
            strncat(repr, "...", p);
            return repr;
        }

        FREE(e);
    }

    return repr;
}

/**
 * @brief traverse the ll and
 * free all the terms of polynomial
 * 
 * @param pol head of the ll
 */
void bpol_free(bpol_t * pol) {
    if (pol->l != NULL) bpol_free(pol->l);
    if (pol->r != NULL) bpol_free(pol->r);
    FREE(pol);
}

void btpol_free(btpol_t * btpol) {
    if (btpol->root != NULL) bpol_free(btpol->root);
    FREE(btpol);
}


void aapol_free(aapol_t * aapol) {
    if (aapol->terms != NULL) FREE(aapol->terms);
    FREE(aapol);
}


double exp_norm(u64 e, u8 nvar) {
    if (nvar <= 0) {
        dbgerr("nvar is no positive!");
        exit(EXIT_FAILURE);
    }

    double cumsum = 0;
    u16 step   = 64 / nvar;
    const u64 * mask;

    switch (nvar) {
    case 1:
        return sqrt(e);
        break;
    case 2: 
        mask = MASK_LIST_2V;
        break;
    case 3: 
        mask = MASK_LIST_3V;
        break;
    case 4: 
        mask = MASK_LIST_4V;
        break;
    case 5: 
        mask = MASK_LIST_5V;
        break;
    case 6: 
        mask = MASK_LIST_6V;
        break;
    case 7: 
        mask = MASK_LIST_7V;
        break;
    case 8: 
        mask = MASK_LIST_8V;
        break;
    default:
        dbgerr("Not implemented. nvars : %d", nvar);
        exit(EXIT_FAILURE);
        break;
    }

    for (int i = 0; i < nvar; i++) {
        cumsum += (e & *(mask + i))>>((nvar-i-1)*step);
    }

    return sqrt(cumsum); 
}


int exp_cmp(u64 a, u64 b, u8 nvar, enum MONOMIAL_ORDER mo) {
    // todo: handle different ways to compare
    if (mo == lex) return exp_lex_cmp(a, b, nvar);
}


int exp_lex_cmp(u64 a, u64 b, u8 nvar) {
    if (a == b) return 0;
    else if (a < b) return -1;
    else if (a > b) return 1;
}

int exp_glex_cmp(u64 a, u64 b, u8 nvar) {
    u64 * e_a = exp_unpack(a, nvar);
    u64 * e_b = exp_unpack(b, nvar);
    u64 sum_a = 0;
    u64 sum_b = 0;
    int cmp   = 0;

    for (int i = 0; i < nvar; i++) {
        sum_a += *(e_a + i);
        sum_b += *(e_b + i);
        
        if (cmp == 0) {
            cmp = *(e_a + i) - *(e_b + i);
        }
    }

    FREE(e_a);
    FREE(e_b);

    if (sum_a == sum_b) return cmp < 0 ? -1 : 1;

    if (sum_a < sum_b) return -1;
    if (sum_a > sum_b) return 1;
}


int exp_revlex_cmp(u64 a, u64 b, u8 nvar) {
    u64 * e_a = exp_unpack(a, nvar);
    u64 * e_b = exp_unpack(b, nvar);
    int cmp = 0;

    for (int i = 0; i < nvar; i++) {
        if (cmp == 0) {
            cmp = *(e_a + i) - *(e_b + i);
        } else break;
    }

    FREE(e_a);
    FREE(e_b);

    if (cmp < 0) return 1;
    if (cmp > 0) return -1;
    
    return 0;
}


int exp_grevlex_cmp(u64 a, u64 b, u8 nvar) {
    u64 * e_a = exp_unpack(a, nvar);
    u64 * e_b = exp_unpack(b, nvar);
    u64 sum_a = 0;
    u64 sum_b = 0;
    int cmp   = 0;

    for (int i = 0; i < nvar; i++) {
        sum_a += *(e_a + i);
        sum_b += *(e_b + i);
        
        if (cmp == 0) {
            cmp = *(e_a + i) - *(e_b + i);
        }
    }

    FREE(e_a);
    FREE(e_b);

    if (sum_a == sum_b) return cmp < 0 ? 1 : -1;

    if (sum_a < sum_b) return -1;
    if (sum_a > sum_b) return 1;
}


u64 * exp_add(u64 * a, u64 * b, u64 * c) {
    *c = *a + *b; // be carefull
    /* todo: handle "local" overflow */
    return c;
}

/**
 * @brief unpacks the exponent stored in u64 var.
 *        if nvar is 0, exits with status 1.
 *        returned pointer has to be freed.
 * @param e packed exponents
 * @param nvar number of variables on e
 * @return u64* that stores unpacked variables
 */
u64 * exp_unpack(u64 e, u8 nvar) {
    if (nvar <= 0) {
        dbgerr("nvar is no positive!");
        exit(EXIT_FAILURE);
    }
    u64 * exp  = malloc(nvar * sizeof(u64));
    CHECKPTR(exp);
    u16 step   = 64 / nvar;
    const u64 * mask;

    switch (nvar) {
    case 1:
        *exp = e;
        break;
    case 2: 
        mask = MASK_LIST_2V;
        break;
    case 3: 
        mask = MASK_LIST_3V;
        break;
    case 4: 
        mask = MASK_LIST_4V;
        break;
    case 5: 
        mask = MASK_LIST_5V;
        break;
    case 6: 
        mask = MASK_LIST_6V;
        break;
    case 7: 
        mask = MASK_LIST_7V;
        break;
    case 8: 
        mask = MASK_LIST_8V;
        break;
    default:
        dbgerr("Not implemented. nvars : %d", nvar);
        exit(EXIT_FAILURE);
        break;
    }

    for (int i = 0; i < nvar; i++) {
        *(exp + i) = (e & *(mask + i))>>((nvar-i-1)*step);
    }
    
    return exp;
}

u64 exp_pack(u64 * e, u8 nvar) {
    if (nvar <= 0) {
        dbgerr("nvar is no positive!");
        exit(EXIT_FAILURE);
    }

    if (e == NULL) return 0;

    u64 exp  = 0;
    u16 step = 64 / nvar; 

    for (int i = 0; i < nvar; i++)
        exp += *(e+i)<<((nvar-i-1)*step);
    
    return exp;
}