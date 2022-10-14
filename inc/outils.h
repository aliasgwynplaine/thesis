#ifndef _OUTILS_H
#define _OUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#define SAYNEXITWERROR(MSG) \
    do {                    \
        dbgerr(MSG);        \
        exit(EXIT_FAILURE); \
    } while (0);

#define SWAPINT(A, B) (A)^=(B);(B)^=(A);(A)^=(B);

#ifndef TESTPTR
#define TESTPTR(PTR)                    \
    do {                                \
        if (!(PTR)) {                   \
            SAYNEXITWERROR("No memory!")\
        }                               \
    } while (0);
#endif

#ifndef FREE
#define FREE(PTR) do { free(PTR); (PTR) = NULL; } while(0);
#endif

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;


void print_lstr(char ** lstr);

void print_lstrb(char ** lstr);

#endif