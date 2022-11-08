#ifndef _POL_H
#define _POL_H
#include "outils.h"
#include "debug.h"
#include "matrix.h"

#define MAX_NUM_O_VARS 8

typedef struct pol_t pol_t;
typedef struct lpol_t lpol_t;

struct pol_t {
    COEFTYPE  coef; // could be an AP num
    u64       exp;  // packed exponent
};

struct lpol_t {   // pol type for llist
    COEFTYPE coef;
    u64      exp;
    lpol_t * l;
    lpol_t * r;
};

/* from brandt's. no implemented yet */
typedef struct aapol_t aapol_t; // alternated array
typedef struct llpol_t llpol_t; // tree-like pol struct

struct aapol_t { // heap-like structure
    u8      nvar;
    u16     sz;
    u16     cap;     // current capacity
    pol_t * terms;
};

struct llpol_t {
    u16      sz;
    u8       nvar;
    lpol_t * root;
};


pol_t   * str2pol(char *); // todo
llpol_t * addterm2llpol(llpol_t * llpol, COEFTYPE coef, u64 exp);


void      minheapify(pol_t * terms, int i, int sz);
void      buildminheap(pol_t * terms, int hsz);
void      sortaapol(aapol_t * aapol);
aapol_t * addterm2aapol(aapol_t * aapol, COEFTYPE coef, u64 exp);


/* polynomial operations */

pol_t * addpol(pol_t * p, double a, pol_t * q, double b); // a*p + b*q
pol_t * mulpol(pol_t * p, pol_t * q);

/* matrix2pol & pol2matrix transformations*/

smatrix_t * aapol2smatrix(aapol_t * aapol, int sz);
smatrix_t * aapol2smatrix_(aapol_t * aapol, int sz);


/* memory handling */
lpol_t  * lpolmalloc(size_t sz);
llpol_t * llpolmalloc(u8 n);
aapol_t * aapolmalloc(u8 n);
void      freelpol(lpol_t *);
void      freeaapol(aapol_t *);
void      freellpol(llpol_t *);

/* pretty printing */
void printpol(pol_t * pol);
void printllpol(llpol_t * llpol);
void printaapol(aapol_t * aapol);

/* define polynomial operations here */



/* bit masks and bit extraction*/

int   cmpexplex(u64, u64, u8);
int   cmpexprevlex(u64, u64, u8);
void  expadd(u64 *, u64 *, u64 *);
u64 * unpackexp(u64, u8);
u64   packexp(u64 *, u8);


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