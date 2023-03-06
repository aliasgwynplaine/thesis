#include "tree.h"

void * default_malloc(size_t sz) {
    void * ptr = malloc(sz);
    CHECKPTR(ptr);

    return ptr;
}

void default_free(void * ptr) {
    FREE(ptr);
}

bnode_t * bnode_create()  {
    bnode_t * node = malloc(sizeof(bnode_t));
    CHECKPTR(node);
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


rbnode_t * rbnode_create(enum RB_COLOR c) {
    rbnode_t * node = malloc(sizeof(*node));
    CHECKPTR(node);
    node->c = c;
    node->l = NULL;
    node->r = NULL;
    node->d =NULL;
    
    return node;
}


void rbnode_free(rbnode_t * root, freefux_t * freefux) {
    if (freefux == NULL) freefux = default_free;
    if (root->l != NULL) rbnode_free(root->l, freefux);
    if (root->r != NULL) rbnode_free(root->r, freefux);
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
    
    if (allocfux != NULL) bst->alloc  = allocfux;
    else bst->alloc = default_malloc;

    if (freefux != NULL) bst->free = freefux;
    else bst->free = default_free;

    bst->sz = 0;
    return bst;
}

void bstree_free(bstree_t * bst) {
    bnode_free(bst->root, bst->free);
    FREE(bst);
}

void * bstree_search(bstree_t * bst, void * item) {
    if (bst == NULL || item == NULL) return NULL;
    //bnode_t * curr;
    // todo
    return NULL;
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
    } else if ((cmp = tree->cmp(d, y->d) < 0)){
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


rbtree_t * rbtree_create(cmpfux_t * cfux, allocfux_t * afux, freefux_t * ffux) {
    rbtree_t * rbt = malloc(sizeof(*rbt));
    rbt->root =NULL;

    if (cfux == NULL) SAYNEXITWERROR("cfux is null");
    else rbt->cmp = cfux;
    
    if (afux != NULL) rbt->alloc = afux;
    else rbt->alloc = default_malloc;

    if (ffux != NULL) rbt->free = ffux;
    else rbt->free = default_free;

    rbt->sz = 0;

    return NULL;
}


void rbtree_free(rbtree_t * rbt) {
    rbnode_free(rbt->root, rbt->free);
    FREE(rbt);
}

/**
 * @brief 
 * 
 * @param rbt rbtree structure
 * @param data the value we're searching
 * @return NULL if value is not found. The pointer to the datum
 */
void * rbtree_search(const rbtree_t * rbt, const void * data) {
    if (rbt == NULL || data == NULL) SAYNEXITWERROR("rbt or data is null");
    const rbnode_t * p;

    for (p = rbt->root; p != NULL; /* empty */) {
        int cmp = rbt->cmp(data, p->d);

        if (cmp < 0) p = p->l;
        else if (cmp > 0) p = p->r;
        else return p->d;
    }
    
    return NULL;
}


int rbtree_insert(rbtree_t * rbt, void * data) {
    rbnode_t * p = NULL;
    rbnode_t * n = NULL;
    rbnode_t * stack[rbt->sz + 1];
    unsigned char dir[rbt->sz + 1];
    int k = 0;

    stack[k] = (rbnode_t *)&rbt->root; // look up
    dir[k++] = 0;

    for (p = rbt->root; p != NULL; /* empty */) {
        int cmp = rbt->cmp(data, p->d);
        if (cmp == 0) return 0;

        stack[k] = p;
        dir[k++] = cmp > 0;

        if (cmp < 0) p = p->l;
        else p = p->r;
    }

    if (dir[k - 1] == 0) n = stack[k - 1]->l = rbnode_create(red);
    else n = stack[k - 1]->r = rbnode_create(red);

    n->d = data;
    rbt->sz++;

    while (k <= 3 && stack[k - 1]->c == red) {
        if (dir[k - 2] == 0) {
            rbnode_t * y = stack[k - 2]->r;

            if (y != NULL && y->c == red) {
                stack[--k]->c = y->c = black;
                stack[--k]->c = red; 
            } else {
                rbnode_t * x;

                if (dir[k - 1] == 0) {
                    y = stack[k - 1];
                } else {
                    x = stack[k - 1];
                    y = x->r;
                    x->r = y->l;
                    y->l = x;
                    stack[k - 2]->l = y;

                }

                x = stack[k - 2];
                x->c = red;
                y->c = black;
                x->l = y->r;
                y->r = x;

                if (dir[k - 3] == 0) stack[k - 3]->l = y;
                else stack[k - 3]->r = y;

                break;
            }
        } else {
            rbnode_t * y = stack[k - 2]->l;

            if (y != NULL && y->c == red) {
                stack[--k]->c = y->c = black;
                stack[--k]->c = red;
            } else {
                rbnode_t * x;

                if (dir[k - 1] == 1) {
                    y = stack[k - 1];
                } else {
                    x = stack[k - 2];
                    x->c = red;
                    y->c = black;
                    x->r = y->l;
                    y->l = x;
                    if (dir[k - 3] == 0) stack[k - 3]->l = y;
                    else stack[k - 3]->r = y;
                }

                x = stack[k - 1];
                y = x->l;
                x->l = y->r;
                y->r = x;
                stack[k - 2]->r = y;

                break;
            }
        }
    }

    rbt->root->c = black;  

    return 1;
}


void rbtree_delete(rbtree_t * rbt, void * data) {
    rbnode_t * stack[rbt->sz];
    uchar dir[rbt->sz];
    rbnode_t * p;
    int cmp;
    int k = 0;

    if (rbt == NULL || data == NULL) SAYNEXITWERROR("rbt or data equals null");

    p = (rbnode_t *)&rbt->root;
    
    for (cmp = -1; cmp != 0; cmp = rbt->cmp(p->d, data)) {
        stack[k] = p;
        dir[k++] = cmp > 0;

        if (p == NULL) return NULL;
    }

    data = p->d;
    
    if (p->r == NULL) {
        if (dir[k - 1] == 0) stack[k - 1]->l = p->l;
        else stack[k - 1]->r = p->r;
    } else {
        RB_COLOR t;
        rbnode_t * r = p->r;

        if (r->l == NULL) {
            r->l = p->l;
            t = r->c;
            r->c = p->c;
            p->c = t;

            if (dir[k - 1] == 0) stack[k - 1]->l = r;
            else stack[k - 1]->r = r;

            dir[k] = 1;
            stack[k++] = r;
        } else {
            rbnode_t * s;
            int j = k++;
            for (;;) {
                dir[k] = 0;
                stack[k++] = r;
                s = r->l;

                if (s->l == NULL) break;

                r = s;
            }

            dir[j] = 1;
            stack[j] = s;

            if (dir[j - 1] == 0) stack[j - 1]->l = s;
            else stack[j - 1]->r = s;

            s->l = p->l;
            r->l = s->r;
            s->r = p->r;
            t = s->c;
            s->c = p->c;
            p->c = t;
        }
    }

    if (p->c == black) {
        for (;;) {
            rbnode_t * x = dir[k - 1] == 0? stack[k - 1]->l : stack[k-1]->r;

            if (x != NULL && x->c == red) {
                x->c = black;
                break;
            }

            if (k < 2) break;

            if (dir[k - 1] == 0) { // left side balancing
                rbnode_t * w = stack[k - 1]->r;

                if (w->c == red) {
                    w->c = black;
                    stack[k - 1]->c = red;
                    stack[k - 1]->r = w->l;
                    w->l = stack[k - 1];
                    
                    if (dir[k - 2] == 0) stack[k - 2]->l = w;
                    else stack[k - 2]->r = w;
                    stack[k] = stack[k - 1];
                    dir[k] = 0;
                    stack[k - 1] = w;
                    k++;
                    w = stack[k - 1]->r;

                }

                if ((w->l == NULL || w->l->c == black) && (w->r == NULL || w->r->c == black)) {
                    w->c = red;
                } else {
                    if (w->r == NULL || w->r->c == black) {
                        rbnode_t * y = w->l;
                        y->c = black;
                        w->c = red;
                        w->l = y->r;
                        y->r = w;
                        w = stack[k - 1]->r = y;
                    }
                    w->c = stack[k - 1]->c;
                    stack[k - 1]->c = black;
                    w->r->c = black;
                    stack[k - 1]->r = w->l;
                    w->l = stack[k - 1];
                    
                    if (dir[k - 2] == 0) stack[k - 2]->l = w;
                    else stack[k - 2]->r = w;

                    break;
                }
            } else { //right side balancing
                rbnode_t * w = stack[k - 1]->r;

                if (w->c == red) {
                    w->c = black;
                    stack[k - 1]->c = red;
                    stack[k - 1]->l = w->r;
                    w->r = stack[k - 1];

                    if (dir[k - 2] == 0) stack[k - 2]->l = w;
                    else stack[k - 2]->r = w;

                    stack[k] = stack[k - 1];
                    dir[k]   = 1;
                    stack[k - 1] = w;
                    k++;
                    w = stack[k - 1]->r;
                }

                if ((w->l == NULL || w->l->c == black) && (w->r == NULL || w->r->c == black)) {
                    w->c = red;
                } else {
                    if (w->l == NULL | w->l->c == black) {
                        rbnode_t * y = w->r;
                        y->c = black;
                        w->c = red;
                        w->r = y->l;
                        y->r = w;
                        w = stack[k - 1]->l = y;
                    }

                    w->c = stack[k - 1]->c;
                    stack[k - 1]->c = black;
                    w->l->c = black;
                    stack[k - 1]->l =w->r;
                    w->r = stack[k - 1];
                    
                    if (dir[k - 2] == 0) stack[k - 2]->l = w;
                    else stack[k - 2]->r = w;

                    break;
                }
            }
        }
    }

    rbt->free(p); // check
    rbt->sz--;

    return (void *) data;

}