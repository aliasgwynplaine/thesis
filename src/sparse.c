#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "sparce.h"

int gapxpy(const cs * A, const double * x, double * y) {
    int p, j, n, *Ap, *Ai;
    double * Ax;

    if (!CS_CSC(A) || !x || !y) return 0;

    n = A->n; Ap = A->p; Ai = A->i; Ax = A->x;

    for (j = 0; j < n; j++) {
        for (p = Ap[j]; p < Ap[j+1]; p++) {
            y[Ai[p]] += Ax[p] * x[j];
        }
    }

    return 1;
}

int gatxpy(const cs * A, const double * x, double * y) {
    int p, j, n, *Ap, *Ai;
    double *Ax;

    if (!CS_CSC(A) || !x || !y) return 0; /* 0: false */

    n = A->n; Ap = A->p; Ai = A->i; Ax = A->x;

    for (j = 0; j < n; j++) {
        for (p = Ap[j]; p < Ap[j+1]; p++) {
            y[j] += Ax[p] * x[Ai[p]];
        }
    }

    return 1;
}


cs * cs_find(const cs * C) {
    int m, n, nz, p, *Cp, *Ci, *w, *Ti, *Tj;
    double *Cx, *Tx;
    cs * T;

    if (!CS_CSC(C)) return NULL;
    // todo : complete
}


cs * cs_done(cs * C, void *w, void *x, int ok) {
    cs_free(w);
    cs_free(x);
    return ok ? C : cs_spfree(C);
}


double cs_cumsum(int *p, int *c, int n) {
    int i, nz = 0;
    double nz2 = 0;

    if (!p || !c) return 0;

    for (i = 0; i < n; i++) {
        p[i] = nz;
        nz  += c[i];
        nz2 += c[i];
        c[i] = p[i];
    }

    p[n] = nz;
    return nz2;
}

cs * cs_transpose(const cs * A, int values) {
    int p, q, j, *Cp, *Ci, n, m, *Ap, *Ai, *w;
    double *Cx, *Ax;
    cs *C;
    
    if( !CS_CSC(A)) return NULL;

    m = A->m; n = A->n; Ap = A->p; Ai = A->i; Ax = A->x;
    C = cs_spalloc(n, m, Ap[n], values && Ax, 0/* triplet: false*/);
    w = cs_calloc(m, sizeof(int));

    if (!C || !w ) return cs_done(C, w, NULL, 0);
    
    Cp = C->p; Ci = C->i; Cx = C->x;

    for (p = 0; p < Ap[n]; p++) w[Ai[p]]++;

    cs_cumsum(Cp, w, m);

    for (j = 0; j < n; j++) {
        for(p = Ap[j]; p < Ap[j+1]; p++) {
            Ci[q = w[Ai[p]]++] = j;
            if (Cx) Cx[q] = Ax[p];
        }
    }

    return cs_done(C, w, NULL, 1);
}


int cs_dupl(const cs * A) {
    int i,j, p, q, nz = 0, n, m, *Ap, *Ai, *w;
    double *Ax;

    if (!CS_CSC(A)) return 0;
    m = A->m; n = A->n; Ap = A->p; Ai = A->i; Ax = A->x;
    w = cs_malloc(m, sizeof(int));

    if (!w) return 0;

    for (i = 0; i < m; i++) w[i] = -1;

    for (j = 0; j < n; j++) {
        q = nz;

        for (p = Ap[j]; p < Ap[j+1]; p++) {
            i = Ai[p];

            if (w[i] >= q) {
                Ax[w[i]] +=Ax[p];
            } else {
                w[i]     = nz;
                Ai[nz]   = i;
                Ax[nz++] = Ax[p];
            }
        }

        Ap[j] = q;
    }

    Ap[n] = nz;
    cs_free(w);

    return 1;
}


int cs_fkeep(cs * A, int (*fkeep) (int, int, double, void*), void *other) {
    int j, p, nz = 0, n, *Ap, *Ai;
    double *Ax;

    if (!CS_CSC(A) || !fkeep) return -1;

    n = A->n; Ap = A->p; Ai = A->i; Ax = A->x;

    for (j = 0; j < n; j++) {
        p = Ap[j];
        Ap[j] = nz;

        for (/* empty */; p < Ap[j+1]; p++) {
            if (fkeep(Ai[p], j, Ax ? Ax[p] : 1, other)) {
                if (Ax) Ax[nz] = Ax[p];
                Ai[nz++] = Ai[p];
            }
        }
    }
    
    Ap[n] = nz;
    cs_sprealloc(A, 0);
    return nz;
}

