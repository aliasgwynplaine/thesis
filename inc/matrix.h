#ifndef _MATRIX_H
#define _MATRIX_H

#include "outils.h"
#include "tree.h"
#include "memory.h"

typedef struct mlvector_t         ml_t;
typedef struct smatrix_t         csr_t; // 
typedef struct smatrix_t         csc_t; // 
typedef struct smatrix_t     smatrix_t; // generic sparce matrix
typedef struct mmatrix_t     mmatrix_t; // macaulay matrix 
typedef struct fgmatrix_t   fgmatrix_t; // faugere matrix
typedef struct stbmatrix_t stbmatrix_t;
typedef struct srbmatirx_t srbmatrix_t;
typedef struct hrbmatrix_t hrbmatrix_t;


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
    smatrix_t * mat;  // matrix
};


struct fgmatrix_t {
    stbmatrix_t * a;
    srbmatrix_t * b;
    hrbmatrix_t * c;
    int         nnz;
};


struct stbmatrix_t {
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
csc_t * csc_realloc(smatrix_t * smat, size_t sz);       // todo
csc_t * csc_load(FILE * f);                             // todo
void    smatrix_free(smatrix_t * smat);                 // todo

/*
    insert & delete
*/

int smatrix_entry(smatrix_t * smat, int i, int j, COEFTYPE x);


/* matpol transformations*/

mmatrix_t * aapol2mmatrix(aapol_t * laapol, int sz);

void csr_print(csr_t *);

#endif