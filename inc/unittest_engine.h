#ifndef _UNITTEST_ENGINE_H
#define _UNITTEST_ENGINE_H

#include <stdio.h>

#ifdef assert
#undef assert
#endif
#define assert(test, msg) do {  \
    if (!(test)) return msg;    \
} while (0)

#define run_uniitest(test) do {       \
    char * msg = test(); tests_run++; \
    if (msg) {                        \
        failed_tests++;               \
        fprintf(stderr, "%s\n", msg); \
    }                                 \
} while(0)

extern int tests_run;
extern int failed_tests;

#endif