static int cs_nonzero(int i, int j, double aij, void *other) {
    return aij != 0;
}

int cs_dropzeros(cs *A) { return cs_fkeep(A, &cs_nonzero, NULL); }

static int cs_tol(int i, int j, double aij, void *tol) {
    return fabs(aij) > *((double *) tol);
}

int cs_droptol(cs *A, double tol) {
    return cs_fkeep(A, &cs_tol, &tol);
}


cs *cs_multiply(const cs *A, const cs *B) {
    int p, j, nz = 0, anz, *Cp, *Ci, *Bp, m, n, bnz, *w, values, *Bi;
    double *x, *Bx, *Cx;
    cs *C;
    if (!CS_CSC(A) || !CS_CSC(B)) return NULL;
    m = A->m; anz = A->p[A->n];
    n = B->n; Bp = B->p; Bi = B->i; Bx = B->x; bnz = B->p[B->n];
    w = cs_calloc(m, sizeof(int));
    values = (A->x != NULL) && (Bx != NULL);
    x = values ? cs_malloc(m, sizeof(double)) : NULL;
    C = cs_spalloc(m, n, anz + bnz, values, 0);

    if (!C || !w || (values && !x)) return cs_done(C, w, x, 0);
    
    Cp = C->p;

    for (j = 0; j < n; j++) {
        if (nz + m > C-> nzmax && !cs_sprealloc(C, 2*(C->nzmax) + m)) {
            return cs_done(C, w, x, 0);
        }

        Ci = C->i; Cx = C->x;
        Cp[j] = nz;
        for (p = Bp[j]; p < Bp[j+1]; p++) {
            nz = cs_scatter(A, Bi[p], Bx ? Bx[p] : 1, w, x, j+1, C, nz);
        }

        if (values) for (p = Cp[j]; p < nz; p++) Cx[p] = x[Ci[p]];
    }

    Cp[n] = nz;
    cs_sprealloc(C, 0);
    return cs_done(C, w, x, 1);
}

int cs_scatter(const cs *A, int j, double beta, int *w, double *x, int mark, 
    cs *C, int nz) {
    int i, p, *Ap, *Ai, *Ci;
    double *Ax;
    
    if(!CS_CSC(A) || !w || !CS_CSC(C)) return -1;
    
    Ap = A->p; Ai = A->i; Ax = A->x; Ci = C->i;

    for (p = Ap[j]; p < Ap[j+1]; p++) {
        i = Ai[p];

        if (w[i] < mark) {
            w[i] = mark;
            Ci[nz++] = i;

            if (x) x[i] = beta *Ax[p];

        } else if (x) x[i] += beta * Ax[p];
    }

    return nz;
}


cs *cs_add(const cs *A, const cs *B, double alpha, double beta) {
    int p, j, nz = 0, anz, *Cp, *Ci, *Bp, m, n, bnz, *w, values;
    double *x, *Bx, *Cx;
    cs *C;

    if (!CS_CSC(A) || !CS_CSC(B)) return NULL;

    m = A->m; anz = A->p[A->n];
    n = B->n; Bp = B->p; Bx = B->x; bnz = Bp[n];
    w = cs_calloc(m, sizeof(int));
    values = (A->x != NULL) && (Bx != NULL);
    x = values ? cs_malloc(m, sizeof(double)) : NULL;
    C = cs_spalloc(m, n, anz + bnz, values, 0);

    if (!C || !w || (values && !x)) return cs_done(C, w, x, 0);

    Cp = C->p; Ci = C->i; Cx = C->x;

    for (j = 0; j < n; j++) {
        Cp[j] = nz;
        nz = cs_scatter(A, j, alpha, w, x, j+1, C, nz);
        nz = cs_scatter(B, j, beta, w, x, j+1, C, nz);
        if (values) for (p = Cp[j]; p < nz; p++) Cx[p] =  x[Ci[p]];
    }

    Cp[n] = nz;
    cs_sprealloc(C, 0);
    return cs_done(C, w, x, 1);
}


