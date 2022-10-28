#include "pol.h"
#define _GNU_SOURCE
#include <search.h>

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

void sortaapol_t(aapol_t * aapol) {
    debug("preparing pol...");
    pol_t aux;
    int sz  = aapol->sz;
    int hsz = sz - 1;
    buildminheap(aapol->terms, hsz);

    for (int i = sz - 1; i >= 1; i--) {
        // printf("[%d]: aapol - ", i);
        // printaapol_t(aapol);
        aux             = aapol->terms[i];
        aapol->terms[i] = aapol->terms[0];
        aapol->terms[0] = aux;
        hsz            -= 1;
        // debug("[%d]: sorting...", i);
        minheapify(aapol->terms, 0, hsz);
    }

    while (aapol->terms[aapol->sz - 1].coef == 0)
        aapol->sz--;
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

    while (i > 0 && aapol->terms[pi].exp > aapol->terms[i].exp) {
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


/**
 * @brief transforms a list of aapol_t 
 * to matrix for groebner basis calculation 
 * @param aapol list of aapols
 * @param sz size of list
 */
void aapol2matrix(aapol_t * aapol, int sz) {
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

    /* 
       todo: considera usar una tabla hash
       que almacene los exponentes y los asocie
       a índices.
    */
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

/**
 * @brief creates a linked list like pol in n
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
    llpol->head = NULL;
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
    lpol_t * newterm = malloc(sizeof(lpol_t));
    TESTPTR(newterm);

    newterm->coef = coef;
    newterm->exp  = exp;
    newterm->nxt  = llpol->head;
    llpol->head = newterm;

    return llpol;
}


/**
 * @brief prints a pol_t *
 * 
 * @param pol polynomial
 */

void printllpol_t(llpol_t * llpol) {
    //debug("checking if pol is null");
    if (llpol == NULL) {
        printf("pol is  empty!\n");
        return;
    }
    lpol_t * curr;
    u64 * e;
    //int sz = 0;
    curr = llpol->head;

    while (curr) {
        e = unpackexp(curr->exp, llpol->nvar);
        if (curr->coef >= 0) printf("+ ");
        if (curr->exp == 0) {
            printf("%f", curr->coef);
            continue;
        }
        printf("%f*x^(", curr->coef);

        for (int i = 0; i < llpol->nvar - 1; i++) {
            printf("%ld, ", *(e + i));
        }
        printf("%ld)", *(e + llpol->nvar - 1));
        curr = curr->nxt;
        FREE(e);
    }
    printf("\n");
}

void printaapol_t(aapol_t * aapol) {
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
void freelpol_t(lpol_t * pol) {
    lpol_t * curr;
    
    while (pol) {
        curr = pol;
        pol = pol->nxt;
        FREE(curr);
    }
}

void freellpol_t(llpol_t * llpol) {
    freelpol_t(llpol->head);
    FREE(llpol);
}


void freeaapol_t(aapol_t * aapol) {
    FREE(aapol->terms);
    FREE(aapol);
}



int cmpexplex(u64 a, u64 b, u8 nvar) {
    if (a == b) return 0;
    if (a < b)  return -1;
    if (a > b)  return 1;
    /* next is so that the compiler does not cry */
    return 0;
    /*if (a == b) return 0;
    
    u64 * adegs = unpackexp(a, nvar);
    u64 * bdegs = unpackexp(b, nvar);
    int r = 0;
    int i = 0;

    while (i < nvar) {
        if (*(adegs+i) != *(bdegs+i)) {
            goto nq;
        }
        i++;
    }

    goto exit;

    nq:

    if (*(adegs+i) > *(bdegs+i)) return 1;
    if (*(adegs+i) < *(bdegs+i)) return -1;

    exit:
    FREE(adegs);
    FREE(bdegs);
    return r;*/
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