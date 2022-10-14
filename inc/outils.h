#ifndef _OUTILS_H
#define _OUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#define SWAPINT(A, B) (A)^=(B);(B)^=(A);(A)^=(B);

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