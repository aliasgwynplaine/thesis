#include "pol.h"

/*
how do i control the vars???
x0,x1,x2
Using masks
*/

/**
 * @brief creates an alternate array like pol in n
 * vars. It's necesary to call freeaapol_t at
 * the end. 
 * 
 * @param n number of variables
 * @return aapol_t* pointer with head pointing
 * to NULL, nvars equals n and sz equals to 0
 */
aapol_t * aapolmalloc(u8 n) {
    if (n > MAX_NUM_O_VARS) SAYNEXITWERROR("Not implemented!");
    aapol_t * aapol = malloc(sizeof(aapol_t));
    TESTPTR(aapol);
    aapol->nvar  = n;
    aapol->sz    = 0;
    aapol->cap   = 1;
    aapol->terms = NULL;

    return aapol;
}


#define PARENT(I)  I>>1
#define LEFT(I)   (I<<1) + 1
#define RIGHT(I)  (I<<1) + 2


void minheapify(pol_t * terms, int i, int hsz) {
    int l = LEFT(i);
    int r = RIGHT(i);
    int smallest = i;

    // debug("[%d]: comparing left... %d", i, l);
    if (l <= hsz) { 
        if(terms[l].exp < terms[i].exp)
            smallest = l;
        else {
            if (terms[l].exp == terms[i].exp) {
                terms[l].coef += terms[i].coef;
                terms[i].coef = 0;
                terms[i].exp  = 0;
            }

            //smallest = i;
        }
    }

    // debug("[%d] comparing right... %d", i, r);
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
        pol_t aux       = terms[i];
        terms[i]        = terms[smallest];
        terms[smallest] = aux;
        //debug("[%d]: moving forward (%d)", i, smallest);
        minheapify(terms, smallest, hsz);
    }
}

void buildminheap(pol_t * terms, int hsz) {
    debug("building heap... %d", hsz);
    for (int i = hsz / 2; i >= 0; i--) {
        //debug("[%d]: minheapifying", i);
        minheapify(terms, i, hsz);
    }
}


