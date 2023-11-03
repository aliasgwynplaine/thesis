#ifndef _POL_H
#define _POL_H

#include "outils.h"

#define MAX_NUM_O_VARS 8
#define MAX_LEN_O_MSG  32
#define REPR_MAX_SZ    64

#define PARENT(I) (I - 1)>>1
#define LEFT(I)   (I<<1) + 1
#define RIGHT(I)  (I<<1) + 2

#define AAPOL_HEAD_COEF(pol) ((pol)->terms[0].coef)

#define LLPOL_HEAD(pol) ((pol)->first)
#define LLPOL_HEAD_COEF(pol) ((pol)->first->coef)

typedef struct term_t term_t;
typedef struct bpol_t bpol_t;
typedef struct lpol_t lpol_t;
/* from brandt's. */
typedef struct aapol_t aapol_t; // alternated array
typedef struct llpol_t llpol_t; // linked list pol

/* experimental */
typedef struct btpol_t btpol_t; // tree-like pol struct


enum MONOMIAL_ORDER {
    lex,
    revlex,
    grevlex,
    glex
};


struct term_t {
    COEFTYPE  coef; // could be an AP num
    u64       exp;  // packed exponent
};

struct lpol_t {
    COEFTYPE coef;
    u64      exp;
    lpol_t * nxt;
};

struct llpol_t {
    u8         nvar;
    u16          sz;
    lpol_t  * first;
};

struct bpol_t {   // pol type as btree
    COEFTYPE coef;
    u64      exp;
    bpol_t * l;
    bpol_t * r;
};

struct aapol_t { 
    u8       nvar;
    u16      sz;
    u16      cap;     // current capacity
    term_t * terms;
};

struct btpol_t {
    u16      sz;
    u8       nvar;
    bpol_t * root;
};


/* memory handling */
term_t  * term_malloc(size_t);

lpol_t  * lpol_malloc(size_t);
void      lpol_free(lpol_t *);

bpol_t  * bpol_malloc(size_t sz);
void      bpol_free(bpol_t *);

llpol_t * llpol_create(u8);
void      llpol_free(llpol_t *);

btpol_t * btpol_create(u8 n);
void      btpol_free(btpol_t *);

aapol_t * aapol_create(u8 n);
void      aapol_free(aapol_t *);



/* sorting */

void minheapify(term_t * terms, int i, int sz);
void buildminheap(term_t * terms, int hsz);
void aapol_sort(aapol_t * aapol);
void aapol_list_sort(aapol_t ** loaapol, int sz); // todo : test
void btpol_list_sort(btpol_t ** lollpol, int sz); // todo: test


/* polynomial operations */

term_t * term_multiply(term_t * p, term_t * q); // todo 


/* reading */

btpol_t  * str2btpol(const char *, char **, u8);
aapol_t  * str2aapol(const char *, char **, u8);


/* pretty printing */

void term_print(term_t *);
void llpol_print(llpol_t *);
void btpol_print(btpol_t *);
void aapol_print(aapol_t *);
char * llpol_repr(llpol_t *); // todo
char * btpol_repr(btpol_t *);
char * aapol_repr(aapol_t *);

/* define polynomial operations here */

lpol_t  * llpol_head(llpol_t *); 
llpol_t * llpol_addterm(llpol_t *, COEFTYPE, u64); 
llpol_t * llpol_add(llpol_t *, COEFTYPE, llpol_t *, COEFTYPE);
void      llpol_inplace_add(llpol_t *, COEFTYPE, llpol_t *, COEFTYPE);
llpol_t * llpol_coef_multiply(llpol_t *, COEFTYPE);
void      llpol_inplace_coef_multiply(llpol_t *, COEFTYPE);
llpol_t * llpol_multiply(llpol_t * , llpol_t *); // todo
void      llpol_inplace_multiply(llpol_t *, llpol_t *); // todo
int       llpol_hard_cmp(llpol_t * a, llpol_t * b); // todo
llpol_t * llpol_cpy(llpol_t * dst, llpol_t * src); // todo: test
lpol_t  * llpol_head_lcm(llpol_t * l1, llpol_t * l2, enum MONOMIAL_ORDER mo); // todo


term_t   * btpol_head(btpol_t *);
btpol_t  * btpol_addterm(btpol_t * btpol, COEFTYPE coef, u64 exp);
btpol_t  * btpol_add(btpol_t * a, COEFTYPE alpha, btpol_t * b, COEFTYPE betha); // todo
void       btpol_inplace_add(btpol_t * a, COEFTYPE alpha, btpol_t * b, COEFTYPE betha);// todo
btpol_t  * btpol_coef_multiply(btpol_t *a, COEFTYPE alpha);
btpol_t  * btpol_inplace_coef_multiply(btpol_t * a, COEFTYPE alpha);
btpol_t  * btpol_multiply(btpol_t * a, btpol_t * b); // todo 
int        btpol_monomial_cmp(btpol_t * a, btpol_t * b); 
int        btpol_hard_cmp(btpol_t * a, btpol_t * b);
btpol_t  * btpol_cpy(btpol_t * dst, btpol_t * src);
term_t   * btpol_head_lcm(btpol_t * l1, btpol_t * l2, enum MONOMIAL_ORDER mo); // todo

