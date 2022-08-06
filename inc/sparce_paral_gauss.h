#ifndef _SPARCE_PARAL_GAUSS_H
#define _SPARCE_PARAL_GAUSS_H

#include <stdlib.h>
#include <inttypes.h>

/* INT DEFS */

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef  int8_t   s8;
typedef  int16_t s16;
typedef  int32_t s32;
typedef  int64_t s64;


/* DATA STRUCTURES */

typedef uint64_t elm_t;

typedef struct _sparse_block { /* sparse block format */
    int nnz;
    elm_t * v;
    int * p;
    int * nz;
} sb_t;

typedef struct _hybrid_block { /* hybrid block format */
    float d;                /* density */
    elm_t * v;
    /* todo */
} hb_t;

typedef struct _mat_stb_t { /* sparce triangular block format */
    int nnz;                /* non zero entries */
    int * a;

} mat_stb_t;

typedef struct _ml_t {
    int nnz;
    int n;                  /* n - multiline vector */
    elm_t * v;
    int * p;
} ml_t;

/* WRAPPERS FOR MEM ALLOC */



mat_stb_t * stb_malloc() { return NULL; }



#endif