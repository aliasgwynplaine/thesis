#include "matrix.h"


typedef struct colsmat_t  colsmat_t;
/**
 * @brief stores information of a 
 * column for the list of pols to 
 * CC sparse matrix transformation
*/
struct colsmat_t {
    u64      exp;   /* exponent */
    int        p;   /* numnber of entries in col */
    int      * i;   /* row index for each entry  */
    COEFTYPE * x;   /* entry   */
};


mm_t * mmatrix_malloc(int m, int n, int nnzmax) {
    mm_t * mmat = malloc(sizeof(mm_t));
    mmat->exps = calloc(n, sizeof(u64));
    mmat->mat  = csc_malloc(m, n, nnzmax);

    return mmat;
}


void mmatrix_free(mm_t * mmat) {
    FREE(mmat->exps);
    smatrix_free(mmat->mat); // todo: change this
    FREE(mmat);
}

csr_t * csr_malloc(int m, int n, int nnzmax) {
    csr_t * csr = calloc(1, sizeof(sm_t));
    CHECKPTR(csr);
    csr->nnz    = 0;
    csr->m      = __max(m, 0);
    csr->n      = __max(n, 0);
    csr->nnzmax = __max(nnzmax, 1);
    csr->p = calloc((m + 1), sizeof(int));
    CHECKPTR(csr->p);
    csr->i = malloc(sizeof(int) * nnzmax);
    CHECKPTR(csr->i);
    csr->x = malloc(sizeof(COEFTYPE) * nnzmax);
    CHECKPTR(csr->x);

    return csr;
}


/**
 * @brief loads a csr matrix in matrix market
 * format.
*/
csr_t * csr_load(FILE * f) {
    int i, j;
    int m, n, sz;
    COEFTYPE x;
    csr_t * csr;

    if (!f) return NULL;
    if (fscanf(f, "%d %d %d\n", &m, &n, &sz) != 3) SAYNEXITWERROR("error reading matrix");

    csr = csr_malloc(m, n, sz);

    int i_idx = 0;
    int p_idx = 0;

    int * ci = malloc(sizeof(int) * sz);
    CHECKPTR(ci);
    int * cp = malloc(sizeof(int) * sz);
    CHECKPTR(cp);
    COEFTYPE * cx = malloc(sizeof(COEFTYPE) * sz);
    CHECKPTR(cx);
    int * w  = calloc(m, sizeof(int));
    CHECKPTR(w);

    while (fscanf(f, "%d %d %f\n", &i, &j, &x) == 3) {
        cp[i_idx]   = i;
        ci[i_idx]   = j;
        cx[i_idx++] = x;
    }

    for (int k = 0; k < sz; k++) w[cp[k]]++;

    int_cumsum(csr->p, w, m);

    for (int k = 0; k < sz; k++) {
        p_idx = w[cp[k]]++;
        csr->i[p_idx] = ci[k];
        csr->x[p_idx] = cx[k];
        csr->nnz++;
    }

    FREE(ci);
    FREE(cp);
    FREE(cx);
    FREE(w);

    return csr;
}


void csr_free(csr_t * csr) {
    if (csr->i != NULL) FREE(csr->i);
    if (csr->p != NULL) FREE(csr->p);
    if (csr->x != NULL) FREE(csr->x);
    if (csr != NULL) FREE(csr);
}


csc_t * csc_malloc(int m, int n, int nnzmax) {
    csc_t * smat = calloc(1, sizeof(sm_t));
    CHECKPTR(smat);
    smat->nnz    = 0;
    smat->m      = __max(m, 0);
    smat->n      = __max(n, 0);
    smat->nnzmax = __max(nnzmax, 1);
    smat->p = calloc((n + 1), sizeof(int));
    CHECKPTR(smat->p);
    smat->i = malloc(sizeof(int) * nnzmax);
    CHECKPTR(smat->i);
    smat->x = malloc(sizeof(COEFTYPE) * nnzmax);
    CHECKPTR(smat->x);

    return smat;
}


void smatrix_free(sm_t * smat) {
    if (!smat) return;
    FREE(smat->i);
    FREE(smat->p);
    FREE(smat->x);
    FREE(smat);
}


