#ifndef _MATRIX_H
#define _MATRIX_H

#include <memory.h>
#include "outils.h"

typedef struct new_sparse_matrix_t nsm_t;
/**
 * x[i][j] is the element a[i, c[i][j]]
*/
struct new_sparse_matrix_t {
    COEFTYPE ** x; // x[i] is row i
    u64      ** c; // column index of x[i]
    u64       * w; // w[i] is nnz entries in x[i]
    float       d; // density
    u64       nnz;
    u64         m; // number of rows
    u64         n; // number of cols
};



typedef struct sparse_block_t                      sb_t;
typedef struct sparse_matrix_t                    csr_t; 
typedef struct sparse_matrix_t                    csc_t; 
typedef struct sparse_matrix_t                     sm_t;
typedef struct macaulay_matrix_t                   mm_t;
typedef struct multiline_vector_t                  ml_t;  
typedef struct dense_row_matrix_t                 drm_t;
typedef struct dense_column_matrix_t              dcm_t;
typedef struct faugere_lachartre_matrix_t         flm_t;
typedef struct sparse_triangular_matrix_t         stm_t;
typedef struct sparse_rectangular_matrix_t        srm_t;
typedef struct hybrid_rectangular_matrix_t        hrm_t;
typedef struct faugere_lachartre_sparse_matrix_t flsm_t;
typedef struct sparse_triangular_block_matrix_t  stbm_t;
typedef struct sparse_rectangular_block_matrix_t srbm_t;
typedef struct hybrid_rectangular_block_matrix_t hrbm_t;

typedef struct decomposer_ctx_t  dctx_t; // pivot set struct

struct dense_column_matrix_t {
    COEFTYPE ** c;
    idx_t       m;
    idx_t       n;
};

struct dense_row_matrix_t {
    COEFTYPE ** r;
    idx_t       m;
    idx_t       n;
};

/**
 * @brief sparse block for triangular 
 * and rectagular sparse block matrix
 * 
 * @note this is from gbla
*/
struct sparse_block_t {
    COEFTYPE ** x;
    idx_t    ** i;
    idx_t    * sz;
    int       nnz;
};

/**
 * @brief sparse matrix struct
 * based on Davis CC sparse matrix.
 * You are using this just to begin.
 * Eventually, this must be changed 
 * by Faugere-Lacharte matrix for fast
 * Groebner basis calculation
 */
struct sparse_matrix_t {
    int   nnzmax;   /* max num o entries */
    int        m;   /* row sz   */
    int        n;   /* col sz   */
    int      * i;   /* row/col idx */
    int      * p;   /* row/col pos */
    COEFTYPE * x;   /* values   */
    int      nnz;   /* num o entries in triplet. -1 if colcomp */
};



struct macaulay_matrix_t {
    u64       * exps; // multigrads
    sm_t       * mat;  // matrix
};


struct multiline_vector_t {
    int      * pos; /* position      */
    COEFTYPE * val; /* values        */
    int         sz; /* size          */
    int        cap; /* capacity      */
    u8           n; /* n-line vector */
};


struct faugere_lachartre_matrix_t {
    stbm_t * a;
    srbm_t * b;
    hrbm_t * c;
    int    nnz;
};


struct sparse_triangular_block_matrix_t {
    sb_t    ** blk;
    int      *  nb;
    float        d;  // density %
    int         sz;
};


struct sparse_triangular_matrix_t {
    COEFTYPE ** r;
    int      *  i;
    float       d;
    int        sz;
};


struct sparse_rectangular_matrix_t {
    COEFTYPE ** r;
    int      *  i;
    float       d;
    int        sz;
};


struct hybrid_rectangular_matrix_t {
    COEFTYPE ** r;
    int      *  i;
    float       d;
    int        sz;
};


struct faugere_lachartre_sparse_matrix_t {
    u32 n;
    u32 m;
    COEFTYPE ** rows;
    u32 * width;
    u32 ** pos;
    float d;
    u64 nnz;
};


struct temp_mat_t {
    flsm_t * a;
    flsm_t * b;
    flsm_t * c;
    flsm_t * d;
    int    nnz;
    dctx_t * dctx;
};

typedef struct temp_mat_t tmat_t;


/**
 * @brief stores the index of the pivot 
 * and non-pivot rows and columns
*/
struct decomposer_ctx_t {
    idx_t *   pr;  // pivot rows indexes. order!
    idx_t *  npr;  // non-pivot rows indexes.
    idx_t *   pc;  // pivot columns indexes order!
    idx_t *  rpc;  // reverse pivot column index
    idx_t *  npc;  // non-pivot column indexes
    idx_t * rnpc;  // reverse non-pivot column indexes
    idx_t   npiv;  // Npiv
    int   blk_sz;  // block size
};


/*
    memory handling
*/

mm_t * mmatrix_malloc(int m, int n, int nnzmax);
void   mmatrix_free(mm_t * mmat);

csr_t * csr_malloc(int m, int n, int nnzmax);
csr_t * csr_realloc(csr_t * csrmatrix, size_t sz);  // todo
csr_t * csr_load(FILE * f);
void    csr_free(csr_t *);

csc_t * csc_malloc(int m, int n, int nnzmax);
csc_t * csc_realloc(sm_t * smat, size_t sz);       // todo
csc_t * csc_load(FILE * f);                        // todo
void    smatrix_free(sm_t * smat);                 // todo


flsm_t * flsm_malloc(int m, int n); // todo
flsm_t * flsm_load(FILE * f); // todo
void     flsm_print(flsm_t * flsm);
void     flsm_free(flsm_t * flsm);
dctx_t * flsm_analyse(flsm_t * flsm);
flsm_t * csr2flsm(csr_t * csr);

/*
    operations
*/

idx_t  csr_head_idx(csr_t *, idx_t );
idx_t  csr_width(csr_t *, idx_t);
void   csr_swap_col(csr_t *, idx_t, idx_t);
void   csr_print(csr_t *);
void   csr_dense_print(csr_t *);
void   multiply_csr_dense(csr_t *, dcm_t *, dcm_t *);
tmat_t * csr_decompose(csr_t *);
dctx_t * csr_analyse(csr_t *);

/* nsm */

void nsm_print(nsm_t * );
void nsm_free(nsm_t * );
void nsm_rref(nsm_t * );

/*
    insert & delete
*/

int smatrix_entry(sm_t * smat, int i, int j, COEFTYPE x);

/*
    dense-sparse transformations
*/

COEFTYPE * sparse2dense(COEFTYPE *, idx_t *, idx_t, idx_t);


/*
    decomposer_ctx utils
*/

void dctx_print(dctx_t *, idx_t, idx_t);
void dctx_free(dctx_t * dctx);



/* matpol transformations*/

// mm_t * aapol2mmatrix(aapol_t * laapol, int sz);
// csc_t * aapol_list2csc(aapol_t ** laapol, int sz); // todo: convert aapol 2 csc. look up in trash
// csr_t * aapol_list2csr(aapol_t ** laapol, int sz); // todo: convert aapol 2 csr. look up in trash
// flm_t * csr2flm(csr_t * csr);                 // todo: convert csr_t to block sparse fgmatrix
// flm_t * aapol_list2flm(aapol_t ** laapol, int sz); // todo: convertaapol 2 fgmatrix







#endif