#include "matrix.h"

smatrix_t * smatrixmalloc(size_t sz);
smatrix_t * smatrixrealloc(smatrix_t * smat, size_t sz);
void        freesmatrix(smatrix_t * smat);

int smatrixentry(smatrix_t * smat, int i, int j, COEFTYPE x) {
    if (i < 0 || j < 0) return 0;
    if (smat->nz >= smat->nzmax) {
        // get more memory;
        return 0;
    }

    if (smat->x) smat->x[smat->nz] = x;
    smat->i[smat->nz]   = i;
    smat->p[smat->nz++] = j;
    smat->m = __max(smat->m, i+1);
    smat->n = __max(smat->n, j+1);
    return 1;
}
