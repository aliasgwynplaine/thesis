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


rbnode_t * rbnode_create(rbtree_t * rbt, enum RB_COLOR c) {
    rbnode_t * node = rbt->alloc->tree_malloc(rbt->alloc, sizeof(*node));
    CHECKPTR(node);
    node->c = c;
    node->l = NULL;
    node->r = NULL;
    node->d = NULL;
    
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


rbtree_t * rbtree_create(rbt_cmpfux_t * cfux, void * param, tree_allocator_t * alloc) {
    rbtree_t * rbt;
    assert(cfux != NULL);

    if (alloc == NULL) alloc = &rbt_allocator_default;
    
    rbt = alloc->tree_malloc(alloc, sizeof(*rbt));

    if (rbt == NULL) return NULL;
    
    rbt->root  = NULL;
    rbt->cmp   = cfux;
    rbt->param = param;
    rbt->alloc = alloc;
    rbt->sz    = 0;
    rbt->gen   = 0;

    return rbt;
}


static void copy_error_recovery (rbnode_t **stack, int h, rbtree_t *new, rbt_item_fux_t *destroy) {
    assert (stack != NULL && h >= 0 && new != NULL);

    for (; h > 2; h -= 2)
        stack[h - 1]->r = NULL;

    rbtree_destroy (new, destroy);
}


rbtree_t * rbtree_cpy (const rbtree_t *src, rbt_cpy_fux_t *cpy, rbt_item_fux_t *destroy, tree_allocator_t *alloc) {
    rbnode_t *stack[2 * (MAX_HEIGHT + 1)];
    int h = 0;

    rbtree_t *new;
    const rbnode_t *x;
    rbnode_t *y;

    assert (src != NULL);
    new = rbtree_create(src->cmp, src->param, alloc != NULL ? alloc : src->alloc);
    
    if (new == NULL) return NULL;
    
    new->sz = src->sz;
    
    if (new->sz == 0)
        return new;

    x = (const rbnode_t *) &src->root;
    y = (rbnode_t *) &new->root;

    for (;;) {
        while (x->l != NULL) {
            assert (h < 2 * (MAX_HEIGHT + 1));

            y->l = new->alloc->tree_malloc (new->alloc, sizeof *y->l);
            
            if (y->l == NULL) {
                    if (y != (rbnode_t *) &new->root) {
                            y->d = NULL;
                            y->r = NULL;
                    }

                    copy_error_recovery (stack, h, new, destroy);
                    return NULL;
            }

            stack[h++] = (rbnode_t *) x;
            stack[h++] = y;
            x = x->l;
            y = y->l;
        }

        y->l = NULL;

        for (;;) {
            y->c = x->c;
            if (cpy == NULL)
                y->d = x->d;
            else {
                y->d = cpy (x->d, src->param);
                
                if (y->d == NULL) {
                    y->r = NULL;
                    copy_error_recovery (stack, h, new, destroy);
                    return NULL;
                }
            }

            if (x->r != NULL) {
                y->r = new->alloc->tree_malloc (new->alloc, sizeof *y->r);

                if (y->r == NULL) {
                    copy_error_recovery (stack, h, new, destroy);
                    return NULL;
                }

                x = x->r;
                y = y->r;
                break;
            } else y->r = NULL;

            if (h <= 2)
                return new;

            y = stack[--h];
            x = stack[--h];
        }
    }
}

/**
 * @brief 
 * 
 * @param rbt rbtree structure
 * @param data the value we're searching
 * @return NULL if value is not found. The pointer to the datum
 */
void * rbtree_find(const rbtree_t * rbt, const void * data) {
    if (rbt == NULL || data == NULL) SAYNEXITWERROR("rbt or data is null");
    const rbnode_t * p;

    for (p = rbt->root; p != NULL; /* empty */) {
        int cmp = rbt->cmp(data, p->d, rbt->param);

        if (cmp < 0) p = p->l;
        else if (cmp > 0) p = p->r;
        else return p->d;
    }
    
    return NULL;
}


void ** rbtree_probe(rbtree_t * rbt, void * data) {
    assert(rbt != NULL && data != NULL);
    rbnode_t * p = NULL;
    rbnode_t * n = NULL;
    rbnode_t * stack[rbt->sz + 1];  // be careful
    unsigned char dir[rbt->sz + 1]; // be careful
    int k = 0;

    stack[k] = (rbnode_t *)&rbt->root; // look up
    dir[k++] = 0;
    //printf("order: %d\n", *((int *)rbt->param));

    for (p = rbt->root; p != NULL; /* empty */) {
        int cmp = rbt->cmp(data, p->d, rbt->param);
        if (cmp == 0) return &p->d;

        stack[k] = p;
        dir[k++] = cmp > 0;

        if (cmp < 0) p = p->l;
        else p = p->r;
    }

    if (dir[k - 1] == 0) n = stack[k - 1]->l = rbnode_create(rbt, red);
    else n = stack[k - 1]->r = rbnode_create(rbt, red);
    
    n->d = data;
    rbt->sz++;
    rbt->gen++;

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
                    x = stack[k - 1];
                    y = x->l;
                    x->l = y->r;
                    y->r = x;
                    stack[k - 2]->r = y;
                }

                x = stack[k - 2];
                x->c = red;
                y->c = black;
                x->r = y->l;
                y->l = x;
                if (dir[k - 3] == 0) stack[k - 3]->l = y;
                else stack[k - 3]->r = y;

                break;
            }
        }
    }

    rbt->root->c = black;  

    return &n->d;
}


