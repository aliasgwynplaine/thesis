#include "outils.h"



bnode_t * bnode_create()  {
    bnode_t * node = malloc(sizeof(bnode_t));
    node->d        = NULL;
    node->l        = NULL;
    node->r        = NULL;
    return node;
}

void bnode_free(bnode_t * root) {
    if (root->l != NULL) bnode_free(root->l);
    if (root->r != NULL) bnode_free(root->r);
    FREE(root->d);
    FREE(root);
}


setoint_t * setoint_create() {
    setoint_t * s = malloc(sizeof(setoint_t));
    s->root       = NULL;
    s->sz         = 0;
    
    return s;
}


void setoint_free(setoint_t * s) {
    bnode_free(s->root);
    FREE(s);
}


void setoint_insert(setoint_t * s , u64 d) {
    /*
        todo: implement redblacktree
    */
    bnode_t * y = NULL;
    bnode_t * x = s->root;

    while (x != NULL) {
        y = x;

        if (d < *(u64 *) x->d) x = x->l;
        else x = x->r;
    }

    if (y == NULL) {
        s->root = bnode_create();
        s->root->d = malloc(sizeof(u64));
        TESTPTR(s->root->d);
        *(u64 *)s->root->d = d;
        s->sz++;
    } else if (d < *(u64 *)y->d) {
        y->l = bnode_create();
        y->l->d = malloc(sizeof(u64));
        TESTPTR(y->l->d);
        *(u64 *)y->l->d = d;
        s->sz++;
    } else if (d > *(u64 *)y->d) {
        y->r = bnode_create();
        y->r->d = malloc(sizeof(u64));
        TESTPTR(y->r->d);
        *(u64 *)y->r->d = d;
        s->sz++;
    }
}

void inorderpush(bnode_t * root, u64 * stack, int * pos) {
    if (root == NULL) return;

    inorderpush(root->r, stack, pos);
    *(stack + *pos) = *(u64 *)root->d;
    *pos = *pos + 1;
    inorderpush(root->l, stack, pos);
}

u64 * setoint_dump(setoint_t * s) {
    u64 * dump = malloc(s->sz * sizeof(u64));
    TESTPTR(dump);
    int * pos = malloc(sizeof(int));
    *pos = 0;
    inorderpush(s->root, dump, pos);
    FREE(pos);
    return dump;
}

/**
 * @brief allocates mem for a
 * btree_t. Is necesary to call
 * bstree_free.
 * 
 * @return bstree_t* 
 */
bstree_t * bstree_create() {
    bstree_t * bst = malloc(sizeof(bstree_t));
    bst->root      = bnode_create();
    bst->sz        = 0;
    return bst;
}

void bstree_free(bstree_t * bst) {
    bnode_free(bst->root);
    FREE(bst);
}


/**
 * @brief prints a list of strings
 * as chars
 * 
 * @param lstr 
 */
void print_lstr(char ** lstr)  {
    if (!lstr) {printf("NULLL");return;}
    printf("not null\n");

    int pos = 0;
    char ** ptr = lstr;

    while (*(ptr + pos) != NULL) {
        printf("%s ", *(ptr+pos));
        pos++;
    }

    printf("\n");
}


/**
 * @brief prints a list of strings
 * as bytes
 * 
 * @param lstr 
 */
void print_lstrb(char ** lstr)  {
    if (!lstr) {return;}

    int pos = 0;
    char ** ptr = lstr;
    char * s;
    
    while (*(ptr + pos) != NULL) {
        s = *(ptr + pos);
        while (*s) {
            printf("%02x ", *s++);
        }
        pos++;
    }

    printf("\n");
}