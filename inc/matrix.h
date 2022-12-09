#ifndef _MATRIX_H
#define _MATRIX_H

#include <memory.h>
#include "outils.h"
#include "tree.h"

typedef struct sparse_block_t                      sb_t;
typedef struct sparse_matrix_t                    csr_t; 
typedef struct sparse_matrix_t                    csc_t; 
typedef struct sparse_matrix_t                     sm_t;
typedef struct macaulay_matrix_t                   mm_t;
typedef struct multiline_vector_t                  ml_t;  
typedef struct dense_row_matrix_t                 drm_t;
typedef struct dense_column_matrix_t              dcm_t;
typedef struct faugere_lachartre_matrix_t         flm_t; 
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
    float    *   d;  // density %
    int         sz;
};



/**
 * @brief stores the index of the pivot 
 * and non-pivot rows and columns
*/
struct decomposer_ctx_t {
    idx_t *  pr;  // pivot rows indexes. order!
    idx_t * npr;  // non-pivot rows indexes.
    idx_t *  pc;  // pivot columns indexes order!
    idx_t * rpc;  // reverse pivot column index
    idx_t * npc;  // non-pivot columns indexes
    idx_t  npiv;  // Npiv
    int  blk_sz;  // block size
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
csc_t * csc_load(FILE * f);                        // todo : complete test
void    smatrix_free(sm_t * smat);                 // todo


/*
    operations
*/

u64  csr_head(csr_t *, u64 );
u32  csr_width(csr_t *, u64);
void csr_swap_col(csr_t *, idx_t, idx_t);
void multiply_csr_dense(csr_t *, dcm_t *, dcm_t *);

/*
    insert & delete
*/

int smatrix_entry(sm_t * smat, int i, int j, COEFTYPE x);


/*
    faugere-lachartre matrix transformations
*/

flm_t *  csr_decompose(csr_t *, u32);
dctx_t * csr_analyse(csr_t *);


void dctx_free(dctx_t * dctx);



/* matpol transformations*/

// mm_t * aapol2mmatrix(aapol_t * laapol, int sz);
// csc_t * aapol_list2csc(aapol_t ** laapol, int sz); // todo: convert aapol 2 csc. look up in trash
// csr_t * aapol_list2csr(aapol_t ** laapol, int sz); // todo: convert aapol 2 csr. look up in trash
// flm_t * csr2flm(csr_t * csr);                 // todo: convert csr_t to block sparse fgmatrix
// flm_t * aapol_list2flm(aapol_t ** laapol, int sz); // todo: convertaapol 2 fgmatrix




void csr_print(csr_t *);
void csr_dense_print(csr_t *);
void dctx_print(dctx_t *, idx_t, idx_t);


#endif