int cs_lsolve(const cs * L, double *x) {
    int p, j, n, *Lp, *Li;
    double *Lx;

    if (!CS_CSC(L) || !x) return 0;

    n = L->n; Lp = L->p; Li = L->i; Lx = L->x;

    for (j = 0; j < n; j++) {
        x[j] /= Lx[Lp[j]];

        for (p = Lp[j] + 1; p < Lp[j+1]; p++) {
            x[Li[p]] -= Lx[p] * x[j];
        }
    }

    return 1;
}


int cs_ltsolve(const cs *L, double * x) {
    int p, j, n, *Lp, *Li;
    double *Lx;

    if (!CS_CSC(L) || !x) return 0;

    n = L->n; Lp = L->p; Li = L->i; Lx = L->x;

    for (j = n - 1; j >= 0; j--) {
        for (p = Lp[j] + 1; p < Lp[j+1]; p++) {
            x[j] -= Lx[p] * x[Li[p]];
        }

        x[j] /= Lx[Lp[j]];
    }

    return 1;
}


int cs_usolve(const cs *U, double *x) {
    int p, j, n, *Up, *Ui;
    double *Ux;

    if (!CS_CSC(U) || !x) return 0;

    n = U->n; Up = U->p; Ui = U->i; Ux = U->x;

    for(j = n-1; j >= 0; j--) {
        x[j] /= Ux[Up[j + 1] - 1];

        for (p = Up[j]; p < Up[j + 1] - 1; p++) {
            x[Ui[p]] -= Ux[p] * x[j];
        }
    }

    return 1;
}


int cs_utsolve(const cs *U, double *x) {
    int p, j, n, *Up, *Ui;
    double *Ux;

    if (!CS_CSC(U) || !x) return 0;

    n = U->n; Up = U->p; Ui = U->i; Ux = U->x;

    for (j = 0; j < n; j++) {
        for (p = Up[j]; p < Up[j + 1] - 1; p++) {
            x[j] -= Ux[p] * x[Ui[p]];
        }
    }

    x[j] /= Ux[Up[j + 1] - 1];

    return 1;
}


int reachr(const cs *L, const cs *B, int *xi, int *w) {
    int p, n = L->n;
    int top = n;

    for (p = B->p[0]; p < B->p[1]; p++) {
        if (w[B->i[p]] != 1)
            dfsr(B->i[p], L, &top, xi, w);
    }

    return top;
}


void dfsr(int j, const cs *L, int *top, int *xi, int *w) {
    int p;
    w[j] = 1;

    for (p = L->p[j]; p < L->p[j+1]; p++) {
        if (w[L->i[p]] != 1) 
            dfsr(L->i[p], L, top, xi, w);
    }

    xi[--(*top)] = j;
}


int cs_entry(cs * T, int i, int j, double x) {
    if (!CS_TRIPLET(T) || i < 0 || j < 0) return 0;
    if (T->nz >= T->nzmax || !cs_sprealloc(T, 2*(T->nzmax))) return 0;
    if (T->x) T->x[T->nz] = x;
    T->i[T->nz] = i;
    T->p[T->nz++] = j;
    T->m = CS_MAX(T->m, i+1);
    T->n = CS_MAX(T->n, j+1);
    return 1;
}


cs * cs_compress(const cs * T) {
    int m, n, nz, p, k, *Cp, *Ci, *w, *Ti, *Tj;
    double *Cx, *Tx;
    cs *C;
    if (!CS_TRIPLET(T)) return NULL;				/* check inputs */
    m = T->m; n = T->n; Ti = T->i;
    Tj = T->p; Tx = T->x ; nz = T->nz;
    C = cs_spalloc(m, n, nz, Tx != NULL, 0);		/* allocate result */
    w = cs_calloc(n, sizeof (int));			/* get workspace */  
    
    if (!C || !w) return cs_done(C, w, NULL, 0);	/* out of memory */
    
    Cp = C->p; Ci = C->i; Cx = C->x;
    
    for (k = 0 ; k < nz ; k++) w [Tj [k]]++;		/* column counts */
   
    cs_cumsum(Cp, w, n);				/* column pointers */
    
    for (k = 0; k < nz; k++)
    {
        Ci [p = w [Tj [k]]++] = Ti [k];    /* A(i,j) is the pth entry in C */
        if (Cx) Cx [p] = Tx [k];
    }
    return cs_done (C, w, NULL, 1);	    /* success; free w and return C */
}


