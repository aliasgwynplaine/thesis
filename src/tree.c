#include "tree.h"

void * default_malloc(size_t sz) {
    void * ptr = malloc(sz);
    TESTPTR(ptr);

    return ptr;
}

void default_free(void * ptr) {
    FREE(ptr);
}

bnode_t * bnode_create()  {
    bnode_t * node = malloc(sizeof(bnode_t));
    node->d        = NULL;
    node->l        = NULL;
    node->r        = NULL;
    return node;
}

void bnode_free(bnode_t * root, freefux_t * freefux) {
    if (freefux == NULL) freefux = default_free;
    if (root->l != NULL) bnode_free(root->l, freefux);
    if (root->r != NULL) bnode_free(root->r, freefux);
    freefux(root->d);
    FREE(root);
}

/**
 * @brief allocates mem for a
 * btree_t. Is necesary to call
 * bstree_free.
 * 
 * @return bstree_t* 
 */
bstree_t * bstree_create(cmpfux_t * cmpfux, allocfux_t *allocfux, freefux_t * freefux) {
    bstree_t * bst = malloc(sizeof(bstree_t));
    bst->root      = NULL;

    if (cmpfux == NULL) SAYNEXITWERROR("Cannot use a null cmp fux");
    
    bst->cmp = cmpfux;
    
    if (allocfux != NULL) bst->allocfux  = allocfux;
    else bst->allocfux = default_malloc;

    if (freefux != NULL) bst->freefux = freefux;
    else bst->freefux = default_free;

    bst->sz = 0;
    return bst;
}

void bstree_free(bstree_t * bst) {
    bnode_free(bst->root, bst->freefux);
    FREE(bst);
}


int bstree_insert(bstree_t * tree, void * d) {
    bnode_t * y = NULL;
    bnode_t * x = tree->root;
    int cmp;

    while (x != NULL) {
        y = x;
        cmp = tree->cmp(d, x->d);

        if (cmp < 0) x = x->l;
        else x = x->r;
    }

    if (y == NULL) {
        tree->root    = bnode_create();
        tree->root->d = d;
        tree->sz++;
    } else if (cmp = tree->cmp(d, y->d) < 0){
        y->l    = bnode_create();
        y->l->d = d;
        tree->sz++;
        return 1;
    } else if (cmp > 0) {
        y->r    = bnode_create();
        y->r->d = d;
        tree->sz++;
        return 1;
    }

    return 0;
}