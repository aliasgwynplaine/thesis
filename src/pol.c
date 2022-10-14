#include "pol.h"

/*
how do i control the vars???
x0,x1,x2
Using masks
*/





pol_t * polnvars(u8 n) {
    pol_t * pol = malloc(sizeof(pol_t));
    pol->nvar   = n;

    return pol; // needs to call free outside fux.
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


void addexp(u64 * a, u64 * b, u64 * c) {
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
        exit(1);
    }
    u64 * exp  = malloc(nvar * sizeof(u64 *));
    u16 step   = 64 / nvar; 
    if (exp == NULL) {
        dbgerr("No memory!");
        exit(EXIT_FAILURE);
    }
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

/**
 * @brief traverse the ll and
 * free all the terms of polynomial
 * 
 * @param pol head of the ll
 */
void freepol_t(pol_t * pol) {
    pol_t * curr;
    
    while (pol) {
        curr = pol;
        pol = pol->nxt;
        FREE(curr);
    }
    debug("mem is free!");
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
pol_t * addterm2pol(pol_t * pol, float coef, u64 exp) {
    if (!pol) {
        debug("pol is null... calling malloc.");
        pol = malloc(sizeof(pol_t));
        if (pol == NULL) {
            dbgerr("No memory!");

        }
        pol->coef = coef;
        pol->exp  = exp;
        pol->nxt  = NULL;
        return pol;
    }
    debug("creating newterm...");
    pol_t * newterm = malloc(sizeof(pol_t));
    if (newterm == NULL) {
        dbgerr("No memory!");
    }
    newterm->coef = coef;
    newterm->exp  = exp;
    newterm->nvar = pol->nvar;
    newterm->nxt  = pol;
    pol = newterm;

    return pol;
}


/**
 * @brief prints a pol_t *
 * 
 * @param pol polynomial
 */

void printpol(pol_t * pol) {
    debug("checking if pol is null");
    if (pol == NULL) {
        printf("pol is  empty!\n");
        return;
    }
    pol_t * curr;
    u64 * e;
    //int sz = 0;
    curr = pol;

    while (curr) {
        e = unpackexp(curr->exp, curr->nvar);
        if (curr->coef >= 0) printf("+ ");
        printf("%f*x^(", curr->coef);

        for (int i = 0; i < curr->nvar - 1; i++) {
            printf("%ld, ", *(e + i));
        }
        printf("%ld)", *(e + curr->nvar - 1));
        curr = curr->nxt;
        FREE(e);
    }
    printf("\n");
}