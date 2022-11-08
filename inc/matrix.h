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
    int    nzmax;     /* max num o entries */
    int        m;     /* row sz   */
    int        n;     /* col sz   */
    int      * i;     /* row idex */
    int      * p;     /* columns  */
    COEFTYPE * x;     /* values   */
    int       nz;     /* num o entries in triplet. -1 if colcomp */
};


/*
    memory handling
*/

smatrix_t * smatrixmalloc(size_t sz);
smatrix_t * smatrixrealloc(smatrix_t * smat, size_t sz);
void        freesmatrix(smatrix_t * smat);

/*
    insert & delete
*/

int smatrixentry(smatrix_t * smat, int i, int j, COEFTYPE x);

#endif