void mergeaapol(aapol_t * a, int p, int q, int r) {
    int n_1, n_2, i, j, c;
    pol_t * l_;
    pol_t * r_;
    n_1 = q - p + 1;
    n_2 = r - q;
    l_ = malloc((n_1 + 1) * sizeof(pol_t));
    TESTPTR(l_);
    r_ = malloc((n_2 + 1) * sizeof(pol_t));
    TESTPTR(r_);

    for (i = 0; i < n_1; i++) l_[i] = a->terms[i + p];

    for (i = 0; i < n_2; i++) r_[i] = a->terms[i + q + 1];

    l_[n_1].exp = -1; r_[n_2].exp = -1;
    i = 0; j = 0;

    for (int k = p; k <= r; k++) {
        if (i != n_1 || j != n_2) {
            c = l_[i].exp - r_[j].exp;

            if (c > 0) a->terms[k] = l_[i++];
            else if (c < 0) a->terms[k] = r_[j++];
            else if (c == 0) {
                a->terms[k].coef = l_[i++].coef + r_[j].coef;
                a->terms[k].exp  = r_[j++].exp;
            }
        } else { 
            a->terms[k].coef = 0;
            a->terms[k].exp = 0;
        }
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

void sortaapol(aapol_t * aapol) {
    debug("preparing pol...");
    mergesortaapol(aapol, 0, aapol->sz - 1);
    /* todo: get rid off the remaining 0's */
}

/**
 * @brief adds a term to the aapol.
 * Note that is not necesary to store the 
 * returning value because is, in fact, the
 * same as aapol
 * @param aapol pointer to aapol
 * @param coef  coefficient of the new term
 * @param exp   exponent of the new term
 * @return aapol_t* pointer to the aapol with
 * the new term
 */
aapol_t * addterm2aapol(aapol_t * aapol, COEFTYPE coef, u64 exp) {
    if (coef == 0) {
        return aapol;
    }

    if (!aapol->terms) {
        aapol->terms = malloc(2*sizeof(pol_t));
        TESTPTR(aapol);
        aapol->cap = 2;
    }


    if (aapol->sz + 1 >= aapol->cap) {
        // need to realloc
        // todo: make a smarter use of space.
        debug("trying to alloc %d chunks", aapol->cap<<1);
        aapol->terms = realloc(aapol->terms, (aapol->cap<<1) * sizeof(pol_t));
        TESTPTR(aapol->terms);
        aapol->cap = aapol->cap<<1;
    }

    debug("creating a new term...");
    (aapol->terms + aapol->sz)->coef = coef;
    (aapol->terms + aapol->sz)->exp  = exp;
    int i  = aapol->sz++;
    int pi = PARENT(i);

    while (i > 0 && aapol->terms[pi].exp < aapol->terms[i].exp) {
        pol_t aux        = aapol->terms[i];
        aapol->terms[i]  = aapol->terms[pi];
        aapol->terms[pi] = aux;
        i  = PARENT(i);
        pi = PARENT(i);
        
        // if (aapol->terms[i].exp == aapol->terms[pi].exp) {
        //     aapol->terms[pi].coef += aapol->terms[i].coef;
        //     aapol->terms[i].coef  = 0;
        //     aapol->terms[i].exp   = 0;
        // }
    }

    // sortaapol_t(aapol);

    return aapol;
}


COEFTYPE extractcoef(aapol_t * aapol, u64 exp) {
    return 0;
}

smatrix_t * aapol2smatrix_(aapol_t * aapol, int sz) {
    setoint_t * s = setoint_create();

    for (int j = 0; j < sz; j++) {
        for (int i = 0; i < (aapol+j)->sz; i++) {
            setoint_insert(s, (aapol+j)->terms[i].exp);
        }
    }

    u64 * exps = setoint_dump(s);
    aapol_t * aux;
    int * idx = calloc(sz, sizeof(int));
    
    for (int i = 0; i < s->sz; i++) {
        for (int k = 0; k < sz; k++) printf("%d ", idx[k]);
        printf("%.3ld ", *(exps + i));


        for (int j = 0; j < sz; j++) {
            aux = (aapol+j);
            if (idx[j] < aux->sz) {
                if (*(exps + i) == aux->terms[idx[j]].exp)
                    printf("%0.1f ", aux->terms[idx[j]++].coef);
                else
                    printf(" -  ");
            }
        }
        printf("\n");
    }

    FREE(idx);
    FREE(exps);
    printf("\n");
    debug("Done!\n");
}


/**
 * @brief transforms a list of aapol_t 
 * to matrix for groebner basis calculation 
 * @param aapol list of aapols
 * @param sz size of list
 */
smatrix_t * aapol2smatrix(aapol_t * aapol, int sz) {
    setoint_t * s = setoint_create();

    for (int j = 0; j < sz; j++) {
        for (int i = 0; i < (aapol+j)->sz; i++) {
            setoint_insert(s, (aapol+j)->terms[i].exp);
        }
    }

    u64 * exps = setoint_dump(s);
    debug("\n");

    
    for (int i = 0; i < s->sz; i++) {
        printf("%.3ld ", *(exps + i));
    }

    printf("\n");
    aapol_t * aux;
    int k;
    int i;

    for (int j = 0; j < sz; j++) {
        k = 0;
        aux = (aapol+j);
        i = 0;
        while (i < aux->sz) {
            if (aux->terms[i].exp < *(exps+k)) {
                printf(" -  ");
            }
            else if (aux->terms[i].exp == *(exps+k)) {
                /* construct matrix here */
                /*
                    insert matcoef in pos (i,k)
                */
                printf("%0.1f ", aux->terms[i].coef);
                i++;
            } else { break; }
            k++;
        }
        printf("eol\n");
    }
    debug("Done!\n");
    //return b;
}

lpol_t * lpolmalloc(size_t sz) {
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
llpol_t * llpolmalloc(u8 n) {
    if (n > MAX_NUM_O_VARS) SAYNEXITWERROR("Not implemented!");
    llpol_t * llpol = malloc(sizeof(llpol_t));
    TESTPTR(llpol);
    llpol->root = NULL;
    llpol->nvar = n;
    llpol->sz   = 0;

    return llpol;
}

/**
 * @brief add term to polinomial at the begining
 * of the linked list.
 * free_pol_t must be called.
 * 
 * @param pol pointer to pol_t that is a ll
 * @param coef coefficient of the new term
 * @param exp exponent of the new term
 * @return pointer to the new
 * term of polynomial
 */

llpol_t * addterm2llpol(llpol_t * llpol, COEFTYPE coef, u64 exp) {
    if (!llpol) {
        dbgerr("pol is null");
        exit(EXIT_FAILURE);
    }
    debug("creating newterm...");
    lpol_t * curr    = llpol->root;
    lpol_t * aux     = NULL;
    lpol_t * newterm;

    while (curr != NULL) {
        aux = curr;

        if (exp < curr->exp) curr = curr->l;
        else curr = curr->r;
    }

    newterm = lpolmalloc(sizeof(lpol_t));
    TESTPTR(newterm);
    newterm->coef = coef;
    newterm->exp  = exp;

    if (aux == NULL) llpol->root = newterm;
    else if (exp < aux->exp) aux->l = newterm;
    else if (exp > aux->exp) aux->r = newterm;
    else {
        aux->coef += coef;
        FREE(newterm);
    }

    return llpol;
}

void printlpol(lpol_t * lpol, u8 nvar) {
    u64 * e = unpackexp(lpol->exp, nvar);
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


/**
 * @brief prints a pol_t *
 * 
 * @param pol polynomial
 */

void printllpol(llpol_t * llpol) {
    //debug("checking if pol is null");
    if (llpol == NULL) {
        printf("pol is  empty!\n");
        return;
    }

    inorderprintllpol(llpol->root, llpol->nvar);
    printf("\n");
}

void printaapol(aapol_t * aapol) {
    //debug("checking if pol is null");
    if (aapol == NULL) {
        printf("pol is  empty!\n");
        return;
    }
    pol_t * terms;
    u64 * e;
    terms = aapol->terms;

    for (int i = 0; i < aapol->sz; i++) {
        if (terms[i].coef >= 0) printf("+ ");
        
        if (terms[i].exp == 0) {
            printf("%f", terms[i].coef);
            continue;
        }
        
        e = unpackexp(terms[i].exp, aapol->nvar);
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
void freelpol(lpol_t * pol) {
    if (pol->l != NULL) freelpol(pol->l);
    if (pol->r != NULL) freelpol(pol->r);
    FREE(pol);
}

void freellpol(llpol_t * llpol) {
    freelpol(llpol->root);
    FREE(llpol);
}


void freeaapol(aapol_t * aapol) {
    FREE(aapol->terms);
    FREE(aapol);
}



int cmpexplex(u64 a, u64 b, u8 nvar) {
    if (a == b) return 0;
    if (a < b)  return -1;
    if (a > b)  return 1;
    /* next is so that the compiler does not cry */
    return 0;
}


void expadd(u64 * a, u64 * b, u64 * c) {
    *c = *a + *b; // be carefull
    /* todo: handle "local" overflow */
}

/**
 * @brief unpacks the exponent stored in u64 var
 *        returned pointer has to be freed.
 *        if nvar is 0, exits with status 1
 * @param e packed exponents
 * @param nvar number of variables on e
 * @return u64* that stores unpacked variables
 */
u64 * unpackexp(u64 e, u8 nvar) {
    if (nvar <= 0) {
        dbgerr("nvar is no positive!");
        exit(EXIT_FAILURE);
    }
    u64 * exp  = malloc(nvar * sizeof(u64 *));
    TESTPTR(exp);
    u16 step   = 64 / nvar; 

    switch (nvar) {
    case 1:
        *exp = e;
        break;
    case 2: 
        for (int i = 0; i < nvar; i++) {
            *(exp + i) = (e & *(MASK_LIST_2V + i))>>((nvar-i-1)*step);
        }
        break;
    case 3: 
        for (int i = 0; i < nvar; i++) {
            *(exp + i) = (e & *(MASK_LIST_3V + i))>>((nvar-i-1)*step);
        }
        break;
    case 4: 
        for (int i = 0; i < nvar; i++) {
            *(exp + i) = (e & *(MASK_LIST_4V + i))>>((nvar-i-1)*step);
        }
        break;
    case 5: 
        for (int i = 0; i < nvar; i++) {
            *(exp + i) = (e & *(MASK_LIST_5V + i))>>((nvar-i-1)*step);
        }
        break;
    case 6: 
        for (int i = 0; i < nvar; i++) {
            *(exp + i) = (e & *(MASK_LIST_6V + i))>>((nvar-i-1)*step);
        }
        break;
    case 7: 
        for (int i = 0; i < nvar; i++) {
            *(exp + i) = (e & *(MASK_LIST_7V + i))>>((nvar-i-1)*step);
        }
        break;
    case 8: 
        for (int i = 0; i < nvar; i++) {
            *(exp + i) = (e & *(MASK_LIST_8V + i))>>((nvar-i-1)*step);
        }
        break;
    default:
        dbgerr("Not implemented. nvars : %d", nvar);
        exit(EXIT_FAILURE);
        break;
    }
    
    return exp;
}

u64 packexp(u64 * e, u8 nvar) {
    if (nvar <= 0) {
        dbgerr("nvar is no positive!");
        exit(EXIT_FAILURE);
    }
    u64 exp  = 0;
    u16 step = 64 / nvar; 

    for (int i = 0; i < nvar; i++)
        exp += *(e+i)<<((nvar-i-1)*step);
    
    return exp;
}