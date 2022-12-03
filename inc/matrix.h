#ifndef _MATRIX_H
#define _MATRIX_H

#include "outils.h"
#include "tree.h"
#include "memory.h"

typedef struct mlvector_t      ml_t; // multiline vector
typedef struct smatrix_t      csr_t; // compressed sparse row matrix
typedef struct smatrix_t      csc_t; // compressed sparse column matrix
typedef struct smatrix_t       sm_t; // generic sparce matrix
typedef struct mmatrix_t  mmatrix_t; // macaulay matrix 
typedef struct flmatrix_t     flm_t; // faugere-lachartre matrix
typedef struct stbmatrix_t   stbm_t;
typedef struct srbmatirx_t   srbm_t;
typedef struct hrbmatrix_t   hrbm_t;


#include "pol.h"

struct mlvector_t {
    int      * pos; /* position      */
    COEFTYPE * val; /* values        */
    int         sz; /* size          */
    int        cap; /* capacity      */
    u8           n; /* n-line vector */
};


/**
 * @brief sparse matrix struct
 * based on Davis CC sparse matrix.
 * You are using this just to begin.
 * Eventually, this must be changed 
 * by Faugere-Lacharte matrix for fast
 * Groebner basis calculation
 */
struct smatrix_t {
    int   nnzmax;     /* max num o entries */
    int        m;     /* row sz   */
    int        n;     /* col sz   */
    int      * i;     /* row/col idex */
    int      * p;     /* row/col pos */
    COEFTYPE * x;     /* values   */
    int      nnz;     /* num o entries in triplet. -1 if colcomp */
};



struct mmatrix_t {
    u64       * exps; // multigrads
    sm_t * mat;  // matrix
};


struct flmatrix_t {
    stbm_t * a;
    srbm_t * b;
    hrbm_t * c;
    int         nnz;
};


struct stbm_t {
    COEFTYPE * val;
    int      * pos;
    int      *  nb;
    float    *  sp;  // sparcity %
    int         sz;
};

/*
    memory handling
*/

mmatrix_t * mmatrix_malloc(int m, int n, int nnzmax);
void        mmatrix_free(mmatrix_t * mmat);

csr_t * csr_malloc(int m, int n, int nnzmax);
csr_t * csr_realloc(csr_t * csrmatrix, size_t sz);      // todo
csr_t * csr_load(FILE * f);
void    csr_free(csr_t *);

csc_t * csc_malloc(int m, int n, int nnzmax);
csc_t * csc_realloc(sm_t * smat, size_t sz);       // todo
csc_t * csc_load(FILE * f);                             // todo
void    smatrix_free(sm_t * smat);                 // todo

/*
    insert & delete
*/

int smatrix_entry(sm_t * smat, int i, int j, COEFTYPE x);


/* matpol transformations*/

// mmatrix_t * aapol2mmatrix(aapol_t * laapol, int sz);
csc_t * aapol2csc(aapol_t ** laapol, int sz); // todo: convert aapol 2 csc. look up in trash
csr_t * aapol2csr(aapol_t ** laapol, int sz); // todo: convert aapol 2 csr. look up in trash
flm_t * csr2flm(csr_t * csr);                 // todo: convert csr_t to block sparse fgmatrix
flm_t * aapol2flm(aapol_t ** laapol, int sz); // todo: convertaapol 2 fgmatrix




void csr_print(csr_t *);

#endif