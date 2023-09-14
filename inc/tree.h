#ifndef _TREE_H
#define _TREE_H

#include "outils.h"

#define MAX_HEIGHT 48
#define sz(t) (((size_t) (t)->sz))

typedef struct bnode_t            bnode_t; // binary node
typedef struct tnode_t            tnode_t; // ternary node
typedef struct bstree_t          bstree_t;
typedef struct rbnode_t          rbnode_t;
typedef struct rbtree_t          rbtree_t;
typedef struct avltree_t        avltree_t;
typedef struct setoint_t        setoint_t;
typedef struct rbt_traverser_t rbt_trav_t;

typedef int    rbt_cmpfux_t(const void *rbt_a, const void *rbt_b, void *rbt_param);
typedef void   rbt_item_fux_t(void * rbt_item, void * rbt_param);
typedef void * rbt_cpy_fux_t(void * rbt_item, void * rbt_param);


#ifndef TREE_ALLOCATOR
#define TREE_ALLOCATOR
/* Memory allocator for trees */

struct tree_allocator_t {
    void * (* tree_malloc) (struct tree_allocator_t *, size_t sz);
    void   (* tree_free)   (struct tree_allocator_t *, void * block);
};

typedef struct tree_allocator_t tree_allocator_t;

#endif

extern tree_allocator_t rbt_allocator_default;
void * rbt_malloc(tree_allocator_t *, size_t);
void   rbt_free(tree_allocator_t *, void *);

typedef enum ORDER ORDER;
typedef enum RB_COLOR RB_COLOR;

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
    allocfux_t *    alloc;
    freefux_t  *     free;
    size_t             sz;
};

struct avltree_t {
    bnode_t * root;
    cmpfux_t * cmp;
    size_t      sz;
};

enum RB_COLOR {
    black,
    red
};

struct rbnode_t {
    rbnode_t * l;
    rbnode_t * r;
    void     * d; // data
    uchar      c; // color
};

struct rbtree_t {
    rbnode_t         *  root;
    rbt_cmpfux_t      *   cmp; // todo change to make everything else compatible
    tree_allocator_t * alloc; // alloc for the nodes
    void             * param;
    size_t                sz;
    u32                  gen;
};

struct rbt_traverser_t {
    rbtree_t              * tree;
    rbnode_t              * node;
    rbnode_t * stack[MAX_HEIGHT];
    size_t                     h;
    u32                      gen;
};


bnode_t   * bnode_create();
void        bnode_free(bnode_t *, freefux_t *);

rbnode_t  * rbnode_create(rbtree_t *, enum RB_COLOR);
void        rbnode_free(rbnode_t *, freefux_t *);

bstree_t  * bstree_create(cmpfux_t *, allocfux_t * , freefux_t *);
void        bstree_free(bstree_t *);
void      * bstree_search(bstree_t *, void *); // todo
int         bstree_insert(bstree_t *, void *);
void        bstree_walk(bnode_t *, ORDER);    // todo
void        bstree_inorder_walk(bnode_t *);   // todo
void        bstree_preorder_walk(bnode_t *);  // todo
void        bstree_postorder_walk(bnode_t *); // todo

avltree_t * avltree_create(cmpfux_t *, allocfux_t, freefux_t *);   // todo 
void      * avltree_search(bnode_t *, void * data);                // todo 
void        avltree_insert(bnode_t *, void * data);                // todo 
void        avltree_delete(bnode_t *, void * data);                // todo 
void        avltree_walk(bnode_t *, ORDER);                        // todo 
void        avltree_inorderwalk(bnode_t *);                        // todo 
void        avltree_preorderwalk(bnode_t *);                       // todo 
void        avltree_postorderwalk(bnode_t *);                      // todo 

rbtree_t  * rbtree_create(rbt_cmpfux_t *, void *, tree_allocator_t *); 
rbtree_t  * rbtree_cpy(const rbtree_t *, rbt_cpy_fux_t *, rbt_item_fux_t *, tree_allocator_t *); //todo
void        rbtree_destroy(rbtree_t *, rbt_item_fux_t *);
void        rbtree_free(rbtree_t *);
void      * rbtree_find(const rbtree_t *, const void * data);
void      * rbtree_insert(rbtree_t *, void *);
void     ** rbtree_probe(rbtree_t *, void * data);
void      * rbtree_repl(rbtree_t *, void *);
void      * rbtree_delete(rbtree_t *, void * data);
void        rbtree_assert_insert(rbtree_t *, void *);
void      * rbtree_assert_delete(rbtree_t *, void *);
void        rbtree_walk(rbtree_t *, ORDER);                        // todo 
void        rbtree_inorderwalk(rbtree_t *);                        // todo 
void        rbtree_preorderwalk(rbtree_t *);                       // todo 
void        rbtree_postorderwalk(rbtree_t *);                      // todo 
void        rbtree_trav_refresh(rbt_trav_t * trav);
void        rbtree_trav_init(rbt_trav_t * trav, rbtree_t * tree);
void      * rbtree_trav_first(rbt_trav_t * trav, rbtree_t * tree);
void      * rbtree_trav_last(rbt_trav_t * trav, rbtree_t * tree);
void      * rbtree_trav_find(rbt_trav_t * trav, rbtree_t * tree, void * data);
void      * rbtree_trav_insert(rbt_trav_t * trav, rbtree_t * tree, void * data);
void      * rbtree_trav_cpy(rbt_trav_t * trav, rbt_trav_t * src);
void      * rbtree_trav_next(rbt_trav_t * trav); 
void      * rbtree_trav_prev(rbt_trav_t * trav);
void      * rbtree_trav_curr(rbt_trav_t * trav);
void      * rbtree_trav_repl(rbt_trav_t * trav, void * new);




#endif
