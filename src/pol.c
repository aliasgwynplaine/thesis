#include "pol.h"

/*
how do i control the vars???
x0,x1,x2
Using masks
*/





pol_t * polnvars(u8 n) {
    pol_t * pol = malloc(sizeof(pol_t));
    pol->nvar = n;

    return pol; // needs to call free outside fux.
}


int cmpexplex(u64 a, u64 b) {

    return;
}


void addexp(u64 * a, u64 * b, u64 * c) {
    return;
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
    u16 step   = 64 / nvar; // this could lead to fltpnt exception
    if (exp == NULL) {
        dbgerr("No memory!");
        exit(1);
    }
    debug("going to switch...");
    switch (nvar) {
    case 1:
        *exp = e;
        break;
    case 2: 
        for (int i = 0; i < nvar; i++) {
            *(exp + i) = (e & *(MASK_LIST_2V + i))>>(i*step);
            debug("exp[%d] : %ld", i, *(exp + i));     
        }
        break;
    case 3: 
        for (int i = 0; i < nvar; i++) {
            *(exp + i) = (e & *(MASK_LIST_3V + i))>>(i*step);
            debug("exp[%d] : %ld", i, *(exp + i));     
        }
        break;
    case 4: 
        for (int i = 0; i < nvar; i++) {
            *(exp + i) = (e & *(MASK_LIST_4V + i))>>(i*step);
            debug("exp[%d] : %ld", i, *(exp + i));     
        }
        break;
    case 5: 
        for (int i = 0; i < nvar; i++) {
            *(exp + i) = (e & *(MASK_LIST_5V + i))>>(i*step);
            debug("exp[%d] : %ld", i, *(exp + i));     
        }
        break;
    case 6: 
        for (int i = 0; i < nvar; i++) {
            *(exp + i) = (e & *(MASK_LIST_6V + i))>>(i*step);
            debug("exp[%d] : %ld", i, *(exp + i));     
        }
        break;
    case 7: 
        for (int i = 0; i < nvar; i++) {
            *(exp + i) = (e & *(MASK_LIST_7V + i))>>(i*step);
            debug("exp[%d] : %ld", i, *(exp + i));     
        }
        break;
    case 8: 
        for (int i = 0; i < nvar; i++) {
            *(exp + i) = (e & *(MASK_LIST_8V + i))>>(i*step);
            debug("exp[%d] : %ld", i, *(exp + i));     
        }
        break;
    default:
        dbgerr("Not implemented. nvars : %d", nvar);
        exit(1);
        break;
    }
    debug("returning...");
    return exp;
}

/**
 * @brief traverse the ll and
 * free all the terms of polynomial
 * 
 * @param pol head of the ll
 */
void free_pol_t(pol_t * pol) {
    pol_t * curr;
    
    while (pol) {
        curr = pol;
        pol = pol->nxt;
        free(curr);
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

void print_pol(pol_t * pol) {
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
        free(e);
    }
    printf("\n");
}