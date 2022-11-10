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


mmatrix_t * mmatrix_malloc(int m, int n, int nnzmax) {
    mmatrix_t * mmat = malloc(sizeof(mmatrix_t));
    mmat->exps = calloc(n, sizeof(u64));
    mmat->mat  = smatrix_malloc(m, n, nnzmax);

    return mmat;
}


void mmatrix_free(mmatrix_t * mmat) {
    FREE(mmat->exps);
    smatrix_free(mmat->mat); // todo: change this
    FREE(mmat);
}

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

int smatrix_entry(smatrix_t * smat, int i, int j, COEFTYPE x) {
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


void colsmat_inorderpush(bnode_t * root, colsmat_t * stack, int * pos) {
    if (root == NULL) return;

    colsmat_inorderpush(root->r, stack, pos);
    *(stack + *pos) = *(colsmat_t *)root->d;
    *pos = *pos + 1;
    colsmat_inorderpush(root->l, stack, pos);
}

colsmat_t * colsmat_btree_dump(bstree_t * tree) {
    colsmat_t * dump = malloc(sizeof(colsmat_t) * tree->sz);
    TESTPTR(dump);
    int * pos = malloc(sizeof(int));
    *pos      = 0;
    colsmat_inorderpush(tree->root, dump, pos);
    FREE(pos);
    return dump;

}


colsmat_t * colsmat_malloc() {
    colsmat_t * cm = calloc(1, sizeof(colsmat_t));
    cm->exp = 0;
    cm->i   = NULL;
    cm->x   = NULL;
    cm->p   = 0;

    return cm;        
}


void colsmat_free(void * cm) {
    colsmat_t * c = cm;
    if (c->i != NULL) FREE(c->i);
    if (c->x != NULL) FREE(c->x);

    FREE(cm);
}


int colsmat_cmp(const void * a, const void * b) {
    int cmp = ((colsmat_t *)a)->exp - ((colsmat_t *)b)->exp;
    return cmp;
}


int colsmat_btree_insert(bstree_t * tree, u64 exp, int i, int xx) {
    bnode_t * y = NULL;
    bnode_t * x = tree->root;
    colsmat_t * aux;
    int cmp;

    while (x != NULL) {
        y = x;
        aux = (colsmat_t *) x->d;
        cmp = exp - aux->exp;

        if (cmp < 0) x = x->l;
        else x = x->r;
    }

    if (y == NULL) {
        tree->root    = bnode_create();
        tree->root->d = colsmat_malloc();
        aux = (colsmat_t *)tree->root->d;
        aux->exp = exp;
        aux->p++;
        aux->i = malloc(sizeof(int));
        aux->x = malloc(sizeof(COEFTYPE));
        aux->i[aux->p - 1] = i;
        aux->x[aux->p - 1] = xx;
        tree->sz++;
    } else {
        aux = (colsmat_t *) y->d;
        cmp = exp - aux->exp;

        if (cmp < 0){
            y->l    = bnode_create();
            y->l->d = colsmat_malloc();
            aux = y->l->d;
            aux->exp = exp;
            aux->p++;
            aux->i = malloc(sizeof(int));
            TESTPTR(aux->i);
            aux->x = malloc(sizeof(COEFTYPE));
            TESTPTR(aux->x);
            aux->i[aux->p - 1] = i;
            aux->x[aux->p - 1] = xx;
            tree->sz++;
        } else if (cmp > 0) {
            y->r    = bnode_create();
            y->r->d = colsmat_malloc();
            aux = y->r->d;
            aux->exp = exp;
            aux->p++;
            aux->i = malloc(sizeof(int));
            TESTPTR(aux->i);
            aux->x = malloc(sizeof(COEFTYPE));
            TESTPTR(aux->x);
            aux->i[aux->p - 1] = i;
            aux->x[aux->p - 1] = xx;
            tree->sz++;
        } else {
            aux->p++;
            aux->i = realloc(aux->i, sizeof(int) * aux->p);
            TESTPTR(aux->i);
            aux->x = realloc(aux->x, sizeof(int) * aux->p);
            TESTPTR(aux->x);
            aux->i[aux->p - 1] = i;
            aux->x[aux->p - 1] = xx;

            return 0;
        }

        return 1;
    }
}


/**
 * @brief
 * @param aapol list of pols. They must be
 * ordered and compressed.
 * @param sz number of polinomials
 * @return mmatrix_t * macaulay matrix
*/
mmatrix_t * aapol2mmatrix(aapol_t * laapol, int sz) {
    mmatrix_t * mmat;
    smatrix_t * smat;
    aapol_t   * aux;
    bstree_t  * bst = bstree_create(colsmat_cmp, NULL, colsmat_free);
    int nnzmax = 0;

    for (int j = 0; j < sz; j++) {
        aux = (laapol + j);
        for (int i = 0; i < aux->sz; i++) {
            debug("push: %ld, %d, %f", aux->terms[i].exp, j, aux->terms[i].coef);
            colsmat_btree_insert(bst, aux->terms[i].exp, j, aux->terms[i].coef);
            nnzmax++;
        }
    }

    colsmat_t * dump = colsmat_btree_dump(bst);
    mmat = mmatrix_malloc(sz, bst->sz, nnzmax);
    smat = mmat->mat;

    for (int i = 0; i < bst->sz; i++) {
        smat->p[i+1]  = smat->p[i] + dump[i].p;
        mmat->exps[i] = dump[i].exp;
        memcpy(&smat->i[smat->p[i]], dump[i].i, dump[i].p * sizeof(int));
        memcpy(&smat->x[smat->p[i]], dump[i].x, dump[i].p * sizeof(COEFTYPE));
    }

    mmat->mat->nnz = nnzmax;

    #ifdef _DEBUG
    
    printf("p: ");
    for (int i = 0; i <= bst->sz; i++)
        printf("%d ", smat->p[i]);

    printf("\ni: ");

    for (int i = 0; i < smat->nnz; i++)
        printf("%d ", smat->i[i]);
    
    printf("\nx: ");
    for (int i = 0; i < smat->nnz; i++)
        printf("%0.1f ", smat->x[i]);

    printf("\n");
    #endif

    bstree_free(bst);
    FREE(dump);

    return mmat;
}

