#include "matrix.h"





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
    if (f == NULL) return NULL;
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


void flsm_print(flsm_t * flsm) {

    printf("nnz: %d\n", flsm->nnz);
    for (int i = 0; i < flsm->m; i++) {
        printf("%.2d  ", i);

        for (int j = 0; j < flsm->width[i]; j++) {
            printf("%.0f ", flsm->rows[i][j]);
        }

        endl;
        printf("    ");

        for (int j = 0; j < flsm->width[i]; j++) {
            printf("%d ", flsm->pos[i][j]);
        }

        endl;
    }
}


void flsm_free(flsm_t * flsm) {
    if (flsm == NULL) return;
    if (flsm->rows != NULL) {
        for (int i = 0; i < flsm->m; i++) {
            if (flsm->rows[i] != NULL) 
                FREE(flsm->rows[i]);
        }
        FREE(flsm->rows);
    }
    if (flsm->pos != NULL) {
        for (int i = 0; i < flsm->m; i++) {
            if (flsm->pos[i] != NULL) 
                FREE(flsm->pos[i]);
        }
        FREE(flsm->pos);
    }
    if (flsm->width != NULL) FREE(flsm->width);

    FREE(flsm);
}

dctx_t *flsm_analyse(flsm_t * flsm) {
    dctx_t * ctx = malloc(sizeof(dctx_t));
    CHECKPTR(ctx);
    idx_t max_sz = __max(flsm->m, flsm->n);
    idx_t idx;
    idx_t pc_idx;
    idx_t npc_idx;

    ctx->npiv = 0;
    ctx->pr  = malloc(sizeof(idx_t) * flsm->n);
    CHECKPTR(ctx->pr);
    for (int i = 0; i < flsm->n; i++) ctx->pr[i] = -1;
    
    ctx->npr = malloc(sizeof(idx_t) * max_sz);
    CHECKPTR(ctx->npr);
    for (int i = 0; i < max_sz; i++) ctx->npr[i] = -1;
    
    ctx->npc = malloc(sizeof(idx_t) * flsm->n);
    CHECKPTR(ctx->npc);
    for (int i = 0; i < flsm->n; i++) ctx->npc[i] = -1;

    ctx->rpc = malloc(sizeof(idx_t) * flsm->n);
    CHECKPTR(ctx->npc);
    for (int i = 0; i < flsm->n; i++) ctx->rpc[i] = -1;

    ctx->rnpc = malloc(sizeof(idx_t) * flsm->n);
    CHECKPTR(ctx->npc);
    for (int i = 0; i < flsm->n; i++) ctx->rnpc[i] = -1;
    
    ctx->pc  = malloc(sizeof(idx_t) * flsm->n);
    CHECKPTR(ctx->pc);
    for (int i = 0; i < flsm->n; i++) ctx->pc[i] = -1;

    /* pivot n non-pivot rows */
    for (idx_t i = 0; i < flsm->m; i++) {
        if (flsm->width[i] != 0) {
            idx = flsm->pos[i][0];

            if (ctx->pr[idx] == -1) {
                ctx->pr[idx] = i;
                ctx->npiv++;
            } else {
                if (flsm->width[ctx->pr[idx]] > flsm->width[i]) {
                    ctx->npr[ctx->pr[idx]] = ctx->pr[idx];
                    ctx->pr[idx] = i;
                } else {
                    ctx->npr[i] = i;
                }
            }
        } else {
            printf("width zero! %d\n", i);
            ctx->npr[i] = i;
        }
    }

    pc_idx = 0;
    npc_idx = 0;

    for (idx_t j = 0; j < flsm->n; j++) {
        if (ctx->pr[j] != -1) {
            ctx->pc[j] = pc_idx;
            ctx->rpc[pc_idx] = j;
            pc_idx++;
        } else {
            ctx->npc[j] = npc_idx;
            ctx->rnpc[npc_idx] = j;
            npc_idx++;
        }
    }

    return ctx;
}

flsm_t *csr2flsm(csr_t *csr) {
    if (csr == NULL) return NULL;
    
    flsm_t * flsm = malloc(sizeof(flsm_t));
    flsm->m = csr->m;
    flsm->n = csr->n;
    flsm->nnz = csr->nnz;
    flsm->width = malloc(sizeof(int) * flsm->m);
    flsm->rows  = malloc(sizeof(COEFTYPE *) * flsm->m);
    flsm->pos   = malloc(sizeof(int *) * flsm->m);

    for (idx_t i = 0; i < csr->m; i++) {
        flsm->width[i] = csr_width(csr, i);
        flsm->rows[i] = malloc(sizeof(COEFTYPE) * flsm->width[i]);
        flsm->pos[i] = malloc(sizeof(int) * flsm->width[i]);
        COEFTYPE * rows = flsm->rows[i];
        int      * pos  = flsm->pos[i];
        idx_t k = 0;

        for (idx_t j = csr->p[i]; j < csr->p[i + 1]; j++) {
            rows[k] = csr->x[j];
            pos[k]  = csr->i[j];
            k++;
        }
    }

    return flsm;
}

