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
typedef uint8_t   u8;

typedef struct bdat_t bdat_t;


struct bdat_t {
    void * k;
    void * v;
};


bdat_t    * bdat_create();
void        bdat_free(void * bdat);

void        print_lstr(char ** lstr);
void        print_lstrb(char ** lstr);

int         int_cumsum(int * p, int * c, int n);

#endif