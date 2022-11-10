#ifndef _MATRIX_H
#define _MATRIX_H

#include "outils.h"
#include "tree.h"
#include "memory.h"

typedef struct mmatrix_t   mmatrix_t;  // macaulay matrix 
typedef struct smatrix_t   smatrix_t;  // generic cc sparce matrix
typedef struct fgmatrix_t  fgmatrix_t; // faugere matrix
typedef struct stbmatrix_t stbmatrix_t;
typedef struct srbmatirx_t srbmatrix_t;
typedef struct hrbmatrix_t hrbmatrix_t;


#include "pol.h"


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
    int      * i;     /* row idex */
    int      * p;     /* columns  */
    COEFTYPE * x;     /* values   */
    int      nnz;     /* num o entries in triplet. -1 if colcomp */
};



struct mmatrix_t {
    u64       * exps; // multigrads
    smatrix_t * mat;  // matrix
};


struct fgmatrix_t {
    stbmatrix_t * stbmat;
    srbmatrix_t * srbmat;
    hrbmatrix_t * hrbmat;
    int              nnz;
};


/*
    memory handling
*/

mmatrix_t * mmatrix_malloc(int m, int n, int nnzmax);
void        mmatrix_free(mmatrix_t * mmat);


smatrix_t * smatrix_malloc(int m, int n, int nnzmax);
smatrix_t * smatrixrealloc(smatrix_t * smat, size_t sz);
void        smatrix_free(smatrix_t * smat);

/*
    insert & delete
*/

int smatrix_entry(smatrix_t * smat, int i, int j, COEFTYPE x);


/* matpol transformations*/

mmatrix_t * aapol2mmatrix(aapol_t * laapol, int sz);


#endif