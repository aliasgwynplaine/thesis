#include "pol.h"
#include <math.h>

typedef struct parser_ctx_t parser_ctx_t;

struct parser_ctx_t {
    u8 nvar;
    const char * pol_str_head;
    const char * pol_str;
    const char ** var_lst; // ordered list of vars
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


int llpol_quicksort_partition(llpol_t ** lollpol, int p, int r) {
    int i, cmp;
    llpol_t * x = lollpol[r];
    llpol_t * aux;
    i = p - 1;

    for (int j = p; j < r; j++) {
        //cmp = exp_cmp(lollpol[j]->root->exp, x->root->exp, x->nvar);
        cmp = llpol_monomial_cmp(lollpol[j], x);
        if (cmp <= 0) { // todo: implement other comparition methods
            i++;
            aux = lollpol[i];
            lollpol[i] = lollpol[j];
            lollpol[j] = aux;
        }
    }

    aux = lollpol[i + 1];
    lollpol[i + 1] = lollpol[r];
    lollpol[r] = aux;

    return i + 1;
}

void llpol_list_quicksort(llpol_t ** lollpol, int p, int r) {
    if (p < r) {
        int q = llpol_quicksort_partition(lollpol, p, r);
        llpol_list_quicksort(lollpol, p, q - 1);
        llpol_list_quicksort(lollpol, q + 1, r);
    }
}



void llpol_list_sort(llpol_t ** lollpol, int sz) {
    llpol_list_quicksort(lollpol, 0, sz - 1);
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
        cmp = exp_cmp(aterms[i].exp, bterms[j].exp, a->nvar);
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
        cmp = exp_cmp(rterms[k].exp, dp[s], a->nvar);

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
        cmp = exp_cmp(a_term_p[i].exp, b_term_p[i].exp, a->nvar);

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
        cmp = exp_cmp(a_term_p[i].exp, b_term_p[i].exp, a->nvar);

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


int llpol_monomial_cmp(llpol_t * a, llpol_t * b) {
    if (a->nvar != b->nvar) SAYNEXITWERROR("Cannot cmp polynomials of different nvar");

    lpol_t ** stacka = malloc(sizeof(lpol_t *) * a->sz);
    CHECKPTR(stacka);
    lpol_t ** stackb = malloc(sizeof(lpol_t *) * b->sz);
    CHECKPTR(stackb);
    int ha = 0;
    int hb = 0;
    lpol_t * nodea = a->root;
    lpol_t * nodeb = b->root;
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
        cmp = exp_cmp(nodea->exp, nodeb->exp, a->nvar);

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

int llpol_hard_cmp(llpol_t * a, llpol_t * b) {
    if (a == NULL || b == NULL) return 1;
    if (a->nvar != b->nvar) return 1;
    if (a->sz != b->sz) return 1;

    lpol_t ** stacka = malloc(sizeof(lpol_t *) * a->sz);
    CHECKPTR(stacka);
    lpol_t ** stackb = malloc(sizeof(lpol_t *) * b->sz);
    CHECKPTR(stackb);
    int ha = 0;
    int hb = 0;
    lpol_t * nodea = a->root;
    lpol_t * nodeb = b->root;
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
        cmp = exp_cmp(nodea->exp, nodeb->exp, a->nvar);

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
lpol_t * lpol_malloc(size_t sz) {
    lpol_t * lpol = malloc(sz);
    lpol->coef = 0;
    lpol->exp  = 0;
    lpol->l    = NULL;
    lpol->r    = NULL;

    return lpol;
}

/**
 * @brief creates a tree like pol struct in n
 * vars. It's necesary to call freellpol_t at
 * the end. 
 * 
 * @param n number of variables
 * @return llpol_t* pointer with head pointing
 * to NULL, nvars equals n and sz equals to 0
 */
llpol_t * llpol_create(u8 n) {
    if (n > MAX_NUM_O_VARS) SAYNEXITWERROR("Not implemented!");
    llpol_t * llpol = malloc(sizeof(llpol_t));
    CHECKPTR(llpol);
    llpol->root = NULL;
    llpol->nvar = n;
    llpol->sz   = 0;

    return llpol;
}


term_t * llpol_head(llpol_t * llpol) {
    term_t * term = term_malloc(sizeof(term_t));
    lpol_t * x = llpol->root;

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

llpol_t * llpol_addterm(llpol_t * llpol, COEFTYPE coef, u64 exp) {
    if (!llpol) {
        dbgerr("pol is null");
        exit(EXIT_FAILURE);
    }
    
    if (coef == 0) return llpol;

    lpol_t * curr = llpol->root;
    lpol_t * aux  = NULL;
    lpol_t * newterm;

    while (curr != NULL) {
        aux = curr;

        if (exp < curr->exp) curr = curr->l;
        else curr = curr->r;
    }

    newterm = lpol_malloc(sizeof(lpol_t));
    CHECKPTR(newterm);
    newterm->coef = coef;
    newterm->exp  = exp;
    llpol->sz++;

    if (aux == NULL) llpol->root = newterm;
    else if (exp < aux->exp) aux->l = newterm;
    else if (exp > aux->exp) aux->r = newterm;
    else {
        aux->coef += coef;
        FREE(newterm);
        llpol->sz--;
    }

    return llpol;
}


llpol_t  * llpol_add(llpol_t * a, COEFTYPE alpha, llpol_t * b, COEFTYPE betha) {
    if (a->nvar != b->nvar) SAYNEXITWERROR("Cannot add polynomials of different nvar");
    
    
    if (alpha == 0 && betha != 0) {
        return llpol_coef_multiply(b, betha);
    }

    if (betha == 0 && alpha != 0) {
        return llpol_coef_multiply(a, alpha);
    }

    llpol_t * res = llpol_create(a->nvar);
    /* todo */
    return res;
}


llpol_t * llpol_coef_multiply(llpol_t *a, COEFTYPE alpha) {
    llpol_t * res = llpol_create(a->nvar);
    lpol_t ** stack = malloc(sizeof(lpol_t) * a->sz);
    CHECKPTR(stack);
    lpol_t * node = a->root;
    int h = 0;

    while (1) {
        while (node != NULL) {
            stack[h++] = node;
            node = node->r;
        }

        if (h == 0) break;

        node = stack[--h];
        llpol_addterm(res, alpha * node->coef, node->exp);
        node = node->l;
    }

    FREE(stack);

    return res;
}


llpol_t * llpol_inplace_coef_multiply(llpol_t * a, COEFTYPE alpha) {
    lpol_t ** stack = malloc(sizeof(lpol_t) * a->sz);
    CHECKPTR(stack);
    lpol_t * node = a->root;
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


llpol_t * llpol_cpy(llpol_t * dst, llpol_t * src) {
    lpol_t ** stack = malloc(sizeof(lpol_t *) * src->sz);
    CHECKPTR(stack);
    lpol_t * node = src->root;
    int h = 0;
    
    while (1) {
        while (node != NULL) {
            stack[h++] = node;
            node = node->r;
        }

        if (h == 0) break;

        node = stack[--h];
        llpol_addterm(dst, node->coef, node->exp);
        node = node->l;
    }
    
    FREE(stack);
    return dst;
}

void printlpol(lpol_t * lpol, u8 nvar) {
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

void inorderprintllpol(lpol_t * root, u8 nvar) {
    if (root == NULL) return;
    
    inorderprintllpol(root->r, nvar);
    printlpol(root, nvar);
    inorderprintllpol(root->l, nvar);
    
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


static inline int char_varlist_lookup(char * c, const char **var_lst, int n) {
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
        fprintf(stderr, "Syntax error: '%s' at ", ctx->pol_str_head);
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
        } else {
            //printf("curr: %c\n", *ctx->pol_str);
            // this part will change in the future
            fprintf(stderr, "Syntax error: '%s' at ", ctx->pol_str_head);
            fprintf(stderr, "'%c' <-\n", *ctx->pol_str);
            ctx->status = SyntaxError;
            FREE(exp);
            return -1;
        }
    } while (*ctx->pol_str == '*');

    accept_space(ctx);
    if (*ctx->pol_str != '+' && *ctx->pol_str != '-' && *ctx->pol_str != 0) {
        fprintf(stderr, "Syntax error: '%s' at ", ctx->pol_str_head);
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

llpol_t * str2llpol(const char * llpol_str, const char ** var_lst, u8 nvar) {
    llpol_t * llpol;
    term_t  * term;
    parser_ctx_t * ctx = malloc(sizeof(parser_ctx_t));

    llpol        = llpol_create(nvar);
    ctx->nvar    = nvar;
    ctx->pol_str_head = llpol_str;
    ctx->pol_str = llpol_str;
    ctx->var_lst = var_lst;
    ctx->status  = 0;

    while (*ctx->pol_str) {
        term = accept_term(ctx);

        if (ctx->status == NameError) {
            FREE(ctx);
            FREE(term);
            llpol_free(llpol);
            return NULL;
        }

        if (ctx->status == SyntaxError) {
            FREE(ctx);
            FREE(term);
            llpol_free(llpol);
            return NULL;
        }

        llpol_addterm(llpol, term->coef, term->exp);
        FREE(term);
    }

    FREE(ctx);

    return llpol;
}


aapol_t  * str2aapol(const char * aapol_str, const char ** var_lst, u8 nvar) {
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
    //debug("checking if pol is null");
    if (llpol == NULL) {
        printf("pol is  empty!\n");
        return;
    }

    inorderprintllpol(llpol->root, llpol->nvar);
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

/**
 * @brief traverse the ll and
 * free all the terms of polynomial
 * 
 * @param pol head of the ll
 */
void lpol_free(lpol_t * pol) {
    if (pol->l != NULL) lpol_free(pol->l);
    if (pol->r != NULL) lpol_free(pol->r);
    FREE(pol);
}

void llpol_free(llpol_t * llpol) {
    lpol_free(llpol->root);
    FREE(llpol);
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


int exp_cmp(u64 a, u64 b, u8 nvar) {
    // todo: handle different ways to compare
    return exp_lex_cmp(a, b, nvar);
}


int exp_lex_cmp(u64 a, u64 b, u8 nvar) {
    return a - b;
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