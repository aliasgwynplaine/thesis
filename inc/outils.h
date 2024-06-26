#ifndef _OUTILS_H
#define _OUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stddef.h>
#include <assert.h>
#include <time.h>
#include "debug.h"

#ifndef COEFTYPE
#define COEFTYPE float

#endif

#define SAYNEXITWERROR(MSG) \
    do {                    \
        dbgerr(MSG);        \
        exit(EXIT_FAILURE); \
    } while (0)

#define endl printf("\n")

#ifndef __max
#define __max(a,b)    (((a) > (b)) ? (a) : (b))

#endif

#ifndef __min
#define __min(a,b)    (((a) < (b)) ? (a) : (b))

#endif

#define SWAPINT(A, B) \
    do {              \
        (A)^=(B);     \
        (B)^=(A);     \
        (A)^=(B);     \
    } while(0)        \

#define SWAP(A, B, Temp) \
    do {                 \
        (Temp) = (A);    \
        (A) = (B);       \
        (B) = (Temp);    \
    } while (0)          \

#ifndef CHECKPTR
#define CHECKPTR(PTR)                    \
    do {                                 \
        if (!(PTR)) {                    \
            SAYNEXITWERROR("No memory!");\
        }                                \
    } while (0)

#endif

#ifndef FREE
#define FREE(PTR) do { free(PTR); (PTR) = NULL; } while(0)
#endif

typedef unsigned char uchar;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t   u8;

typedef int64_t  i64;
typedef int32_t  i32;
typedef int16_t  i16;
typedef int8_t    i8;

typedef i32    coef_t;
typedef int32_t idx_t;

typedef struct tictac_t tictac_t;

struct tictac_t {
    struct timespec ops_set;
    struct timespec cpy_exp;
    struct timespec cmp_exp;
    struct timespec ops_exp;
    struct timespec rref;
    struct timespec total;
};

#define TIMESPEC_DIFF(diff_ts, ts0, ts1)                   \
    do {                                                   \
        (diff_ts).tv_sec = (ts0).tv_sec - (ts1).tv_sec;    \
        (diff_ts).tv_nsec = (ts0).tv_nsec - (ts1).tv_nsec; \
    } while (0);

#define TIMESPEC_ADD(add_ts, ts0, ts1)                     \
    do {                                                   \
        (add_ts).tv_sec = (ts0).tv_sec + (ts1).tv_sec;     \
        (add_ts).tv_nsec = (ts0).tv_nsec + (ts1).tv_nsec;  \
    } while (0);

typedef struct bdat_t bdat_t;

typedef int    cmpfux_t(const void * a, const void * b);
typedef void * allocfux_t(const size_t sz);
typedef void   freefux_t(void *);

struct bdat_t {
    void * k;
    void * v;
};


bdat_t * bdat_create();
void     bdat_free(void * bdat);

void settimespec2zero(struct timespec *);
void settictac2zero(tictac_t *);
void print_tictac(tictac_t *);

int  is_str_in_lstr(char * str, char ** lstr);
void print_lstr(char ** lstr);
void print_lstrb(char ** lstr);

double  int_cumsum(int * p, int * c, int n);
int     int_max(int * a, int p, int q);
int     int_max_idx(int * a, int p, int q);
u64     u64_max(u64 * a, int p, int q);
u64     u64_max_idx(u64 * a, int p, int q);

#endif