term_t   * aapol_head(aapol_t *);
aapol_t  * aapol_addterm(aapol_t * aapol, COEFTYPE coef, u64 exp);
aapol_t  * aapol_add(aapol_t * a, COEFTYPE alpha, aapol_t * b, COEFTYPE betha); 
void       aapol_inplace_add(aapol_t * a, COEFTYPE alpha, aapol_t * b, COEFTYPE betha); //todo
aapol_t  * aapol_coef_multiply(aapol_t * a, COEFTYPE alpha);
aapol_t  * aapol_inplace_coef_multiply(aapol_t * a, COEFTYPE alpha);
aapol_t  * aapol_multiply(aapol_t * a, aapol_t * b); 
int        aapol_monomial_cmp(aapol_t * a, aapol_t * b);
int        aapol_hard_cmp(aapol_t * a, aapol_t * b); 
aapol_t  * aapol_cpy(aapol_t * dst, aapol_t * src);
term_t   * aapol_head_lcm(aapol_t * a1, aapol_t * a2, enum MONOMIAL_ORDER mo);

/* bit masks and bit extraction*/

double s_exp_norm(u64, u8);
int    s_exp_cmp(u64, u64, u8, enum MONOMIAL_ORDER);
int    s_exp_lex_cmp(u64, u64, u8);
int    s_exp_glex_cmp(u64, u64, u8); 
int    s_exp_revlex_cmp(u64, u64, u8);
int    s_exp_grevlex_cmp(u64, u64, u8);
int    d_exp_lex_cmp(u64 *, u64 *, u8);
int    d_exp_glex_cmp(u64 *, u64 *, u8); 
int    d_exp_revlex_cmp(u64 *, u64 * , u8);
int    d_exp_grevlex_cmp(u64* , u64 * , u8);
u64 *  exp_add(u64 *, u64 *, u64 *);
u64 *  exp_unpack(u64, u8);
u64    exp_pack(u64 *, u8);


/* exponent's binary masks */
/*
 remeber you've generated this 
 with bmask_gen.py
 todo: improve with bit wizardry and
 inline functions.
*/
/* 1 variables */
#define BMASK_1V_0 0xffffffffffffffff

/* masks list for 1 vars*/
static const u64 MASK_LIST_1V[] = {
    BMASK_1V_0 
};

/* 2 variables */
#define BMASK_2V_1 0x00000000ffffffff
#define BMASK_2V_0 0xffffffff00000000

/* masks list for 2 vars*/
static const u64 MASK_LIST_2V[] = {
    BMASK_2V_0, BMASK_2V_1 
};

/* 3 variables */
#define BMASK_3V_2 0x00000000001fffff
#define BMASK_3V_1 0x000003ffffe00000
#define BMASK_3V_0 0xfffffc0000000000

/* masks list for 3 vars*/
static const u64 MASK_LIST_3V[] = {
    BMASK_3V_0, BMASK_3V_1, BMASK_3V_2 
    
};

/* 4 variables */
#define BMASK_4V_3 0x000000000000ffff
#define BMASK_4V_2 0x00000000ffff0000
#define BMASK_4V_1 0x0000ffff00000000
#define BMASK_4V_0 0xffff000000000000

/* masks list for 4 vars*/
static const u64 MASK_LIST_4V[] = {
    BMASK_4V_0, BMASK_4V_1, BMASK_4V_2, 
    BMASK_4V_3 
};

/* 5 variables */
#define BMASK_5V_4 0x0000000000000fff
#define BMASK_5V_3 0x0000000000fff000
#define BMASK_5V_2 0x0000000fff000000
#define BMASK_5V_1 0x0000fff000000000
#define BMASK_5V_0 0xffff000000000000

/* masks list for 5 vars*/
static const u64 MASK_LIST_5V[] = {
    BMASK_5V_0, BMASK_5V_1, BMASK_5V_2, 
    BMASK_5V_3, BMASK_5V_4 
};

/* 6 variables */
#define BMASK_6V_5 0x00000000000003ff
#define BMASK_6V_4 0x00000000000ffc00
#define BMASK_6V_3 0x000000003ff00000
#define BMASK_6V_2 0x000000ffc0000000
#define BMASK_6V_1 0x0003ff0000000000
#define BMASK_6V_0 0xfffc000000000000

/* masks list for 6 vars*/
static const u64 MASK_LIST_6V[] = {
    BMASK_6V_0, BMASK_6V_1, BMASK_6V_2, 
    BMASK_6V_3, BMASK_6V_4, BMASK_6V_5 
    
};

/* 7 variables */
#define BMASK_7V_6 0x00000000000001ff
#define BMASK_7V_5 0x000000000003fe00
#define BMASK_7V_4 0x0000000007fc0000
#define BMASK_7V_3 0x0000000ff8000000
#define BMASK_7V_2 0x00001ff000000000
#define BMASK_7V_1 0x003fe00000000000
#define BMASK_7V_0 0xffc0000000000000

/* masks list for 7 vars*/
static const u64 MASK_LIST_7V[] = {
    BMASK_7V_0, BMASK_7V_1, BMASK_7V_2, 
    BMASK_7V_3, BMASK_7V_4, BMASK_7V_5, 
    BMASK_7V_6 
};

/* 8 variables */
#define BMASK_8V_7 0x00000000000000ff
#define BMASK_8V_6 0x000000000000ff00
#define BMASK_8V_5 0x0000000000ff0000
#define BMASK_8V_4 0x00000000ff000000
#define BMASK_8V_3 0x000000ff00000000
#define BMASK_8V_2 0x0000ff0000000000
#define BMASK_8V_1 0x00ff000000000000
#define BMASK_8V_0 0xff00000000000000

/* masks list for 8 vars*/
static const u64 MASK_LIST_8V[] = {
    BMASK_8V_0, BMASK_8V_1, BMASK_8V_2, 
    BMASK_8V_3, BMASK_8V_4, BMASK_8V_5, 
    BMASK_8V_6, BMASK_8V_7 
};

#endif