#ifndef _OUTILS_H
#define _OUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "debug.h"

#ifndef COEFTYPE
#define COEFTYPE float
#endif

#define SAYNEXITWERROR(MSG) \
    do {                    \
        dbgerr(MSG);        \
        exit(EXIT_FAILURE); \
    } while (0)

#define SWAPINT(A, B) (A)^=(B);(B)^=(A);(A)^=(B)

#ifndef __max
#define __max(a,b)    (((a) > (b)) ? (a) : (b))

#endif

#ifndef __min
#define __min(a,b)    (((a) < (b)) ? (a) : (b))

#endif

#ifndef TESTPTR
#define TESTPTR(PTR)                     \
    do {                                 \
        if (!(PTR)) {                    \
            SAYNEXITWERROR("No memory!");\
        }                                \
    } while (0)
#endif

#ifndef FREE
#define FREE(PTR) do { free(PTR); (PTR) = NULL; } while(0)
#endif

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;
typedef enum ORDER ORDER;
typedef struct bnode_t bnode_t; // binary node
typedef struct tnode_t tnode_t; // ternary node
typedef struct setoint_t setoint_t;
typedef struct bstree_t bstree_t;
typedef struct avltree_t avltree_t;
typedef struct rbtree_t rbtree_t;

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
    inorder_,
    preorder_,
    postorder_
};

struct setoint_t {
    bnode_t * root;
    int         sz;
};

struct avltree_t {
    bnode_t * root;
    int         sz;
};

struct bstree_t {
    bnode_t * root;
    int         sz;
};

bnode_t   * bnode_create();
void        bnode_free(bnode_t * root);

setoint_t * setoint_create();
void        setoint_free(setoint_t *);
void        setoint_insert(setoint_t *, u64);
u64       * setoint_dump(setoint_t *);

bstree_t  * bstree_create();
void        bstree_free(bstree_t *);
void      * bstree_search(bnode_t *, void * data);
void        bstree_insert(bnode_t *, void * data);
void        bstree_walk(bnode_t *, ORDER);
void        bstree_inorder_walk(bnode_t *);
void        bstree_preorder_walk(bnode_t *);
void        bstree_postorder_walk(bnode_t *);

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

void        print_lstr(char ** lstr);
void        print_lstrb(char ** lstr);

#endif