cs * cs_load(FILE *f) {
    int i, j;
    double x;
    cs *T;

    if (!f) return NULL;

    T = cs_spalloc(0, 0, 1, 1, 1);
    
    while (fscanf(f, "%d %d %lg\n", &i, &j, &x) == 3) {
        if (!cs_entry(T, i, j, x)) return (cs_spfree(T));
    }

    return T;
}

double cs_norm(const cs *A) {
    int p, j, n, *Ap;
    double *Ax, norm = 0, s;

    if (!CS_CSC(A) || !A->x) return -1;

    n = A->n; Ap = A->p; Ax = A->x;

    for (j = 0; j < n; j++) {
        for (s = 0, p = Ap[j]; p < Ap[j+1]; p++)
            s += fabs(Ax[p]);
        norm = CS_MAX(norm, s);
    }

    return norm;
}

int cs_print(const cs *A, int brief) {
    int p, j, m, n, nzmax, nz, *Ap, *Ai;
    double *Ax;

    if (!A) { printf("(null)\n"); return 0; }

    m = A->m; n = A->n; Ap = A->p; Ai = A->i; Ax =A->x;
    nzmax = A->nzmax; nz = A->nz;
    
    if (nz < 0) {
        printf(
            "%d-by-%d, nzmax: %d nnz: %d, 1-norm: %f\n",
            m,      n,      nzmax,    Ap[n], cs_norm(A)
        );

        for (j = 0; j < n; j++) {
            printf(" col %d : locations %d to %d\n", j, Ap[j], Ap[j+1] - 1);

            for (p = Ap[j]; p < Ap[j+1]; p++) {
                printf("    %d : %g\n", Ai[p], Ax ? Ax[p] : 1);
                if (brief && p > 20) { printf(" ...\n"); return 1; }
            }
        }
    } else {
        printf("triplet: %d-by-%d, nzmax: %d nnz: %d\n", m, n, nzmax, nz);

        for (p = 0; p < nz; p++) {
            printf (" %d %d : %g\n", Ai [p], Ap [p] , Ax ? Ax [p] : 1);
            if (brief && p > 20) { printf(" ...\n"); return 1; }
        }
    }

    return 1;
}



void *cs_malloc (int n, size_t size) {
    return (malloc (CS_MAX (n,1) * size));
}


void *cs_calloc (int n, size_t size) {
    return (calloc (CS_MAX (n,1), size)) ;
}


void *cs_free (void *p) {
    if (p) free (p);
    return (NULL);
}


void *cs_realloc (void *p, int n, size_t size, int *ok) {
    void *pnew;
    pnew = realloc (p, CS_MAX (n,1) * size);
    *ok = (pnew != NULL);
    return ((*ok) ? pnew : p);
}

/* allocate a sparse matrix (triplet form or compressed-column form) */
cs *cs_spalloc (int m, int n, int nzmax, int values, int triplet) {
    cs *A = cs_calloc (1, sizeof (cs));
    if (!A) return (NULL);
    A->m = m ;
    A->n = n ;
    A->nzmax = nzmax = CS_MAX (nzmax, 1);
    A->nz = triplet ? 0 : -1;
    A->p = cs_malloc (triplet ? nzmax : n+1, sizeof (int));
    A->i = cs_malloc (nzmax, sizeof (int));
    A->x = values ? cs_malloc (nzmax, sizeof (double)) : NULL;
    return ((!A->p || !A->i || (values && !A->x)) ? cs_spfree (A) : A);
}

/* change the max # of entries sparse matrix */
int cs_sprealloc (cs *A, int nzmax) {
    int ok, oki, okj = 1, okx = 1 ;
    if (!A) return (0) ;
    nzmax = (nzmax <= 0) ? (A->p [A->n]) : nzmax ;
    A->i = cs_realloc (A->i, nzmax, sizeof (int), &oki) ;
    if (A->nz >= 0) A->p = cs_realloc (A->p, nzmax, sizeof (int), &okj) ;
    if (A->x) A->x = cs_realloc (A->x, nzmax, sizeof (double), &okx) ;
    ok = (oki && okj && okx) ;
    if (ok) A->nzmax = nzmax ;
    return ok ;
}


cs *cs_spfree (cs *A) {
    if (!A) return (NULL);
    cs_free (A->p);
    cs_free (A->i);
    cs_free (A->x);
    return (cs_free (A));
}
