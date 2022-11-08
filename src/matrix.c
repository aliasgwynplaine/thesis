#include "matrix.h"

smatrix_t * smatrix_malloc(int m, int n, int nnzmax) {
    smatrix_t * smat = calloc(1, sizeof(smatrix_t));
    TESTPTR(smat);
    smat->nnz    = 0;
    smat->m      = __max(m, 0);
    smat->n      = __max(n, 0);
    smat->nnzmax = __max(nnzmax, 1);
    smat->p = calloc((n + 1), sizeof(int));
    TESTPTR(smat->p);
    smat->i = malloc(sizeof(int) * nnzmax);
    TESTPTR(smat->i);
    smat->x = malloc(sizeof(COEFTYPE) * nnzmax);
    TESTPTR(smat->x);

    return smat;
}
smatrix_t * smatrixrealloc(smatrix_t * smat, size_t sz);


void smatrix_free(smatrix_t * smat) {
    if (!smat) return;
    FREE(smat->i);
    FREE(smat->p);
    FREE(smat->x);
    FREE(smat);
}

int smatri_xentry(smatrix_t * smat, int i, int j, COEFTYPE x) {
    if (i < 0 || j < 0) return 0;
    if (smat->nnz >= smat->nnzmax) {
        // get more memory;
        return 0;
    }

    if (smat->x) smat->x[smat->nnz] = x;
    smat->i[smat->nnz]   = i;
    smat->p[smat->nnz++] = j;
    smat->m = __max(smat->m, i+1);
    smat->n = __max(smat->n, j+1);
    return 1;
}