idx_t csr_head_idx(csr_t * csr, idx_t rw_idx) {
    if (rw_idx < 0 || rw_idx >= csr->m) 
        SAYNEXITWERROR("rw_idx out of bounds");
    if (csr->p[rw_idx] == csr->p[rw_idx + 1])
        return -1;
    return csr->i[csr->p[rw_idx]];
}

idx_t csr_width(csr_t * csr, idx_t rw_idx) {
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

        for ( ; pf || rf; ) { // could be a binsearch
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


void nsm_print(nsm_t * nsm) {
    printf("m: %ld\n", nsm->m);
    printf("n: %ld\n", nsm->n);
    printf("nnz: %ld\n", nsm->nnz);
    printf("density: %f\n", nsm->d);

    for (int i = 0; i < nsm->m; i++) {
        printf("x: ");
        for (int j = 0; j < nsm->w[i]; j++) {
            printf("%f ", nsm->x[i][j]);
        }

        printf("\nc: ");
        for (int j = 0; j < nsm->w[i]; j++) {
            printf("%ld ", nsm->c[i][j]);
        }
        printf("\n");
    }
}


void nsm_free(nsm_t * nsm) {
    for (int i = 0; i < nsm->m; i++) {
        free(nsm->x[i]);
        free(nsm->c[i]);
    }

    free(nsm->x);
    free(nsm->c);
    free(nsm->w);

    free(nsm);
}


void nsm_rref(nsm_t * nsm) {
    assert(nsm != NULL);

    u64 * piv = malloc(nsm->m * sizeof(*piv));
    CHECKPTR(piv);
    u64 npiv  = 0;
    piv[npiv] = 0;

    for (int i = 1; i < nsm->m; i++) {
        if (nsm->w[i] == 0) continue;
        COEFTYPE * temp = sparse2dense(nsm->x[i], nsm->c[i], nsm->w[i], nsm->n);

        for (int j = 0; j <= npiv; j++) {
            redDenseAxpSparseY(temp, nsm->x[piv[j]], nsm->c[piv[j]], nsm->w[piv[j]]);
        }

        bool r = dense2sparse(temp, nsm->n, nsm, i);
        if (r) {
            piv[++npiv] = i;
            printf("piv: ");
            for (int k = 0; k <= npiv; k++) printf("%d ", piv[k]);
            printf("\n");
        }

        FREE(temp);
    }

    for (idx_t i = 0; i <= npiv; i++) {
        COEFTYPE * temp = sparse2dense(nsm->x[piv[i]], nsm->c[piv[i]], nsm->w[piv[i]], nsm->n);

        for (idx_t j = i+1; j <= npiv; j++) {
            if (nsm->c[piv[j]][0] > nsm->c[piv[i]][0]) {
                redDenseAxpSparseY(temp, nsm->x[piv[j]], nsm->c[piv[j]], nsm->w[piv[j]]);
            }
        }

        dense2sparse(temp, nsm->n, nsm, piv[i]);
        FREE(temp);
    }

    free(piv);
}


int redDenseAxpSparseY(COEFTYPE * a, COEFTYPE * x, u64 * c, int w) {
    if (a[c[0]] == 0) return 0;
    
    COEFTYPE alpha = a[c[0]] / x[0];
    int nne = 0;

    for (int i = 0; i < w; i++) {
        if (a[c[i]] == 0) nne++;
        *(a+c[i]) -= alpha * x[i];
    }

    return nne;
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
 * @brief transforms a sparse vector to a
 * dense vector.
 * 
 * @param v array of coefs
 * @param i array of index
 * @param n sparse vector size
 * @param dim dimention of the vector
 * 
 * @return dv dense vector
 * @note don't forget to free dv
 */
COEFTYPE * sparse2dense(COEFTYPE * v, u64 * i, u64 n, u64 dim) {
    COEFTYPE * dv = calloc(dim, sizeof(COEFTYPE));
    CHECKPTR(dv);

    for (idx_t j = 0; j < n; j++) {
        dv[i[j]] = v[j];
    }

    return dv;
}

bool dense2sparse(COEFTYPE * v, u64 dim, nsm_t * nsm, idx_t idx) {
    idx_t h = 0;
    bool retval; // tells me if there's a new entry and is not null

    COEFTYPE * xbuff = malloc(2 * nsm->w[idx] * sizeof(*xbuff));
    CHECKPTR(xbuff);
    u64 * cbuff = malloc(2 * nsm->w[idx] * sizeof(*cbuff));
    CHECKPTR(cbuff);

    for (idx_t i = 0; i < dim; i++) {
        if (v[i] != 0) {
            cbuff[h] = i;
            xbuff[h++] = v[i];

            if (h > 2 * nsm->w[idx]) {
                cbuff = realloc(cbuff, 2 * h * sizeof(*cbuff));
                CHECKPTR(cbuff);
                xbuff = realloc(xbuff, 2 * h * sizeof(*xbuff));
                CHECKPTR(xbuff);
            }
        }
    }

    if (h == 0) {
        printf("here!");
        retval = false;
        free(xbuff);
        free(cbuff);
        free(nsm->x[idx]);
        free(nsm->c[idx]);
        nsm->x[idx] = calloc(1, sizeof(*nsm->x[idx]));
        nsm->c[idx] = calloc(1, sizeof(*nsm->c[idx]));
        nsm->nnz -= nsm->w[idx];
        nsm->d = nsm->nnz / (nsm->m * nsm->n);
        nsm->w[idx] = 0;

        return retval;
    } else {
        idx_t diff = h - nsm->w[idx];
        
        if (diff > 0) {
            retval = true;
            goto lbl_nxt;
        } 

        /* diff <= 0 */

        for (idx_t i = 0; i < h; i++) {
            if (cbuff[i] != nsm->c[idx][i]) {
                retval = true;
                goto lbl_nxt;
            }
        }

        retval = false;
    }

    lbl_nxt:
    FREE(nsm->x[idx]);
    FREE(nsm->c[idx]);
    nsm->nnz = nsm->nnz + h - nsm->w[idx];
    nsm->d = (float) nsm->nnz / (nsm->m * nsm->n);
    nsm->w[idx] = h;
    nsm->x[idx] = malloc(h * sizeof(*nsm->x[idx]));
    CHECKPTR(nsm->x[idx]);
    nsm->c[idx] = malloc(h * sizeof(*nsm->c[idx]));
    CHECKPTR(nsm->c[idx]);
    memcpy(nsm->x[idx], xbuff, h * sizeof(*nsm->x[idx]));
    memcpy(nsm->c[idx], cbuff, h * sizeof(*nsm->c[idx]));
    FREE(xbuff);
    FREE(cbuff);

    return retval;
}


/**
 * @brief decompose the csr matrix in flmatrix
*/
tmat_t * csr_decompose(csr_t * csr) {
    dctx_t * ctx = csr_analyse(csr);
    idx_t max_sz = __max(csr->m, csr->n);
    flsm_t * a = calloc(1, sizeof(flsm_t));
    flsm_t * b = calloc(1, sizeof(flsm_t));
    flsm_t * c = calloc(1, sizeof(flsm_t));
    flsm_t * d = calloc(1, sizeof(flsm_t));

    a->m = ctx->npiv;
    a->n = a->m;
    a->width = malloc(sizeof(int) * a->m);
    a->rows  = malloc(sizeof(COEFTYPE *) * a->m);
    a->pos   = malloc(sizeof(int *) * a->m);

    b->m = ctx->npiv;
    b->n = csr->n - ctx->npiv;
    b->width = malloc(sizeof(int) * b->m);
    b->rows  = malloc(sizeof(COEFTYPE *) * b->m);
    b->pos   = malloc(sizeof(int *) * b->m);

    c->m = csr->m - ctx->npiv;
    c->n = ctx->npiv;
    c->width = malloc(sizeof(int) * c->m);
    c->rows  = malloc(sizeof(COEFTYPE *) * c->m);
    c->pos   = malloc(sizeof(int *) * c->m);

    d->m = c->m;
    d->n = b->n;
    d->width = malloc(sizeof(int) * d->m);
    d->rows  = malloc(sizeof(COEFTYPE *) * d->m);
    d->pos   = malloc(sizeof(int *) * d->m);

    COEFTYPE * pccoefbuff = malloc(sizeof(COEFTYPE) * ctx->npiv);
    int      * pcposbuff  = malloc(sizeof(int) * ctx->npiv);

    COEFTYPE * npccoefbuff = malloc(sizeof(COEFTYPE) * csr->n - ctx->npiv);
    int      * npcposbuff  = malloc(sizeof(int) * csr->n - ctx->npiv);

    idx_t idx = 0;

    for (idx_t i = 0; idx < ctx->npiv; i++) {
        if (ctx->pr[i] == -1) continue;

        idx_t p = csr->p[ctx->pr[i]];
        idx_t pcsz  = 0;
        idx_t npcsz = 0;

        for (; p < csr->p[ctx->pr[i] + 1]; p++) {
            if (ctx->pc[csr->i[p]] != -1) { // pivot column
                pccoefbuff[pcsz] = csr->x[p];
                pcposbuff[pcsz++]  = ctx->pc[csr->i[p]];
            } else { // no pivot column
                npccoefbuff[npcsz] = csr->x[p];
                npcposbuff[npcsz++]  = ctx->npc[csr->i[p]];
            }
        }

        a->rows[idx]  = malloc(sizeof(COEFTYPE) * pcsz);
        a->pos[idx]   = malloc(sizeof(int) * pcsz);
        a->width[idx] = pcsz;
        a->nnz       += pcsz;

        b->rows[idx]  = malloc(sizeof(COEFTYPE) * npcsz);
        b->pos[idx]   = malloc(sizeof(int) * npcsz);
        b->width[idx] = npcsz;
        b->nnz       += npcsz;

        for (int k = 0; k < pcsz; k++) {
            a->rows[idx][k] = pccoefbuff[k];
            a->pos[idx][k]  = pcposbuff[k];
        }

        for (int k = 0; k < npcsz; k++) {
            b->rows[idx][k] = npccoefbuff[k];
            b->pos[idx][k]  = npcposbuff[k];
        }
        
        idx++;
    }
    
    idx = 0;

    for (idx_t i = 0; idx < c->m; i++) {
        if (ctx->npr[i] == -1) continue;

        idx_t p = csr->p[ctx->npr[i]];
        idx_t pcsz  = 0;
        idx_t npcsz = 0;
        
        for (; p < csr->p[ctx->npr[i] + 1]; p++) {
            if (ctx->pc[csr->i[p]] != -1) { // pivot column
                pccoefbuff[pcsz] = csr->x[p];
                pcposbuff[pcsz++]  = ctx->pc[csr->i[p]];
            } else { // no pivot column
                npccoefbuff[npcsz] = csr->x[p];
                npcposbuff[npcsz++]  = ctx->npc[csr->i[p]];
            }
        }

        c->rows[idx]  = malloc(sizeof(COEFTYPE) * pcsz);
        c->pos[idx]   = malloc(sizeof(int) * pcsz);
        c->width[idx] = pcsz;
        c->nnz       += pcsz;

        d->rows[idx]  = malloc(sizeof(COEFTYPE) * npcsz);
        d->pos[idx]   = malloc(sizeof(int) * npcsz);
        d->width[idx] = npcsz;
        d->nnz       += npcsz;

        for (int k = 0; k < pcsz; k++) {
            c->rows[idx][k] = pccoefbuff[k];
            c->pos[idx][k]  = pcposbuff[k];
        }

        for (int k = 0; k < npcsz; k++) {
            d->rows[idx][k] = npccoefbuff[k];
            d->pos[idx][k]  = npcposbuff[k];
        }

        idx++;
    }

    tmat_t * flm = malloc(sizeof(*flm));
    flm->a = a;
    flm->b = b;
    flm->c = c;
    flm->d = d;

    //dctx_free(ctx);
    flm->dctx = ctx;
    FREE(pccoefbuff);
    FREE(pcposbuff);
    FREE(npccoefbuff);
    FREE(npcposbuff);
    
    return flm;
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

    ctx->rnpc = malloc(sizeof(idx_t) * csr->n);
    CHECKPTR(ctx->npc);
    for (int i = 0; i < csr->n; i++) ctx->rnpc[i] = -1;
    
    ctx->pc  = malloc(sizeof(idx_t) * csr->n);
    CHECKPTR(ctx->pc);
    for (int i = 0; i < csr->n; i++) ctx->pc[i] = -1;

    /* pivot n non-pivot rows */
    for (idx_t i = 0; i < csr->m; i++) {
        if (csr_width(csr, i) != 0) {
            idx = csr_head_idx(csr, i);

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
            printf("width zero! %d\n", i);
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
            ctx->rnpc[npc_idx] = j;
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

    printf("%-12s", "\nps->rnpc:");
    for (idx_t i = 0; i < n; i++) {
        printf(ps->npr[i] < 0 ? "%d " : "%.2d ", ps->rnpc[i]);
    }
    endl;
    
    printf("%-12s", "\nps->npc:");
    for (idx_t i = 0; i < n; i++) {
        printf(ps->npc[i] < 0 ? "%d " : "%.2d ", ps->npc[i]);
    }
    endl;

    printf("%-12s", "\nps->npr:");
    for (idx_t i = 0; i < __max(m, n); i++) {
        printf(ps->npr[i] < 0 ? "%d " : "%.2d ", ps->npr[i]);
    }
    endl;

    
}

void dctx_free(dctx_t * dctx) {
    if (dctx->pr   != NULL) FREE(dctx->pr);
    if (dctx->npr  != NULL) FREE(dctx->npr);
    if (dctx->pc   != NULL) FREE(dctx->pc);
    if (dctx->npc  != NULL) FREE(dctx->npc);
    if (dctx->rnpc != NULL) FREE(dctx->rnpc);
    if (dctx->rpc  != NULL) FREE(dctx->rpc);
    FREE(dctx);
}
