#ifndef _MATRIX_H
#define _MATRIX_H

#include "outils.h"

typedef struct smatrix_t  smatrix_t;   // generic sparce matrix
typedef struct fgmatrix_t fgmatrix_t; // faugere matrix

/**
 * @brief sparse matrix struct
 * based on Davis CC sparse matrix.
 * You are using this just to begin.
 * Eventually, this must be changed 
 * by Faugere-Lacharte matrix for fast
 * Groebner basis calculation
 * 
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


/*
    memory handling
*/

smatrix_t * smatrix_malloc(int m, int n, int nnzmax);
smatrix_t * smatrixrealloc(smatrix_t * smat, size_t sz);
void        smatrix_free(smatrix_t * smat);

/*
    insert & delete
*/

int smatri_xentry(smatrix_t * smat, int i, int j, COEFTYPE x);

#endif