u64 csr_head(csr_t * csr, u64 rw_idx) {
    if (rw_idx < 0 || rw_idx >= csr->m) 
        SAYNEXITWERROR("rw_idx out of bounds");
    if (csr->p[rw_idx] == csr->p[rw_idx + 1])
        return -1;
    return csr->i[csr->p[rw_idx]];
}

u32 csr_width(csr_t * csr, u64 rw_idx) {
    if (rw_idx < 0 || rw_idx >= csr->m) 
        SAYNEXITWERROR("rw_idx out of bounds");
    
    return (u32)(csr->p[rw_idx + 1] - csr->p[rw_idx]);
}

void csr_swap_col(csr_t * csr, idx_t i, idx_t j) {
    if (j == i) return;
    idx_t tmp;
    COEFTYPE x_tmp;
    if (i > j) SWAP(i, j, tmp);
    //printf("i: %d, j: %d\n", i, j);

    idx_t p;
    idx_t r;
    int pf = true;
    int rf = true;

    for (idx_t idx = 0; idx < csr->m; idx++) {
        pf = true;
        rf = true;

        
        p = csr->p[idx];
        r = csr->p[idx + 1] - 1;

        for ( ; pf || rf; ) {
            if (csr->i[p] < i && p < csr->p[idx+1]) {
                p++;
            } else {
                pf = false;
            }

            if (csr->i[r] > j && r >= csr->p[idx]) {
                r--;
            } else {
                rf = false;
            }
        }

        if (csr->i[p] == i && csr->i[r] == j) {
            SWAP(csr->x[p], csr->x[r], x_tmp);
        } else if (csr->i[p] == i) {
            csr->i[p++] = j;

            for ( ; csr->i[p] < j; p++) {
                if (p >= csr->p[idx + 1]) break;
                SWAP(csr->i[p-1], csr->i[p], tmp);
                SWAP(csr->x[p-1], csr->x[p], x_tmp);
            }

        } else if (csr->i[r] == j) {
            csr->i[r--] = i;

            for ( ; csr->i[r] > i; r--) {
                if (r < csr->p[idx]) break;
                SWAP(csr->i[r+1], csr->i[r], tmp);
                SWAP(csr->x[r+1], csr->x[r], x_tmp);
            }
        }
    }
    endl;
}

