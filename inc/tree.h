#ifndef _TREE_H
#define _TREE_H

#include "outils.h"

typedef int    cmpfux_t(const void * a, const void * b);
typedef void * allocfux_t(const size_t sz);
typedef void   freefux_t(void *);

typedef struct bnode_t      bnode_t; // binary node
typedef struct tnode_t      tnode_t; // ternary node
typedef struct bstree_t    bstree_t;
typedef struct rbtree_t    rbtree_t;
typedef struct avltree_t  avltree_t;
typedef struct setoint_t  setoint_t;

typedef enum ORDER ORDER;

/**
 * @brief binary node struct.
 * Definition and functions from 
 * Corwen
 * 
 */
struct bnode_t {
    bnode_t * l;
    bnode_t * r;
    void    * d; //data
};

struct tnode_t {
    tnode_t * l;
    tnode_t * r;
    tnode_t * p; // parent
    void    * d; // data
    int   c : 1; // color
};

enum ORDER {
    inorder,
    preorder,
    postorder
};

struct bstree_t {
    bnode_t    *     root;
    cmpfux_t   *      cmp; // a ¿? b
    allocfux_t * allocfux;
    freefux_t  *  freefux;
    int                sz;
};

struct avltree_t {
    bnode_t * root;
    cmpfux_t * cmp;
    int         sz;
};



bnode_t   * bnode_create();
void        bnode_free(bnode_t *, freefux_t *);

bstree_t  * bstree_create(cmpfux_t *, allocfux_t * , freefux_t *);
void        bstree_free(bstree_t *);
void      * bstree_search(bnode_t *, void *); // todo
int         bstree_insert(bstree_t *, void *);
void        bstree_walk(bnode_t *, ORDER);    // todo
void        bstree_inorder_walk(bnode_t *);   // todo
void        bstree_preorder_walk(bnode_t *);  // todo
void        bstree_postorder_walk(bnode_t *); // todo

avltree_t * avltree_create();
void      * avltree_search(bnode_t *, void * data);
void        avltree_insert(bnode_t *, void * data);
void        avltree_delete(bnode_t *, void * data);
void        avltree_walk(bnode_t *, ORDER);
void        avltree_inorderwalk(bnode_t *);
void        avltree_preorderwalk(bnode_t *);
void        avltree_postorderwalk(bnode_t *);

rbtree_t  * rbtree_create();
void      * rbtree_search(rbtree_t *, void * data);
void        rbtree_insert(rbtree_t *, void * data);
void        rbtree_delete(rbtree_t *, void * data);
void        rbtree_walk(rbtree_t *, ORDER);
void        rbtree_inorderwalk(rbtree_t *);
void        rbtree_preorderwalk(rbtree_t *);
void        rbtree_postorderwalk(rbtree_t *);


#endif