void * rbtree_insert(rbtree_t * rbt, void * data) {
    void ** p = rbtree_probe(rbt, data);
    return p == NULL || *p == data ? NULL : *p;
}


void * rbtree_repl(rbtree_t * rbt, void * data) {
    void ** p = rbtree_probe(rbt, data);
    if (p == NULL || *p == data) return NULL;
    else {
        void * r = *p;
        *p = data;
        return r;
    }
}


void * rbtree_delete(rbtree_t * rbt, void * data) {
    if (rbt == NULL || data == NULL) SAYNEXITWERROR("rbt or data equals null");
    rbnode_t * stack[rbt->sz + 1]; // be careful
    uchar dir[rbt->sz];            // be careful
    rbnode_t * p;
    int cmp;
    int k = 0;

    p = (rbnode_t *)&rbt->root;
    
    for (cmp = -1; cmp != 0; cmp = rbt->cmp(data, p->d, rbt->param)) {
        int d  = cmp > 0;
        stack[k] = p;
        dir[k++] = d;

        if (d == 0) p = p->l;
        else p = p->r;

        if (p == NULL)  return NULL;
    }

    data = p->d;
    
    if (p->r == NULL) {
        if (dir[k - 1] == 0) stack[k - 1]->l = p->l;
        else stack[k - 1]->r = p->l;
    } else {
        RB_COLOR t;
        rbnode_t * r = p->r;

        if (r->l == NULL) {
            r->l = p->l;
            t = r->c;
            r->c = p->c;
            p->c = t;

            if (dir[k - 1] == 0) stack[k - 1]->l = p->r;
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
            rbnode_t * x = dir[k - 1] == 0 ? stack[k - 1]->l : stack[k-1]->r;

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
                rbnode_t * w = stack[k - 1]->l;

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
                    w = stack[k - 1]->l;
                }

                if ((w->l == NULL || w->l->c == black) && (w->r == NULL || w->r->c == black)) {
                    w->c = red;
                } else {
                    if (w->l == NULL || w->l->c == black) {
                        rbnode_t * y = w->r;
                        y->c = black;
                        w->c = red;
                        w->r = y->l;
                        y->l = w;
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
            k--;
        }
    }

    rbt->alloc->tree_free(rbt->alloc, p); // check
    rbt->sz--;
    rbt->gen++;

    return (void *) data;

}


void rbtree_trav_refresh(rbt_trav_t * trav) {
    assert(trav != NULL);
    trav->gen = trav->tree->gen;

    if (trav->node != NULL) {
        rbt_cmpfux_t * cmp = trav->tree->cmp;
        void * param = trav->tree->param;
        rbnode_t * node = trav->node;
        rbnode_t * i;
        trav->h = 0;

        for (i = trav->tree->root; i != node; /* empty */) {
            assert(trav->h < MAX_HEIGHT);
            assert(i != NULL);
            trav->stack[trav->h++] = i;
            
            if (cmp(node->d, i->d, param) > 0) i = i->r;
            else i = i->l;
        }
    }
}

void rbtree_trav_init(rbt_trav_t * trav, rbtree_t * tree) {
    trav->tree = tree;
    trav->node = NULL;
    trav->h    = 0;
    trav->gen  = tree->gen;
}

void * rbtree_trav_first(rbt_trav_t * trav, rbtree_t * tree) {
    assert(tree != NULL && trav != NULL);
    rbnode_t * x;
    trav->tree = tree;
    trav->h    = 0;
    trav->gen  = tree->gen;
    x = tree->root;

    if (x != NULL) {
        while (x->l != NULL) {
            assert(trav->h < MAX_HEIGHT);
            trav->stack[trav->h++] = x;
            x = x->l;
        }
    }

    trav->node = x;

    return x != NULL ? x->d : NULL;
}

void * rbtree_trav_last(rbt_trav_t * trav, rbtree_t * tree) {
    assert(tree != NULL && trav != NULL);
    rbnode_t * x;
    trav->tree = tree;
    trav->h    = 0;
    trav->gen  = tree->gen;
    x = tree->root;

    if (x != NULL) {
        while (x->r != NULL) {
            assert(trav->h < MAX_HEIGHT);
            trav->stack[trav->h++] = x;
            x = x->r;
        }
    }

    trav->node = x;

    return x != NULL ? x->d : NULL;
}

void * rbtree_trav_find(rbt_trav_t * trav, rbtree_t * tree, void * d) {
    assert(tree != NULL && trav != NULL);
    rbnode_t * p, * q;
    trav->tree = tree;
    trav->h    = 0;
    trav->gen  = tree->gen;

    for (p = tree->root; p!= NULL; p = q) {
        int cmp = tree->cmp(d, p->d, tree->param);
        
        if (cmp < 0) q = p->r;
        else if (cmp > 0) q = p->l;
        else {
            trav->node = p;
            return p->d;
        }

        assert(trav->h < MAX_HEIGHT);
        trav->stack[trav->h++] = p;
    }

    trav->h    = 0;
    trav->node = NULL;

    return NULL;
}


void * rbtree_trav_insert(rbt_trav_t * trav, rbtree_t * rbt, void * d) {
    assert(trav != NULL && rbt != NULL && d != NULL);
    void ** p;
    
    p = rbtree_probe(rbt, d);

    if (p != NULL) {
        trav->tree = rbt;
        trav->node = (rbnode_t *) ((char *)p - offsetof(rbnode_t, d));
        trav->gen = rbt->gen - 1;

        return *p;
    } else {
        rbtree_trav_init(trav, rbt);
        return NULL;
    }

}

void * rbtree_trav_cpy(rbt_trav_t * trav, rbt_trav_t * src) {
    assert(trav != NULL && src != NULL);

    if (trav != src) {
        trav->tree = src->tree;
        trav->node = src->node;
        trav->gen  = src->gen;
        
        if (trav->gen == trav->tree->gen) {
            trav->h = src->h;
            memcpy(
                trav->stack, 
                (const void *) src->stack, 
                sizeof(*trav->stack) * trav->h
            );
        }
    }

    return trav->node != NULL ? trav->node->d : NULL;
}


void * rbtree_trav_next(rbt_trav_t * trav) {
    assert(trav != NULL);
    rbnode_t * x;

    if (trav->gen != trav->tree->gen) {
        rbtree_trav_refresh(trav);
    }

    x = trav->node;

    if (x == NULL) {
        return rbtree_trav_first(trav, trav->tree);
    } else if (x->r != NULL) {
        assert(trav->h < MAX_HEIGHT);
        trav->stack[trav->h++] = x;
        x = x->r;

        while (x->l != NULL) {
            assert(trav->h < MAX_HEIGHT);
            trav->stack[trav->h++] = x;
            x = x->l;
        }
    } else {
        rbnode_t * y;

        do {
            if (trav->h == 0) {
                trav->node = NULL;

                return NULL;
            }

            y = x;
            x = trav->stack[--trav->h];
        } while(y == x->r);
    }

    trav->node = x;

    return x->d;
}

void * rbtree_trav_prev(rbt_trav_t * trav) {
    rbnode_t * x;
    assert(trav != NULL);

    if (trav->gen != trav->tree->gen) {
        rbtree_trav_refresh(trav);
    }

    x = trav->node;

    if (x == NULL) {
        return rbtree_trav_last(trav, trav->tree);
    } else if (x->l != NULL) {
        assert(trav->h < MAX_HEIGHT);
        trav->stack[trav->h++] = x;
        x = x->l;

        while (x->r != NULL) {
            assert(trav->h < MAX_HEIGHT);
            trav->stack[trav->h++] = x;
            x = x->r;
        }
    } else {
        rbnode_t * y;

        do {
            if (trav->h == 0) {
                trav->node = NULL;

                return NULL;
            }

            y = x;
            x = trav->stack[--trav->h];
        } while(y == x->l);
    }

    trav->node = x;

    return x->d;
}


void * rbtree_trav_curr(rbt_trav_t * trav) {
    assert(trav != NULL);

    return trav->node != NULL ? trav->node->d : NULL;
}

void * rbtree_trav_repl(rbt_trav_t * trav, void * new) {
    void * old;
    assert(trav != NULL && trav->node != NULL & new != NULL);
    old = trav->node->d;
    trav->node->d = new;

    return old;
}


void rbtree_destroy(rbtree_t * rbt, rbt_item_fux_t * destroy) {
    assert(rbt != NULL);
    rbnode_t * p, * q;

    for (p = rbt->root; p != NULL; p = q) {
        if (p->l == NULL) {
            q = p->r;

            if (destroy != NULL && p->d != NULL) destroy(p->d, rbt->param);

            rbt->alloc->tree_free(rbt->alloc, p);
        } else {
            q = p->l;
            p->l = q->r;
            q->r = p;
        }
    }
    rbt->alloc->tree_free(rbt->alloc, rbt);
}


void * rbt_malloc(tree_allocator_t * allocator, size_t sz) {
    assert(allocator != NULL && sz > 0);
    return malloc(sz);
}

void rbt_free(tree_allocator_t * allocator, void * blck) {
    assert(allocator != NULL && blck != NULL);
    free(blck);
}

tree_allocator_t rbt_allocator_default = {rbt_malloc, rbt_free};

#undef NDEBUG
#include <assert.h>

void (rbtree_assert_insert) (rbtree_t * rbt, void * data) {
    void ** p = rbtree_probe(rbt, data);
    assert(p != NULL && * p == data);
}

void * (rbtree_assert_delete) (rbtree_t * rbt, void * data) {
    void * p = rbtree_delete(rbt, data);
    assert(p != NULL);
    return p;
}