int smatrix_entry(sm_t * smat, int i, int j, COEFTYPE x) {
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
/**
 * @brief decompose the csr matrix in flmatrix
*/
flm_t * csr_real_decompose(csr_t * csr, u32 blk_sz) {
    dctx_t * ctx = csr_analyse(csr);
    idx_t cp = 0;
    idx_t p_idx = 0;
    idx_t r_idx = -1;
    idx_t c_idx;
    idx_t p;

    printf("Found %d npivs\n", ctx->npiv);
    idx_t cc = 0;

    printf("===================================++++++++\n");
    //csr_print(csr);
    printf("==================================++++++++\n");

    for (idx_t k = 0; cc < ctx->npiv; k++) {
        if (ctx->rpc[k] != -1 && ctx->rpc[k] != k) {
            printf("swaping %d <> %d -> ", k, ctx->rpc[k]);
            csr_swap_col(csr, ctx->rpc[k], k);
        }
        cc++;
    }

    printf("===================================****\n");
    //csr_print(csr);
    printf("===================================****\n");

    do {
        r_idx = ctx->pr[p_idx];
        while (r_idx == -1) {
            //printf("r_idx: %d\n", r_idx);
            r_idx = ctx->pr[++p_idx];
        }
        
        printf("%.2d - %.2d| ", cp, r_idx);
        p = csr->p[r_idx];

        for (int j = 0; j < csr->n; j++) {
            if (j == ctx->npiv) printf("| ");
            if (p < csr->nnz && j == csr->i[p]) {
                printf("%.0f ", csr->x[p]);
                // printf("%d %d - %d * %d\n", p_idx, p, csr->i[p], csr->n);
                p++;
            } else {
                printf("  ");
            }
            // printf("%d %d - %d * %d\n", p_idx, p, csr->i[p], csr->n);
            // p++;
        }
        printf("\n");
        p_idx++;
        cp++;
    } while(cp < ctx->npiv);

    idx_t ncp = 0;
    p_idx = 0;

    printf("-- - --| ");
    for(int i = 0; i < csr->n; i++) {
        printf("--");
    }
    endl;

    while(ncp < csr->m - ctx->npiv) {
        r_idx = ctx->npr[p_idx];
        while (r_idx == -1) {
            r_idx = ctx->npr[++p_idx];
            //printf("p_idx: %d r_idx: %d\n", p_idx, r_idx);
        }
        
        printf("%.2d - %.2d| ", cp, r_idx);
        p = csr->p[r_idx];

        for (int j = 0; j < csr->n; j++) {
            if (j == ctx->npiv) printf("| ");
            if (p < csr->nnz && j == csr->i[p]) {
                printf("%.0f ", csr->x[p++]);
            } else {
                printf("  ");
            }
        }
        printf("\n");
        p_idx++;
        ncp++;
        cp++;
    }

    return NULL;
}


/**
 * @brief decompose the csr matrix in flmatrix
*/
flm_t * csr_decompose(csr_t * csr, u32 blk_sz) {
    dctx_t * ctx = csr_analyse(csr);
    idx_t cp = 0;
    idx_t p_idx = 0;
    idx_t r_idx = -1;
    idx_t c_idx;
    idx_t p;

    printf("Found %d npivs\n", ctx->npiv);
    idx_t cc = 0;

    printf("===================================++++++++\n");
    //csr_print(csr);
    printf("==================================++++++++\n");

    for (idx_t k = 0; cc < ctx->npiv; k++) {
        if (ctx->rpc[k] != -1 && ctx->rpc[k] != k) {
            printf("swaping %d <> %d -> ", k, ctx->rpc[k]);
            csr_swap_col(csr, ctx->rpc[k], k);
        }
        cc++;
    }

    printf("===================================****\n");
    //csr_print(csr);
    printf("===================================****\n");

    do {
        r_idx = ctx->pr[p_idx];
        while (r_idx == -1) {
            //printf("r_idx: %d\n", r_idx);
            r_idx = ctx->pr[++p_idx];
        }
        
        printf("%.2d - %.2d| ", cp, r_idx);
        p = csr->p[r_idx];

        for (int j = 0; j < csr->n; j++) {
            if (j == ctx->npiv) printf("| ");
            if (p < csr->nnz && j == csr->i[p]) {
                printf("%.0f ", csr->x[p]);
                // printf("%d %d - %d * %d\n", p_idx, p, csr->i[p], csr->n);
                p++;
            } else {
                printf("  ");
            }
            // printf("%d %d - %d * %d\n", p_idx, p, csr->i[p], csr->n);
            // p++;
        }
        printf("\n");
        p_idx++;
        cp++;
    } while(cp < ctx->npiv);

    idx_t ncp = 0;
    p_idx = 0;

    printf("-- - --| ");
    for(int i = 0; i < csr->n; i++) {
        printf("--");
    }
    endl;

    while(ncp < csr->m - ctx->npiv) {
        r_idx = ctx->npr[p_idx];
        while (r_idx == -1) {
            r_idx = ctx->npr[++p_idx];
            //printf("p_idx: %d r_idx: %d\n", p_idx, r_idx);
        }
        
        printf("%.2d - %.2d| ", cp, r_idx);
        p = csr->p[r_idx];

        for (int j = 0; j < csr->n; j++) {
            if (j == ctx->npiv) printf("| ");
            if (p < csr->nnz && j == csr->i[p]) {
                printf("%.0f ", csr->x[p++]);
            } else {
                printf("  ");
            }
        }
        printf("\n");
        p_idx++;
        ncp++;
        cp++;
    }

    return NULL;
}


/**
 * @brief analyse the matrix to get the pivots
 * @param m sparce matrix in csr format
 * @return pointer to decomposer_ctx_t struct containing 
 * the pivots.
 * @note taken from gbla
*/
dctx_t * csr_analyse(csr_t * csr) {
    dctx_t * ctx = malloc(sizeof(dctx_t));
    CHECKPTR(ctx);
    idx_t max_sz = __max(csr->m, csr->n);
    idx_t idx;
    idx_t pc_idx;
    idx_t npc_idx;

    ctx->npiv = 0;
    ctx->pr  = malloc(sizeof(idx_t) * csr->n);
    CHECKPTR(ctx->pr);
    for (int i = 0; i < csr->n; i++) ctx->pr[i] = -1;
    
    ctx->npr = malloc(sizeof(idx_t) * max_sz);
    CHECKPTR(ctx->npr);
    for (int i = 0; i < max_sz; i++) ctx->npr[i] = -1;
    
    ctx->npc = malloc(sizeof(idx_t) * csr->n);
    CHECKPTR(ctx->npc);
    for (int i = 0; i < csr->n; i++) ctx->npc[i] = -1;

    ctx->rpc = malloc(sizeof(idx_t) * csr->n);
    CHECKPTR(ctx->npc);
    for (int i = 0; i < csr->n; i++) ctx->rpc[i] = -1;
    
    ctx->pc  = malloc(sizeof(idx_t) * csr->n);
    CHECKPTR(ctx->pc);
    for (int i = 0; i < csr->n; i++) ctx->pc[i] = -1;

    /* pivot n non-pivot rows */
    for (idx_t i = 0; i < csr->m; i++) {
        if (csr_width(csr, i) != 0) {
            idx = csr_head(csr, i);

            if (ctx->pr[idx] == -1) {
                ctx->pr[idx] = i;
                ctx->npiv++;
            } else {
                if (csr_width(csr, ctx->pr[idx]) > csr_width(csr, i)) {
                    ctx->npr[ctx->pr[idx]] = ctx->pr[idx];
                    ctx->pr[idx] = i;
                } else {
                    ctx->npr[i] = i;
                }
            }
        } else {
            ctx->npr[i] = i;
        }
    }

    pc_idx = 0;
    npc_idx = 0;

    for (idx_t j = 0; j < csr->n; j++) {
        if (ctx->pr[j] != -1) {
            ctx->pc[j] = pc_idx;
            ctx->rpc[pc_idx] = j;
            pc_idx++;
        } else {
            ctx->npc[j] = npc_idx;
            npc_idx++;
        }
    }

    return ctx;
}

void csr_print(csr_t * csr) {
    printf("m: %d, n: %d nnz: %d\n", csr->m, csr->n, csr->nnz);
    for (idx_t i = 0; i < csr->m; i++) {
        printf("row %d : loc %d : to %d\n", i, csr->p[i], csr->p[i+1] - 1);

        for (idx_t p = csr->p[i]; p < csr->p[i+1]; p++) {
            printf(" %d : %f\n", csr->i[p], csr->x[p]);
        }
    }
}

void csr_dense_print(csr_t * csr) {
    printf("csr->nnz : %d\n",csr->nnz);
    for (idx_t i = 0; i < csr->m; i++) {
        idx_t p = csr->p[i];
        printf("%.2d| ", i);

        for (idx_t j = 0; j < csr->n; j++) {
            if (p < csr->nnz && j == csr->i[p]) {
                printf("%.0f ", csr->x[p++]);
            } else {
                printf("  ");
            }
        }
        printf("\n");
    }
}

void dctx_print(dctx_t * ps, idx_t m, idx_t n) {
    if (ps == NULL) SAYNEXITWERROR("ps is null");
    printf("\nps->npiv: %d", ps->npiv);
    printf("%-12s", "\nps->pr:");
    
    for (idx_t i = 0; i < n; i++) {
        printf(ps->pr[i] < 0 ? "%d " : "%.2d ", ps->pr[i]);
    }
    endl;

    printf("%-12s", "\nps->npr:");
    for (idx_t i = 0; i < __max(m, n); i++) {
        printf(ps->npr[i] < 0 ? "%d " : "%.2d ", ps->npr[i]);
    }
    endl;

    printf("%-12s", "\nps->pc:");
    for (idx_t i = 0; i < n; i++) {
        printf(ps->pc[i] < 0 ? "%d " : "%.2d ", ps->pc[i]);
    }
    endl;

    printf("%-12s", "\nps->rpc:");
    for (idx_t i = 0; i < n; i++) {
        printf(ps->rpc[i] < 0 ? "%d " : "%.2d ", ps->rpc[i]);
    }
    endl;
    
    printf("%-12s", "\nps->npc:");
    for (idx_t i = 0; i < n; i++) {
        printf(ps->npc[i] < 0 ? "%d " : "%.2d ", ps->npc[i]);
    }
    endl;
    
}

void dctx_free(dctx_t * dctx) {
    if (dctx->pr != NULL) FREE(dctx->pr);
    if (dctx->npr != NULL) FREE(dctx->npr);
    if (dctx->pc != NULL) FREE(dctx->pc);
    if (dctx->npc != NULL) FREE(dctx->npc);
    FREE(dctx);
}
