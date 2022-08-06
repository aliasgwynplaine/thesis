#ifndef _SPARSE_H
#define _SPARSE_H

#define CS_CSC(A)      (A && (A->nz == -1))
#define CS_TRIPLET(A)  (A && (A->nz >= 0))
#define CS_MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define CS_MIN(a,b)    (((a) < (b)) ? (a) : (b))
#define CS_FLIP(i)     (-(i)-2)
#define CS_UNFLIP(i)   (((i) < 0) ? CS_FLIP(i) : (i))
#define CS_MARKED(w,j) (w[j] < 0)
#define CS_MARK(w,j)   {w[j] = CS_FLIP(w[j]);}

#define CS_OK   1
#define CS_FAIL 0

#include <stdio.h>

typedef struct cs_sparse {
    int nzmax;  /* max num o entries */
    int m;      /* row sz   */
    int n;      /* col sz   */
    int * i;    /* row idex */
    int * p;    /* columns  */
    double * x; /* values   */
    int nz;     /* num o entries in triplet. -1 if colcomp */
} cs;

typedef struct csr_mat {
    int nzmax;  /* max num o entries */
    int m;      /* row sz   */
    int n;      /* col sz   */
    int * i;    /* col idex */
    int * p;    /* columns  */
    double * x; /* values   */
    int nz;     /* num o entries in triplet. -1 if colcomp */
} csr_mat;

int gapxpy(const cs * A, const double * x, double * y);
int gatxpy(const cs * A, const double * x, double * y);
int cs_lsolve(const cs * L, double *x);
int cs_ltsolve(const cs *L, double * x);
int cs_usolve(const cs *U, double *x);
cs * cs_compress(const cs * T);
cs * cs_find(const cs * C);
cs * cs_transpose(const cs * A, int values);
int cs_dupl(const cs * A);
int cs_fkeep(cs * A, int (*fkeep) (int, int, double, void*), void *other);
cs *cs_multiply(const cs *A, const cs *B);
int cs_scatter(const cs *A, int j, double beta, int *w, double *x, int mark, cs *C, int nz);
cs *cs_add(const cs *A, const cs *B, double alpha, double beta);
int reachr(const cs *L, const cs *B, int *xi, int *w);
void dfsr(int j, const cs *L, int *top, int *xi, int *w);
int cs_reach(cs *G, const cs *B, int k, int *xi, const int *pinv);
int cs_dfs(int j, cs *G, int top, int *xi, int *pstack, const int *pinv);
cs * cs_load(FILE *f);
double cs_norm(const cs *A);
int cs_print(const cs *A, int brief);

/* 
  memory mngmt mostly form T. Davis 
  https://people.sc.fsu.edu/~jburkardt/c_src/csparse/
*/
void *cs_malloc (int n, size_t size);
void *cs_calloc (int n, size_t size);
cs * cs_done(cs * C, void *w, void *x, int ok); // 0: fail 1: ok
double cs_cumsum(int *p, int *c, int n);
void *cs_free (void *p);
void *cs_realloc (void *p, int n, size_t size, int *ok);
cs *cs_spalloc (int m, int n, int nzmax, int values, int triplet);
int cs_sprealloc (cs *A, int nzmax);
cs *cs_spfree (cs *A);


#endif